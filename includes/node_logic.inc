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

function er_node_view($node, $view_mode, $langcode){
  switch ($node->type){
    case 'er_event':
      er_calendar_extras_modify_view($node);
      break;
	}
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
}

function er_validate($entity){
	//d($node, 'validate');
}

function er_node_validate($node){
	switch ($node->type){
		case 'er_engagement':
			_manage_ee_nodes($node, 'validate');
			break;
	}
}

function er_node_insert($node){
	switch ($node->type){
		case 'er_collaboration':
			_manage_er_collaborations($node, 'insert');
      _manage_er_collaboration_status($node);
			break;
    case 'er_collaborators':
      _manage_er_collaborators($node, 'insert');
      break;
	}
}

function er_node_update($node){
  switch ($node->type){
		case 'er_collaboration':
			_manage_er_collaborations($node, 'update');
      _manage_er_collaboration_status($node);
			break;
    case 'er_collaborators':
      _manage_er_collaborators($node, 'update');
      break;
	}
}

function er_node_delete($node){
  switch ($node->type){
    case 'er_collaborators':
      _manage_er_collaborators($node, 'delete');
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

/*
 * This function gets run whenever a collaboration node gets saved (insert or update)
 * It checks the user entity-reference field to see if there are any collaborators listed,
 *  and if so, it displays a warning instructing them to make sure that the name
 *  appaears in the right sidepanel (for collaborator reporting)
 */
function _manage_er_collaborations(&$node, $op){
  $lang = $node->language;
	$warnings = array();
  if (isset($node->field_er_user_entity_reference) && count($node->field_er_user_entity_reference[$lang])){//the field is available
		foreach ($node->field_er_user_entity_reference[$lang] as $k=>$values){
			if (isset($values['target_id']) && $values['target_id'] != ""){
				$user = user_load($values['target_id']);
				$participating = check_user_participating($user);
				if (!$participating){
					$u = entity_metadata_wrapper('user', $user);
					$warnings[$user->uid] = $u->field_er_realname->value();
				}
			}
		}
	}else{//error - sanity check
		drupal_set_message('Unable to find the users associated with this collaboration node! Please contact an administrator about this error!', 'error');
	}
	if (count($warnings)){//Display a general warning
		drupal_set_message("<strong>Note:</strong> Anyone who contributes less than 160 hours of effort per year, and/or their primary institution is not internal to EPSCoR, is considered a <strong>collaborator</strong> by NSF. Collaborators are only counted if their name appears in the list in the right side-panel.", 'warning');
	}
	foreach ($warnings as $uid=>$name){//Display a warning for each collaborator found
		drupal_set_message(l($name, "user/$uid") . ' is currently considered a '.l('collaborator', "user/$uid/status").'. <br><strong>Please make sure to add this person\'s name to the list of collaborators in the right side panel.</strong>', 'warning');
	}
}

/*
 * This function fires whenever an er_collaborator node gets inserted or updated
 * The function runs _manage_er_collaboration_status on the collaboration node it's attached to.
 */
function _manage_er_collaborators(&$node, $op){
  $collab_nid = $node->field_er_collab_ref['und'][0]['target_id'];
  $inst_nid = $node->field_er_collab_inst_ref['und'][0]['target_id'];
  $inst_node = node_load($inst_nid);
  $participating_inst = ($inst_node->field_er_inst_participating['und'][0]['value'] == 1);
  switch ($op){
    case 'insert':
    case 'update':
      if (!$participating_inst){//if it's a participating institution, don't bother checking because it doesn't matter.
        $collab = node_load($collab_nid);
        _manage_er_collaboration_status($collab);//re-check the draft status...
			}
      break;
    case 'delete':
      if (!$participating_inst){//if you delete one of the external collaborator nodes, invalidate the collaboration.
        $flag = flag_get_flag('er_draft_status');
        $flag->flag("flag", $collab_nid);
      }
      break;
  }
  //
}

/*
 * This function gets run whenever a er_collaboration OR er_collaborator node is saved (insert or update)
 * The function checks to make sure that there are collaborators attached to any external institutions
 *  attached to the collaboration node. If not, it sets the collaboration to be in "draft status" mode (using flags).
 */
function _manage_er_collaboration_status(&$node){
  //d($node);
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'er_collaborators')
    ->fieldCondition('field_er_collab_ref', 'target_id', $node->nid, '=');
  //query for all er_collaborator nodes matching...
  $result = $query->execute();
  //d($result, '$result');
  $collab_nodes = array();
  if (isset($result['node'])) 
    $collab_nodes = entity_load('node', array_keys($result['node']));
  //d($collab_nodes, '$collab_nodes');
  $inst_nids_found = array();
  foreach ($collab_nodes as $nid=>$collab_node){
    $inst_nids_found[] = $collab_node->field_er_collab_inst_ref['und'][0]['target_id'];
  }
  
  $n = entity_metadata_wrapper('node', $node);
  $inst_nids_referenced = $n->field_er_inst_ref->raw();
  
  
  /*
   * This will match up what was found with what was referenced.
   * If anything is left over, it means that something is missing.
   */
  //d($inst_nids_found, '$inst_nids_found 1');
  //d($inst_nids_referenced, '$inst_nids_referenced 1');
  foreach ($inst_nids_found as $k1=>$found_nid){
    foreach ($inst_nids_referenced as $k2=>$referenced_nid){
      if ($found_nid == $referenced_nid){
        unset($inst_nids_found[$k1]);
        unset($inst_nids_referenced[$k2]);
      }
    }
  }  
  //d($inst_nids_found, '$inst_nids_found 2');
  //d($inst_nids_referenced, '$inst_nids_referenced 2');
  
  $draft = false;
  //Loop over any node ids that are referenced but don't have a corresponding collaborator node:
  foreach ($inst_nids_referenced as $k=>$referenced_nid){
    $inst_node = node_load($referenced_nid);
    //if it's not a participating institution:
    if ($inst_node->field_er_inst_participating['und'][0]['value'] != 1){
      //d('Flag this node as draft. It needs more info.');
      $draft = true;
      break;
    }
  }
  
  $flag = flag_get_flag('er_draft_status');
  if ($draft){
    $flag->flag('flag', $node->nid);//turn on draft mode
  }else{
    $flag->flag('unflag', $node->nid);//publish
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
    
		if (count($node->field_er_ee_file['und'])){
			$has_old = isset($node->original->field_er_ee_file['und']) && count($node->original->field_er_ee_file['und']);
			if (!$has_old || ($has_old && $node->original->field_er_ee_file['und'][0]['fid'] != $node->field_er_ee_file['und'][0]['fid'])){
				// er_parse_ee_file(&$node);
				er_parse_ee_file($node);
			}
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

/**
 * Implements hook_user_view().
 * The user's account information is being displayed.
 */
function er_user_view($account, $view_mode, $langcode) {
	//d($account, '$account');
	//d($view_mode, '$view_mode');
	//d($langcode, '$langcode');
  
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
  $build['related_content'] = array(
    '#type' => 'fieldset',
    '#title' => 'Related Content',
    '#weight' => 100,
    'table' => array(
      '#title' => 'Related Content',
      '#theme' => 'er_related_content',
    )
  );
}