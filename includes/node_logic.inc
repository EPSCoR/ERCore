<?php
// http://api.drupal.org/api/drupal/modules!field!field.module/function/template_preprocess_field/7
function er_preprocess_field(&$variables) {
	// d($variables, 'variables');
	
	/* this block was used when trying out encryption stuff
	$encrypted_fields_arr = array('field_er_disabilities', 'field_er_gender', 'field_er_race', 'field_er_ethnicity', 'field_er_citizenship');
	if (in_array($variables['element']['#field_name'], $encrypted_fields_arr)) {
		if (function_exists('encrypt')){
			$variables['encrypted'] = encrypt($variables['element']['#field_name']);
		}
		
		d($variables, "variables for {$variables['element']['#field_name']}");
		// if($variables['items']['0']['#markup'] == 'thedefaultvalue') {
			$variables['items']['0']['#markup'] .= ' ALTERED';
		// }
	}
	 * */
}

/*
 *
 * This gets run any time a node is loaded for viewing...
 * NOTICE!:: This hook seems to be getting called during the installation/reset
 *  procedure which can interrupt the batch API process! Beware of using drupal_goto
 */
function er_node_view($node, $view_mode, $langcode){
  switch ($node->type){
    case 'er_event':
      er_calendar_extras_modify_view($node);
      break;
    case 'er_proposal':
	  	er_cnp_extras_modify_view($node);
	  	break;
		case 'er_engagement':
			if ($view_mode == 'full'){
				$node->content['table'] = array(
					'#theme' => 'er_external_engagements_view_table',
					'#node' => $node,
					'#weight' => 500,
				);
			}
			break;
		case 'er_collaborators':
			if ($view_mode == 'full'){
				//if you get directed to a collaborators node, just redirect to the collaboration
				$n = entity_metadata_wrapper('node', $node);
				$collab_nid = $n->field_er_collab_ref->raw();
				drupal_goto('node/'.$collab_nid);
			}
			break;
		case 'er_effort':
			if ($view_mode == 'full'){
				//if you get directed to a effort node, just redirect to the proposal
				$n = entity_metadata_wrapper('node', $node);
				$proposal_nid = $n->field_er_proposal_entity->raw();
				drupal_goto('node/'.$proposal_nid);
			}
			break;
	}
}

/**
 * Implements hook_user_view_alter().
 * The user was built; the module may modify the structured content.
 */
function er_user_view_alter(&$build) {
	global $user;
	unset($build['flags']);//hide the flags, don't need it here.
  //This adds the tags indicating the permissions on each field:
	if (isset($user->roles[3])){//user is admin
		$roles = user_roles();//3 == admin, 2 == authenticated, 1 == anonymous
		$user_defined = $roles;
		unset($user_defined[3], $user_defined[2], $user_defined[1]);
		$role_perms = user_role_permissions($roles);
		//d($role_perms, '$role_perms');
		foreach ($build as $field_name=>$field){
			if (is_array($field) && array_key_exists('#field_name', $field)){//then this is actually a field and not just some other setting.
				//d($field['#field_name'], '$field[\'#field_name\']');
				$field_info = field_info_field($field['#field_name']);
				//d($field_info, $field['#field_name']);
				if (isset($field_info['field_permissions']) && $field_info['field_permissions']['type'] != FIELD_PERMISSIONS_PUBLIC){
					//check permissions per role:
					//Note: It is assumed that Admin role can see everything.
					$class = array();
					$view_perm = 'view '.$field['#field_name'];
					if (array_key_exists($view_perm,  $role_perms[1]))//anonymous
						$class["anon"] = 'pill-important';
					if (array_key_exists($view_perm,  $role_perms[2]))//authenticated
						$class["auth"] = 'pill-warning';
					foreach ($user_defined as $role_id=>$label){
						if (array_key_exists($view_perm,  $role_perms[$role_id]))//authenticated
							$class[$label] = 'pill-info';
					}
				}else{//if field permissions are not set, then the field is automatically public.
					$class = array("public"=>'');
				}
				$badges = '';
				foreach ($class as $label=>$color){
					$badges .= '<div class="pill '.$color.'" style="float:right;margin-top:2px;">'.$label.'</div>';
				}
				//if ($badges) $badges .= '<br>';
				$build[$field_name]['#prefix'] = $badges . '<div style="border-bottom:1px dotted #ddd">';
				$build[$field_name]['#suffix'] = '</div>';
				//d(user_access('view field er disabilities'));
				
			}
		}
		//d($build, '$build');
		//d($build['field_er_disabilities']['#object'], '$build[\'field_er_disabilities\'][\'#object\']');
	}
	$view = views_get_view('er_related_content');
	$build['related_content2'] = array(
    '#type' => 'fieldset',
    '#title' => 'Related Content',
    '#weight' => 100,
    'table' => array(
      '#title' => 'Related Content',
      '#markup' => $view->render('master'),
    )
	);
  //$build['related_content'] = array(
  //  '#type' => 'fieldset',
  //  '#title' => 'Related Content',
  //  '#weight' => 100,
  //  'table' => array(
  //    '#title' => 'Related Content',
  //    '#theme' => 'er_related_content',
  //  )
  //);
}

