<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
class er_debug extends er_page{

	public function __construct(){
			parent::__construct();
	}
	
	//&$query, 'ref', 'field_er_user_reference', 'uid', rand(), '='
	public function fieldCondition(&$query, $entity_name, $field, $column, $value, $op){
		
	}
	
	public function joinField(&$query, $entity_name, $field, $age = FIELD_LOAD_CURRENT){
		static $key = 0;
		if ($age == FIELD_LOAD_CURRENT) { //I'm not really going to worry about this stuff for now...
			$tablename_function = '_field_sql_storage_tablename';
			$id_key = 'entity_id';
		}
		else {
			$tablename_function = '_field_sql_storage_revision_tablename';
			$id_key = 'revision_id';
		}
		if (is_string($field))
			$field = field_info_field($field);
		$tablename = $tablename_function($field);
		//dsm($field, $field_name);
		$table_alias = $field['field_name'].$key++;
		$entity_key = $entity_name == 'node'?'nid':$id_key;
		return $query->leftJoin($tablename, $table_alias, "$entity_name.nid = $table_alias.$id_key");
	}
	
	//public function joinEntities(&$query, $e1, $e2, $on){
	//	
	//}
	
	public function load1(){//This version is considderably faster than version 2
		$file_path = 'sites/all/modules/er/install/bundles/er_award';
		$file_contents = file_get_contents($file_path);
		if (substr($file_contents, 0, 5) == '<?php')
			return substr($file_contents, 5);
		else
			return $file_contents;
	}
	
	public function load2(){//This version is much slower, takes about 34x longer
		$file_path = 'sites/all/modules/er/install/bundles/er_award';
		module_load_include('inc', 'ctools', 'includes/export'); //allows us to use ctools_var_export(), stringifies a datastructure
		include($file_path);//$flags variable should be set now.
		return '$data = ' . ctools_var_export($data) . ';';
	}
	
