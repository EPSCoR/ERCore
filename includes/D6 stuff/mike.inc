<?php

function test_query(){
	// $query = db_select('node', 'n');
// 	
	// $table_alias = $query->join('user', 'u', 'n.uid = u.uid AND u.uid = :uid', array(':uid' => 12));
}

function er_check_edit_permissions(&$form){
	if ($form['nid']['#value'] != NULL){
		// They're NOT creating a new node
	 	$node = new stdClass;
		if (isset($form['#node']->field_er_user_reference)) $node->field_er_user_reference = $form['#node']->field_er_user_reference;
		if (isset($form['uid']['#value'])) $node->uid = $form['uid']['#value'];
	 	if (!isUserRefCreatedAdmin($node)){
	 		// they're not supposed to be in the edit page here
			drupal_access_denied();
			// throwing exit() around willy-nilly is bad idea, but there a glitch in Drupal 6 where the theme
			//	gets printed twice otherwise. see http://drupal.org/node/415474
			module_invoke_all('exit');
			exit;
	 	}	
	}
}

function er_form_er_proposal_node_form_alter(&$form, &$form_state){
	// d($form, 'form');
	
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);
	
	// the line below is added due to a strange bug where the Save and Preview buttons appear at the top when
	// this form is rendered via code in template.php
	$form['buttons']['#weight'] = 100;
	$form['#validate'][] = 'proposal_validate';
}

function er_form_er_book_node_form_alter(&$form, &$form_state){
	// d($form, 'form');
	
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);
	
	// the line below is added due to a strange bug where the Save and Preview buttons appear at the top when
	// this form is rendered via code in template.php
	$form['buttons']['#weight'] = 100;
	$form['#validate'][] = 'book_validate';
}

function er_form_er_project_node_form_alter(&$form, &$form_state){
	// d($form, 'form');
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);
}

/*
 * NOTE: this is different from the other Content Types in that Collaborator is only ever created via the Project
 * 		That is, you don't hit "Create New Content" and create a Collaborator. This doesn't call the function the others do
 */
function er_form_er_collaborator_node_form_alter(&$form, &$form_state){
	// d($form, 'form');
	// the only way to create a new one is through the project
	$projID = null;
	if ($form['nid']['#value'] == NULL){
		// They're creating a new Collaborator node
		// The project ID will be in the $_GET variable
		$projID = $_GET['er_collab_project'];
		// echo 'id of project is '.$_GET['er_collab_project'];		
	}else{
		// Set project ID equal to what's already been entered in the form
		if (isset($form['#node']->field_er_collab_project[0]['nid'])) $projID = $form['#node']->field_er_collab_project[0]['nid'];
	}		
 	$node = new stdClass;
	if (isset($projID)){ // in the rare case it's not
		$projNode = node_load($projID);
		// d($projNode, 'projNode');
		
		if (isset($projNode->field_er_user_reference)) $node->field_er_user_reference = $projNode->field_er_user_reference;
		if (isset($projNode->uid)) $node->uid = $projNode->uid;
	 	if (!isUserRefCreatedAdmin($node)){
	 		// they're not supposed to be in the edit page here
			drupal_access_denied();
			// throwing exit() around willy-nilly is bad idea, but it's a glitch in Drupal where the theme
			//	gets printed twice otherwise. see http://drupal.org/node/415474
			exit;
	 	}				
	}
}

/*
 * NOTE: this is different from the other Content Types in that External Engagement is only ever created via the Calendar Event
 * 		That is, you don't hit "Create New Content" and create an External Engagement . This doesn't call the function the others do
 */
function er_form_er_engagement_node_form_alter(&$form, &$form_state)
{
// check to see if they have permission to edit this
	$projID = null;
	if ($form['nid']['#value'] == NULL){
		// They're creating a new External Engagement node
		// The project ID will be in the args variable
		$projID = arg(3);
	}else{
		// Set project ID equal to what's already been entered in the form
		if (isset($form['#node']->field_er_cal_event_reference[0]['nid'])) $projID = $form['#node']->field_er_cal_event_reference[0]['nid'];
	}		
 	$node = new stdClass;
	if (isset($projID)){ // in the rare case it's not
		$projNode = node_load($projID);
		// d($projNode, 'projNode');
		if (isset($projNode->field_er_user_reference)) $node->field_er_user_reference = $projNode->field_er_user_reference;
		if (isset($projNode->uid)) $node->uid = $projNode->uid;
	 	if (!isUserRefCreatedAdmin($node)){
	 		// they're not supposed to be in the edit page here
			drupal_access_denied();
			// throwing exit() around willy-nilly is bad idea, but it's a glitch in Drupal where the theme
			//	gets printed twice otherwise. see http://drupal.org/node/415474
			exit;
	 	}		
	}		
	
// hiding these fields as they will be generated dynamically in a validation function
	// look under nodeapi case 'validate'
	$form['field_er_estimated']['#access'] = FALSE;	
	$form['field_er_tot_male']['#access'] = FALSE;	
	$form['field_er_tot_female']['#access'] = FALSE;	
	$form['field_er_tot_und']['#access'] = FALSE;	
	$form['field_er_tot_urm']['#access'] = FALSE;	
	$form['group_er_tot']['#access'] = FALSE;	
	$form['field_er_tot_total']['#access'] = FALSE;	
	$form['field_er_ari_stu_total']['#access'] = FALSE;	
	$form['field_er_ari_fac_total']['#access'] = FALSE;	
	$form['field_er_pui_stu_total']['#access'] = FALSE;	
	$form['field_er_pui_fac_total']['#access'] = FALSE;	
	$form['field_er_msi_stu_total']['#access'] = FALSE;	
	$form['field_er_msi_fac_total']['#access'] = FALSE;	
	$form['field_er_k12i_tec_total']['#access'] = FALSE;	
	$form['field_er_k12i_stud_total']['#access'] = FALSE;	
	$form['field_er_k12i_stut_total']['#access'] = FALSE;	
	$form['field_er_oth_total']['#access'] = FALSE;	
	
	// This is the name of the function that will be called once the form build
	// process is complete.
	$function = 'er_alter_event_properties';

	// If there are already other modules which have added their own after_build
	// functions, we will simply add ours to the end of the array. If no other
	// module has defined an after_build function yet we will create the array
	// ourselves.
	if(is_array($form['#after_build']))
	{
		$form['#after_build'][] = $function;
	}
	else
	{
		$form['#after_build'] = array($function);
	}

	// This function does not return anything, because the $form variable was
	// passed in by reference, so any changes we make will be
}

