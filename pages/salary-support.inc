<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
class er_salary_support extends er_page{
	public function __construct() {
		parent::__construct();
  }
	
	public function title(){
		return "Salary Support";
	}
  
	/*
	 * @param $count: boolean value indicating whether we are to display the actual content or just return the entity count.
	 * @param $distinct: boolean value indicating whether we should count distinct users or allow duplicates.
	 */
	public function generate_user_query($count, $distinct = false){
		//see er.module for the definition of these tags:
		//$efq = new EntityFieldQuery();
		//$efq->entityCondition('entity_type', 'user')
		//	->addTag('epscor_participant')
		//	->addTag('epscor_active_participant')
		//$results = $efq->execute();
		//$users = entity_load('user', array_keys($results['user']));
		
		
		$query = db_select('users', 'users');
		
		//Join in the institution data:
		$query->innerJoin('field_data_field_er_inst_ref', 'inst_ref', 'users.uid = inst_ref.entity_id AND inst_ref.bundle = \'user\'');
		$query->innerJoin('node', 'inst', 'inst.nid = inst_ref.field_er_inst_ref_target_id');
		
		//These use the EntityFieldQuery tag definitions:
		//DEPRECATED: We need to rely solely on the date ranges to determine participant status.
		//er_query_epscor_participant_alter($query);//Make sure it's only participants
		er_query_epscor_active_participant_alter($query);//Must be active during the appropriate date range
		//Only faculty:
		$query->innerJoin('field_data_'.self::$field_aliases['position_type'], 'position_type', 'users.uid = position_type.entity_id AND position_type.bundle = \'user\'');
		$query->condition('position_type.'.self::$field_aliases['position_type'].'_value', 'Faculty participants (or equivalent)', '=');
		
		//return the relevant data:
		$query->leftJoin('field_data_'.self::$field_aliases['inst_abbr'], 'inst_abbr', 'inst.nid = inst_abbr.entity_id');
		$query->leftJoin('field_data_'.self::$field_aliases['fname'], 'fname', 'users.uid = fname.entity_id');
		$query->leftJoin('field_data_'.self::$field_aliases['lname'], 'lname', 'users.uid = lname.entity_id');
		$query->leftJoin('field_data_'.self::$field_aliases['department'], 'department', 'users.uid = department.entity_id');
		$query->addField('inst', 'nid', 'inst_nid');
		$query->addField('inst', 'title', 'inst_name');
		$query->addField('inst_abbr', self::$field_aliases['inst_abbr'].'_value', 'inst_abbr');
		$query->addField('fname', self::$field_aliases['fname'].'_value', 'fname');
		$query->addField('lname', self::$field_aliases['lname'].'_value', 'lname');
		$query->addField('department', self::$field_aliases['department'].'_value', 'department');
		//$query->addField('position_type', 'field_er_position_type_value', 'position_type');
		$query->orderBy('inst_name', 'ASC');
		$query->orderBy('department', 'ASC');
		$query->orderBy('lname', 'ASC');
		$query = $this->find_users($query, $distinct);
		//d((string)$query, 'user');
		return $query;
	}
	
	public function data($count = true){
		$query = $this->generate_user_query($count);
		$results = $query->execute();
		$data = array();
		foreach ($results as $result){
			//d($result, '$result');
			$data['users'][$result->inst_nid][$result->uid] = array('fname'=>$result->fname, 'lname'=>$result->lname, 'department'=>$result->department);
			if (!isset($data['inst'][$result->nid]))
				$data['inst'][$result->inst_nid] = array('name'=>$result->inst_name, 'abbr'=>$result->inst_abbr?$result->inst_abbr:$result->inst_name, 'nid'=>$result->inst_nid);
		}
		return $data;
	}
	
	//not really necessary...
	public function attach_fields($entity_type, $bundle, $entities, $fields){
		foreach ($fields as $field){
			$info = field_info_instance($entity_type, $field, $bundle);
			field_attach_load($entity_type, $entities, FIELD_LOAD_CURRENT, array('field_id' => $info['field_id']));
		}
	}
	
	public function html(){
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		$data = $this->data(false);
    if (isset($_GET['debug'])){
      d($this->data(), 'counts');
      d($data, 'full data');
    }
		$output = $this->download_link();
		if (is_array($data['users']))
		foreach ($data['users'] as $inst_nid=>$users){
			$inst_name = $inst_nid?$data['inst'][$inst_nid]['name']:'Blank Institution';
			$output .= "<h2>".$inst_name."</h2>";
			foreach ($users as $uid=>$user){
				$name = $user['lname'] . ', ' . $user['fname'];
				$department = $user['department']?' ('.$user['department'].')':'';
				$output .= "<h3>".l($name, 'user/'.$user['uid']).$department."</h3>";
			}
		}
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
		
		$default = array(
			"Other NSF"=>array("effort"=>0,"funding"=>0),
			"Other Gov"=>array("effort"=>0,"funding"=>0),
			"Other"=>array("effort"=>0,"funding"=>0),
		);
		$add_rows = 0;
		if (is_array($data['users'])){
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
	}

	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Salary-Support-template.xls");
		return $objPHPExcel;
	}
	
}