	public function html(){
		//$addrs = variable_get('er_notification_email_addr');
		//dsm($addrs, 'Notification email addresses');
		global $user;
		$types = array('ari_fac','ari_stu','pui_fac','pui_stu','msi_fac','msi_stu','k12i_tec','k12i_stud','k12i_stut','oth');
		$vals = array('total', 'male', 'female', 'und', 'urm');
		switch(arg(2)){
			case 'test_user_status':
				$max = pow(2, 4);
				d('Compare these values against the excel sheet that admin agreed upon.');
				for ($i = 0; $i<$max;$i++){
					$criteria = (object)array(
						'one_sixty'=>!!($i&8),
						'participating_inst'=>!!($i&4),
						'paid'=>!!($i&2),
						'paid_student'=>!!($i&1)
					);
					$result = er_predict_user_status_from_criteria($criteria);
					d($criteria, ($i+2).' '.($result?'Participant':'Non-Participant'));
				}
				return '';
			case 'test_rusty':
				$file_path = 'sites/all/modules/er/install/bundles/er_event';
				$context = array();
				er_batch_import_fields($file_path, $context);
				return '';
			case 'test_field_rename':
				er_field_rename('field_test_field_1', false);
				return '';
			case 'speed1':
				$start = microtime(true);
				for ($i = 0; $i<50; $i++){
					$this->load1();
				}
				$end = microtime(true);
				d(($end-$start)*100);
				return '';
			case 'speed2':
				$start = microtime(true);
				for ($i = 0; $i<50; $i++){
					$this->load2();
				}
				$end = microtime(true);
				d(($end-$start)*100);
				return '';
			case 'bundle':
				d(get_batch_operations("all"), 'all');
				d(get_batch_operations("flags"), 'flags');
				d(get_batch_operations("bundles"), 'bundles');
				d(get_batch_operations("rules"), 'rules');
				return '';
			case 'EFQ':
				$ptypes = array('Faculty participant (or equivalent)','Technical support staff','Non-technical support staff','Post docs','Graduate students','Undergraduate students','RII Leadership Team');
				$query = db_select('users', 'users');
				
				//Join with attached institutions
				$query->innerJoin('field_data_field_er_inst_ref', 'institution', 'users.uid = institution.entity_id');
				//Join in field: Is this institution a "participating" institution?
				$query->leftJoin('field_data_field_er_inst_participating', 'participating', 'institution.field_er_inst_ref_target_id = participating.entity_id');
				//Join in position type
				$query->leftJoin('field_data_field_er_position_type', 'position_type', 'users.uid = position_type.entity_id');
				//This is used to get the title of the institution node.
				$query->innerJoin('node_revision', 'nr', 'nr.nid = institution.field_er_inst_ref_target_id');
				
				$query->condition('position_type.field_er_position_type_value', $ptypes, 'IN');//only give me valid position types (not Other, or Evaluation)
				$query->condition('participating.field_er_inst_participating_value', '1', '=');//Institution must be "Participating"
				
				$query->addExpression('COUNT(users.uid)', 'count');
				//$query->addField('users', 'uid');
				$query->addField('institution', 'field_er_inst_ref_target_id', 'institution_id');
				$query->addField('position_type', 'field_er_position_type_value', 'position_type');
				$query->addField('nr', 'title', 'institution_title');
				
				$query->groupBy('institution.field_er_inst_ref_target_id');
				$query->groupBy('position_type.field_er_position_type_value');
				
				
				d((string)$query);
				$result = $query->execute();
				db_set_active();
				while($record = $result->fetchAssoc()) {
					d($record);
				}
				
				//require_once ER_MODULE_DIR.'/refq.inc';
				
				//$entity_type = 'user';
				//$query = new EntityFieldQuery();
				//$query->entityCondition('entity_type', $entity_type)
				//			//->entityCondition('bundle', 'er_collaborators')
				//			->addTag('er_participants');
				//$result = $query->execute();
				//d($result[$entity_type], 'result');
				
				
				//$this->q = new refq();
				//$this->q->entityCondition('entity_type', 'node')
				//				->entityCondition('bundle', $this->type)
				//				//->propertyCondition('status', 1)
				//				->fieldCondition('field_er_user_reference', 'uid', rand(), '=');
				//$results = $this->q->execute();
				//foreach ($result['node'] as $r){
				//	dsm($r);
				//}
				break;
			case 'PHPExcel':
				//$lib = libraries_load('PHPExcel');
				//d($lib);
				if (($library = libraries_detect('PHPExcel')) && !empty($library['installed'])) {// The library is installed. Awesome!
					$count = libraries_load_files($library);
					d($count);
					$objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/templates/Collaborations-template.xls");
					//d(new PHPExcel());
				}else {// Something went wrong. :(
					$error = $library['error'];// This contains a short status code of what went wrong, such as 'not found'.
					$error_message = $library['error message'];// This contains a detailed (localized) error message.
					drupal_set_message('PHPExcel library error: ' . $error_message, 'error');
				}
				break;
			case 'node-add':
				//dsm(menu_load_links('main-menu'));
				$nodes = node_type_get_types();
				dsm($nodes, 'node types');
				$output = '<div class="region region-content">';
				$output .= '	<div id="block-system-main" class="block block-system">';
				$output .= '	<div class="content">';
				$output .= '		<ul class="admin-list">';
				$output .= '			<li class="clearfix"><span class="label"><a href="/node/add/article">Article</a></span><div class="description">Use <em>articles</em> for time-sensitive content like news, press releases or blog posts.</div></li>';
				$output .= '			<li class="clearfix"><span class="label"><a href="/node/add/er-award">Award</a></span><div class="description">This will contain information on Awards given to EPSCoR researchers</div></li>';
				$output .= '		</ul>';
				$output .= '		</div>';
				$output .= '	</div>';
				$output .= '</div>';
				return $output;
				break;
			case 'bootstrap':
				$content = '<a href="#" title="first tooltip" data-content="some stuff in here" id="example">hover over me</a><script>jQueryNew(\'#example\').tooltip()</script><br>';
				$content .= '<a href="#" title="first tooltip" data-content="some stuff in here" id="example2">hover over me</a><script>jQueryNew(\'#example2\').popover()</script>';
				return $content;
			case 'ee':
				$query = new EntityFieldQuery();
				$query->entityCondition('entity_type', 'node')
								->entityCondition('bundle', 'er_engagement')
								//->propertyCondition('status', 1)
								;//->fieldCondition('field_er_cal_entity_reference', 'target_id', rand(), '=');
				$result = $query->execute();
				dsm($result);
				break;
			case 'view':
				$view = views_get_view('er_summary_generator');
				//$view->init_display();
				////$view->preview=TRUE;
				$view->is_cacheable = FALSE;
				//$view->get_count_optimized = TRUE;
				////$view->display_handler->set_option('items_per_page',2);
				////$view->set_item('default', 'sort', 'random', NULL);
				////$view->add_item('default', 'sort', 'node', 'created',array('order' => 'DESC'));
				//$view->pre_execute();
				dsm($view);
				$view->execute();
				dsm($view);
				////$output = $view->display_handler->preview();
				$view->post_execute();
				$results = views_get_view_result('er_publication_summary_view', 'count');
				$count = $results[0]->nid;
				dsm($results);
				//dsm($output);
				break;
			case 'test':
				$query = db_select('node', 'n')->condition('n.type', 'er_engagement');
				//$query->addExpression('COUNT(*)', 'uid_count');
				
				$field_name = 'field_er_cal_event_reference';
				$field = field_info_field($field_name);
				//$field_table_alias = $this->joinField($query, 'n', $field);
				$query->leftJoin('field_data_field_er_cal_event_reference', 'ref_field', "n.nid = ref_field.entity_id");
				$query->innerJoin('field_data_field_er_event_date', 'date_field', "ref_field.field_er_cal_event_reference_nid = date_field.entity_id");
				$query->fields('date_field');
				
				/*$sql_fields = array();
				foreach (array_keys($field['columns']) as $column){
					$sql_fields[] = _field_sql_storage_columnname($field_name, $column);
				}
				dsm($sql_fields);*/
				//$query->join('n', )
				
				//$sql_fields[] = $field_column = _field_sql_storage_columnname($field_name, 'nid');
				//$query->fields($field_table_alias, $sql_fields);
				
				
				//$field_name = 'field_er_user_reference';
				//$field = field_info_field($field_name);
				//dsm($field, $field_name);
				//$columns = array_keys($field['columns']);
				//$column = $columns[0];
				//
				//$field_alias = $this->joinField($query, 'n', $field_name);
				//$sql_field = _field_sql_storage_columnname($field_name, $column);
				//$query->fields($field_alias, array($sql_field));
				//dsm($field_alias);
				
				$results = $query->execute();
				//dsm($results->fetchAssoc());
				//dsm($results);
				foreach ($results as $r){
					dsm($r);
				}
				break;
			default:
				$nodes = node_type_get_types();
				d($nodes, 'node types');
				$fields = field_info_fields();
				d($fields, '$fields');
				$bundle_info = field_info_bundles('node');
				d($bundle_info, '$bundle_info');
		}
		return '';
	}
	