function er_preprocess_node(&$variables, $hook){
  //d($variables, '$variables');
  if ($hook == 'node' && $variables['type'] == 'er_collaboration'){
    $flag = flag_get_flag('er_draft_status');
    if ($flag->is_flagged($node->nid))
      $variables['title_suffix'][] = array('#markup'=>'<span class="form-required">DRAFT STATUS</span>');
  }
}

function er_calendar_extras_modify_view(&$node){
	$view = views_embed_view('er_attach_calendar_extras', 'default', $node->nid);
	//this adds the views to the bottom
	$node->content['ee_views'] = array(
		'#weight' => 99,
		'#markup' => $view,
	);
}

function er_node_presave($node){
	switch ($node->type){
		case 'er_engagement':
			_manage_ee_nodes($node, 'presave');
			break;
	}
	_node_autogen_title($node);
}

function _node_autogen_title(&$node){
	$n = entity_metadata_wrapper('node', $node);
	switch ($node->type){
		case 'er_collaborators';
			$collab = $n->field_er_collab_ref->value();
			$name = $n->field_er_collab_name->value();
			$department = $n->field_er_collab_department_unit->value();
			$inst = $n->field_er_collab_inst_ref->value();
			$inst_with_dept = $department?"$department @ {$inst->title}":"{$inst->title}";
			//d($collab, '$collab');
			$node->title = "$name from $inst_with_dept on {$collab->title}";
			break;
		case 'er_engagement';
			$event = $n->field_er_cal_entity_reference->value();
			$total = $n->field_er_tot_total->value();
			$node->title = "{$total} External Engagements: {$event->title}";
			break;
	}
}

function er_validate($entity){
	//d($node, 'validate');
}

function er_node_validate($node){
	_quick_add_institution_validation($node);
	switch ($node->type){
		case 'er_engagement':
			_manage_ee_nodes($node, 'validate');
			break;
		case 'er_collaborators':
			_validate_collaborators($node);
	}
}

function er_node_insert($node){
	switch ($node->type){
    case 'er_proposal':
			er_manage_cnp_extra_nodes($node, 'insert');
			break;			
	}
}

function er_node_update($node){
  switch ($node->type){
		case 'er_proposal':
			er_manage_cnp_extra_nodes($node, 'update');
			break;
		case 'er_effort':
			er_check_cnp_extra_status($node->nid, $node);
			break;			
	}
}

function er_node_delete($node){
  switch ($node->type){
	case 'er_proposal':
	  er_manage_cnp_extra_nodes($node, 'delete');
	  break;	  
  }
}

//see: http://api.drupal.org/api/drupal/modules%21field%21field.attach.inc/function/field_attach_presave/7
function er_field_attach_presave($entity_type, $entity){
  if ($entity_type == 'user'){
    $field = 'field_er_participant_date'; //this is the name of the date field on the user profile
  }else if ($entity_type == 'node' && isset($entity->type)){
    switch ($entity->type){
      case 'er_collaboration':
        $field = 'field_er_collaboration_dates';break;
      case 'er_proposal':
        $field = 'field_er_proposal_date';break;
    }
  }
  if (isset($field) && count($entity->{$field})){
    $lang = field_language($entity_type, $entity, $field);
    //if the user did not select the "show to date" checkbox, we can set the "to date" to null. This helps the queries understand that this event is 'ongoing'.
		if (count($entity->{$field}[$lang]) && isset($entity->{$field}[$lang][0]['show_todate']) && !$entity->{$field}[$lang][0]['show_todate']){
      $entity->{$field}[$lang][0]['value2'] = null;
    }
  }
}

