<?php
/*
 * Note from Andrew W. 12-10-2012:
 * This page is not working, I commented out line 28 in order to stop views from running.
 * The view is broken for some reason, and will WSOD if run.
 * This is the error message: PHP Fatal error:  Cannot access empty property in /import/webservices/tier34/EPSCOR/html/ERcore/modules/field/field.attach.inc on line 325, referer: https://epscor.arsc.edu/ERcore/admin/structure/views/view/er_report_salary_support/edit/count
 */
class er_salary_support extends er_page{
	private $institutions, $support_types;
	
	public function __construct() { 
		parent::__construct();
		//$this->excel_file = "Participants-template.xls";
		//$this->institutions = array_values($this->get_allowed_values("field_er_institution"));
		//$this->support_types = array_values($this->get_allowed_values("field_er_support_type"));
		
		//$this->institutions = list_allowed_values(field_info_field("field_er_institution"));
		//$this->support_types = list_allowed_values(field_info_field("field_er_support_type"));
  }
	
	public function title(){
		return "Salary Support";
	}
    
	public function data($count = true){
		$view = views_get_view('er_report_salary_support');
		$view->set_display($count?'count':'default');
		//$view->execute();
		$data = array('paid'=>array(), 'inst'=>array(), 'users'=>array());
		foreach ($view->result as $result){
			//dsm($result, 'result');
			$inst_nid = $result->node_field_data_field_er_inst_ref_nid;
			if ($inst_nid && !isset($data['inst'][$inst_nid]))
				$data['inst'][$inst_nid] = array(
					'inst_full'=>$this->tryget($result, 'field_field_er_inst_name', 'Node:'.$inst_nid),
					'inst_abbr'=>$this->tryget($result, 'field_field_er_inst_abbreviation', null),
				);
			
			$uid = $result->users_node_uid;
			if (!isset($data['users'][$uid]))
				$data['users'][$uid] = array(
					'name'=>$this->tryget($result, 'field_field_er_realname', 'User:'.$uid),
					'department'=>$this->tryget($result, 'field_field_er_department', ''),
				);
			
			$support_type = $result->field_field_er_support_type[0]['rendered']['#markup'];
			$paid = array(
				"effort"=>$result->field_field_er_funded_effort[0]['raw']['value'],
				"funding"=>$result->field_field_er_funding[0]['raw']['value'],
				"type"=>$support_type,
				"nid"=>$result->nid,
			);
			if ($count){
				$data['paid'][$inst_nid][$uid][$support_type] = $paid;
			}else{
				$data['paid'][$inst_nid][$uid][] = $paid;
			}
		}
		return $data;
	}
	
	private function tryget(&$result, $field, $default){
		if (isset($result->$field) && count($result->$field) > 0){
			$value = array_pop($result->$field);
			if (isset($value['rendered']) && isset($value['rendered']['#markup']))
				return $value['rendered']['#markup'];
			else
				return $value['raw']['value'];
		}else
			return $default;
	}
	
	public function html(){
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		//d($this->institutions, 'institutions');
		//d($this->support_types, 'support_types');
		d($this->data(), 'count');
		$data = $this->data(false);
		d($data, 'data');
		$output = $this->download_link();
		foreach ($data['paid'] as $inst_nid=>$paid){
			$inst_name = $inst_nid?$data['inst'][$inst_nid]['inst_full']:'Blank Institution';
			$output .= "<h2>".$inst_name."</h2>";
			foreach ($paid as $uid=>$types){
				$user = $data['users'][$uid];
				$output .= "<h3>".l($user['name'], 'user/'.$uid)."</h3>";
				$output .= "<ol>";
				foreach ($types as $user_node){
					$output .= "<li>";
					$output .= "Effort: ".$user_node['effort']."<br>";
					$output .= "Funding: ".$user_node['funding']."<br>";
					$output .= "Type: ".$user_node['type']."<br>";
					$output .= l("Edit paid node", 'node/'.$user_node['nid'].'/edit');
					$output .= "</li>";
				}
				$output .= "</ol>";
			}
		}
		return $output;
	}
	
