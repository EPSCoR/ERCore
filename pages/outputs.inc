<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
class er_outputs extends er_page{
  //public function __construct(){
  //  parent::__construct();
  //}
    
  public function title(){
    // Original page title
    //return "Outputs".$this->range_string();
    return "Outputs";
  }
  
	protected function generate_node_output($count, &$query, $distinct = false){
		if ($count){
			$result = $this->count_nodes($query, $distinct)->execute();
			$data = $result->fetchAssoc();
		}else{
			$query->addField('node', 'title', 'label');
			$result = $this->find_nodes($query, $distinct)->execute();
			while ($d = $result->fetchAssoc())
				$data[] = $d;
		}
		return $data;
	}
	
	protected function generate_user_output($count, &$query, $distinct = false){
		if ($count){
			$result = $this->count_users($query, $distinct)->execute();
			$data = $result->fetchAssoc();//intval($result->fetchObject()->count);
		}else{
			$query->addField('users', 'name', 'label');
			$result = $this->find_users($query, $distinct)->execute();
			while ($d = $result->fetchAssoc())
				$data[] = $d;
		}
		return $data;
	}
	
//	protected function applyDateRange(&$query, $date_field, $period, $isrange = false){
//		if ($period){
//			$date_array = array();
//			$date_array[0] = date('Y-m-d H:i:s', $period[0]);
//			$date_array[1] = date('Y-m-d H:i:s', $period[1]);
//			if ($isrange){
//				//find overlapping date ranges...
//				$query->condition("date.{$date_field}_value", $date_array[1], '<=');
//				$db_or = db_or()
//							->condition("date.{$date_field}_value2", $date_array[0], '>=')
//							->isNull("date.{$date_field}_value2");
//				$query->condition($db_or);
//			}else{
//				$query->condition("date.{$date_field}_value", $date_array, 'BETWEEN');
//			}
//		}else{
//			$query->isNotNull("date.{$date_field}_value");
//		}
//	}
	
  private function generate_patent_data($count, $period){
		$data = array();
		foreach (array("Awarded"=>'field_er_patent_award_date', "Pending"=>'field_er_provisional_date', "Licensed"=>'field_er_patent_date') as $label=>$date_field){
			$query = db_select('node', 'node');
			$query->condition("node.type", 'er_patent', '=');
			$query->innerJoin("field_data_{$date_field}", 'date', 'node.nid = date.entity_id');
			$this->applyDateRange($query, $date_field, $period);
			$data[$label] = $this->generate_node_output($count, $query);
		}
		return $data;
  }
  
  private function generate_proposal_data($count, $period){
		$data = array();

		$status_check = array(
			"Submitted" => array("Submitted", "Awarded", "Expired", "Denied"),
			"Awarded"   => array("Awarded"),
			"Pending" 	=> array("Submitted")
		);

		foreach (array("Submitted"=>'field_er_proposal_submit', "Awarded"=>'field_er_proposal_date', "Pending"=>'field_er_proposal_submit') as $label=>$date_field){
			$query = db_select('node', 'node');
			$query->condition("node.type", 'er_proposal', '=');
			
			//we need to return the "funds requested" info:
			$query->innerJoin("field_data_field_er_award_requested_dec", 'amount', 'node.nid = amount.entity_id');
			if ($count)
				$query->addExpression('SUM(amount.field_er_award_requested_dec_value)', 'amount');
			else
				$query->addField('amount', 'field_er_award_requested_dec_value', 'amount');
			
			$query->innerJoin("field_data_{$date_field}", 'date', 'node.nid = date.entity_id');
			$this->applyDateRange($query, $date_field, $period, $label=="Awarded");//Note: Award date is a date range...

			// add status
			$query->innerJoin("field_data_field_er_proposal_status", 'status', 'node.nid = status.entity_id');
			$query->condition("status.field_er_proposal_status_value", $status_check[$label], 'IN');

			$data[$label] = $this->generate_node_output($count, $query);
		}
    return $data;
  }
  
  private function generate_pub_data($count, $period){
		$data = array();
		foreach (array("Primary", "Partial") as $key=>$label){
			$query = db_select('node', 'node');
			$query->condition("node.type", 'er_publication', '=');
			$date_field = 'field_er_publication_date';
			
			$query->innerJoin("field_data_field_er_rii_support", 'support', 'node.nid = support.entity_id');
			$query->condition("support.field_er_rii_support_value", $key, '=');
			
			$query->innerJoin("field_data_{$date_field}", 'date', 'node.nid = date.entity_id');
			$this->applyDateRange($query, $date_field, $period, $label=="Awarded");//Note: Award date is a date range...			
			$data[$label] = $this->generate_node_output($count, $query);
		}
    return $data;
  }
	
	//User queries:
  