	private function manualQuery($q, $debug = true){
    	$t1 = $this->microtime_float();
    	//$q = "SELECT n.uid, uid_t.* FROM content_type_er_user_profile t, {node} n, content_type_er_user_event e, content_field_er_user_reference uid_t WHERE uid_t.nid = e.nid AND (e.field_er_user_event_type_value = 'Hired' OR e.field_er_user_event_type_value = 'Participating') AND t.nid = n.nid AND uid_t.field_er_user_reference_uid = n.uid";
    	//$q = "SELECT * FROM content_type_er_user_profile t, {node} n, content_type_er_user_event e, content_field_er_user_reference uid_t WHERE uid_t.nid = e.nid AND (e.field_er_user_event_type_value = 'Hired' OR e.field_er_user_event_type_value = 'Participating') AND t.nid = n.nid AND uid_t.field_er_user_reference_uid = n.uid";
    	//$q = "SELECT * FROM content_type_er_user_profile t INNER JOIN {node} n ON (t.nid = n.nid) INNER JOIN content_field_er_user_reference uid_t ON uid_t.field_er_user_reference_uid = n.uid INNER JOIN content_type_er_user_event e ON uid_t.nid = e.nid WHERE e.field_er_user_event_type_value = 'Hired' OR e.field_er_user_event_type_value = 'Participating'";
    	//$q = "SELECT count(distinct(n.uid)) FROM content_type_er_user_profile t, {node} n, content_type_er_user_event e, content_field_er_user_reference uid_t, users_roles r WHERE uid_t.nid = e.nid AND (e.field_er_user_event_type_value = 'Hired' OR e.field_er_user_event_type_value = 'Participating') AND r.rid = 16 AND t.nid = n.nid AND uid_t.field_er_user_reference_uid = n.uid AND r.uid = n.uid";
    	//$q = "SELECT count(distinct up.nid) FROM content_type_er_user_profile up, content_field_er_race ethnic_t WHERE up.field_er_involvement_value = 'Undergraduate student' AND ethnic_t.nid = up.nid AND (ethnic_t.field_er_race_value = 'American Indian or Alaskan Native' OR ethnic_t.field_er_race_value = 'Native Hawaiian' OR ethnic_t.field_er_race_value = 'Pacific Islander')"; // GROUP BY ethnic_t.delta";
    	if ($debug) d($q);
    	$query = db_query($q);
    	$ret = array();
    	while ($data = db_fetch_array($query))
			$ret[] = $data;
		if ($debug) d($ret);
		$t2 = $this->microtime_float();
		//$join = 0.027233;
		if ($debug) d($t2-$t1, "time");
		return $ret;
		//dsm($t2-$join, "difference");
	}
	
	public function microtime_float(){
		list($usec, $sec) = explode(" ", microtime());
		return ((float)$usec + (float)$sec);
	}
    
