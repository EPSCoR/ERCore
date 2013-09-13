<?php
class er_page{
	protected $content_type_info;
	protected $template_dir;
	protected $range;
	protected $date_format = 'M j, Y';
	
	protected static $field_aliases = array(
		'position_type'=>'field_er_most_sen_proj_role',
		'fname'=>'field_er_fname',
		'lname'=>'field_er_lname',
		'department'=>'field_er_department',
		'inst_abbr'=>'field_er_inst_abbreviation',
		'inst_ref'=>'field_er_inst_ref',
		'160hours'=>'field_er_160hours',
		'participating'=>'field_er_inst_participating',
		'committee'=>'field_er_committees',
		'gender'=>'field_er_gender',
		'race'=>'field_er_race',
		'ethnicity'=>'field_er_ethnicity',
		'disability'=>'field_er_disabilities',
		'participant_date'=>'field_er_participating_date',
		'hired_date'=>'field_er_hired_date',
		'leadership_team'=>'field_er_leadership_team',
		'mas_act_grad_date'=>'field_er_act_masters',
		'doc_act_grad_date'=>'field_er_act_doctor',
		'und_act_grad_date'=>'field_er_act_under',
		'collab_inst_ref' => 'field_er_collab_inst_ref',
		'collab_department_unit' => 'field_er_collab_department_unit',
		'inst_category' => 'field_er_inst_category',
		'inst_type' => 'field_er_inst_type',
		'collab_user_status' => 'field_er_collab_user_status',
		'collab_name' => 'field_er_collab_name',
		'collab_ref' => 'field_er_collab_ref',
	);
	
	public function __construct() {
		$this->range = er_get_reporting_range();
		//$this->template_dir = drupal_get_path('module', 'er') . '/templates/';
	}
	
	/*
	 * These are the primary methods that you can overload in child classes.
	 */
	
	public function title(){
		return "Reporting".$this->range_string();
	}
	
	public function range_string(){
		return ' ['.date($this->date_format, $this->range[0]).' - '.date($this->date_format, $this->range[1]).']';
	}
	
	public function data(){
		return array();
	}
	
	public function html(){
		$data = $this->data();
		d($data);
		return "";
	}
	
	/*
	 * This function returns a PHPExcel object. 
	 * See: PHPExcel library
	 */
	public function excel(){
		$objPHPExcel = $this->excel_file();
		$this->write_excel($objPHPExcel);
		$objPHPExcel->setActiveSheetIndex(0);
		return $objPHPExcel;
	}
	
	/*
	 * This function writes the data into an excel object.
	 */
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$data = $this->data();
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
	}
	
	/*
	 * This function returns a PHPExcel object. 
	 * See: PHPExcel library
	 */
	public function excel_file(){
		//$objPHPExcel = PHPExcel_IOFactory::load($this->template_dir."Report-template.xls");
		
		//reference: http://stackoverflow.com/questions/3269345/how-to-generate-an-excel-document-with-multiple-worksheets-from-php
		$objPHPExcel = new PHPExcel();
		return $objPHPExcel;
	}

	/*
	 * HELPER METHODS:
	 */
	
	/*
	 * takes a column number (zero-based) and converts it into a string of letters
	 * to be used in setting values in excel sheets.
	 * ex:	0 -> A
	 * 		1 -> B
	 * ...	26 -> AA
	 * 		27 -> AB
	 */ 
	public function itol($num){
		$ret = '';
		for(;;){
			$ret = chr(ord('A')+($num%26)) . $ret;
			if ($num>=26)
				$num = $num/26 - 1;
			else
				break;
		}
		return $ret;
	}
	
	protected function download_link($title = "", $arg = "", $options = array()){
		return er_create_download_link($title, $arg, $options);
	}
	
	/*
	 * Gets allowed values for a cck field provided the name of the field.
	 * See: .../reporting/debug
	 */
	protected function get_allowed_values($field){
		$all_fields = field_info_fields();
		return list_allowed_values($all_fields[$field]);
	}
  
//  protected function current_reporting_year(){
//		return intval(date('n')) < 3 ? intval(date('Y'))-1 : intval(date('Y'));
//	}
//	
//	/*
//	 * Using this in the summary tables, it outputs the reporting year ranges as a date/time stamp.
//	 * This is used to fake the reporting year field for calendar events...
//	 */
//	protected function get_reporting_range($y = null){
//		if ($y == null)
//			$y = $this->current_reporting_year();
//		if ($y == 2009)
//			$date_lower = mktime(0, 0, 0, 9, 15, $y);
//		else
//			$date_lower = mktime(0, 0, 0, 4, 1, $y);
//		$date_upper = mktime(0, 0, 0, 3, 31, $y+1);
//		return array($date_lower, $date_upper);
//	}
	
	//These functions are used in participants.inc or outputs.inc
	//They are used in several places. They are used to generate content (queries) for excel sheets.
	
	public function er_get_realname($uid, $link = true){
		static $names = array();
		if (!array_key_exists($uid, $names)){
			$query = db_select('users', 'users');
			$query->condition('users.uid', $uid, '=');
			$query->leftJoin('field_data_field_er_realname', 'rn', 'rn.entity_id = users.uid');
			$query->addField('rn', 'field_er_realname_value', 'realname');
			$query->addField('users', 'name', 'name');
			$result = $query->execute()->fetchAssoc();
			if (isset($result['realname']) && $result['realname'] != ' ' && $result['realname'] != '')
				$names[$uid] = $result['realname'];
			else
				$names[$uid] = $result['name'];
		}
		//d($names);
		return l($names[$uid], 'user/'.$uid);
	}
	
	protected function count_nodes(&$query, $distinct = false){
		$query->addExpression('COUNT('.($distinct?'DISTINCT ':'').'node.nid)', 'count');
		return $query;
	}
	
	protected function count_users(&$query, $distinct = false){
		$query->addExpression('COUNT('.($distinct?'DISTINCT ':'').'users.uid)', 'count');
		return $query;
	}
	
	protected function find_nodes(&$query, $distinct = false){
		if ($distinct)//This will remove duplicates from the display.
			$query->groupBy('node.nid');
		$query->addField('node', 'nid');
		return $query;
	}
	
	protected function find_users(&$query, $distinct = false){
		if ($distinct)//This will remove duplicates from the display.
			$query->groupBy('users.uid');
		$query->addField('users', 'uid');
		return $query;
	}
	
	protected function markup($text, $options){
		return array_merge(array(
			'#type'=>"markup",
			'#markup'=>$text,
		), $options);
	}
	
	protected function h1($text){
		return $this->markup($text, array('#prefix'=>'<h1>', '#suffix'=>'</h1>'));
	}
	protected function h2($text){
		return $this->markup($text, array('#prefix'=>'<h2>', '#suffix'=>'</h2>'));
	}
	protected function h3($text){
		return $this->markup($text, array('#prefix'=>'<h3>', '#suffix'=>'</h3>'));
	}
	protected function h4($text){
		return $this->markup($text, array('#prefix'=>'<h4>', '#suffix'=>'</h4>'));
	}
}