	private function generate_fac_hired_query(&$query, $period){
		$date_field = self::$field_aliases['hired_date'];
		$query->innerJoin("field_data_{$date_field}", 'date', 'users.uid = date.entity_id');
		$this->applyDateRange($query, $date_field, $period);
		
		//Person must be faculty:
		$query->innerJoin("field_data_".self::$field_aliases['position_type'], 'type', 'users.uid = type.entity_id');
		$query->condition("type.".self::$field_aliases['position_type']."_value", "Faculty participant (or equivalent)", '=');
	}
	
	private function generate_post_docs_query(&$query, $period){
		$date_field = self::$field_aliases['participant_date'];
		$query->leftJoin("field_data_{$date_field}", 'date', 'users.uid = date.entity_id');
		$this->applyDateRange($query, $date_field, $period);
		
		$query->innerJoin("field_data_".self::$field_aliases['position_type'], 'type', 'users.uid = type.entity_id');
		$query->condition("type.".self::$field_aliases['position_type']."_value", "Post Doc", '=');
	}
	
private function generate_grad_students_query(&$query, $period){
		//TODO:
		//NEED TO WORK IN DOCTORATE GRADUATIONS SOMEHOW.
		//Slight modification by cjallen@epscor.unm.edu
	
		
		$date_field1 = self::$field_aliases['mas_act_grad_date'];
		$date_field2 = self::$field_aliases['doc_act_grad_date'];
		
		//Person must be gradstudent 
		$query->leftJoin("field_data_".self::$field_aliases['position_type'], 'type', 'users.uid = type.entity_id');
		$query->condition("type.".self::$field_aliases['position_type']."_value", "Graduate student", '=');
		
		$query->leftJoin("field_data_{$date_field1}", 'date1', 'users.uid = date1.entity_id');
		$query->leftJoin("field_data_{$date_field2}", 'date2', 'users.uid = date2.entity_id');
		//$this->applyDateRange($query, $date_field, $period);
			
		  if ($period){
           $date_array = array();
           $date_array[0] = date('Y-m-d H:i:s', $period[0]);
           $date_array[1] = date('Y-m-d H:i:s', $period[1]);
			
			$db_or = db_or()
				->condition("date1.{$date_field1}_value", $date_array , 'BETWEEN')
				->condition("date2.{$date_field2}_value", $date_array , 'BETWEEN');
			$query->condition($db_or);
		}else{
			$db_or = db_or()
				->isNotNull("date1.{$date_field1}_value")
				->isNotNull("date2.{$date_field2}_value");
			$query->condition($db_or);
		}
	}
	
	private function generate_undergrad_query(&$query, $period){
		//Slight modification by cjallen@epscor.unm.edu
			
		$date_field = self::$field_aliases['und_act_grad_date'];
		
		//Person must be undergrad
		$query->leftJoin("field_data_".self::$field_aliases['position_type'], 'type', 'users.uid = type.entity_id');
		$query->condition("type.".self::$field_aliases['position_type']."_value", "Undergraduate student", '=');
		
		$query->innerJoin("field_data_{$date_field}", 'date', 'users.uid = date.entity_id');
		$this->applyDateRange($query, $date_field, $period);
	}
	
	//Attribute functions:
	
	private function generate_male_query(&$query){
		$query->innerJoin("field_data_".self::$field_aliases['gender'], 'gender', 'users.uid = gender.entity_id');
		$query->condition("gender.".self::$field_aliases['gender']."_value", 0, '=');
	}
	
	private function generate_female_query(&$query){
		$query->innerJoin("field_data_".self::$field_aliases['gender'], 'gender', 'users.uid = gender.entity_id');
		$query->condition("gender.".self::$field_aliases['gender']."_value", 1, '=');
	}
	
	private function generate_urm_query(&$query){
		//This list could be appended:
		//Note: Perhaps we should have some simpler means of customizing this list. Perhaps we can define a custom hook or put a form element in the ER-core settings page.
		static $minorities = array("American Indian or Alaskan Native", "Black or African American", "Native Hawaiian", "Pacific Islander", "Filipino");
		$query->innerJoin("field_data_".self::$field_aliases['race'], 'race', 'users.uid = race.entity_id');
		$query->innerJoin("field_data_".self::$field_aliases['ethnicity'], 'ethnicity', 'users.uid = ethnicity.entity_id');
		$query->condition(db_or()
			->condition("race.".self::$field_aliases['race']."_value", $minorities, 'IN')
			->condition("ethnicity.".self::$field_aliases['ethnicity']."_value", 0, '=')//0 == person is hispanic
		);
	}
	
	private function generate_disabled_query(&$query){
		$query->innerJoin("field_data_".self::$field_aliases['disability'], 'disabilities', 'users.uid = disabilities.entity_id');
		$query->isNotNull("disabilities.".self::$field_aliases['disability']."_value");
		$query->condition("disabilities.".self::$field_aliases['disability']."_value", 'None', '!=');
		//$query->addField("disabilities", "field_er_disabilities_value");
	}
	
