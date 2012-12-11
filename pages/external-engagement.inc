<?php
class er_external_engagement extends er_page{
	private $cols = array("ari_fac", "ari_stu", "pui_fac", "pui_stu", "msi_fac", "msi_stu", "k12i_tec", "k12i_stud", "k12i_stut", "oth", "tot");
	private $rows = array("total", "male", "female", "und", "urm");

	public function __construct() { 
		parent::__construct();
		//$this->excel_file = "Participants-template.xls";
	}
	
	public function title(){
		return "External Engagement";
	}
    
	public function data($count = true){
		$view = views_get_view('er_report_external_engagement');
		$view->set_display($count?'count':'default');
		$view->execute();
		$data = array();
		foreach ($view->result as $k=>$v){
			if (!$count){
				$data[$k]['nid'] = $view->result[$k]->nid;
				$data[$k]['title'] = $view->result[$k]->node_title;
			}
			foreach ($this->rows as $attr){
				foreach ($this->cols as $inst){ //notice it's field_field_er...
					$name = $inst.'_'.$attr;
					//dsm($view->result[0]->{'field_field_er_'.$name}[0]['raw']['value'], $name);
					if (!$count)
						$data[$k][$name] = $view->result[$k]->{'field_field_er_'.$name}[0]['raw']['value'];
					else
						$data[$name] = $view->result[$k]->{'field_field_er_'.$name}[0]['raw']['value'];
				}
			}
		}
		//dsm($view->result, 'external engagement view');
		return $data;
	}
	
	public function html(){
		$attr_names = array('total'=>'Total', 'male'=>'Male', 'female'=>'Female', 'und'=>'Undeclared', 'urm'=>'Under-represented Minority');
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		d($this->data(), 'count');
		$data = $this->data(false);
		d($data, 'data');
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
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/templates/External-Engagement-template.xls");
		return $objPHPExcel;
	}
	
}