function isUserRefCreatedAdmin($node){
	global $user;
	// d($node, 'node isuser');
	$ref = $node->field_er_user_reference;
	$refArray = array();
	if (is_array($ref)) {
		foreach ($ref as $k){
			$refArray[] = $k['uid'];
		}
	}
	// print 'size.'.count($refArray).'.';

	if (!in_array($user->uid, $refArray) && !user_access('administer nodes') && $node->uid != $user->uid){
		// this person is NOT referenced and does NOT have admin privs and they're NOT the creator of the node
		return FALSE;
	} else {
		 return TRUE;
	}
}

function er_form_er_publication_node_form_alter(&$form, &$form_state){
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);
	
	// the line below is added due to a strange bug where the Save and Preview buttons appear at the top when
	// this form is rendered via code in template.php
	$form['buttons']['#weight'] = 100;
	$form['#validate'][] = 'publication_validate';	
}

function er_form_er_patent_node_form_alter(&$form, &$form_state){
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);
	
	// the line below is added due to a strange bug where the Save and Preview buttons appear at the top when
	// this form is rendered via code in template.php
	$form['buttons']['#weight'] = 100;
	$form['#validate'][] = 'patent_validate';	
}

function er_form_er_nsf_fastlane_node_form_alter(&$form, &$form_state){
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);
}

function er_form_er_presentation_node_form_alter(&$form, &$form_state){
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);
}

function er_form_er_event_node_form_alter(&$form, &$form_state) {
	$form["field_er_event_day_before"]["#access"] = FALSE;
	$form["field_er_event_day_after"]["#access"] = FALSE;
	
	// check to see if this person has priviledges to edit the node
	// er_check_edit_permissions($form);	
		
	// I'm keeping the below code that Tom made because it'll be a reference to using formapi with variable_get()
	/*
	$form['node_options_'. $node->type] = array(
	  '#type' => 'checkboxes',
	  '#title' => t('Notify by email (Optional)'),
	  '#default_value' => variable_get('node_options_'. $node->type, array('status', 'promote')),
	  '#options' => array(
	    'CYBER' => t('CYBER'),
	    'ECOGEM' => t('ECOGEM'),
	    'ENDER' => t('ENDER'),
	    'DEW' => t('DEW'),
	
	    ),
	  '#description' => t('Choose which teams, if any, to notify of this posting.'),
	  '#weight' => 99,
	);
	 */
	    //'CAC' => t('CAC (Community Advisory Committee)'),
	    //'MAP' => t('MAP (Monitoring Assessment Panel)'),
	    //'SWC' => t('SWC (Statewide Committee)'),
	    //'ADMIN' => t('Administration'),
}
function publication_validate(&$form, &$form_state){
	// d($form, 'form');
	// d($form_state, 'form state');
	/*
		 Published  => field_er_publication_date
		 Accepted - Awaiting Publication => [blank]
		 Submitted - Under Review => [blank]
		 Other => [blank]
	*/ 
	if ($form['field_er_publication_status']['#value']['value'] == 'Published' && empty($form['field_er_publication_date'][0]['#value']['value']['date'])){
		form_set_error('field_er_publication_date', t('Publication date must be entered if the status is "Published"'));
	}			
}

function patent_validate(&$form, &$form_state){
	// d($form, 'form');
	// d($form_state, 'form state');
	/*
		 Published  => field_er_publication_date
		 Accepted - Awaiting Publication => [blank]
		 Submitted - Under Review => [blank]
		 Other => [blank]
	*/ 
	if ($form['field_er_patent_status']['#value']['value'] == 'Awarded' && empty($form['field_er_patent_date'][0]['#value']['value']['date'])){
		form_set_error('field_er_patent_date', t('Patent date must be entered if the status is "Awarded"'));
	}			
}

function book_validate(&$form, &$form_state){
	// d($form, 'form');
	// d($form_state, 'form state');
	/*
		 Published  => field_er_publication_date
		 Accepted - Awaiting Publication => [blank]
		 Submitted - Under Review => [blank]
		 Other => [blank]
	*/ 
	
	if ($form['field_er_publication_status']['#value']['value'] == 'Published' && empty($form['field_er_book_date'][0]['#value']['value']['date'])){
		form_set_error('field_er_book_date', t('Publication date must be entered if the status is "Published"'));
	}
	
	if ($form['field_er_publication_status']['#value']['value'] == 'Other' && empty($form['field_er_status_other'][0]['#value']['value']['date'])){
		form_set_error('field_er_status_other', t('Please fill in a brief description for "Other"'));
	}
			
}