  private function generate_user_data($count, $period, $event_type){
		//note that this list has to match the array seen in the for loop in function generate_column_data.
		static $event_type_map = array(
			"Total New Faculty Hired"=>"generate_fac_hired_query",
			"Total Post Docs Involved"=>"generate_post_docs_query",
			"Total Graduate Students Graduated"=>"generate_grad_students_query",
			"Total Undergraduates Graduated"=>"generate_undergrad_query"
		);
		static $attribute_map = array(
			"Male" => "generate_male_query",
			"Female" => "generate_female_query",
			"Underrepresented minority" => "generate_urm_query",
			"Disabled" => "generate_disabled_query"
		);
		$data = array();
		$event_type_fn = $event_type_map[$event_type]; //get the name of the function to use.
		foreach ($attribute_map as $label=>$attribute_fn){
			$query = db_select('users', 'users');
			$this->$event_type_fn($query, $period);//perform the section specific query alterations + date range additions.
			$this->$attribute_fn($query);//perform the attribute specific query alterations (Male, Female, URM, Dis.)
			$data[$label] = $this->generate_user_output($count, $query, true);//use the $distinct flag: give me only distinct uids.
		}
		return $data;
  }
	
	private function generate_column_data($count, $date_range = array()){
		$data["Patents"] = $this->generate_patent_data($count, $date_range);
		
		$data["Proposals/Grants/Contracts"] = $this->generate_proposal_data($count, $date_range);
		
		$data["Published Publications"] = $this->generate_pub_data($count, $date_range);
		
		foreach (array("Total New Faculty Hired", "Total Post Docs Involved", "Total Graduate Students Graduated", "Total Undergraduates Graduated") as $type){			
			$data[$type] = $this->generate_user_data($count, $date_range, $type);
		}
		return $data;
	}
  
  public function data($count = true){
    $data = array();
    //$disabilities = $this->get_allowed_values("field_er_disabilities");
    //unset($disabilities['None']); // not interested in counting "None" as a disability
    
    // This is super hacky! Just hard-coding in the range of EPSCoR project, 8/1/14 - 7/31/19
    $cumulative_range = array(1406869200, 1564549200);

	$data["<hr/>Cumulative"] = $this->generate_column_data($count, $cumulative_range);
	if ($this->range)
		$data["<hr/>Reporting Period -" . $this->range_string()] = $this->generate_column_data($count, $this->range);
		
    return $data;
  }
	
  public function html(){
    if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
    $data = $this->data(false);
    if (isset($_GET['debug'])){
      d($this->data(), 'counts');
      d($data, 'full data');
    }
		//$data = array();
    $output = '';
		$output .= $this->download_link();
		$output = array(
			'link'=>array(
				'#type'=>'markup',
				'#markup'=>$this->download_link()
			)
		);
    foreach ($data as $range_label => $categories){
			$output[$range_label.'-label'] = $this->h1($range_label);
			foreach ($categories as $cat_key=>$cat_data){
				$output[$range_label . '-' . $cat_key . '-label'] = $this->h2($cat_key);
				foreach ($cat_data as $row_key=>$row_data){
					if (is_array($row_data)){
						$list = array(
							'#theme'=>'item_list',
							'#title'=>$row_key,
							'#type'=>'ol',
							'#items'=>array()
						);
						foreach ($row_data as $row){
							if (isset($row['amount'])){
								$list['#items'][] = l($row['label'], 'node/'.$row['nid']) . ' (' . $row['amount'] . ')';
							}else if (isset($row['nid'])){
								$list['#items'][] = l($row['label'], 'node/'.$row['nid']);
							}else if (isset($row['uid'])){
								$list['#items'][] = l($row['label'], 'user/'.$row['uid']);
							}
						}
						//$output[$cat_key . '-table'] = $table;
						$output["$range_label-$cat_key-$row_key-list"] = $list;
					}
				}
			}
		}
		return $output;
  }
  
  public function write_excel(&$objPHPExcel, $sheet = 0){
    $data = $this->data();
		//d($data, '$data');
    $worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
    $base_row = 4;
		$col = 3;
		foreach ($data as $range_label => $categories){
			$row = $base_row;
			foreach ($categories as $cat_key=>$cat_data){
				foreach ($cat_data as $row_key=>$row_data){
					if ($cat_key == "Proposals/Grants/Contracts"){//just need to treat the second one different
						$worksheet->setCellValueByColumnAndRow($col, $row, intval($row_data['count']));
						$worksheet->setCellValueByColumnAndRow($col+1, $row, intval($row_data['amount']));
					}else{
						$worksheet->setCellValueByColumnAndRow($col, $row, intval($row_data['count']));
					}
					$row++;
				}
				$row++;
			}
			$col -= 2;
		}
  }

  public function excel_file(){
    $objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Outputs-template.xls");
    return $objPHPExcel;
  }
  
}