/**
 * This function gets run during the save phase of a er_collaborators node.
 * The function prevents a user from adding a person as a participant while not being tagged with a participating institution.
 */
function _validate_collaborators(&$node){
	//d($node, '$node');
	$n = entity_metadata_wrapper('node', $node);
	$inst = $n->field_er_collab_inst_ref->value();
	if ($inst){
		$status = $n->field_er_collab_user_status ->raw();
		$i = entity_metadata_wrapper('node', $inst);
		$participating = $i->field_er_inst_participating->value();
		//d($participating, '$participating');
		if ($status == 'Participant' && !$participating){
			$message = t("Sorry, '@title' is not currently considered a participating institution. Every participant must belong to a participating institution. This person should therefore be considered a collaborator. If you believe this is incorrect, please contact an administrator and ask them to change the institution's status.", array('@title'=>$inst->title));
			form_set_error('field_er_collab_user_status', $message);
		}
	}
}

/**
 * Custom validation for use with the quick-add institution feature.
 * See form_logic.inc: function _quick_add_institution
 */
function _quick_add_institution_validation(&$node){
	//d($node, '$node');
	if ($node->op == 'Add New Institution' && isset($node->quick_add_inst)){//quick add institution has been fired
		if (count_chars($node->quick_add_inst['inst_name']) > 1){
			form_set_error('quick_add_inst][inst_name', t('You must provide the full name of the institution that you wish to add to the system.'));
		}
	}
}

//EE = External Engagements
function _manage_ee_nodes(&$node, $op){
	//static $types = array('ari_fac','ari_stu','pui_fac','pui_stu','msi_fac','msi_stu','k12i_tec','k12i_stud','k12i_stut','oth');
	//static $catLabels = array("Academic Research Institutions", "Primary Undergraduate Institutions", "Minority Serving Institutions", "K-12 Institutions", "Other");
	static $types = array(
		'ari_fac'=>"Academic Research Institutions",
		'ari_stu'=>"Academic Research Institutions",
		'pui_fac'=>"Primary Undergraduate Institutions",
		'pui_stu'=>"Primary Undergraduate Institutions",
		'msi_fac'=>"Minority Serving Institutions",
		'msi_stu'=>"Minority Serving Institutions",
		'k12i_tec'=>"K-12 Institutions",
		'k12i_stud'=>"K-12 Institutions",
		'k12i_stut'=>"K-12 Institutions",
		'oth'=>"Other"
	);
	//dsm($node, $op);
	//$lang = field_language('node', $node);
	$lang = 'und';
	if ($op == 'presave'){
    //Change the title to be that of the calendar event:
    $event_nid = $node->field_er_cal_entity_reference[$lang][0]['target_id'];
    $event_node = node_load($event_nid);
    $node->title = $event_node->title . ' - External Enagements';
    
		$has_new_file = false;
		if (count($node->field_er_ee_file['und'])){
			$has_old = isset($node->original->field_er_ee_file['und']) && count($node->original->field_er_ee_file['und']);
			if (!$has_old || ($has_old && $node->original->field_er_ee_file['und'][0]['fid'] != $node->field_er_ee_file['und'][0]['fid'])){
				$has_new_file = true;
			}
		}
		
		if ($has_new_file){
			er_parse_ee_file($node);
		}else{
			//fill out the totals based on the breakdown
			$node->field_er_tot_total[$lang][0]['value'] = 0;
			$node->field_er_tot_male[$lang][0]['value'] = 0;
			$node->field_er_tot_female[$lang][0]['value'] = 0;
			$node->field_er_tot_und[$lang][0]['value'] = 0;
			$node->field_er_tot_urm[$lang][0]['value'] = 0;
			foreach ($types as $type=>$categoryLabel){
				$male = $node->{'field_er_'.$type.'_male'}[$lang][0]['value'];
				$female = $node->{'field_er_'.$type.'_female'}[$lang][0]['value'];
				$und = $node->{'field_er_'.$type.'_und'}[$lang][0]['value'];
				$urm = $node->{'field_er_'.$type.'_urm'}[$lang][0]['value'];
				$total = $node->{'field_er_'.$type.'_total'}[$lang][0]['value'] = $male + $female + $und;			
				$node->field_er_tot_total[$lang][0]['value'] += $total;
				$node->field_er_tot_male[$lang][0]['value'] += $male;
				$node->field_er_tot_female[$lang][0]['value'] += $female;
				$node->field_er_tot_und[$lang][0]['value'] += $und;
				$node->field_er_tot_urm[$lang][0]['value'] += $urm;
			}
		}
	}else if ($op == 'validate'){
		//d($node, $op);
		foreach ($types as $type=>$categoryLabel){
			$male = $node->{'field_er_'.$type.'_male'}[$lang][0]['value'];
			$female = $node->{'field_er_'.$type.'_female'}[$lang][0]['value'];
			$und = $node->{'field_er_'.$type.'_und'}[$lang][0]['value'];
			$urm = $node->{'field_er_'.$type.'_urm'}[$lang][0]['value'];
			$total = $male + $female + $und;
			if ($urm > $total){
				$urmFieldData = field_info_instance('node', 'field_er_'.$type.'_urm', $node->type);
				form_set_error('field_er_'.$type.'_urm][i][value', "The values for <u>{$urmFieldData->label}</u> must be less than the total. See: <u>$categoryLabel</u> section.");
			}
		}
		
		if ($node->nid == NULL){//NEW NODE IS BEING SAVED
			//d($node, $op);
			$cal_nid = $node->field_er_cal_entity_reference[$lang][0]['target_id'];
			$query = new EntityFieldQuery();
			$query->entityCondition('entity_type', 'node')
							->entityCondition('bundle', 'er_engagement')
							//->propertyCondition('status', 1)
							->fieldCondition('field_er_cal_entity_reference', 'target_id', $cal_nid, '=');
			$result = $query->execute();
			//Here we are checking if a EE already exists, and if so, redirect the user to that node instead.
			if (isset($result['node']) && count($result['node']) > 0){
				$ee = array_pop($result['node']);
				$node->validated = false;
				$link = l('link', 'node/'.$ee->nid.'/edit', array('query'=>array('destination'=>'node/'.$cal_nid)));
				form_set_error('text', "External Engagement information has already been attached to this calendar event! Please visit this $link to edit that node.");
			}
		}
	}
}

