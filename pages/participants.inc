<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
class er_participants extends er_page{
	//private $types = array(//deprecated...
	//	array("male","field_er_gender","Male"),
	//	array("female","field_er_gender","Female"),
	//	array("black","field_er_race","black or African American"),
	//	array("hispanic","field_er_ethnicity","hispanic or Latino"),
	//	array("ethnic","field_er_race",array('American Indian or Alaskan Native', 'Native Hawaiian', 'Pacific Islander')),
	//	array("disabled","field_er_disabilities",array("Hearing Impairment", "Visual Impairment", "Mobility/Orthopedic Impairment", "Other"))
	//);
	private $categories = array('Faculty participants (or equivalent)','Technical support staff','Non-technical support staff','Post docs','Graduate students','Undergraduate students','RII Leadership Team');

	public function __construct() { 
		parent::__construct();
		$this->columns = array('Total', 'Male', 'Female', 'Black', 'Hispanic', 'Other Ethnic', 'Disabled');
		if ($this->range){//a reporting year date range has been specified.
			$this->columns[] = 'New Investigators';//So we can look up the new investigators, otherwise don't bother.
		}
	}
	
	public function title(){
		return "Participants".$this->range_string();
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
	
	public function get_institution_title($nid, $short = false){
		static $titles = array();
		if (!array_key_exists($nid, $titles)){
			$query = db_select('node_revision', 'nr');
			$query->condition('nr.nid', $nid, '=');
			$query->leftJoin('field_data_field_er_inst_abbreviation', 'abbr', 'abbr.entity_id = nr.nid');
			$query->addField('nr', 'title');
			$query->addField('abbr', 'field_er_inst_abbreviation_value', 'abbr');
			$result = $query->execute()->fetchAssoc();
			$titles[$nid] = $result;
		}
		if ($short && array_key_exists('abbr', $titles[$nid]) && $titles[$nid]['abbr'] != '')
			return $titles[$nid]['abbr'];
		else
			return $titles[$nid]['title'];
	}
	
	/*
	 * @param $count: boolean value indicating whether we are to display the actual content or just return the entity count.
	 * @param $distinct: boolean value indicating whether we should count distinct users or allow duplicates.
	 */
	public function generate_inst_query($count, $distinct = false){
		$query = db_select('users', 'users');
		
		//Join with attached institutions
		$query->innerJoin('field_data_field_er_inst_ref', 'institution', 'users.uid = institution.entity_id');
		//Join in field: Is this institution a "participating" institution?
		$query->leftJoin('field_data_field_er_inst_participating', 'participating', 'institution.field_er_inst_ref_target_id = participating.entity_id');
		//Join in position type
		$query->leftJoin('field_data_field_er_position_type', 'position_type', 'users.uid = position_type.entity_id');
		
		//only give me valid position types (not Other, or Evaluation)
		$query->condition('position_type.field_er_position_type_value', $this->categories, 'IN');
		//Institution must be "Participating"
		$query->condition('participating.field_er_inst_participating_value', '1', '=');
		
		$query->addField('institution', 'field_er_inst_ref_target_id', 'institution_id');
		$query->addField('position_type', 'field_er_position_type_value', 'position_type');
		
		if ($count){
			//We do the grouping when counting, otherwise show everything individually.
			$query->groupBy('institution.field_er_inst_ref_target_id');
			$query->groupBy('position_type.field_er_position_type_value');
			//Count the user ids...
			//$query->addExpression('COUNT('.($distinct?'DISTINCT ':'').'users.uid)', 'count');
			$query = $this->count_users($query, $distinct);
		}else{
			//if ($distinct)//This will remove duplicate names from the display.
			//	$query->groupBy('users.uid');
			//$query->addField('users', 'uid');
			$query = $this->find_users($query, $distinct);
		}
		//d((string)$query);
		return $query;
	}
	
	/*
	 * @param $count: boolean value indicating whether we are to display the actual content or just return the entity count.
	 * @param $distinct: boolean value indicating whether we should count distinct users or allow duplicates.
	 */
	public function generate_advisory_query($count, $distinct = false){
		$query = db_select('users', 'users');
		
		$query->innerJoin('field_data_field_er_committees', 'comm', 'users.uid = comm.entity_id');
		
		if ($count){
			//Count the user ids...
			//$query->addExpression('COUNT('.($distinct?'DISTINCT ':'').'users.uid)', 'count');
			$query = $this->count_users($query, $distinct);
		}else{
			//if ($distinct)//This will remove duplicate names from the display.
			//	$query->groupBy('users.uid');
			//$query->addField('users', 'uid');
			$query->addField('comm', 'field_er_committees_value', 'committee');
			$query = $this->find_users($query, $distinct);
		}
		//d((string)$query);
		return $query;
	}
	
	public function sort_inst_results(&$data, $label, $result, $count, $dbg = false){
		if ($count){
			while($record = $result->fetchAssoc()) {
				$data['Institutions'][$record['institution_id']][$record['position_type']][$label] = $record['count'];
				if ($dbg) d($record);
			}
		}else{
			while($record = $result->fetchAssoc()) {
				$data['Institutions'][$record['institution_id']][$record['position_type']][$label][] = $record['uid'];
				if ($dbg) d($record);
			}
		}
		return $data;
	}
	
	public function sort_advisory_results(&$data, $label, $result, $count, $dbg = false){
		if ($count){
			while($record = $result->fetchAssoc()) {
				$data['Advisory Board(s)'][$label] = $record['count'];
				if ($dbg) d($record);
			}
		}else{
			while($record = $result->fetchAssoc()) {
				$data['Advisory Board(s)'][$label][] = $record;
				if ($dbg) d($record);
			}
		}
		return $data;
	}
	
	public function generate_totals(&$data){
		$total = array();
		foreach($data['Institutions'] as $inst_id=>$data_cat){
			foreach ($data_cat as $cat_label => $cols){
				foreach ($cols as $col_label=>$count){
					//if ($cat_label = 'Undergraduate students')
					//	d($count, "[$inst_id][$cat_label][$col_label]");
					if (is_array($count))
						if (is_array($total[$cat_label][$col_label]))
							$total[$cat_label][$col_label] = array_merge($total[$cat_label][$col_label], $count);
						else
							$total[$cat_label][$col_label] = $count;
					else
						$total[$cat_label][$col_label] += $count;
				}
			}
		}
		$data['Institutions']['Total'] = $total;
	}
	
	private function apply_column_query(&$query, $col){
		switch($col){
			case 'Male':
				$query->innerJoin('field_data_field_er_gender', 'gender', 'users.uid = gender.entity_id');
				$query->condition('gender.field_er_gender_value', 0, '='); //0 = Male
				break;
			case 'Female':
				$query->innerJoin('field_data_field_er_gender', 'gender', 'users.uid = gender.entity_id');
				$query->condition('gender.field_er_gender_value', 1, '='); //1 = Female
				break;
			case 'Black':
				$query->innerJoin('field_data_field_er_race', 'race', 'users.uid = race.entity_id');
				$query->condition('race.field_er_race_value', "Black or African American", '=');
				break;
			case 'Hispanic':
				$query->innerJoin('field_data_field_er_ethnicity', 'ethnicity', 'users.uid = ethnicity.entity_id');
				$query->condition('ethnicity.field_er_ethnicity_value', 1, '='); //1 == true (this person is hispanic)
				break;
			case 'Other Ethnic':
				$query->innerJoin('field_data_field_er_race', 'race', 'users.uid = race.entity_id');
				$query->condition('race.field_er_race_value', array('American Indian or Alaskan Native', 'Native Hawaiian', 'Pacific Islander'), 'IN');
				break;
			case 'Disabled':
				$query->innerJoin('field_data_field_er_disabilities', 'disability', 'users.uid = disability.entity_id');
				$query->condition('disability.field_er_disabilities_value', 'None', '!=');
				break;
			case 'New Investigators':
				$query->innerJoin('field_data_field_er_participant_date', 'date', 'users.uid = date.entity_id');
				$range = array(date('Y-m-d', $this->range[0]).' 00:00:00', date('Y-m-d', $this->range[1]));
				//This is for overlapping date ranges:
				//$query->condition('date.field_er_participant_date_value', $range[1], '<=');
				//$db_or = db_or()
				//			->condition('date.field_er_participant_date_value2', $range[0], '>=')
				//			->isNull('date.field_er_participant_date_value2');
				//$query->condition($db_or);
				
				//Start date is within my range:
				$query->condition('date.field_er_participant_date_value', $range, 'BETWEEN');
				//d((string)$query);
				break;
		}
		return $query;
	}
	
	public function data($count = true){
		$data = array();
		foreach ($this->columns as $col){
			//Institution counts:
			$query = $this->apply_column_query($this->generate_inst_query($count), $col);
			$this->sort_inst_results($data, $col, $query->execute(), $count);
			//Advisory Board count:
			$query = $this->apply_column_query($this->generate_advisory_query($count, ($col == 'New Investigators')), $col);
			$this->sort_advisory_results($data, $col, $query->execute(), $count);
		}
		
		$this->generate_totals($data);
		return $data;
	}
	
	public function html(){
		//drupal_session_start();
		//d($_SESSION);
		if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff
		$data = array();
		d($this->data(), 'count');
		$data = $this->data(false);
		d($data, 'data');
		$ret = '<strong>NOTICE: The date range only affects the New Investigators column at this time. </strong>'.'<br>';
		$ret .= $this->download_link();
		foreach ($data['Institutions'] as $inst_id => $categories){
			if ($inst_id == "Total")
				$ret .= "<h1>RII Total</h1>";
			else
				$ret .= "<h1>".l($this->get_institution_title($inst_id), 'node/'.$inst_id)."</h1>";
			foreach ($categories as $cat => $count){
				$ret .= "<h3>$cat</h3>";
				$ret .= "<ul>";
				foreach ($count as $type=>$v){
					if (count($v)){
						//$s = implode(', ', array_map("link_map", $v));
						$ret .= "<li>" . "$type: ";
						$first = true;
						foreach ($v as $uid){
							if ($first) $first = false;
							else $ret .= ", ";
							$ret .= $this->er_get_realname($uid);
							//$ret .= "<li>".l($person['name'], 'user/'.$person['uid'].'/edit')."<br>Committees: ".$person['committees']."</li>";
						}
						$ret .= "</li>";
					}
				}
				$ret .= "</ul>";
			}
		}
		$ret .= "<h1>Advisory Board(s)</h1>";
		$ret .= "<ul>";
		foreach ($data['Advisory Board(s)'] as $type=>$v){
			if (count($v)){
				$ret .= "<li>$type:";
				$ret .= "<ul>";
				foreach ($v as $user){
					$ret .= "<li>".$this->er_get_realname($user['uid'])." - ".$user['committee']."</li>";
					//$ret .= "<li>".l($person['name'], 'user/'.$person['uid'].'/edit')."<br>Committees: ".$person['committees']."</li>";
				}
				$ret .= "</ul></li>";
			}
		}
		$ret .= "</ul>";
		return $ret;
	}
	
	public function write_excel(&$objPHPExcel, $sheet = 0){
		$use_abbreviation = false;
		$worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
		$data = $this->data();
		$base_row = 4;
		//if (count($data) > 2)
		//	$worksheet->insertNewRowBefore($base_row+1,count($data["RII total"])*(count($data)-1)-2); //add in a few more lines
		$num_categories = count($this->categories);
		$worksheet->insertNewRowBefore($base_row+1,count($data['Institutions'])*$num_categories);
		
		$row = $base_row;
		foreach ($data['Institutions'] as $inst_id => $data_cat){
			//institution label
			$label = 'RII Total';
			if ($inst_id != "Total")
				$label = $this->get_institution_title($inst_id, $use_abbreviation);
			
			$worksheet->setCellValue('A'.$row, $label);
			//This merges the institution title cells on the left (may be unnecessary):
			$bottom_row = ($row+$num_categories-1);
			$worksheet->mergeCells('A'.$row.':A'.$bottom_row);
			//This part blanks out the cells on the right:
			$worksheet->mergeCells('J'.($row+4).':J'.($bottom_row-1));
			$this->blankout_cell($worksheet, 'J'.($row+4));//set background to gray
			$this->double_border_bottom($worksheet, 'A'.$bottom_row.':J'.$bottom_row);
			
			foreach ($this->categories as $k => $category){
				$worksheet->setCellValue('B'.$row, $category);//category label
				$fill_inv = (
					!empty($this->range) //Only show this if we have a date range specified
					//We don't fill in Graduate students or Undergrad students:
					&& $category != 'Graduate students' 
					&& $category != 'Undergraduate students'
				);
				$this->fill_cell_data($worksheet, $row, isset($data_cat[$category])?$data_cat[$category]:array(), $fill_inv);
				$row++;
			}
		}
		$worksheet->removeRow($row,2);
		//fill out the Advisory Board section...
		$this->fill_cell_data($worksheet, $row, $data['Advisory Board(s)'], !empty($this->range));
	}
	
	//Takes the data for one row, and fills it into the table. It places a zero in the cell if no value is found.
	private function fill_cell_data(&$worksheet, $row, $cell_data, $fill_inv = false){
		$worksheet->setCellValue('C'.$row, get($cell_data, 'Total', 0));//get function defined in er.module
		$worksheet->setCellValue('D'.$row, get($cell_data, 'Male', 0));
		$worksheet->setCellValue('E'.$row, get($cell_data, 'Female', 0));
		$worksheet->setCellValue('F'.$row, get($cell_data, 'Black', 0));
		$worksheet->setCellValue('G'.$row, get($cell_data, 'Hispanic', 0));
		$worksheet->setCellValue('H'.$row, get($cell_data, 'Other Ethnic', 0));
		$worksheet->setCellValue('I'.$row, get($cell_data, 'Disabled', 0));
		if ($fill_inv)
			$worksheet->setCellValue('J'.$row, get($cell_data, 'New Investigators', 0));
	}
	
	//This grays out the background of a given cell.
	private function blankout_cell(&$worksheet, $cell){
		$worksheet->getStyle($cell)->applyFromArray(array(
			'fill' => array(
					'type' => PHPExcel_Style_Fill::FILL_SOLID,
					'color' => array('rgb' => '808080')
			)
		));
	}
	
	private function double_border_bottom(&$worksheet, $cell){
		$worksheet->getStyle($cell)->applyFromArray(array(
			'borders' => array(
				'bottom' => array(
					'style' => PHPExcel_Style_Border::BORDER_DOUBLE,
				)
			)
		));
	}

	public function excel_file(){
		$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Participants-template.xls");
		return $objPHPExcel;
	}
	
}