function proposal_validate(&$form, &$form_state){
	// d($form, 'form');
	// d($form_state, 'form state');
	/*
		 Submitted => field_er_proposal_submit
		 Pending => field_er_proposal_pending
		 Awarded => field_er_proposal_date
		 Expired => field_er_proposal_expired
		 Denied => field_er_proposal_denied
	*/ 

// if they have entered an Award Amount, make sure they've selected the 'Awarded' option
	// technically: field_er_proposal_status must be 'Awarded' or 'Expired' if they enter in a positive value for field_er_award_amount
	// Programming Note: in the first two if statements the form_set_error calls the field_er_award_amount, this is because
	//	the $element doesn't seem to get set properly when it's radio options, so that's the reason.		

	$awardedExpired = array('Awarded', 'Expired');
	if ($form['field_er_award_amount'][0]['#value']['value'] > 0 && !in_array($form['field_er_proposal_status']['#value']['value'], $awardedExpired)){
		form_set_error('field_er_award_amount', t('The Award Amount is entered only when the status of this propsal is "Awarded" or "Expired", not "'.$form['field_er_proposal_status']['#value']['value'].'".'));
	}
	
	// if they have selected Awarded of Expired but not entered the Amount Awarded, prompt them to do so.
	if (empty($form['field_er_award_amount'][0]['#value']['value']) && in_array($form['field_er_proposal_status']['#value']['value'], $awardedExpired)){
		form_set_error('field_er_award_amount', t('The Award Amount must be entered when the status of the propsal is "Awarded" or "Expired"'));
	}		
		
// Validate that all the fields that are highlighted red upon change of status are required
	// if status is Submitted then they need the Date Submitted to be filled in
	if ($form['field_er_proposal_status']['#value']['value'] == 'Submitted' && empty($form['field_er_proposal_submit'][0]['#value']['value']['date'])){
		form_set_error('field_er_proposal_submit', t('The "Date Submitted" must be entered when the status of the propsal is "Submitted"'));
	}
	// if status is Pending, then they need Date of Pending Notification filled in
	if ($form['field_er_proposal_status']['#value']['value'] == 'Pending' && empty($form['field_er_proposal_pending'][0]['#value']['value']['date'])){
		form_set_error('field_er_proposal_pending', t('The "Date of Pending Notification" must be entered when the status of the propsal is "Pending"'));
	}
	// if status is Awarded, the Award Dates need to be filled in
	// NOTE: this if statement is longer because we have to check for existence of To and From dates
	if ($form['field_er_proposal_status']['#value']['value'] == 'Awarded'){
		if (empty($form['field_er_proposal_date'][0]['#value']['value']['date']) || empty($form['field_er_proposal_date'][0]['#value']['value2']['date'])){
			form_set_error('field_er_proposal_date', t('The "Award Dates" for <i>To</i> and <i>From</i> must be entered when the status of the propsal is "Awarded"'));
		}
	} 
	
	
	// if the status is Expired, we need the To Date to be filled in on Award Dates
	// if ($form['field_er_proposal_status']['#value']['value'] == 'Expired' && empty($form['field_er_proposal_expired'][0]['#value']['value']['date'])){
		// form_set_error('field_er_proposal_expired', t('The "End Date of Award" must be entered when the status of the propsal is "Expired"'));
	// }	
	if ($form['field_er_proposal_status']['#value']['value'] == 'Expired'){
		if(empty($form['field_er_proposal_date'][0]['#value']['value']['date']) || empty($form['field_er_proposal_date'][0]['#value']['value2']['date'])){
			form_set_error('field_er_proposal_date', t('The "Award Dates" for <i>To</i> and <i>From</i> must be entered when the status of the propsal is "Expired"'));
		}		
	} 
	
	// if the status is Denied, we need the Date of Denial Notification to be filled in
	if ($form['field_er_proposal_status']['#value']['value'] == 'Denied' && empty($form['field_er_proposal_denied'][0]['#value']['value']['date'])){
		form_set_error('field_er_proposal_denied', t('The "Date of Denial Notification" must be entered when the status of the propsal is "Denied"'));
	}
	
// Person months, make ONE of them required. NOTE: the form_set_error is not actually highlighting the field group, but I couldn't put NULL
	// NOTE: this floatval is needed because someone can add .4 to the value and if we use intval it rounds down to 0
	if ((floatval($form['group_person_months']['field_er_pm_cal'][0]['#value']['value']) + floatval($form['group_person_months']['field_er_pm_acad'][0]['#value']['value']) + floatval($form['group_person_months']['field_er_pm_sumr'][0]['#value']['value'])) == 0){
		form_set_error('group_person_months', t('At least ONE field under "Person-Months Per Year" must be entered.'));
	}
	
}

