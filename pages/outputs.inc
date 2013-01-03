<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
class er_outputs extends er_page{
	private $reporting_range, $count;

	public function __construct(){
    parent::__construct();
			$r = $this->get_reporting_range();
			$this->reporting_range = array(date(DATE_ATOM, $r[0]), date(DATE_ATOM, $r[1]));
  }
    
  public function title(){
		return "Outputs";
	}
    
	private function generate_patent_query($period, $type){
		$p = new Table("content_type_er_patent", 'p');
		//$p	->field('field_er_patent_status_value', 'status')->where("status", $status);
		$p	->field($type[0], $type[1])->where($type[1], $type[2])
			->field('field_er_patent_date', 'date');
			
		$this->add_date_query($p, $period);
		$q = new Query($p);
		if (!$this->count){
			$p->select($type[1])->select('date')->field('nid')->select('nid');
			$n = new Table('{node}', 'n');
			$n->field('nid')->field('title')->select('title');
		$q->join($n);
		}
		if ($this->count)
			return intval($q->getObject()->count);
		else
			return $q->getData();
		
	}
	
	private function generate_cnp_query($period, $status){
		$p = new Table("content_type_er_proposal", 'p');
		$p	->field('field_er_award_amount', 'funds')
			->field('field_er_proposal_status', 'status')
			->where('status', $status)
			->field("field_er_proposal_date", 'date');
			
		$this->add_date_query($p, $period);
		$q = new Query($p, $count);
		if ($this->count){
			$p->select('funds', array('fn'=>'SUM'));
		}else{
			$p->select('funds')->select('status')->select('date')->field('nid')->select('nid');
			$n = new Table('{node}', 'n');
	 	    $n->field('nid')->field('title')->select('title');
			$q->join($n);
		}
		//return array('count'=>intval($data->count), 'funds'=>intval($data->funds));
		if ($this->count){
			$data = $q->getObject();
			return array('count'=>intval($data->count), 'funds'=>intval($data->funds));
		}
		else
			return $q->getData();
	}
	
	private function generate_pub_query($period, $support){
		$p = new Table("content_type_er_publication", 'p');
		$p	->field('field_er_rii_support', 'support')
			->where('support', $support)
			->field("field_er_publication_date", 'date');
		$this->add_date_query($p, $period);
		$q = new Query($p);
		if (!$this->count){
			$p->select('support')->select('date')->field('nid')->select('nid');
			$n = new Table('{node}', 'n');
			$n->field('nid')->field('title')->select('title');
			$q->join($n);
		}
		//return intval($q->getObject()->count);
		if ($this->count)
			return intval($q->getObject()->count);
		else
			return $q->getData();
	}
	//
	//private function generate_user_query($period, $event_type, $person_type, $s = NULL){
	//	static $minorities = array("American Indian or Alaskan Native", "Black or African American", "Native Hawaiian", "Pacific Islander", "Filipino");
	//	static $employed = array("Employed", "Employed – STEM job Hawaii", "Employed – STEM job non-Hawaii", "Employed – non-STEM job Hawaii", "Employed – non-STEM job non-Hawaii");
	//	$debug = false;
	//	$p = new Table("content_type_er_user_profile", 'p');
	//	$p	->field('nid');
	//	if ($person_type == "minorities"){
	//		$p->field("field_er_race", 'race')->field("field_er_ethnicity", 'ethnicity')->where(array(array('race', $minorities), array('ethnicity', "Hispanic or Latino")));
	//		if (!$this->count) $p->select('race');
	//	}else if ($person_type == "gender"){
	//		$p->field("field_er_gender", 'gender')->where('gender', $s);
	//		if (!$this->count) $p->select('gender');
	//	}else if ($person_type == "disabilities"){
	//		$p->field("field_er_disabilities", 'disabilities')->where('disabilities', null, array('op'=>'!='))->where('disabilities', 'None', array('op'=>'!='));
	//		if (!$this->count) $p->select('disabilities');
	//	}
	//	if ($event_type == "Employed"){
	//		$e = new Table("content_type_er_user_event", 'e');
	//		$e	->field("field_er_event_user", 'uid')
	//			->field("field_er_user_event_type", 'event_type')
	//			->field("field_er_user_event_date", 'date')
	//			->where('event_type', $employed);
	//		$p	->field("field_er_involvement", 'involvement')->where('involvement', 'Faculty participant (or equivalent)');
	//		if (!$this->count) {
	//			$p->select('involvement');
	//			$e->select('date2');
	//		}
	//		$debug = true;
	//	}else{
	//		$e = new Table("content_type_er_user_event_instance", 'e');
	//		$e	->field("field_er_event_inst_user", 'uid')
	//			->field("field_er_user_event_inst_type", 'event_type')
	//			->field("field_er_user_event_inst_date", 'date')
	//			->where('event_type', $event_type);
	//	}
	//	
	//	
	//	$this->add_date_query($e, $period);
	//	$n = new Table("{node}", 'n');
	//	$n	->field('nid')->field('uid')->group('uid');
	//	$q = new Query($p);
	//	$q->join($n)->join($e, $n, 'uid');
	//	if (!$this->count){
	//		$e->select('date')->select('uid')->select('event_type')->field('nid')->select('nid');
	//		//$n->field('title')->select('title');
	//		$p	->field('field_er_lname', 'lname')->field('field_er_fname', 'fname')
	//			->select('lname')->select('fname')
	//			->order('lname');
	//	}
	//	//return intval($q->getObject($debug)->count);
	//	if ($this->count)
	//		return intval($q->getObject()->count);
	//	else
	//		return $q->getData();
	//}
	//
	//private function add_date_query($table, $period){
	//	if ($period == 0)
	//		$table->where('date', $this->reporting_range[0], array('op'=>'>=', 'fn2'=>'date'))->where('date', $this->reporting_range[1], array('op'=>'<=', 'fn2'=>'date'));
	//}
	
