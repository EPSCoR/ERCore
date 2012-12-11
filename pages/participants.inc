<?php
class er_participants extends er_page{
	private $types = array(
		array("male","field_er_gender","Male"),
		array("female","field_er_gender","Female"),
		array("black","field_er_race","black or African American"),
		array("hispanic","field_er_ethnicity","hispanic or Latino"),
		array("ethnic","field_er_race",array('American Indian or Alaskan Native', 'Native Hawaiian', 'Pacific Islander')),
		array("disabled","field_er_disabilities",array("Hearing Impairment", "Visual Impairment", "Mobility/Orthopedic Impairment", "Other"))
	);
	private $institutions;

	public function __construct() { 
		parent::__construct();
	}
	
	public function title(){
		return "Participants";
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
	
	public function data($count = true){
		$data = array();
		$total = array();
		$advisory = array();
		// + means OR according to view's arguments setup...
		$other_ethnic = implode('+', array('American Indian or Alaskan Native', 'Native Hawaiian', 'Pacific Islander'));
		$disabilities = implode('+', array("Hearing Impairment", "Visual Impairment", "Mobility/Orthopedic Impairment", "Other"));
		$types = array(
			'total'=>'',
			'male'=>array(0, '0'),//(arguement index, expected value)
			'female'=>array(0, '1'),
			'black'=>array(1, 'Black or African American'),
			'hispanic'=>array(2, '0'),
			'ethnic'=>array(1, $other_ethnic),
			'disabled'=>array(3, $disabilities)
		);
		/*
		$types = array(
			'total'=>'',
			'male'=>array('field_er_gender_value', '0'),//(arguement index, expected value)
			'female'=>array('field_er_gender_value', '1'),
			'black'=>array('field_er_race_value', 'Black or African American'),
			'hispanic'=>array('field_er_ethnicity_value', '0'),
			'ethnic'=>array('field_er_race_value', $other_ethnic),
			'disabled'=>array('field_er_disabilities_value', $disabilities)
		);
		*/
		
		foreach ($types as $type=>$type_val){
			$view = views_get_view('er_report_participants');
			$view->set_display($count?'count':'default');
			if (is_array($type_val)){
				//note: you can change "all" to something else in the views arguements setup under "exception"
				$args = array('all', 'all', 'all', 'all');//gender, race, ethnicity, disabilities
				$args[$type_val[0]] = $type_val[1];
				$view->set_arguments($args);
			}
			$view->execute();
			//if ($type == 'male' || $type == 'female')
			//	dsm($view->result, 'result');
			//dsm($view->argument, $type);
			//dsm($view->argument['field_er_gender_value'], $type . '- gender');
			foreach ($view->result as $result){
				$institution = $this->tryget($result, 'field_field_er_inst_name', '');
				$involvement = $this->tryget($result, 'field_field_er_involvement', 'Unknown');
				if ($count){
					$data[$institution][$involvement][$type] = $result->uid_1;
					if (isset($total[$involvement][$type]))
						$total[$involvement][$type] += $result->uid_1;
					else
						$total[$involvement][$type] = 1;
				}else{
					$data[$institution][$involvement][$type][] = array('name'=>$this->tryget($result, 'field_field_er_realname', 'BLANK'), 'uid'=>$result->uid);
				}
			}
			
			//do advisory board stuff:
			$view = views_get_view('er_report_participants');
			$view->set_display('advisory'.($count?'_count':''));
			if (is_array($type_val)){
				//note: you can change "all" to something else in the views arguements setup under "exception"
				$args = array('all', 'all', 'all', 'all');//gender, race, ethnicity, disabilities
				$args[$type_val[0]] = $type_val[1];
				$view->set_arguments($args);
			}
			$view->execute();
			//dsm($view->result, 'advisory board mofos');
			$user_count = 0;
			foreach ($view->result as $result){
				if ($count){
					$user_count+= $result->uid;
				}else{
					//dsm($result, 'result');
					//$teams = array();
					//foreach ($result->field_field_er_teams as $n){
					//	$teams[] = $n['rendered']['#markup'];
					//}
					//$teams = implode(', ', $teams);
					$committees = array();
					foreach ($result->field_field_er_committees as $n){
						$committees[] = $n['rendered']['#markup'];
					}
					$committees = implode(', ', $committees);
					$advisory[$type][] = array('name'=>$this->tryget($result, 'field_field_er_realname', 'BLANK'), 'uid'=>$result->uid, 'committees'=>$committees);
				}
			}
			$advisory[$type] = $user_count;
			//
			//if ($count){
			//	$advisory[$type] = 0;
			//}else{
			//	$advisory[$type][] = array('name'=>'test', 'uid'=>0);
			//}
		}
		if ($count)
			$data['RII total'] = $total;
		$data['Any']['Advisory Board(s)'] = $advisory;
		return $data;
	}
	
	public function html(){
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		
		d($this->data(), 'count');
		$data = $this->data(false);
		d($data, 'data');
		//d($this->data2(), 'old data');
		function link_map($n){
			return l($n['name'], 'user/'.$n['uid'].'/edit');
		}
		$ret = $this->download_link();
		foreach ($data as $inst => $categories){
			if ($inst != "Any"){
				$ret .= "<h1>$inst</h1>";
				foreach ($categories as $cat => $count){
					$ret .= "<h3>$cat</h3>";
					$ret .= "<ul>";
					foreach ($count as $type=>$v){
						if (count($v)){
							$s = implode(', ', array_map("link_map", $v));
							$ret .= "<li>$type: ".$s."</li>";
						}
					}
					$ret .= "</ul>";
				}
			}else{//handle the advisory board differently
				$ret .= "<h1>Advisory Board(s)</h1>";
				$ret .= "<ul>";
				foreach ($categories['Advisory Board(s)'] as $type=>$v){
					if (count($v)){
						$ret .= "<li>$type:";
						$ret .= "<ul>";
						foreach ($v as $person){
							$ret .= "<li>".l($person['name'], 'user/'.$person['uid'].'/edit')."<br>Committees: ".$person['committees']."</li>";
						}
						$ret .= "</ul></li>";
					}
				}
				$ret .= "</ul>";
			}
		}
		
		return $ret;
	}
	
	public function microtime_float(){
		list($usec, $sec) = explode(" ", microtime());
		return ((float)$usec + (float)$sec);
	}
	
	public function time_this($fn){
		$t1 = microtime();
		$data = $fn();
		$t2 = microtime()-$t1;
		d($t2, "ms elapsed");
		return $data;
	}
	
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
		$data = $this->data();
		$base_row = 4;
		//if (count($data) > 2)
		//	$worksheet->insertNewRowBefore($base_row+1,count($data["RII total"])*(count($data)-1)-2); //add in a few more lines
		
		$row = $base_row;
		foreach ($data as $inst => $categories){
			if ($inst == "Any"){
				$worksheet->removeRow($row,2);
			}else{
				$worksheet->setCellValue('A'.$row, $inst);//institution label
			}
			foreach ($categories as $cat => $count){
				if ($inst != "Any")
					$worksheet->insertNewRowBefore($row+1,1); //add in a few more lines
				$worksheet->setCellValue('B'.$row, $cat);//category label
				$worksheet->setCellValue('C'.$row, get($count, 'total', 0));//get function defined in er.module
				$worksheet->setCellValue('D'.$row, get($count, 'male', 0));
				$worksheet->setCellValue('E'.$row, get($count, 'female', 0));
				$worksheet->setCellValue('F'.$row, get($count, 'black', 0));
				$worksheet->setCellValue('G'.$row, get($count, 'hispanic', 0));
				$worksheet->setCellValue('H'.$row, get($count, 'ethnic', 0));
				$worksheet->setCellValue('I'.$row, get($count, 'disabled', 0));
				$row++;
			}
		}
	}

	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/templates/Participants-template.xls");
		return $objPHPExcel;
	}
	
}