function old_date_fields($data){
	// if ($data->node_users_nid == 208) d($data, 'data');
	
	$prof_id = $data->node_users_nid;
	$dates = array();
	
	$sql = "SELECT field_er_term_date_value FROM {content_type_er_user_profile} WHERE nid=".$prof_id; //status=1 published, status=0 unpublished

    $result = db_query(db_rewrite_sql($sql));
	$gotTerm = false;
    while ($r = db_fetch_object($result)) {
    // $node = node_load( $data->nid );
    // print node_view( $node, TRUE );
    	// if ($data->node_users_nid == 208) d($r);
		if ($r->field_er_term_date_value != NULL ){
			$gotTerm = true;
			$dates['term'] = date('n/j/Y', strtotime($r->field_er_term_date_value));			
		} 
		
		
		
    }
	
	$sql = "SELECT field_er_start_date_value FROM {content_field_er_start_date} WHERE nid=".$prof_id; //status=1 published, status=0 unpublished

    $result = db_query(db_rewrite_sql($sql));

    while ($r = db_fetch_object($result)) {
    // $node = node_load( $data->nid );
    // print node_view( $node, TRUE );
    	// if ($data->node_users_nid == 208) d($r);
		if ($r->field_er_start_date_value == NULL){
			$dates['start'] = '';
		}
		else{
			$dates['start'] = date('n/j/Y', strtotime($r->field_er_start_date_value));			
		}
		
    }	
	
		// $date = strtotime($row['date']);
		// $newdate = date ( 'n/j/Y' , $date ); 
	if ($gotTerm)
		$ret = $dates['start'] . ' - ' . $dates['term'];
	else
		$ret = $dates['start'];
			
	return $ret;
}


/*
function er_views_api() {
if (on_live()) return; // this should exit if we're on the live site, otherwise work on dev site
  return array(
    'api' => 2,
    'path' => drupal_get_path('module', 'er') . '/includes',
  );
}
 */
 
 /**
* Implementation of hook_block().
*/
//
//ANDREW MOVED THIS TO "blocks.inc".
//
//function er_block($op='list', $delta=0) {
//	if ($op == "list") {
//		$block[0]["info"] = t('Login (with destination)');
//		return $block;
//	}
//	else if($op == 'view'){
//		$wherearewe = ($_GET['q'] == 'frontpage') ? '' : $_GET['q'];
//		$block_content = l('Login', 'login', array('query' => array('destination' => $wherearewe))); 
//		$block['content'] = '<ul class="login-save-dest"><li>'. $block_content . '</li></ul>';
//		$block['content'] = '<span class="login-save-dest">'. $block_content . '</span>';
//		return $block;
//	}
//}
 
function to_date_in_bounds_validate($element, &$form_state){
	$from = date('Ymd', strtotime($element[0]['#value']['value']['date']));
	// According to Kevin Kelly, this Start date cannot be before the beginning of the award
	// which is 9/15/2009
	if ($from < 20090915){
		form_error($element, t('From date cannot be BEFORE the award date, which is 9/15/2009.'));
	}
}
 
function er_views_query_alter(&$view, &$query){
	// if ($view->name == 'All_user_events'){
		// $og1 = $query->where['date']['clauses'][0];
		// // below is dev
		// // $query->where['date']['clauses'][0] = '(node_node_data_field_er_event_user__node_data_field_er_user_event_date.field_er_user_event_date_value2 IS NULL OR ' . $og1 .')';
		// $query->where['date']['clauses'][0] = '(node_node_data_field_er_event_user_node_data_field_er_user_event_date.field_er_user_event_date_value2 IS NULL OR ' . $og1 .')';
	// }
	// elseif ($view->name == 'participants' && $view->current_display == 'page_1'){
	// elseif ($view->name == 'participants'){
		// d($query,'query');
		// // d($view, 'view');
		// $og1 = $query->where['date']['clauses'][1];
		// // below is dev
		// // $query->where['date']['clauses'][1] = '(node_node_data_field_er_event_user__node_data_field_er_user_event_date2.field_er_user_event_date_value2 IS NULL OR ' . $og1 .')';
		// $query->where['date']['clauses'][1] = '('. $query->tables['node_node_data_field_er_event_user']['node_data_field_er_user_event_date']['alias'] . '.field_er_user_event_date_value2 IS NULL OR ' . $og1 .')';
	// }
	if ($view->name == 'accomplishments_views' || $view->name == 'participants' || $view->name == 'All_user_events') {
		// d($view, 'view object');	
		// d($query, 'query object');
		
		// if they didn't fill in dates in the exposed fields, then no 'date' section of the array will exist, remember that.
		if (isset($query->where['date'])){
		
			$clauses = $query->where['date']['clauses'];
			// d($clauses, 'clauses');
			$findNum = '';
			foreach ($clauses as $key => $value) {
				// $findNum = array_search('value2', $value);
				// d($value, 'value');
				$findNum = strstr($value, 'value2');
				// d($findNum);			
				if (!empty($findNum)) {
					$findNum = $key;
					break;	
				}
			}
			// d('out of loop with '.$findNum);
			// do regex to get the specific name of the field
			preg_match("/^DATE_FORMAT\((.*)\.(.*),/", $query->where['date']['clauses'][$findNum], $matches);
			
			// only move on if it's finding the expecting amount of regex matches
			if (count($matches) == 3){ 
			
				$field_name = $matches[1];
				$field_value = $matches[2];
		
				// d($field_name, 'fn');
				// d($field_value, 'val');
						
				$original_query = $query->where['date']['clauses'][$findNum];				
				$query->where['date']['clauses'][$findNum] = "($field_name.$field_value IS NULL OR $original_query)";
				
				// d($query->where['date']['clauses'][$findNum], 'final query');
			}
		}
	}
}

