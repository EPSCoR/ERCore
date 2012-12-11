<?php
class er_page{
	protected $content_type_info;
	protected $template_dir;
	
	public function __construct() {
		//$this->template_dir = drupal_get_path('module', 'er') . '/templates/';
	}
	
	/*
	 * These are the primary methods that you can overload in child classes.
	 */
	
	public function title(){
		return "Reporting";
	}
	
	public function data(){
		return "No output.";
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
		$options += array(
			'html'=>true,
			'attributes' => array('title' => 'Download','class' => 'downloadButton')
		);
		if (!$arg) $arg = arg(1);
		if (!$title) $title = "Download this page";
		return l('<i class="icon-download-alt"></i>'.t($title), ER_ROOT_NAME."/$arg/download", $options);
	}
	
	/*
	 * Gets allowed values for a cck field provided the name of the field.
	 * See: .../reporting/debug
	 */
	protected function get_allowed_values($field){
		$all_fields = field_info_fields();
		return list_allowed_values($all_fields[$field]);
	}
  
  protected function current_reporting_year(){
		return intval(date('n')) < 3 ? intval(date('Y'))-1 : intval(date('Y'));
	}
	
	/*
	 * Using this in the summary tables, it outputs the reporting year ranges as a date/time stamp.
	 * This is used to fake the reporting year field for calendar events...
	 */
	protected function get_reporting_range($y = null){
		if ($y == null)
			$y = $this->current_reporting_year();
		if ($y == 2009)
			$date_lower = mktime(0, 0, 0, 9, 15, $y);
		else
			$date_lower = mktime(0, 0, 0, 4, 1, $y);
		$date_upper = mktime(0, 0, 0, 3, 31, $y+1);
		return array($date_lower, $date_upper);
	}
}