/*
 * This is part of the code that parses the external engagements uploads 
 */
function er_parse_ee_file(&$node){
  if (($library = libraries_detect('PHPExcel')) && !empty($library['installed']) && libraries_load_files($library)) {// The library is installed, and files are loaded.
    $lang = 'und';
    $file = file_load($node->field_er_ee_file['und'][0]['fid']);
    //dsm($file, '$file');
    $path = $file->uri;//$_SERVER["DOCUMENT_ROOT"].base_path()."/".$file['filepath'];
    //d($path);
    //d(file_exists($path)?"file exists":"file doesn't exist!");
    $objReader = PHPExcel_IOFactory::createReader('Excel5');
    $objPHPExcel = $objReader->load($path);
    $worksheet = $objPHPExcel->getActiveSheet();
    //$worksheet = $objPHPExcel->setActiveSheetIndex(0); //initialize to the first page...
  
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
    
    //really simple validation; this just makes sure that it's the expected excel sheet by checking for the header text.
    if ($worksheet->getCellByColumnAndRow(2, 1)->getValue() != "External Engagement Reporting Sheet")
      break;
    
    $title = $worksheet->getCellByColumnAndRow(2, 4)->getValue();
    //d($title);
    $location = $worksheet->getCellByColumnAndRow(2, 5)->getValue();
    //d($location);
    $date = gmdate('m-d-Y',PHPExcel_Shared_Date::ExcelToPHP($worksheet->getCellByColumnAndRow(2, 6)->getValue()));
    //d($date);
    $poc = $worksheet->getCellByColumnAndRow(2, 7)->getValue();
    //d($poc);
    $notes = $worksheet->getCellByColumnAndRow(2, 8)->getValue();
    //d($notes);
    
    /* 1 = Academic Research * 2 = Primary Undergrad. * 3 = Minority Serving * 4 = K-12 * //5 = Other (pls specify) */
    $instCodes = array('1'=>'ari', '2'=>'pui', '3'=>'msi', '4'=>'k12i');
    /* 1 = Faculty * 2 = Student * 3 = Teacher * //4 = Other (pls specify) */
    $personCodes1 = array('1'=>'tec', '2'=>'stud', '3'=>'tec'); //used for k12i
    $personCodes2 = array('1'=>'fac', '2'=>'stu', '3'=>'fac'); //used for everything else
    $genderCodes = array('m'=>'male', 'f'=>'female');
    $genderCodes = array('m'=>'male', 'f'=>'female', ''=>'und');
    $genderCodes = array('m'=>'male', 'f'=>'female');
    $minorityCodes = array('y'=>'urm');
    
    //Use this to clear out all of the values in preparation for new values
    foreach (array('ari_fac','ari_stu','pui_fac','pui_stu','msi_fac','msi_stu','k12i_tec','k12i_stud','k12i_stut','oth', 'tot') as $prefix){
      foreach (array("total", "male", "female", "und", "urm") as $attr){
        $node->{"field_er_{$prefix}_{$attr}"}[$lang][0]['value'] = 0;
      }
    }
    
    $highestRow = $worksheet->getHighestRow();//last filled out row of the template...
    for ($y = 14; $y<=$highestRow; $y++){//14 is the start of the data rows
      //note: using "get" function here is to avoid errors in cases where things should be classified into the catchall "other" fields.
      //get function is defined in er.module
      
      //$number = $worksheet->getCellByColumnAndRow(0, $y)->getValue();
      $name = $worksheet->getCellByColumnAndRow(1, $y)->getValue();
      $inst = get($instCodes, $worksheet->getCellByColumnAndRow(2, $y)->getValue());
      $personCodes = $inst=='k12i'?$personCodes1:$personCodes2;
      $person = get($personCodes, $worksheet->getCellByColumnAndRow(3, $y)->getValue());
      //$person = personCodes($inst, $worksheet->getCellByColumnAndRow(3, $y)->getValue());
      
      if (!$name && !$inst && !$person) //if the information dries up, just assume this is the end of the list.
        break;
      
      $gender = get($genderCodes, strtolower($worksheet->getCellByColumnAndRow(4, $y)->getValue()));
      $minority = get($minorityCodes, strtolower($worksheet->getCellByColumnAndRow(5, $y)->getValue()));
      $paid = strtolower($worksheet->getCellByColumnAndRow(6, $y)->getValue()); //has no bearing on this form
      //we can't count paid participants...
      if (substr($paid, 0, 1) != 'y'){//someone might put in "yes" and not just "y"...
        $col = $inst.'_'.$person;
        if (!$person || !$inst)
          $col = "oth";
        
        if ($gender){
          $node->{"field_er_{$col}_{$gender}"}[$lang][0]['value']++;
          $node->{"field_er_tot_{$gender}"}[$lang][0]['value']++;
        } else {
          $node->{"field_er_{$col}_und"}[$lang][0]['value']++;
          $node->{"field_er_tot_und"}[$lang][0]['value']++;
        }
        
        if ($minority){
          $node->{"field_er_{$col}_urm"}[$lang][0]['value']++;
          $node->{"field_er_tot_urm"}[$lang][0]['value']++;
        }
        
        $node->{"field_er_{$col}_total"}[$lang][0]['value']++;
        $node->{"field_er_tot_total"}[$lang][0]['value']++;
      }
    }
    
    $message = 'The attachment "%fn" was processed and the External Engagements have been filled out. Please verify that the counts are correct.';
    $vars = array('%fn'=>$file->filename);
    watchdog('er', $message, $vars, WATCHDOG_INFO);
    drupal_set_message(t($message, $vars));
  }else{//Libraries had a problem loading PHPExcel...
		$error = $library['error'];// This contains a short status code of what went wrong, such as 'not found'.
		$error_message = $library['error message'];// This contains a detailed (localized) error message.
		drupal_set_message('PHPExcel library error: ' . $error_message, 'error');
		watchdog('er', "User tried to use EE Excel Upload, error: ".$error_message, NULL, WATCHDOG_CRITICAL);
	
  }
}