function simultaneous_graduations($data){
	$from = $data->date_info->date_handler_fields['field_er_user_event_inst_date']['view_field']->query->where['date']['clauses'][0]; //->field_er_user_event_date['view_field']['query']['where']['date']['clauses'][0];
	$to = $data->date_info->date_handler_fields['field_er_user_event_inst_date']['view_field']->query->where['date']['clauses'][1]; //->field_er_user_event_date['view_field']['query']['where']['date']['clauses'][0];
	
	$from = strstr($from, '>= ');
	$from = substr($from, 4, -1);
	
	$to = strstr($to, '<= ');
	$to = substr($to, 4, -1);
	// d($from, 'from');
	$data->from = $from;
	$data->to = $to;
	
	// d($data, 'data in simultaneous');
	
// FIRST PART ORIGINALLY
	// $url = url('node/'.$data->node_node_data_field_er_event_user_nid, array('query'=>'format=simple'));
	// $output = '<a href="'.$url.'" rel="lightframe[groupSorted|width:800px;height:750px]">'.$data->node_node_data_field_er_event_inst_user__node_data_field_er_user_event_inst_date_field_er_user_event_inst_type_value.'</a>';
// 	
	// $addURL1 = url('node/add/er-user-event-instance', array('query'=>'format=simple'));
	// $addURL = '<a href="'.$addURL1.'&uid='.$data->uid. '" rel="lightframe[groupAddGrad|width:800px;height:750px]">Add Graduation</a>';
	// $output .= '<br/><span class="addButton"></span>'.$addURL;
// END FIRST PART
	
	$uid = $data->uid;
	// $nodeID = $data->node_node_data_field_er_event_user_inst_nid;
	$nodeID = $data->node_node_data_field_er_event_inst_user_nid;
	
	$t = new Table('content_type_er_user_event_instance','t');
	$qq = new Query($t);
	$t	->field('field_er_user_event_inst_type', 'type')->select('type')
		->field('field_er_user_event_inst_date', 'date')->select('date')
		->field('nid')->select('nid')
		->field('field_er_event_inst_user', 'uid')->where('uid', $uid)
		->where('nid', $nodeID, array('op'=>'!='));
	$results = $qq->getData();
	
	$arr = array();
	
	$n = 0;
	// while ($row = db_fetch_array($query)) {
	if (empty($results)){
		// d("EMPTY RESULTS");
		$addURL1 = url('node/add/er-user-event-instance', array('query'=>'format=simple'));
		$addURL = '<a href="'.$addURL1.'&uid='.$data->uid. '" rel="lightframe[groupAddGrad|width:800px;height:750px]">Add Graduation</a>';
		$output .= '<br/><span class="addButton"></span>'.$addURL;			
		return $output;
	} 
	foreach ($results as $row) {
		$url = url('node/'.$row['nid'], array('query'=>'format=simple'));
		$arr[$n]['type'] = '<a href="'.$url.'" rel="lightframe[group|width:800px;height:750px]">'.$row['type'].'</a>'; 

		$date = strtotime($row['date']);
		$newdate = date ( 'n/j/Y' , $date ); // m d 
		$arr[$n]['date'] = $newdate;
		
		if (isset($row['date2'])){
			$arr[$n]['date'] .= ' - ' . date ( 'n/j/Y', strtotime($row['date2']));
		}
		
        $n++;
	}
	
	$labels = array();
	// $output = '<div class="GradEvents">'. theme_table($labels, $arr, array('class'=>'UEGrad')) .'</div>';
	$output .= '<div class="GradEvents">'. theme_table($labels, $arr, array('class'=>'UEGrad')) .'</div>';
	
	// now add the ADD A USER EVENT section
	$addURL1 = url('node/add/er-user-event', array('query'=>'format=simple'));
	$addURL = '<a href="'.$addURL1.'&uid='.$data->uid. '" rel="lightframe[groupAdd|width:800px;height:750px]">Add User Event</a>';
	$output .= '<span class="addButton" aiotitle=""></span>'.$addURL;
	
	return $output;	
	
	
}
 
 
 
 
 
 
 
 
 
 
 
 
 
