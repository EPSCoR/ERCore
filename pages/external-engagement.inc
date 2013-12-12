<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
class er_external_engagement extends er_page{
	private $cols = array("ari_fac", "ari_stu", "pui_fac", "pui_stu", "msi_fac", "msi_stu", "k12i_tec", "k12i_stud", "k12i_stut", "oth", "tot");
	private $rows = array("total", "male", "female", "urm");
	private $fields = array("ari_fac_total","ari_stu_total","pui_fac_total","pui_stu_total","msi_fac_total","msi_stu_total","k12i_tec_total","k12i_stud_total","k12i_stut_total","oth_total","tot_total","ari_fac_male","ari_stu_male","pui_fac_male","pui_stu_male","msi_fac_male","msi_stu_male","k12i_tec_male","k12i_stud_male","k12i_stut_male","oth_male","tot_male","ari_fac_female","ari_stu_female","pui_fac_female","pui_stu_female","msi_fac_female","msi_stu_female","k12i_tec_female","k12i_stud_female","k12i_stut_female","oth_female","tot_female","ari_fac_urm","ari_stu_urm","pui_fac_urm","pui_stu_urm","msi_fac_urm","msi_stu_urm","k12i_tec_urm","k12i_stud_urm","k12i_stut_urm","oth_urm","tot_urm");

	public function __construct() { 
		parent::__construct();
		//$this->excel_file = "Participants-template.xls";
	}
	
	public function title(){
		return "External Engagement";
	}
  
	public function data($count = true){
		$query = db_select('node', 'ee');
		$query->condition('ee.type', 'er_engagement');
		
		if (!$count){
			$query->addField('ee', "nid");
			$query->addField('ee', "title");
		}
		
		foreach ($this->fields as $f){
			$field = "field_er_".$f;
			$query->leftJoin("field_data_{$field}", $field, "ee.nid = $field.entity_id");
			if ($count){
				$query->addExpression("SUM($field.{$field}_value)", $f);
			}else{
				$query->addField($field, "{$field}_value)", $f);
			}
		}
    
    $data = array();
    $result = $query->execute();
		if ($count){
			$data = $result->fetchAssoc();
		}else{
			while ($record = $result->fetchAssoc()){
				$data[] = $record;
			}
		}
    return $data;
  }
	
	public function html(){
		$attr_names = array('total'=>'Total', 'male'=>'Male', 'female'=>'Female', 'urm'=>'Under-represented Minority');
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		$data = $this->data(false);
    if (isset($_GET['debug'])){
      d($this->data(), 'counts');
      d($data, 'full data');
    }
		$ret = $this->download_link() . '<br>';
		$ret .=
			 "This is a view meant only for admin. This data corresponds to the counts seen on the accomplishments table. Scroll to the  bottom to download the excel sheet. Mouse over the node id (the number in blue) to see the title of the node.<br>"
			."For the sake of bevity, the headings display the names used internally by the system."
			."<h3>Prefixes:</h3>"
			."<ul>"
				."<li>ari = 'Academic Research Institutions'</li>"
				."<li>pui = 'Primary Undergraduate Institutions'</li>"
				."<li>msi = 'Minority Serving Institutions'</li>"
				."<li>k12i = 'K-12 Institutions'</li>"
				."<li>oth = 'Other'</li>"
				."<li>tot = 'Total'</li>"
			."</ul>"
			."<h3>Postfixes:</h3>"
			."<ul>"
				."<li>fac = 'Faculty'</li>"
				."<li>stu = 'Students'</li>"
				."<li>tec = 'Teachers'</li>"
				."<li>stud = 'Students Reached Directly'</li>"
				."<li>stut = 'Students Reached via Teach. Training'</li>"
			."</ul>";
		foreach ($this->rows as $attr){
			$ret .= "<h1>".$attr_names[$attr]."</h1>";
			$ret .= "<table>";
			//$ret .= "<tr colspan=\"11\"><td>".$attr."</td></tr>";
			//$ret .= "<td></td>";
			$ret .= "<tr>";
			$ret .= "<th>Edit</th>";
			foreach ($this->cols as $inst){
				$ret .= "<th>$inst</th>";
			}
			$ret .= "</tr>";
			foreach ($data as $k=>$ee){
				$ret .= '<tr class="'.($k%2?'odd':'even').'" alt="'.$ee['title'].'" title="'.$ee['title'].'">';
				$ret .= "<td>".l($ee['nid'], 'node/'.$ee['nid'], array('attributes'=>array('alt'=>$ee['title'], 'title'=>$ee['title'])))."</td>";
				foreach ($this->cols as $inst){
					$ret .= "<td>".$ee[$inst.'_'.$attr]."</td>";
				}
				$ret .= "</tr>";
			}
			$ret .= "</table>";
		}
		return $ret;
	}
	
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$data = $this->data();
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
		$base_row = 5;
		$columns = 11;
		$x = 0;
		foreach ($data as $d){
			$column = $x%$columns;
			$column_letter = chr(ord('B')+$column);
			$row = $base_row + intval($x/$columns);
			//var_dump($column_letter.$row);
			$worksheet->setCellValue($column_letter.$row, $d);
			$x++;
		}
	}

	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/External-Engagement-template.xls");
		return $objPHPExcel;
	}
	
}