function er_manage_cnp_extra_nodes($cnp_node, $op){
	//retrieve the user-ids currently attached to this proposal via paid nodes.
	$query = db_select('field_data_field_er_proposal_entity', 'e')->condition('e.field_er_proposal_entity_target_id', $cnp_node->nid);//->condition('e.bundle', $type);
	$query->innerJoin('node', 'n', "n.nid = e.entity_id");
	$query->fields('n', array('uid', 'nid', 'type'));
	$returned = $query->execute()->fetchAll();
	// d($returned, 'nodes found');
	
	$nodes = array('er_paid'=>array(), 'er_effort'=>array());
	foreach ($returned as $ent)
		$nodes[$ent->type][$ent->nid] = $ent;
	
	
	$referenced_users = array();//this becomes the new node list
	if (isset($cnp_node->field_er_user_entity_reference['und'])){
		//$referenced_users = $cnp_node->field_er_user_entity_reference['und'];
		foreach($cnp_node->field_er_user_entity_reference['und'] as $ref_user){
			$referenced_users[$ref_user['target_id']] = (object)array('uid'=>$ref_user['target_id']);
		}
	}
	// d($cnp_node, 'cnp node');
	// add the PI/Co-PI to the list of referenced users. We will not get into this function unless it passes validation
	// this is the uid we want
	// d($cnp_node->field_er_epscor_pi_co_pi[LANGUAGE_NONE][0]['target_id'], 'uid of the PI');
	$piUID = $cnp_node->field_er_epscor_pi_co_pi[LANGUAGE_NONE][0]['target_id'];
	// $piObj = new stdClass($piUID);
	$piObj = (object) array('uid' => $piUID);
	// $piObj->uid = $piUID;
	// d($piObj, 'piObj');
	$referenced_users[$piUID] = $piObj;

	// d($referenced_users, 'Users referenced');
	
	$types = array('er_effort');
	//TODO: don't want logic with field_er_proposal_paid
	//DONE
	// if ($cnp_node->field_er_proposal_paid['und'][0]['value'])//if the flag is set to not use paid nodes, we can skip this, potentially deleting any paid nodes attached
		// $types[] = 'er_paid';
		
	$users = array('er_paid'=>array(), 'er_effort'=>array());
	if ($op != 'delete'){//if the node is being deleted, skip this step and simply delete all paid nodes
		foreach ($types as $type){
			$users[$type] = $referenced_users;
			//d($nodes[$type], "$type nodes:");
			//d($users[$type], "$type users");
			foreach ($users[$type] as $uk=>$user){
				foreach ($nodes[$type] as $nk=>$node){
					if ($user->uid == $node->uid){
						unset($users[$type][$uk]);
						unset($nodes[$type][$nk]);
						break;//found a match, kill the search
					}
				}
			}
			//d($nodes[$type], "Need to delete $type:");
			//d($users[$type], "Need to create $type:");
		}
	}
	
	$flag = flag_get_flag('er_needs_review');
	$created = false;
	$deleted = false;
	foreach (array('er_paid', 'er_effort') as $type){
		//if (isset($users[$type]))//there's no er_paid when paid = false... throws an error without this.
		foreach ($users[$type] as $uk=>$user){//add these new nodes
			//d('Adding user: '.$user['target_id']);
			$newNode = er_create_cnp_extra_node($cnp_node, $user, $type);
			node_save($newNode);
			$flag->flag('flag', $newNode->nid, null, TRUE);
			//d('Created node: '.$newNode->nid);
			$created = true;
		}
		
		foreach ($nodes[$type] as $nk=>$node){//delete these nodes
			//NOTE: This function requires that the user has sufficient priveles to delete the node in question
			node_delete($node->nid);
			//d('Deleting: '.$node->nid);
			$deleted = true;
		}
	}
	
	if ($created)//If new nodes were created
		$flag->flag('flag', $cnp_node->nid, null, TRUE); //So flag the proposal as "needs review"
	
	if ($deleted)//If something was deleted, it's possible that the cnp is now valid.
		er_check_cnp_status($cnp_node->nid);
	
// TODO: dont want logic with field_er_proposal_paid
	// if (!$cnp_node->field_er_proposal_paid['und'][0]['value']){ //weird edge case occurs when proposal gets flagged, then uncheck "paid" checkbox.
		// $flag->flag('unflag', $cnp_node->nid, null, TRUE);
	// }
}