function user_event_grad($data, $showRange = true){
	// d($data, 'data in Grad');
	
	$uid = $data->uid;
	
	$t = new Table('content_type_er_user_event_instance','t');
	$qq = new Query($t);
	$t	->field('field_er_user_event_inst_type', 'type')->select('type')
		->field('field_er_user_event_inst_date', 'date')->select('date')
		->field('nid')->select('nid')
		->field('field_er_event_inst_user', 'uid')->where('uid', $uid);
	$results = $qq->getData();	
	// d($results, 'results Grad');
	
	$n = 0;
	$inRange = false;
	
	//TODO:CHECK THIS
	if (empty($results)){
		// d("EMPTY RESULTS");
		$addURL1 = url('node/add/er-user-event-instance', array('query'=>'format=simple'));
		$addURL = '<a href="'.$addURL1.'&uid='.$data->uid. '" rel="lightframe[groupAddGrad|width:800px;height:750px]">Add Graduation</a>';
		$output .= '<br/><span class="addButton"></span>'.$addURL;			
		return $output;
	} 	
	foreach ($results as $row) {
		
		// TODO: check that this works the same on the live site
		
		// $from = date ('Ymd', strtotime($data->node_node_data_field_er_event_user__node_data_field_er_user_event_date_field_er_user_event_date_value));
		// $to = date ('Ymd', strtotime($data->node_node_data_field_er_event_user__node_data_field_er_user_event_date_field_er_user_event_date_value2));
		
		$from = $data->date_info->date_handler_fields['field_er_user_event_date']['view_field']->query->where['date']['clauses'][0]; //->field_er_user_event_date['view_field']['query']['where']['date']['clauses'][0];
		$to = $data->date_info->date_handler_fields['field_er_user_event_date']['view_field']->query->where['date']['clauses'][1]; //->field_er_user_event_date['view_field']['query']['where']['date']['clauses'][0];
		
		$from = strstr($from, '>= ');
		$from = substr($from, 4, -1);
		
		$to = strstr($to, '<= ');
		$to = substr($to, 4, -1);
		
		$from = date('Ymd', strtotime($from));
		$to = date('Ymd', strtotime($to));
		$grad = date('Ymd', strtotime($row['date']));
		// var_dump($data);
		// d($data, 'data');
		// d($from, 'from');
		// d($grad, 'GRAD');
		// d($to, 'to');
// 		
// 
// 		
		// d($from, 'new from');
		// d($to, 'new to');
		
		if ($grad >= $from && $grad <= $to){
			$inRange = true;
		}		
		
		$url = url('node/'.$row['nid'], array('query'=>'format=simple'));
		
		$arr[$n]['type'] = '<a href="'.$url.'" rel="lightframe[groupGrad|width:800px;height:750px]">'.$row['type'].'</a>'; 
		$date = strtotime($row['date']);
		$newdate = date ( 'n/j/Y' , $date ); // m d	
		
		$arr[$n]['date'] = $newdate;
		// Add the In Range business 
		if ($showRange){
			if ($inRange){
				$arr[$n]['date'] .= ' <i>(in date range)</i>';		
			}
			else{
				$arr[$n]['date'] .= ' <i>(not in date range)</i>';
			}									
		}

		$arr[$n]['type'] .= '<br/>'.$arr[$n]['date'];
		unset($arr[$n]['date']);
				
		$n++;
	}

	$labels = array();
	$output = '<div class="userEvents">'. theme_table($labels, $arr) .'</div>';
	 
	
	// now add the ADD A USER EVENT section
	$addURL1 = url('node/add/er-user-event-instance', array('query'=>'format=simple'));
	$addURL = '<a href="'.$addURL1.'&uid='.$data->uid. '" rel="lightframe[groupAddGrad|width:800px;height:750px]">Add Graduation</a>';
	$output .= '<span class="addButton"></span>'.$addURL;
	
	//TODO: REMEMBER TO ADD NODEAPI FORM ALTER HOOK TO NEW CONTENT TYPE
	
	return $output;

	// return 'mike';
} 

function user_event_view($data){
	// d($data, 'DATA');
	$uid = $data->uid;
	$type = $data->node_node_data_field_er_event_user_node_data_field_er_user_event_type_field_er_user_event_type_value;
	// $nodeID = $data->node_node_data_field_er_event_user_nid;
	// $nodeID = $data->node_node_data_field_er_event_user_1_nid;
	$nodeID = $data->node_node_data_field_er_event_user_nid;
	// $q= "SELECT field_er_user_event_type_value, field_er_user_event_date_value, nid FROM {content_type_er_user_event} WHERE field_er_event_user_uid = $uid AND field_er_user_event_type_value != '$type' ORDER BY field_er_user_event_type_value";
	// $q= "SELECT field_er_user_event_type_value, field_er_user_event_date_value, nid FROM {content_type_er_user_event} WHERE field_er_event_user_uid = $uid AND nid != '$nodeID' ORDER BY field_er_user_event_type_value";
// 	
	// $q= "SELECT field_er_user_event_type_value, field_er_user_event_date_value, field_er_user_event_date_value2, nid FROM {content_type_er_user_event} WHERE field_er_event_user_uid = $uid AND nid != '$nodeID' ORDER BY field_er_user_event_type_value";
	
	$t = new Table('content_type_er_user_event','t');
	$qq = new Query($t);
	$t	->field('field_er_user_event_type', 'type')->select('type')
		->field('field_er_user_event_date', 'date')->select('date')->select('date2')
		->field('nid')->select('nid')
		->field('field_er_event_user', 'uid')->where('uid', $uid)
		->where('nid', $nodeID, array('op'=>'!='));
	$results = $qq->getData();
	// d($nodeID, 'nodeid');
	// d($results, 'results');
		//field_er_event_user_uid = $uid AND nid != '$nodeID' ORDER BY field_er_user_event_type_value"
	
	// $header = array(array('data' => 'TYPE', 'field' => 'field_er_user_event_type_value', 'sort' => 'asc'), array('data' => 'DATE', 'field' => 'field_er_user_event_date_value'), array('data' => 'NID', 'field' => 'nid'));
	// $q= "SELECT field_er_user_event_type_value, field_er_user_event_date_value, nid FROM {content_type_er_user_event} WHERE field_er_event_user_uid = $uid AND field_er_user_event_type_value != '$type' ORDER BY field_er_user_event_type_value" . tablesort_sql($header);
	
	// $query = db_query($q); before query builder
	
	// $query = pager_query($q);
	$arr = array();
	$arrDateF = array();
	
	$n = 0;
	// while ($row = db_fetch_array($query)) {
			
	//TODO: CHECK THIS
	if (empty($results)){
		// d("EMPTY RESULTS");
		$addURL1 = url('node/add/er-user-event', array('query'=>'format=simple'));
		$addURL = '<a href="'.$addURL1.'&uid='.$data->uid. '" rel="lightframe[groupAdd|width:800px;height:750px]">Add User Event</a>';
		$output .= '<span class="addButton" aiotitle=""></span>'.$addURL;	
		return $output;
	} 
	foreach ($results as $row) {
		// d($row, 'row');
		// $arr[$n]['type'] = l($row['field_er_user_event_type_value'],'node/'.$row['nid']);
		
		$url = url('node/'.$row['nid'], array('query'=>'format=simple'));
		
		// $arr[$n]['type'] = '<a href="'.$url.'" rel="lightframe[group|width:800px;height:750px]">'.$row['field_er_user_event_type_value'].'</a>'; 
		$arr[$n]['type'] = '<a href="'.$url.'" rel="lightframe[group|width:800px;height:750px]">'.$row['type'].'</a>'; 
		// $arr[$n]['type'] = l($row['field_er_user_event_type_value'],'node/'.$row['nid']);
		// $arr[$n]['type'] = l($row['type'],'node/'.$row['nid']);
		
		// $date = strtotime($row['field_er_user_event_date_value']);
		$date = strtotime($row['date']);
		$newdate = date ( 'n/j/Y' , $date ); // m d 
		$arr[$n]['date'] = $newdate;
		
		// if there's a TO part of this
		// if (isset($data->node_node_data_field_er_event_user_node_data_field_er_user_event_date_field_er_user_event_date_value2) && $data->node_users_nid == $row['nid']){
		// if (isset($row['field_er_user_event_date_value2'])){
		if (isset($row['date2'])){
			// $arr[$n]['date'] .= ' - ' . date ( 'n/j/Y', strtotime($row['field_er_user_event_date_value2']));
			$arr[$n]['date'] .= ' - ' . date ( 'n/j/Y', strtotime($row['date2']));
		}
		
		// $arr[$n]['date'] = $row['field_er_user_event_date_value'];
		
		// $arr[$n]['nid'] = $row['nid'];
		
        $n++;
	}
	
	//$ret = implode("<br/>", $arr);
	//return $ret;
	// $labels = array('Type','Date','nid');
	$labels = array();
	// $table_attributes = array('border' => 1, 'cellpadding' => 3);
	$output = '<div class="userEvents">'. theme_table($labels, $arr, array('class'=>'UEReg')) .'</div>';
	// $output = '<div class="userEvents">'. theme_table($labels, $results) .'</div>';
	
	// now add the ADD A USER EVENT section
	$addURL1 = url('node/add/er-user-event', array('query'=>'format=simple'));
	$addURL = '<a href="'.$addURL1.'&uid='.$data->uid. '" rel="lightframe[groupAdd|width:800px;height:750px]">Add User Event</a>';
	$output .= '<span class="addButton" aiotitle=""></span>'.$addURL;
	
	return $output;
}