    private function testExcel(){
    	//date_default_timezone_set('UTC');
    	$objReader = PHPExcel_IOFactory::createReader('Excel5');
    	$objPHPExcel = $objReader->load($this->template_dir."External-Engagement-upload.xls");
    	//$objPHPExcel = $objReader->load($this->template_dir."Web-Meeting.xls");
    	$worksheet = $objPHPExcel->setActiveSheetIndex(0); //initialize to the first page...
		//$objWriter = PHPExcel_IOFactory::createReader($objPHPExcel, 'Excel2007');
		
		/* Excel Sheet Key:
		 * (2,4) title
		 * (2,5) location
		 * (2,6) date
		 * (2,7) poc
		 * (2,8) notes
		 * (0,14+) #
		 * (1,14+) name
		 * (2,14+) inst code
		 * (3,14+) person code
		 * (4,14+) gender
		 * (5,14+) minority
		 * (6,14+) paid?
		 */
    	
    	if ($worksheet->getCellByColumnAndRow(2, 1)->getValue() != "External Engagement Reporting Sheet")
    		break;
    	
    	$title = $worksheet->getCellByColumnAndRow(2, 4)->getValue();
    	//dsm($title);
    	$location = $worksheet->getCellByColumnAndRow(2, 5)->getValue();
    	//dsm($location);
    	$date = gmdate('m/j/Y',PHPExcel_Shared_Date::ExcelToPHP($worksheet->getCellByColumnAndRow(2, 6)->getValue()));
    	//dsm($date);
    	$poc = $worksheet->getCellByColumnAndRow(2, 7)->getValue();
    	//dsm($poc);
    	$notes = $worksheet->getCellByColumnAndRow(2, 8)->getValue();
    	//dsm($notes);
    	
    	/* 1 = Academic Research * 2 = Primary Undergrad. * 3 = Minority Serving * 4 = K-12 * 5 = Other (pls specify) */
    	$instCodes = array('1'=>'ari', '2'=>'pui', '3'=>'msi', '4'=>'k12i');
    	/* 1 = Faculty * 2 = Student * 3 = Teacher * 4 = Other (pls specify) */
    	$personCodes1 = array('1'=>'tec', '2'=>'stud', '3'=>'tec');//, '4'=>'oth'); //used for k12i
    	$personCodes2 = array('1'=>'fac', '2'=>'stu', '3'=>'fac');//, '4'=>'oth'); //used for everything else
    	$genderCodes = array('m'=>'male', 'f'=>'female');
    	$minorityCodes = array('y'=>'urm');
    	$highestRow = $worksheet->getHighestRow();
		
    	$data = array();
    	for ($y = 14; $y<=$highestRow; $y++){
			//$number = $worksheet->getCellByColumnAndRow(0, $y)->getValue();
			$name = $worksheet->getCellByColumnAndRow(1, $y)->getValue();
			$inst = $instCodes[$worksheet->getCellByColumnAndRow(2, $y)->getValue()];
			$person = $worksheet->getCellByColumnAndRow(3, $y)->getValue();
			$person = $inst=='k12i'?$personCodes1[$person]:$personCodes2[$person]; //different for k12i
			$gender = $genderCodes[strtolower($worksheet->getCellByColumnAndRow(4, $y)->getValue())];
			$minority = $minorityCodes[$worksheet->getCellByColumnAndRow(5, $y)->getValue()];
			$paid = $worksheet->getCellByColumnAndRow(6, $y)->getValue(); //has no bearing on this form
			
			if (!$name && !$inst && !$person) //if the information dries up, just assume this is the end of the list.
				break;
			
			$col = $inst.'_'.$person;
    		if (!$person || !$inst)
				$col = "oth";
			if ($gender)
				$data["er_{$col}_{$gender}"]++;
			if ($minority)
				$data["er_{$col}_{$minority}"]++;
			$data["er_{$col}_total"]++;
		}
		foreach (array("ari_", "pui_", "msi_", "k12i_", "oth_") as $inst){
			$affis = array(""); //for "other"
			if ($inst == "k12i_"){
				$affis = array("tec_", "stud_");//, "stut"); //don't need stut because i wont be filling it up with anything.
			}else if ($inst != "oth_"){
				$affis = array("fac_", "stu_");
			}
			foreach($affis as $affi){
				foreach (array("total", "male", "female", "urm") as $attr){
					//dsm("er_tot_$attr", "er_$inst$affi$attr");
					$data["er_tot_$attr"] += intval($data["er_$inst$affi$attr"]);
				}
			}
		}
		d($data);
    	//dsm(node_load(621));
    }

}