function er_cnp_extras_modify_view(&$node){
	$flag = flag_get_flag('er_needs_review');
	// $paid = views_embed_view('er_attach_cnp_extras', 'er_paid', $node->nid); // will not get used even if it's uncommented
	$effort = views_embed_view('er_attach_cnp_extras', 'er_effort', $node->nid);
	if ($flag->is_flagged($node->nid)){
		$node->content['needs_review'] = array(
			'#weight' => -1,
			'#markup' => '<span class="er-needs-review-label">This content needs review</span>',
		);
	}
	//this adds the views to the bottom
	$node->content['extra_views'] = array(
		'#weight' => 99,
		'#markup' => $effort.$paid,
	);
}

//Paid node stuff
//Add or Remove extra nodes (Paid or Effort) after saving a proposal
function er_create_cnp_extra_node(&$cnp_node, $user, $type){
	//we need to know which fields are important on this content type, it wont save otherwise.
	static $node_fields = array(
		'er_paid'=>array('field_er_funded_effort', 'field_er_funding'),
		'er_effort'=>array('field_er_pm_cal', 'field_er_pm_acad', 'field_er_pm_sumr')
	);
	//the node doesn't exist, so we create it.
	$newNode = new StdClass;
	$newNode->type = $type;
	node_object_prepare($newNode);
	
	//dsm($user);
	//dsm(strip_tags(theme('username', array('account' => $user['entity']))));
	
	$user = user_load($user->uid);
	
	//kind of fudge the "realname":
	$name = count($user->field_er_lname)?$user->field_er_fname['und'][0]['safe_value'].' '.$user->field_er_lname['und'][0]['safe_value']:$user->name;
	$newNode->title = "$name financial support - {$cnp_node->title}";
	$newNode->uid = $user->uid;
	$newNode->moderate = 0;
	$newNode->comment = 0;
	$newNode->language = 'und';
	$newNode->created = strtotime("now");
	$newNode->changed = strtotime("now");
	
	// add CCK field data
	//$newNode->field_er_paid_user[0]['uid'] = $user['uid'];
	$newNode->field_er_proposal_entity['und'][0]['target_id'] = $cnp_node->nid;
	$newNode->field_er_proposal_entity['und'][0]['entity'] = $cnp_node;
	
	//need the placeholder for these fields or the node won't save.
	if (isset($node_fields[$type]))
		foreach ($node_fields[$type] as $field_name)
			$newNode->{$field_name} = array();
	
	//dsm($newNode, '$newNode');
	return $newNode;
}