function on_live(){ 
	if (stristr(dirname(__FILE__), 'web41.its.hawaii.edu.dev.epscor.hawaii.edu'))
		return false;
	else
		return true;
}

function on_dev(&$one = 'first by reference', &$two = 'second', $three = 'third') {
	//dsm('from mike inc');
	if (on_live()) return;
		if ($one == 'user_filter_form') {
			d($two, 'user view form');
			
			$two['filters']['status']['status']['#options'][2] = 'kustomike';
			d($two['filters']['status']['status']['#options']);
		}
	
		// We're indeed in the DEV site, code here only runs on dev site
		//dsm('were in the dev site');
		/*
		$sql = "SELECT DISTINCT(nid) FROM {mikecheck}";
		$ids = db_query(db_rewrite_sql($sql));
		$nodes = array();
		while($data = db_fetch_object($ids)){
			$nodes[] = $data->nid;
		}	
		foreach ($nodes as $key => $value) {
			dsm($value, 'value');
			$sql = "SELECT * FROM {mikecheck} WHERE nid = $value";
			$ids = db_query(db_rewrite_sql($sql));
			$count = 0;
			while($data = db_fetch_object($ids)){
				dsm($data, 'data');
				$sql = "UPDATE {mikecheck} SET delta = ".$count." WHERE vid = ".$data->vid." AND nid = ".$data->nid." AND field_cimes_researcher_uid = ".$data->field_cimes_researcher_uid;
				db_query(db_rewrite_sql($sql));
				$count++;
			}
			
		}
		 * 
		$t = array('cimes_news_item', 'cimes_congress_testimony', 'event', 'cimes_other_funding', 'image', 'cimes_program', 'cimes_course', 'cimes_other_contacts', 'page', 'cimes_paper', 'cimes_patent', 'cimes_presentation', 'cimes_projects', 'cimes_dhs_request', 'cimes_gov_request', 'cimes_software', 'cimes_student');
		dsm($t, 'content types in array');
		$r1 = in_array("page", $t);
		$r2 = in_array("0age", $t);
		dsm($r1, 'page');
		dsm($r2, 'fakie');
		 */
	

	// now the nodes array is filled with unique nids that may have multiple researchers, need to add deltas
	/*
	if ($node->nid == 2092){
		dsm('here!');
		$sql = "SELECT * FROM {mikecheck}";
		$ids = db_query(db_rewrite_sql($sql));
		$count = 0;
		while($data = db_fetch_object($ids)){
			dsm($data, 'data');
			$sql = "UPDATE {mikecheck} SET delta = ".$count." WHERE vid = ".$data->vid." AND nid = ".$data->nid." AND field_cimes_researcher_uid = ".$data->field_cimes_researcher_uid;
			db_query(db_rewrite_sql($sql));
			
			$count++;
		}
								
	}
	*/		
	
}

