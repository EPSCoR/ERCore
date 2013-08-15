<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
//I'm going to use this file to collect the other excel functions, so they all modify the same excel sheet.
//This system currently assumes that there are "pages" linked into the er module using the names stored in $sheets
class er_excel extends er_page{
	public $sheets = array("salary-support", "participants", "collaborations", "external-engagement", "outputs", "expenditures", "cost-sharing", "leveraged-support");	
	//private $data;
	
	public function __construct(){ 
		parent::__construct();
		//$data = array();
	}
	
	public function title(){
		return "EPSCoR Reporting Core Tables".$this->range_string();;
	}
    
	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Report-template.xls");
		return $objPHPExcel;
	}
    
	public function html(){
		$output = $this->download_link();
		return $output;
	}
	
	public function excel(){
		$objPHPExcel = $this->excel_file();
		foreach($this->sheets as $k=>$s){
			if (module_load_include('inc', 'er', 'pages/'.$s)!==false){
				$class = 'er_' . str_replace( "-" , "_" , $s );
				$class = new $class();
				$class->write_excel($objPHPExcel, $k);
			}
		}
		$objPHPExcel->setActiveSheetIndex(0);
		return $objPHPExcel;
	}

}