//If the paid or effort node gets passed validation we can safely unflag the node, we then check to see if it validates the proposal.
function er_check_cnp_extra_status($nid, $node = null){
	if (!isset($node)) $node = node_load($nid);
	$flag = flag_get_flag('er_needs_review');
	$flag->flag('unflag', $nid, null, TRUE);
	//get the node id of the attached proposal:
	$proposal_nid = $node->field_er_proposal_entity['und'][0]['target_id'];
	er_check_cnp_status($proposal_nid);
}

//This function determines if the proposal should still be flagged as "needs review".
//It checks for any attached paid nodes, checks their status, and if all is well, it removes the "needs review" flag from the proposal.
function er_check_cnp_status($nid){
	//dsm('checking paid status here...', $nid);
	$flag = flag_get_flag('er_needs_review');
	if ($flag->is_flagged($nid)){
		//dsm('we need to check...');
		//get all of the attached paid nodes:
		$query = new EntityFieldQuery();
		$query  ->entityCondition('entity_type', 'node')
						->fieldCondition('field_er_proposal_entity', 'target_id', $nid, '=');
		$returned = $query->execute();
		if (isset($returned['node'])){//if we found something:
			$flagged = false;
			foreach ($returned['node'] as $node){ //check the review status of each paid node
				//dsm('checking node: '.$paid_node->nid);
				if ($flag->is_flagged($node->nid)){
					//dsm('flagged!', $paid_node->nid);
					$flagged = true;//we just need one paid node to be pending for it to invalidate the proposal
					break;
				}
			}
			if (!$flagged){//if there is no reviews pending, we can set the proposal to be completed
				//dsm('unflagging proposal');
				$flag->flag('unflag', $nid, null, TRUE);
			}
		}
	}
}