function er_action_info() {
if (on_live()) return;
  return array(
    'er_notify_action' => array(
      'description' => t('Do not notify by email'),
      'type' => 'user',
      'configurable' => FALSE,
        
      ),
  );	

}

function er_notify_action(&$object, $context = array()) {
if (on_live()) return;

	//dsm($object, 'object');
	//dsm($context, 'context');
	  if (isset($object->uid)) {
	    $uid = $object->uid;
	  }
	  drupal_set_message("UID is $uid");
	  
	  
	  //db_query("UPDATE {users} SET status = 0 WHERE uid = %d", $uid);	
	  
	  $er_rows = array();
	  //$query = "SELECT uid FROM {er_user_notify} WHERE uid = $uid";
	  $query = "SELECT uid, ignore FROM {er_user_notify} e WHERE uid = $uid";
	  $q = db_query($query);
		while ($r = db_fetch_array($q)) {
			d($r, 'r');
		  //$er_rows[] = $r;
		}
		
		//$r = db_result(db_query("SELECT uid FROM {er_user_notify} WHERE uid = %d", $uid));
		//dsm($r, 'results');	
		//dsm($rows, 'rows');	
		
		//if($r){
		//	dsm('this is where an update would be done');
			//db_query("UPDATE {er_user_notify} SET ignore = 9 WHERE uid = %d", $uid);
		//}
		
	

	
}
function er_schema_alter(&$schema){
  // Suppress errors
	// $schema['advanced_blog']['fields']['title']['not null'] = FALSE;
	// $schema['advanced_blog']['fields']['description']['not null'] = FALSE;  
	// $schema['fupload_previewlist']['fields']['uid']['length'] = 10;  
	// $schema['locales_source']['fields']['location']['length']=300;
	
	// These two fields are in Current and Pending Support. The contain values greater than a million
	// 	and FLOAT can't keep the precision properly, so we had to change the data type to DOUBLE  
	$schema['content_type_er_proposal']['fields']['field_er_award_amount_value']['size']= 'big';
	$schema['content_type_er_proposal']['fields']['field_er_award_requested_value']['size']= 'big';
}
/*
function er_js(&$form_state) {

  return $form;	
	
}

function er_js_callback() {
	dsm('something said in er js callback');
	$form_state = array('storage' => NULL, 'submitted'	=> FALSE);
	$form_build_id = $_POST['form_build_id'];
	// Step #4.
	$form = form_get_cache($form_build_id, $form_state);

	// Preparing for #5.
	$args = $form['#parameters'];
	$form_id = array_shift($args);
	$form_state['post'] = $form['#post'] = $_POST;
	$form['#programmed'] = $form['#redirect'] = FALSE;

	// Step #5.
	drupal_process_form($form_id, $form, $form_state);
	// Step #6 and #7 and #8.
	$form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
	
	
	// customize this
	$choice_form = $form['textfields']['first_name'];
	unset($choice_form['#prefix'], $choice_form['#suffix']);
	$output = theme('status_messages') . drupal_render($choice_form);

	// Final rendering callback.
	drupal_json(array('status' => TRUE, 'data' => $output));
		/*
		 $form = ahah_example_callback_helper2();
		 $textfields = $form['field_er_fname'];
		 $output = 'in mike inc';
		 $output .= drupal_render($textfields);
		 drupal_json(array('status' => TRUE, 'data' => $output));
		 exit();
		 * *
  //return drupal_json(array('status' => TRUE, 'data' => "Hello Drupal World"));
}

function er_form_er_user_profile_node_form_alter(&$form, &$form_state) {
  $form['ask_first_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('click here1'),
    '#default_value' => TRUE,
    '#ahah' => array(
      //'path' => 'examples/ahah_example/autotextfields/callback',
      'path' => 'er/js/callback',
      
      'wrapper' => 'textfield-wrapper',
      'effect' => 'fade',
    )
  );
  $form['ask_last_name'] = array(
   '#type' => 'checkbox',
   '#title' => t('click here2'),
   '#default_value' => FALSE,

    '#ahah' => array(
      //'path' => 'examples/ahah_example/autotextfields/callback',
      'path' => 'er/js/callback',
      'wrapper' => 'textfield-wrapper',
      'effect' => 'fade',
      // 'event' => 'change', // default value: does not need to be set explicitly.

    ),
  );

  $form['textfields'] = array(
    '#title' => t("Generated text fields for first and last name"),
    '#prefix' => '<div id="textfield-wrapper">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
    '#description' => t('This is where we put automatically generated textfields'),
  );

   if (!empty($form_state['values']['ask_first_name'])) {
    $form['textfields']['first_name'] = array(
      '#type' => 'textfield',
      '#title' => t('First Name'),
    );
  }
  if (!empty($form_state['values']['ask_last_name'])) {
    $form['textfields']['last_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Last Name'),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Click Me'),
  );  
		
	//dsm($form, 'from mike');
}
	*/
	
function mike_nodeapi(&$node, $op, $a3 = NULL, $a4  = NULL){
	if (on_live()) return;
	
  $query = db_query("SELECT uid,name,mail FROM {users}");

  $data = array();

  $i = 0;
  while ($row = db_fetch_array($query)) {
    $data[$i] = $row;
    $i++;
  }

  $output = theme_table(array('ID', 'Look', 'Taste'), $data);	
  //print $output;
  $node->content['new']['#value'] = $output;
	
	
	d($node);
}	

function mike_form_alter(&$form, &$form_state, $form_id){
	d($form, 'form');
	$form['field_er_event_user']['#title'] = 'test';
}

