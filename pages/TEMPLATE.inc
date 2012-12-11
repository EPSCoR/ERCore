<?php
/*
 * Purpose:	This file serves as a starting place if you want to define a new er_page.
 * Usage:	Copy this file, and modify it in the following ways.
 * 			Rename the class to be of the form "er_[name of the file]"
 * 				Ex: "class er_summary extends er_page" in file summary.inc
 * 			You should start working on the data method, then take a look at the html method.
 * 			If this page is going to produce an excel sheet, take a look at the excel methods near the bottom.
 */
class er_TEMPLATE extends er_page{
	var $something;
	
	//You can initialize some stuff in here if you like.
	public function __construct() { 
        parent::__construct();
		$this->something = 'a value.';
    }
    
	/*
	 * Purpose:	This method should run whatever necessary queries, and produce some kind of data structure (most likely a nested array of values).
	 * Usage:	Run your queries in this function, 
	 * Returns:	Any data that you want to display to the user.
	 */
	public function data(){
		$data = array("test1", "test2");	
		return $data;
	}
	
	/*
	 * Purpose:	This method is called when a standard page request comes through. 
	 * Usage:	Simply return a string containing the final html output for the page.
	 * Returns:	String
	 */
	public function html(){
		$data = $this->data();
		d($data);
		$output = "From {$data[0]} to {$data[1]}. All is working fine.";
		$output .= $this->download_link(); //helper method produces a fancy download link.
		return $output;
	}
	
	/*
	 * Purpose:	This writes the data to an excel sheet using the PHPExcel library.
	 * Usage:	Use the methods found in the PHPExcel library to take the data and display it in the desired excel cells.
	 * 			When the download link is clicked, this method gets fired off, then the excel sheet is sent to the user.
	 * Returns:	Void
	 */
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
		$data = $this->data();
		$base_row = 4;
		
		/*
		 * Use $worksheet->setCellValue() to set cell values
		 */
		$worksheet->setCellValue("B".$row, $data[0]);
		
		$col = $this->itol(3);//itol = "integer to letter" converter
		$worksheet->setCellValue($col.$row, $data[1]);
		
		
		/*
		 * Lookup $worksheet->insertNewRowBefore() in order to add new rows to the table.
		 * I sometimes use this in order to preserve formatting in some template.
		 */
	}
	
	/*
	 * Purpose:	Define the template file used with this page.
	 * 			This is useful in times when you want to have the formatting of the excel sheet stay the same,
	 * 			and you just want to fill in the blanks.
	 * Usage:	If this method is not defined, a blank excel sheet will be used.
	 * 			Otherwise, replace the path to that of your excel template.
	 * Returns:	PHPExcel object
	 */
	//public function excel_file(){
	//	$objPHPExcel = PHPExcel_IOFactory::load($this->template_dir."TEMPLATE-template.xls");
	//	return $objPHPExcel;
	//}

}