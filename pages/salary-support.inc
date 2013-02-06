<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
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
  
	/*
	 * @param $count: boolean value indicating whether we are to display the actual content or just return the entity count.
	 * @param $distinct: boolean value indicating whether we should count distinct users or allow duplicates.
	 */
	public function generate_inst_query($count, $distinct = false){
		$query = db_select('node', 'node');
		//$query = db_select('field_data_field_er_inst_ref', 'inst');
		$query->innerJoin('field_data_field_er_inst_ref', 'inst', 'node.nid = inst.field_er_inst_ref_target_id');
		$query->innerJoin('field_data_field_er_inst_participating', 'participating', 'node.nid = participating.entity_id');
		$query->leftJoin('field_data_field_er_inst_abbreviation', 'abbr', 'inst.field_er_inst_ref_target_id = abbr.entity_id');
		$query->leftJoin('field_data_field_er_fname', 'fname', 'inst.entity_id = fname.entity_id');
		$query->leftJoin('field_data_field_er_lname', 'lname', 'inst.entity_id = lname.entity_id');
		$query->leftJoin('field_data_field_er_department', 'department', 'inst.entity_id = department.entity_id');
		$query->condition('inst.bundle', 'user', '=');//must be attached to a user.
		$query->condition('participating.field_er_inst_participating_value', '1', '=');//yes, this is a participating institution
		$query->addField('inst', 'entity_id', 'uid');
		$query->addField('abbr', 'field_er_inst_abbreviation_value', 'abbr');
		$query->addField('node', 'nid');
		$query->addField('node', 'title', 'inst_name');
		$query->addField('fname', 'field_er_fname_value', 'fname');
		$query->addField('lname', 'field_er_lname_value', 'lname');
		$query->addField('department', 'field_er_department_value', 'department');
		$query->orderBy('inst_name', 'ASC');
		$query->orderBy('department', 'ASC');
		$query->orderBy('lname', 'ASC');
		return $query;
	}
	
	/*
	 * @param $count: boolean value indicating whether we are to display the actual content or just return the entity count.
	 * @param $distinct: boolean value indicating whether we should count distinct users or allow duplicates.
	 */
	public function generate_user_query($count, $distinct = false){
		$query = db_select('users', 'users');
		
		////Join with attached institutions
		//$query->innerJoin('field_data_field_er_inst_ref', 'institution', 'users.uid = institution.entity_id');
		////Join in field: Is this institution a "participating" institution?
		//$query->leftJoin('field_data_field_er_inst_participating', 'participating', 'institution.field_er_inst_ref_target_id = participating.entity_id');
		////Join in position type
		//$query->leftJoin('field_data_field_er_position_type', 'position_type', 'users.uid = position_type.entity_id');
		//
		////only give me valid position types (not Other, or Evaluation)
		//$query->condition('position_type.field_er_position_type_value', $this->categories, 'IN');
		////Institution must be "Participating"
		//$query->condition('participating.field_er_inst_participating_value', '1', '=');
		//
		//$query->addField('institution', 'field_er_inst_ref_target_id', 'institution_id');
		//$query->addField('position_type', 'field_er_position_type_value', 'position_type');
		//
		$query = $this->find_users($query, $distinct);
		////d((string)$query);
		return $query;
	}
	
	public function data($count = true){
		$query = $this->generate_inst_query($count);
		$results = $query->execute();
		$data = array();
		foreach ($results as $result){
			//d($result, '$result');
			$data['users'][$result->nid][$result->uid] = array('fname'=>$result->fname, 'lname'=>$result->lname, 'department'=>$result->department);
			if (!isset($data['inst'][$result->nid]))
				$data['inst'][$result->nid] = array('name'=>$result->inst_name, 'abbr'=>$result->abbr);
		}
		return $data;
	}
	
	public function html(){
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		//d($this->institutions, 'institutions');
		//d($this->support_types, 'support_types');
		d($this->data(), 'count');
		$data = $this->data(false);
		d($data, 'data');
		$output = $this->download_link();
		//er_get_realname($uid);
		foreach ($data['users'] as $inst_nid=>$users){
			$inst_name = $inst_nid?$data['inst'][$inst_nid]['name']:'Blank Institution';
			$output .= "<h2>".$inst_name."</h2>";
			foreach ($users as $uid=>$user){
				$name = $user['lname'] . ', ' . $user['fname'];
				$department = $user['department']?' ('.$user['department'].')':'';
				$output .= "<h3>".l($name, 'user/'.$user['uid']).$department."</h3>";
			}
		}
		//foreach ($data['paid'] as $inst_nid=>$paid){
		//	$inst_name = $inst_nid?$data['inst'][$inst_nid]['inst_full']:'Blank Institution';
		//	$output .= "<h2>".$inst_name."</h2>";
		//	foreach ($paid as $uid=>$types){
		//		$user = $data['users'][$uid];
		//		$output .= "<h3>".l($user['name'], 'user/'.$uid)."</h3>";
		//		$output .= "<ol>";
		//		foreach ($types as $user_node){
		//			$output .= "<li>";
		//			$output .= "Effort: ".$user_node['effort']."<br>";
		//			$output .= "Funding: ".$user_node['funding']."<br>";
		//			$output .= "Type: ".$user_node['type']."<br>";
		//			$output .= l("Edit paid node", 'node/'.$user_node['nid'].'/edit');
		//			$output .= "</li>";
		//		}
		//		$output .= "</ol>";
		//	}
		//}
		return $output;
	}
	
	private function write_total_formatting(&$worksheet, $row){
		$worksheet->getStyle('A'.$row.':L'.$row)->applyFromArray(array(
			//'borders' => array(
			//	'outline' => array(
			//		'style' => PHPExcel_Style_Border::BORDER_DOUBLE,
			//		//'color' => array('argb' => 'FFFF0000'),
			//	),
			//),
			'fill' => array(
					'type' => PHPExcel_Style_Fill::FILL_SOLID,
					'color' => array('rgb' => 'C0C0C0')
			),
			'font' => array(
				'bold'=>true,
			),
			
		));
		$worksheet->mergeCells('A'.$row.':C'.$row);
		$worksheet->getStyle('A'.$row.':C'.$row)->applyFromArray(array(
			'font' => array(
				'italic'=>true,
				'size'=>12,
			)
		));
	}
	
	//Just fill out the names
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
		foreach ($data['users'] as $p){
			$add_rows += 1 + count($p);
		}
		if ($add_rows > 2)
			$worksheet->insertNewRowBefore($base_row+1, $add_rows-2);
		
		foreach ($data['users'] as $inst_nid=>$users){
			$inst_name = $inst_nid?$data['inst'][$inst_nid]['name']:'Blank Institution';
			$abbr_name = $inst_nid?$data['inst'][$inst_nid]['abbr']:$inst_name;
			$output .= "<h2>".$inst_name."</h2>";
			foreach ($users as $uid=>$user){
				$name = $user['lname'] . ', ' . $user['fname'];
				$department = $user['department']?$user['department']:'?';
				$output .= "<h3>".l($user['lname'] . ', ' . $user['fname'], 'user/'.$user['uid'])."</h3>";
				$worksheet->setCellValue('A'.$row, $abbr_name);
				$worksheet->setCellValue('B'.$row, $department);
				$worksheet->setCellValue('C'.$row, $name);
				$row++;
			}
			$worksheet->setCellValue('A'.$row, "Total for $inst_name");
			$this->write_total_formatting($worksheet, $row);
			$row++;
		}
	}
	
	////Old version which fills everything.
	//public function write_excel(&$objPHPExcel, $sheet = 0){
	//	$data = $this->data();
	//	$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
	//	$base_row = 7;
	//	$row = $base_row;
	//	
	//	//these are used in placing values in the correct column
	//	$E = ord('E');
	//	$I = ord('I');
	//	
	//	//
	//	//$add = $data['count']-2;
	//	//if ($add > 0)
	//	//	$worksheet->insertNewRowBefore($base_row + 1, $add);
	//	$default = array(
	//		"Other NSF"=>array("effort"=>0,"funding"=>0),
	//		"Other Gov"=>array("effort"=>0,"funding"=>0),
	//		"Other"=>array("effort"=>0,"funding"=>0),
	//	);
	//	$add_rows = 0;
	//	foreach ($data['paid'] as $p){
	//		$add_rows += 1 + count($p);
	//	}
	//	if ($add_rows > 2)
	//		$worksheet->insertNewRowBefore($base_row+1, $add_rows-2);
	//	
	//	$totals = $default;//this is the final tally of everything...
	//	foreach ($data['paid'] as $inst_nid=>$paid){
	//		$inst_name = $inst_nid?$data['inst'][$inst_nid]['inst_full']:'Blank Institution';
	//		$inst_abbr = $inst_nid && $data['inst'][$inst_nid]['inst_abbr']?$data['inst'][$inst_nid]['inst_abbr']:$inst_name;
	//		$type_total = $default;//this is the tally for this particular institution
	//		foreach ($paid as $uid=>$types){
	//			$user = $data['users'][$uid];
	//			$values = array_merge($default, $types);
	//			$worksheet->setCellValue('A'.$row, $inst_abbr);
	//			$worksheet->setCellValue('B'.$row, $user['department']);
	//			$worksheet->setCellValue('C'.$row, $user['name']);
	//			
	//			foreach (array("Other NSF","Other Gov","Other") as $t=>$type){
	//				$type_total[$type]["effort"] += $values[$type]["effort"];
	//				$worksheet->setCellValue(chr($E+$t).$row, $values[$type]["effort"]);
	//				$type_total[$type]["funding"] += $values[$type]["funding"];
	//				$worksheet->setCellValue(chr($I+$t).$row, $values[$type]["funding"]);
	//			}
	//			$row++;
	//		}
	//		$this->write_total_formatting($worksheet, $row);
	//		$worksheet->setCellValue('A'.$row, "Total for $inst_name");
	//		foreach (array("Other NSF","Other Gov","Other") as $t=>$type){
	//			$totals[$type]["effort"] += $type_total[$type]["effort"];
	//			$worksheet->setCellValue(chr($E+$t).$row, $type_total[$type]["effort"]);
	//			$totals[$type]["funding"] += $type_total[$type]["funding"];
	//			$worksheet->setCellValue(chr($I+$t).$row, $type_total[$type]["funding"]);
	//		}
	//		$row++;
	//	}
	//	foreach (array("Other NSF","Other Gov","Other") as $t=>$type){
	//		$worksheet->setCellValue(chr($E+$t).$row, $totals[$type]["effort"]);
	//		$worksheet->setCellValue(chr($I+$t).$row, $totals[$type]["funding"]);
	//	}
	//}

	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Salary-Support-template.xls");
		return $objPHPExcel;
	}
	
}