	public function data($count = true){
		$this->count = $count;
		$data = array();
		//$reporting_range = $this->get_reporting_range();
		//$disabilities = $this->get_allowed_values("field_er_disabilities");
		//unset($disabilities['None']); // not interested in counting "None" as a disability
		
		
		foreach (array("Reporting Period", "Cumulative") as $period=>$label){//used for reporting period/cumulative
			//if ($x)	$period = $reporting_range;
			//else	$period = array();
			
			//foreach (array("Awarded", "Pending") as $s)
			//	$data["Patents"][$s][$label] = $this->generate_patent_query($period, array('field_er_patent_status', 'status', $s));
			//$data["Patents"]['Licensed'][$label] = $this->generate_patent_query($period, array('field_er_patent_licensed', 'licensed', 1));
			
			/*
			foreach (array("Submitted", "Awarded", "Pending", "Expired", "Denied") as $s)
				$data["Proposals/Grants/Contracts"][$s][$label] = $this->generate_cnp_query($period, $s);
			
			foreach (array("Primary", "Partial") as $s)
				$data["Published Publications"][$s][$label] = $this->generate_pub_query($period, $s);
			
			$types = array(
				"Employed"=>"Total New Faculty Hired",
				"Undergraduate Graduation"=>"Total Post Docs Completed",
				"Masters Graduation"=>"Total Graduate Students Graduated",
				"Doctorate Graduation"=>"Total Undergraduates Graduated"
			);
			foreach ($types as $type=>$name){
				foreach (array("Male", "Female") as $s)
					$data[$name][$s][$label] = $this->generate_user_query($period, $type, 'gender', $s);
				
				$data[$name]['Underrepresented minority'][$label] = $this->generate_user_query($period, $type, 'minorities');
				
				//$data[$name]['Underrepresented minority'][$label] += $this->generate_user_query($period, $type, array("field_er_ethnicity", 'ethnicity', "Hispanic or Latino"));
				//Need to integrate "Hispanic or Latino" selection?
				
				$data[$name]['Disabled'][$label] = $this->generate_user_query($period, $type, 'disabilities');
			}*/
		}
		return $data;
	}
	
	public function html(){
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		d($this->data(), 'counts');
		$data = $this->data(false);
		d($data, 'data');
		$output = '';
		foreach ($data as $content_type=>$content){
			$output .= '<h1>'.$content_type.'</h1>';
			foreach ($content as $subtype => $periods){
				$output .= '<h2>'.$subtype.'</h1>';
				foreach ($periods as $period=>$period_data){
					$output .= '<h3>'.$period.'</h3>';
					$output .= '<ol>';
					foreach ($period_data as $k=>$dat){
						$output .= '<li>';
						$output .= l($dat['title']?$dat['title']:$dat['lname'].', '.$dat['fname'], 'node/'.$dat['nid']) . '<br>';
						foreach ($dat as $k=>$v){
							if ($k != 'title' && $k != 'nid' && $k != 'lname' && $k != 'fname')
								$output .= "$k: $v<br>";
						}
						$output .= '</li>';
					}
					$output .= '</ol>';
				}
			}
		}
		$output .= $this->download_link();
		return $output;
	}
	
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$data = $this->data();
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
		$row = 4;
		foreach ($data as $cat_key => $category){
			foreach ($category as $row_key=>$row_data){
				if ($cat_key == "Proposals/Grants/Contracts"){//just need to treat the second one different
					$worksheet->setCellValue("B".$row, $row_data['Reporting Period']['count']);
					$worksheet->setCellValue("C".$row, $row_data['Reporting Period']['funds']);
					$worksheet->setCellValue("D".$row, $row_data['Cumulative']['count']);
					$worksheet->setCellValue("E".$row, $row_data['Cumulative']['funds']);
				}else{
					$worksheet->setCellValue("B".$row, $row_data['Reporting Period']);
					$worksheet->setCellValue("D".$row, $row_data['Cumulative']);
				}
				$row++;
			}
			$row++;
		}
	}

	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Outputs-template.xls");
		return $objPHPExcel;
	}
	
}