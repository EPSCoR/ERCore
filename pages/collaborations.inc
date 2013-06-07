<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
//I'm going to use this file to collect the other excel functions, so they all modify the same excel sheet.
//This system currently assumes that there are "pages" linked into the er module using the names stored in $sheets
class er_collaborations extends er_page{
	static private	$types = array(
		'local'=>'Jurisdictional',
		'domestic'=>'External-Domestic',
		'foreign'=>'External-Foreign',
		'total'=>'Total'
	);
	static private $categories = array(
		'academic'=>'Academic Research Institutions',
		'undergrad'=>'Primarily Undergraduate Institutions',
		'black'=>'Historically Black Colleges and Universities',
		'hispanic'=>'Hispanic Serving Institutions',
		'tribal'=>'Tribal Colleges and Universities',
		'lab'=>'National Laboratories',
		'industry'=>'Industry',
		'none'=>'Other',
		'total'=>'Total'
	);
	
	public function __construct(){ 
			parent::__construct();
	}
	
	public function title(){
		return "Collaborations";
	}	
	
	public function data($count = true){
		$data = array();
		
		$display = $count?'count':'default';
		foreach (self::$categories as $category=>$cat_label){
			foreach (self::$types as $type=>$type_label){
				$view = views_get_view('er_report_collaborations');
				$view->set_display($display);
				//dsm($cat_label);
				
				if ($category != 'total'){
					$view->exposed_input['category']['value'] = $category;
				}
				if ($type != 'total'){
					$view->exposed_input['type']['value'] = $type;
				}
				
				//if ($type_label == 'Total' && $cat_label == 'Academic Research Institutions')
				//	dsm($view);
				
				$view->execute();
				$results = $view->result;
				//unset($view);
				//dsm($view, "$type - $category");
				//dsm($result, '$result');
				if ($count){
					//dsm($view, "$type - $category");
					//dsm($results);
					if (count($results)){
						//d($results, '$results');
						$data[$category][$type]["Institutions"] = $results[0]->field_data_field_er_collab_inst_ref_field_er_collab_inst_ref;
						$data[$category][$type]["Collaborators"] = $results[0]->field_data_field_er_collab_names_field_er_collab_names_value;
					}
				}else{
					//dsm($results, "$type - $category");
					foreach ($results as $result){
						$nid = $result->node_field_data_field_er_collab_ref_nid;
						//this is necessary because it would show the institutions once for each person on the collab, which isn't correct.
						if (!isset($data[$category][$type]["Institutions"][$nid])){
							//d($result, '$result');
							$data[$category][$type]["Institutions"][$nid] = array(
								"title"=>$result->node_field_data_field_er_collab_ref_title ,
								"inst_title"=>$result->node_field_data_field_er_collab_inst_ref_title,
								"inst_type"=>$this->tryget($result, 'field_field_er_inst_type', 'undefined'),
								"inst_category"=>$this->tryget($result, 'field_field_er_inst_category', 'undefined'),
								"inst_nid"=>$result->node_field_data_field_er_collab_inst_ref_nid,
								"nid"=>$nid,
							);
						}
						$data[$category][$type]["Collaborators"][] = array(
							"title"=>$result->node_field_data_field_er_collab_ref_title,
							"inst_title"=>$result->node_field_data_field_er_collab_inst_ref_title,
							"names"=>$this->tryget($result, 'field_field_er_collab_names', 'None'),
							"nid"=>$nid,
						);
					}
				}
			}
		}
		//dsm(memory_get_usage(true));
		return $data;
	}
	
	private function tryget(&$result, $field, $default){
		if (isset($result->$field) && count($result->$field) > 0){
			$value = array_pop($result->$field);
			if (isset($value['raw']['safe_value']))
				return $value['raw']['safe_value'];
			else
				return $value['raw']['value'];
		}else
			return $default;
	}
	
	public function html(){
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		d($this->data(), 'counts');
		$data = $this->data(false);
		d($data, 'full data');
		$ret = $this->download_link();
		foreach ($data as $category => $cat_data){
			$ret .= "<h1>".self::$categories[$category]."</h1>";
			foreach($cat_data as $inst_type=>$type_data){
				if (count($type_data) > 0){
					$ret .= "<h3>".self::$types[$inst_type]."</h3>";
					foreach($type_data as $count_type=>$values){
						//d($count_type, 'count_type');
						if (count($values) > 0){
							$ret .= '<div style="float:left;margin-right:3em;">';
							$ret .= "<h4>$count_type</h4>";
							$ret .= "<ol>";
							foreach($values as $k=>$v){
								$ret .= "<li>";
								if ($count_type == 'Collaborators'){
									$ret .= "<strong>".$v['names']."</strong><br>"
												 ."Institution: ".l($v['inst_title'], 'node/'.$v['inst_nid'])."<br>";
								}else{
									$ret .= "<strong>".l($v['inst_title'], 'node/'.$v['inst_nid'])."</strong><br>";
										//."Type: ".$v['inst_type']."<br>"
										//."Category: ".$v['inst_category']."<br>"
								}
								$ret .= "Collaboration Title: ".l($v['title'], 'node/'.$v['nid'])."</li>";
								//d($v, '$v');
							}
							$ret .= "</ol>";
							$ret .= '</div>';
						}
					}
					$ret .= '<div style="clear: both;"></div>';
				}
			}			
		}
		return $ret;
	}
	
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$data = $this->data();
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
		$base_row = 5;
		$row = $base_row;
		foreach ($data as $category => $temp){
			//Jurisdictional
			$worksheet->setCellValue('B'.$row, $temp['local']["Institutions"]);
			$worksheet->setCellValue('C'.$row, $temp['local']["Collaborators"]);
			//External-Domestic
			$worksheet->setCellValue('D'.$row, $temp['domestic']["Institutions"]);
			$worksheet->setCellValue('E'.$row, $temp['domestic']["Collaborators"]);
			//External-Foreign
			$worksheet->setCellValue('F'.$row, $temp['foreign']["Institutions"]);
			$worksheet->setCellValue('G'.$row, $temp['foreign']["Collaborators"]);
			$row++;
		}
	}
	
	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Collaborations-template.xls");
		return $objPHPExcel;
	}

}