	private function write_total_formatting(&$worksheet, $row){
		$worksheet->getStyle('A'.$row.':L'.$row)->applyFromArray(array(
			'borders' => array(
				'outline' => array(
					'style' => PHPExcel_Style_Border::BORDER_DOUBLE,
					//'color' => array('argb' => 'FFFF0000'),
				),
			),
			'font' => array(
				'bold'=>true,
			)
		));
		$worksheet->mergeCells('A'.$row.':C'.$row);
		$worksheet->getStyle('A'.$row.':C'.$row)->applyFromArray(array(
			'font' => array(
				'bold'=>false,
				'italic'=>true,
				'size'=>12,
			)
		));
	}
	
	//I modified this function to be able to handle a variable number of institutions.
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$data = $this->data();
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
		$base_row = 7;
		$row = $base_row;
		
		//these are used in placing values in the correct column
		$E = ord('E');
		$I = ord('I');
		
		//
		//$add = $data['count']-2;
		//if ($add > 0)
		//	$worksheet->insertNewRowBefore($base_row + 1, $add);
		$default = array(
			"Other NSF"=>array("effort"=>0,"funding"=>0),
			"Other Gov"=>array("effort"=>0,"funding"=>0),
			"Other"=>array("effort"=>0,"funding"=>0),
		);
		$add_rows = 0;
		foreach ($data['paid'] as $p){
			$add_rows += 1 + count($p);
		}
		if ($add_rows > 2)
			$worksheet->insertNewRowBefore($base_row+1, $add_rows-2);
		
		$totals = $default;//this is the final tally of everything...
		foreach ($data['paid'] as $inst_nid=>$paid){
			$inst_name = $inst_nid?$data['inst'][$inst_nid]['inst_full']:'Blank Institution';
			$inst_abbr = $inst_nid && $data['inst'][$inst_nid]['inst_abbr']?$data['inst'][$inst_nid]['inst_abbr']:$inst_name;
			$type_total = $default;//this is the tally for this particular institution
			foreach ($paid as $uid=>$types){
				$user = $data['users'][$uid];
				$values = array_merge($default, $types);
				$worksheet->setCellValue('A'.$row, $inst_abbr);
				$worksheet->setCellValue('B'.$row, $user['department']);
				$worksheet->setCellValue('C'.$row, $user['name']);
				
				foreach (array("Other NSF","Other Gov","Other") as $t=>$type){
					$type_total[$type]["effort"] += $values[$type]["effort"];
					$worksheet->setCellValue(chr($E+$t).$row, $values[$type]["effort"]);
					$type_total[$type]["funding"] += $values[$type]["funding"];
					$worksheet->setCellValue(chr($I+$t).$row, $values[$type]["funding"]);
				}
				$row++;
			}
			$this->write_total_formatting($worksheet, $row);
			$worksheet->setCellValue('A'.$row, "Total for $inst_name");
			foreach (array("Other NSF","Other Gov","Other") as $t=>$type){
				$totals[$type]["effort"] += $type_total[$type]["effort"];
				$worksheet->setCellValue(chr($E+$t).$row, $type_total[$type]["effort"]);
				$totals[$type]["funding"] += $type_total[$type]["funding"];
				$worksheet->setCellValue(chr($I+$t).$row, $type_total[$type]["funding"]);
			}
			$row++;
		}
		foreach (array("Other NSF","Other Gov","Other") as $t=>$type){
			$worksheet->setCellValue(chr($E+$t).$row, $totals[$type]["effort"]);
			$worksheet->setCellValue(chr($I+$t).$row, $totals[$type]["funding"]);
		}
	}

	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Salary-Support-template.xls");
		return $objPHPExcel;
	}
	
}