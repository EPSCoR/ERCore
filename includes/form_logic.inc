<?php
/*
 *  Generic form alter
 * 		See: http://api.drupal.org/api/drupal/modules!system!system.api.php/function/hook_form_alter/7
 */

function er_form_alter(&$form, &$form_state, $form_id){
	// d($form_id, 'form id in er_form_alter');
	// d($form, 'form');
	// d($form_state, 'fs');
	// var_dump($form_state);
	// $blob = variable_get(drupal_private_key);
	// $blob = variable_get(menu_options_er_publication);
	// d($blob, 'private key blob');
	//d($blob, 'blob');
	// drupal_set_message($form_id . ' is the form id'); // good for showing id for non-admin
	// d($form, 'form in er_form_alter');
	
	// This is used to make sure people can't edit others' content
	// 		Unless they're referenced, the creator, or an admin
	$check_edit_permission = list_cts_having_field('field_er_user_entity_reference', '_node_form');									
	if (in_array($form_id, $check_edit_permission)){
		er_check_edit_permissions($form);
	}
}

// this adds a warning not to change the values of the Most Senior Project values 
function er_form_field_ui_field_edit_form_alter(&$form, &$form_state){
	$form['field']['settings']['allowed_values']['#title'] .= " (ER CORE: Attention - Do not change these)";
}

/*
 *	This function is called by er_form_alter
 * 		The purpose is to check to make sure the User has permission to edit the given field
 */  
function er_check_edit_permissions(&$form){
	// d($form, 'form in er_check_edit_permissions');
	// drupal_set_message('in er_check_edit_permissions');
	if (isset($form['nid'])){
		if ($form['nid']['#value'] != NULL){
			// They're NOT creating a new node (we dont want to deny access if they're simply creating a new node, obviously)
		 	$node = new stdClass;
			if (isset($form['#node']->field_er_user_entity_reference)) 
				$node->field_er_user_entity_reference = $form['#node']->field_er_user_entity_reference;
			if (isset($form['uid']['#value'])) $node->uid = $form['uid']['#value'];
		 	if (!isUserRefCreatedAdmin($node)){
		 		// they're not supposed to be in the edit page here
		 		drupal_set_message('Sorry, only those referenced in the content "' . 
		 			$form['#node']->title . '" (' . $form['#node']->nid . ') ' . 'may edit it.', 'warning');
				drupal_access_denied();
		 	}	
		}
	}
}

  
 /*
 * Below are all the content types listed with their specific form_alters, as well as extra forms
  * 	See: http://api.drupal.org/api/drupal/modules!system!system.api.php/function/hook_form_FORM_ID_alter/7
 */

// Award
function er_form_er_award_node_form_alter(&$form, &$form_state, $form_id){
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

// Calendar Event
function er_form_er_event_node_form_alter(&$form, &$form_state, $form_id){
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);

// "Event Type" = "Other" ---- show More information text field
// modified by Chris - cjallen@epscor.unm.edu

  $eventOther = array(
			':input[name="field_er_event_type[und]"]' => array(
			'value' => '6'), //Oddly '6' works - which is the value of Other in the 'Event Type' Listing
			);
		//Calendar Event Other - More Information
	$form['field_er_other_more_information']['#states'] = array(
		'visible' =>  $eventOther,
	);
}

// External Collaborators
function er_form_er_collaborators_node_form_alter(&$form, &$form_state, $form_id){
	if (_deny_direct_form_access($form)) return;
	_form_autogen_title($form, $form_state, $form_id);//Hide the title + autogenerate a title.
	
	$form['field_er_collab_ref']['#disabled'] = TRUE;
	$ref_field = 'field_er_user_lookup';
	$form_state['autofill_ref_field'] = $ref_field;
	$form['autofill_collab_submit'] = array(
		'#value' => 'Auto-fill fields below',
		'#type' => 'submit',
		'#limit_validation_errors' => array(array($ref_field)),
		'#submit' => array('autopopulate_collab_submit_callback'),
		'#states' => array(//This is pretty unnecessary, but looks kinda neato.
			'visible' => array(
        ':input[name*="'.$ref_field.'"]' => array('!value' => ''), //note the use of wildcard (*=), this is because the field is in a fieldgroup
			),
		),
	);
	$form['autofill_collab_submit']['#weight'] = $form[$ref_field]['#weight'] + 0.5;
	if (isset($form['#group_children'][$ref_field]))
		$form['#group_children']['autofill_collab_submit'] = $form['#group_children'][$ref_field];
	
	if (isset($form_state['attached_user']) && $user = user_load($form_state['attached_user'])){
		$u = entity_metadata_wrapper('user', $user);
		//get a reference to the linked collaboration node
		$collab_nid = isset($_GET['field_er_collab_ref'])?intval($_GET['field_er_collab_ref']):$form['#node']->field_er_collab_ref['und'][0]['target_id'];
		$collab_node = node_load($collab_nid);
		$cn = entity_metadata_wrapper('node', $collab_node);
		$dates = $cn->field_er_collaboration_dates->value();
		$collab_range = array(strtotime($dates['value']), strtotime($dates['value2']));//the range of time that the collaboration is going on...
		
		//This part is a bit shakey. Some of them seem to work with ['und'] = value, while other's need ['und'][0]['value']
		//I haven't figured out how to make this more robust yet...
		$participating = er_is_user_participating($user->uid, $collab_range);
		$status = $participating?'Participant':'Collaborator';
		$form_state['input']['field_er_collab_name']['und'][0]['value'] = $u->field_er_realname->value();
		
		// Earlier/original code	
		//$form_state['input']['field_er_collab_inst_ref']['und'] = array_pop($u->field_er_inst_ref->raw());
		
		// New Version - that resolves the error:
		//
		// modified by Chris - cjallen@epscor.unm.edu
		$arraypopvalue = $u->field_er_inst_ref->raw();
		$form_state['input']['field_er_collab_inst_ref']['und'] = array_pop($arraypopvalue);
		
		$form_state['input']['field_er_collab_user_status']['und'] = $status;
		$form_state['input']['field_er_collab_department_unit']['und'][0]['value'] = $u->field_er_department->value();
		unset($form_state['attached_user']);
		drupal_set_message(t('Please verify the auto-filled values below, then click save.'), 'warning');

	}
	//require_once(ER_INCLUDES_DIR . '/quick_add_institution.inc');//lazy load the code; less clutter in here + doesn't need to load on every page load.
	//_quick_add_inst_form_alter($form, $form_state, 'field_er_collab_inst_ref');
	//_quick_add_institution($form, $form_state);
	//_set_states_for_inst($form, 'field_er_collab_inst_ref');
	
	_quick_add_inst($form, $form_state, 'field_er_collab_inst_ref');
	//d($form, '$form');
	//d($form_state, '$form_state');
}

function _quick_add_inst(&$form, &$form_state, $inst_field_name = 'field_er_inst_ref'){
	$form_state['quick_add_inst_field_name'] = $inst_field_name;
	_set_states_for_inst($form, $inst_field_name);
	//$form_state['quick_add_inst_field_name'] = 'field_er_collab_inst_ref';
	$form['#validate'][] = '_load_quick_add_inst_validate';
	array_unshift($form['#submit'], '_load_quick_add_inst_process');
	$form[$inst_field_name]['#after_build'] = array('_remove_NA_option');
}

function _load_quick_add_inst_validate(&$form, &$form_state){
	require_once(ER_INCLUDES_DIR . '/quick_add_institution.inc');//lazy load the code; less clutter in here + doesn't need to load on every page load.
	_quick_add_inst_validate($form, $form_state);
}

function _load_quick_add_inst_process(&$form, &$form_state){
	require_once(ER_INCLUDES_DIR . '/quick_add_institution.inc');//lazy load the code; less clutter in here + doesn't need to load on every page load.
	_quick_add_inst_process($form, $form_state);
}

//This function is used to create the "smart form" functionality necessary
// for creating new institutions while editing another node (like a collaboration)
function _set_states_for_inst(&$form, $inst_field_name = 'field_er_inst_ref'){
	$addInst = array(
		':input[name="field_er_inst_add_bool[und]"]' => array('checked' => TRUE),
	); 
//  $addInstUnchecked = array(
//		':input[name="field_er_inst_add_bool[und]"]' => array('checked' => FALSE),
//	); 
	// These fields will be visible, and required
	$form['field_er_inst_name']['#states'] = array(
		'visible' => $addInst,
		'required' => $addInst,
	);
	//These fields will be visible but not required
	$form['field_er_inst_country']['#states'] =
	$form['field_er_inst_link']['#states'] =
	$form['field_er_inst_description']['#states'] =
	$form['field_er_inst_abbreviation']['#states'] = array(
		'visible' => $addInst,
	);
	//Make the institution field required/non-required based on the checkbox.
	$form[$inst_field_name]['#states'] = array(
		//'disabled' => $addInst,
		'optional' => array($addInst),
	);	
}

function autopopulate_collab_submit_callback(&$form, &$form_state){
	//d('form submitted');
	//d($form_state, '$form_state');
	//d($form, '$form submit');
	if ($uid = $form_state['values'][$form_state['autofill_ref_field']]['und'][0]['target_id'])
		$form_state['attached_user'] = $uid;
	$form_state['rebuild'] = TRUE;
}

// Collaboration
function er_form_er_collaboration_node_form_alter(&$form, &$form_state, $form_id){
	// d($form, 'er_form_er_collaboration_node_form_alter');
	
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

// Current and Pending Support
function er_form_er_proposal_node_form_alter(&$form, &$form_state, $form_id){
	$form['#validate'][] = '_states_validate'; //Automatically try to validate the form based on the #states.
	// d($form, 'er_form_er_proposal_node_form_alter');

	// Setting up variables that will be used
	$submitted = array(
		':input[name="field_er_proposal_status[und]"]' => array('value' => 'Submitted'),
	);
	$pending = array(
		':input[name="field_er_proposal_status[und]"]' => array('value' => 'Pending'),
	);
	$awarded = array(
		':input[name="field_er_proposal_status[und]"]' => array('value' => 'Awarded'),
	);
	$denied = array(
		':input[name="field_er_proposal_status[und]"]' => array('value' => 'Denied'),
	);	 
  $expired = array(
		':input[name="field_er_proposal_status[und]"]' => array('value' => 'Expired'),
	);
	  
	// The Date fields:
	// Submitted (asterisk not showing up for some reason)
	$form['field_er_proposal_submit']['#states'] = array(
	  'visible'=> array($submitted, $awarded, $expired, $pending, $denied),
	  'required' => array($submitted, $awarded, $expired, $pending, $denied),
	); 
	// Pending
	$form['field_er_proposal_pending']['#states'] = array(
	  'visible'=> $pending,
	  'required' => $pending,
	); 
	
	// Award Dates
	$form['field_er_proposal_date']['#states'] = array(
	  'visible' => array($awarded, $expired),
	  'required' => array($awarded, $expired),
	);
	// Award Amount (Textfield)
	$form['field_er_award_amount']['#states'] = array(
	  'visible' => array($awarded, $expired),
	  'required' => array($awarded, $expired, $pending, $denied),
	);
	
	// Denied Date
	$form['field_er_proposal_denied']['#states'] = array(
	  'visible' => $denied,
	  'required' => $denied,
	);	
	
	// $form['#validate'][] = '_proposal_validate';

	// This automatically attaches the user's name to this field...	
	//er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

/*
function _proposal_validate(&$form, &$form_state){
	//d($form_state, 'fs');
	switch ($form_state['values']['field_er_proposal_status'][LANGUAGE_NONE][0]['value']){
		// remember that the Date Submitted is required in the CCK field, so we don't have to check for that ~mike
		case "Pending":
			//selected pending but havent put a pending date
			if (empty($form_state['values']['field_er_proposal_pending'][LANGUAGE_NONE][0]['value']))
				form_set_error('field_er_proposal_pending', t('Please select a Date of Pending Notification as the Status is Pending.'));				
			break;
			
		case "Awarded":
		case "Expired":
			// awarded/expired but haven't put  award date, award amount
			if (empty($form_state['values']['field_er_proposal_date'][LANGUAGE_NONE][0]['value']))				
				form_set_error('field_er_proposal_date', t('Please select an Award date.'));
			if (empty($form_state['values']['field_er_award_amount'][LANGUAGE_NONE][0]['value'])){
				form_set_error('field_er_award_amount', t('Please select an Award Amount.'));
			}
			break;
			
		case "Denied":
			if (empty($form_state['values']['field_er_proposal_denied'][LANGUAGE_NONE][0]['value']))
				form_set_error('field_er_proposal_denied', t('Please select a Date of Denial Notification as the Status is Denied.'));				
			break;
	}
}
 */

// External Engagements
function er_form_er_engagement_node_form_alter(&$form, &$form_state, $form_id){
	_form_autogen_title($form, $form_state, $form_id);//Hide the title + autogenerate a title.
	if (_deny_direct_form_access($form)) return;
		
	//['#access'] = false;
	$form['field_er_cal_entity_reference']['und']['#disabled'] = true;
	
	// d($form, 'er_form_er_engagement_node_form_alter');
	//This hook hides the total fields on the external engagements form. The fields are hidden because they will be automatically filled during the node save. See node_logic.inc
	$cols = array("ari_fac", "ari_stu", "pui_fac", "pui_stu", "msi_fac", "msi_stu", "k12i_tec", "k12i_stud", "k12i_stut", "oth", "tot");
	
	foreach ($cols as $col){ //hide the sub-totals
		$form['field_er_'.$col.'_total']['#access'] = FALSE;
	}
	$form['field_er_tot_male']['#access'] = FALSE;
	$form['field_er_tot_female']['#access'] = FALSE;
	$form['field_er_tot_und']['#access'] = FALSE;
	$form['field_er_tot_urm']['#access'] = FALSE;
	//d($form_id, 'form id in er_form_alter');
	//d($form, 'form in er_form_alter');
}

// Highlights
function er_form_er_news_node_form_alter(&$form, &$form_state, $form_id){
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

// Institution
function er_form_er_institution_node_form_alter(&$form, &$form_state, $form_id){
	// d($form, 'er_form_er_institution_node_form_alter');
}

// Other Research Products
function er_form_er_other_product_node_form_alter(&$form, &$form_state, $form_id){
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

// Patent
function er_form_er_patent_node_form_alter(&$form, &$form_state, $form_id){
	$form['#validate'][] = '_states_validate'; //Automatically try to validate the form based on the #states.
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
	
	//field_er_patent_licensed
	  $patentChecked = array(
			':input[name="field_er_patent_licensed[und]"]' => array('checked' => TRUE),
		); 
	  $patentUnchecked = array(
			':input[name="field_er_patent_licensed[und]"]' => array('checked' => FALSE),
		); 
		  
	//Require field_er_patent_date
	$form['field_er_patent_date']['#states']  	  
	  = array(
	      'visible' => $patentChecked,
	      'required' => $patentChecked,
	  );
	//Require field_er_patent_licensee
	$form['field_er_patent_licensee']['#states']  	  
	  = array(
	      'visible' => $patentChecked,
	      'required' => $patentChecked,
	  );
	  
	  // $form['#validate'][] = 'patent_validate';
}

/*
function patent_validate(&$form, &$form_state){
	if ($form_state['values']['field_er_patent_licensed'][LANGUAGE_NONE][0]['value']){
		// if they've checked that it's licensed, then it needs to have a license date and licensee
		if (empty($form_state['values']['field_er_patent_date'][LANGUAGE_NONE][0]['value'])){
			form_set_error('field_er_patent_date', t('Please enter a license date'));
		}
		if (empty($form_state['values']['field_er_patent_licensee'][LANGUAGE_NONE][0]['value'])){
			form_set_error('field_er_patent_licensee', t('Please enter a licensee'));
		}
	}	
}
 */

// Presentation
function er_form_er_presentation_node_form_alter(&$form, &$form_state, $form_id){
	$form['#validate'][] = '_states_validate'; //Automatically try to validate the form based on the #states.
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
	
	// smart form to show "Other" text box...
	  $presOther = array(
			':input[name="field_er_type_of_presentation[und]"]' => array('value' => 'Other'),
		);
	  // These fields will be visible, and required
	  $form['field_er_other_pres']['#states'] = array(
	      'visible' => $presOther,
	      'required' => $presOther
	  );
	  // $form['#validate'][] = 'presentation_validate';	
}

/*
function presentation_validate(&$form, &$form_state){
	// Make sure they have "Other filled in" if they've selected Other
	if ($form_state['values']['field_er_type_of_presentation'][LANGUAGE_NONE][0]['value'] == 'Other' && empty($form_state['values']['field_er_other_pres'][LANGUAGE_NONE][0]['value'])){
		form_set_error('field_er_other_pres', t('Please enter a value for Other'));
	}
}
 */

// Publication
// Modified by Brent on 2013-01-09
function er_form_er_publication_node_form_alter(&$form, &$form_state, $form_id){
	$form['#validate'][] = '_states_validate'; //Automatically try to validate the form based on the #states.
	// d($form_id, 'form_id entering er_form_er_publication_node_form_alter');
	// d($form, 'form entering er_form_er_publication_node_form_alter');
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
	
// Setting up SmartForm variables that will be used
	$submitted = array(
		':input[name="field_er_publication_status[und]"]' => array('value' => '2'), 
	);
	$accepted = array(
		':input[name="field_er_publication_status[und]"]' => array('value' => '1'), 
	);
	$published = array(
		':input[name="field_er_publication_status[und]"]' => array('value' => '0'), 
	);
	$yesdata = array(
		':input[name="field_er_pub_data[und]"]' => array('value' => '1'),
	); 
	$where = array(
		':input[name="field_er_pub_archived[und]"]' => array('value' => '2'),
	); 
	// values for Publication Type field
	$journal = array(
		':input[name="field_er_publication_type[und]"]' => array(
			'value' => 'Journal Article'),
	); 
	$book = array(
		':input[name="field_er_publication_type[und]"]' => array(
			'value' => 'Book'),
	); 
	$proceedings = array(
		':input[name="field_er_publication_type[und]"]' => array(
			'value' => 'Conference Proceedings'),
	); 
	$magazine = array(
		':input[name="field_er_publication_type[und]"]' => array(
			'value' => 'Magazine Article'),
	); 
	$newspaper = array(
		':input[name="field_er_publication_type[und]"]' => array(
			'value' => 'Newspaper Article'),
	); 	
	
	

// The Date fields	
	// Submitted
	// Date Submitted field is visible & required if:
	// ... Publication Status contains values:  Submitted, Accepted, or Published
	// Date Submitted field is invisible & optional if:
	// ... Publication Status contain other values:  (eg. '-none-')

	$form['field_er_publication_submit']['#states'] = array(
	  'visible'=> array($submitted, $accepted, $published),
	  'required' => array($submitted, $accepted, $published),
	); 	
	// Published
	// Date Published, Abstract, NSF Ack fields are visible & required if:
	// ... Publication Status contains value:  Published
	// These fields are invisible & optional if:
	// ... Publication Status contain other values:  (eg. '-none-', Accepted)
	
	$form['field_er_publication_date']['#states'] =
	$form['body']['#states'] =
	$form['field_er_nsf_ack']['#states'] = array(
	  'visible' => $published,
	  'required' => $published,
	);	
// Data Collection	
	// Yes
	// 'Is it being archived?' field is visible & required if:
	// ... 'Are you collecting data?' contains value:  1|Yes
	// 'Is it being archived?' field is invisible & optional if:
	// ... 'Are you collecting data?' contains any other value:  (eg. 0|No)

	$form['field_er_pub_archived']['#states'] = array(
	  'visible'=> $yesdata,
	  'required' => $yesdata,
	); 		
	// 'What Archive?' field is visible & required if:
	// ... 'Is it being archived?' contains value:  2|Another Repository
	// 'What Archive?' field is invisible & optional if:
	// ... 'Are you collecting data?' contains other values:
	// ... (eg. 0|Not Yet, or 1|EPSCoR Repository)

	$form['field_er_pub_what_archive_']['#states'] = array(
	  'visible'=> $where,
	  'required' => $where,
	);

// Publication Information group
	// Journal Name field is required if:
	// ... 'Publication Type' contains values: Journal Article, Book, ...
	// Journal Name field is optional if:
	// ... 'Publication Type' contains any other value.

	$form['field_er_journal_name']['#states'] = array(
	  'required'=> array($journal, $book, $proceedings, $magazine, $newspaper),
	);
	
	// d($form, 'form exiting er_form_er_publication_node_form_alter');
	// $form['#validate'][] = 'publication_validate';
	
	// we want to remove the N/A option from this radio list
	$form['field_er_pub_archived']['#after_build'] = array('_remove_NA_option');	
}

/*
function publication_validate(&$form, &$form_state){
	// d($form_state, 'fs');
	// This is sort of silly because there are only three values that it can be, all of which make it required
	if (empty($form_state['values']['field_er_publication_submit'][LANGUAGE_NONE][0]['value'])){
		form_set_error('field_er_publication_submit', t('Please enter a date of submission'));
	}
	
	// If Published, make sure three fields are included
	if ($form_state['values']['field_er_publication_status'][LANGUAGE_NONE][0]['value'] == 0){
		// date of publication
		if (empty($form_state['values']['field_er_publication_date'][LANGUAGE_NONE][0]['value'])){
			form_set_error('field_er_publication_date', t('Please enter the date of publication.'));
		}
		// body
		if (empty($form_state['values']['body'][LANGUAGE_NONE][0]['value'])){
			form_set_error('body', t('Please enter an abstract.'));
		}
		// nsf acknowledgement
		if (empty($form_state['values']['field_er_nsf_ack'][LANGUAGE_NONE][0]['value'])){
			form_set_error('field_er_nsf_ack', t('Please enter acknowledgement information.'));
		}		
	}
	
	// If collecting data, ask if it's being archived (required)
	if ($form_state['values']['field_er_pub_data'][LANGUAGE_NONE][0]['value'] == 1){
		// if they don't have anything for archived, stop them
		if (empty($form_state['values']['field_er_pub_archived'][LANGUAGE_NONE][0]['value'])){
			form_set_error('field_er_pub_archived', t('Please select whether it\'s archived.'));
		}
		// if they've selected Another Repository, have the textbox required
		if ($form_state['values']['field_er_pub_archived'][LANGUAGE_NONE][0]['value'] == 2 && empty($form_state['values']['field_er_pub_what_archive_'][LANGUAGE_NONE][0]['value'])){
			form_set_error('field_er_pub_what_archive_', t('Please enter the name of the archive.'));
		}		
	}
	
	// Here's a list of options that should make the Journal Title required
	$journal_title = array('Journal Article', 'Book', 'Conference Proceedings', 'Magazine Article', 'Newspaper Article');
	// if the value is one of those, and the journal title is empty, let them know
	if (in_array($form_state['values']['field_er_publication_type'][LANGUAGE_NONE][0]['value'], $journal_title) && empty($form_state['values']['field_er_journal_name'][LANGUAGE_NONE][0]['value'])){
		form_set_error('field_er_journal_name', t('Please enter the name of the Journal, Book, Magazine, or Newspaper.'));
	}
	
	// if ($form_state['values']['field_er_publication_status'][LANGUAGE_NONE][0]['value'])
}
 */

// Research Project
function er_form_er_project_node_form_alter(&$form, &$form_state, $form_id){
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

// User Event (DEPRECATED)
//function er_form_er_user_event_node_form_alter(&$form, &$form_state, $form_id){
//	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
//}

// User Graduation Event (DEPRECATED)
//function er_form_er_user_event_instance_node_form_alter(&$form, &$form_state, $form_id){
//	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
//}

// User Profile Registration 
//	This isn't a content type, instead, it's where people go to register:
//		Example: http://wwwdev.epscor.hawaii.edu/user/register
// This is "Quick add user" basically
function er_form_user_register_form_alter(&$form, &$form_state, $form_id){
	// d($form, 'er_form_user_register_form_alter');
	
	// all of this is to make the add user form quick without all the required fields having to be entered
	$form['field_er_fname']['#weight'] = -12;
	$form['field_er_lname']['#weight'] = -11;
	
	$form['field_er_months_of_effort']['#access'] = FALSE;
	$form['field_er_committees']['#access'] = FALSE;
	$form['field_er_mentoring_sign_off']['#access'] = FALSE;
	$form['field_er_inst_description']['#access'] = FALSE;
	$form['field_er_most_sen_proj_role']['#access'] = FALSE;
	$form['field_er_components']['#access'] = FALSE;
	$form['field_er_paid']['#access'] = FALSE;
	$form['field_er_160hours']['#access'] = FALSE;
	$form['field_er_most_sen_proj_role']['#access'] = FALSE;
	$form['field_er_leadership_team']['#access'] = FALSE;
	$form['account']['notify']['#default_value'] = TRUE;
	// $form['account']['name']['#access'] = FALSE;
	$form['account']['pass']['#required'] = FALSE;
	$form['account']['pass']['#description'] .= ' You may leave it blank to create a random password.';
	unset($form['#groups']);
	unset($form['#fieldgroups']);
	unset($form['#group_children']);
	$form['flag']['#access'] = FALSE;
	

}

//This class processes the #states values in a form, checks the conditions, and verified the required status of appropriate fields.
//@author: Andrew W.
//@email: shixish@gmail.com
class states_validate {
	public function states_validate(&$form, &$form_state){
		$this->form = $form;
		//d($form, '$form');
		$this->form_state = $form_state;
		foreach ($form as $field_name=>&$field){
			if (substr($field_name, 0, 6) == 'field_' && isset($field['#states']) && $states = $field['#states']){//just inspect the fields
				//d($states, 'Rule:'.$field_name);
				foreach (array('required', 'optional') as $type){//for now we are only concerned with the 'required' states.
					if (isset($states[$type])){
						//there is some variation in the way you can set up the states...
						if (is_array($states[$type]) && count($states[$type]) > 1){
							foreach ($states[$type] as $rule){
								$this->check_rule($type, $field_name, $rule);
							}
						}else{
							$this->check_rule($type, $field_name, $states[$type]);
						}
					}
				}
			}
		}
	}
	
	//Process a rule.
	//The function parses, and checks the rule condition, if the condition is satisfied, it then attempts to enforce the rule.
	//@param string $type is the state type: required, optional, visible, etc...
	//@param string $field_name is the full name of the field to which the rule applies.
	//@param array $rule The condition that must be met for the rule to be enforced
	//  Ex: $rule = array(':input[name="field_er_most_sen_proj_role[und]"]' => array('value' => 'Evaluation')));
	protected function check_rule($type, $field_name, &$rule){
		if (count($rule) == 1){
			$key = key($rule);
			try{
				list($condition_field_name, $lang) = $this->parse_selector($key);
				$check = array_pop($rule);
				try{
					$values = $this->form_state['values'][$condition_field_name][$lang];
					$condition_is_met = $this->check_condition($check, $values);
					//Field is either required, or non-optional:
					if (($type == 'required' && $condition_is_met) || ($type == 'optional' && !$condition_is_met)){
						if (!$this->check_field_required($field_name, $lang)){//field is empty
							$title = $this->get_field_title($field_name, $lang);
							if (!$title) watchdog('er', 'States validation error: Unable to find the field title for: '.$field_name, array(), WATCHDOG_ERROR);
							form_set_error($field_name, t('@name field is required.', array('@name'=>$title)) );
						}
					}
				}catch (Exception $e){
					watchdog('er', 'States validation error: Unable to verify condition statement for field @field', array('@field'=>$field_name), WATCHDOG_WARNING);
				}
			}catch (Exception $e){
				if (intval($key) === 0){
					$rule = array_pop($rule);
					$this->check_rule($type, $field_name, $rule);//try it again, one step deeper.
				}else{
					watchdog('er', 'States validation error: @key unknown in field @field', array('@key'=>$key, '@field'=>$field_name), WATCHDOG_WARNING);
				}
			}
		}else{
			watchdog('er', 'States validation error: Unable to process states for @field. Unexpected formatting...', array('@field'=>$field_name), WATCHDOG_WARNING);
		}
	}
	
	//Function checks to make sure that a field satisfies the required/non-optional rule.
	//@param string $field_name The full name of the field.
	//@param string $lang The language the field is using. Most of the time this is 'und'.
	//@returns bool True if the field is filled, False if empty/unset.
	protected function check_field_required($field_name, $lang = 'und'){
		$values = $this->form_state['values'][$field_name][$lang];
		if (is_array($values)){
			foreach ($values as $value){
				if (isset($value['value'])){
					if (is_array($value['value']) || strlen($value['value'])>0)//The value can be an array for date fields.
						return TRUE;
				}else if (isset($value['target_id']) && !empty($value['target_id'])){
					return TRUE;
				}else if (isset($value['fid']) && !empty($value['fid'])){
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	
	//This function is responsible for checking if the rule condition has been met.
	//@param array $check This refers to the check that states means to make...
	//  Ex: $check = array('value' => 'Evaluation');
	//@param array $values The contents of $this->form_state['values'][field_name][field_language]
	//@returns bool Indicates whether the condition has been met or not.
	protected function check_condition($check, $values){
		//d($check, '$check');
		if (isset($check['value'])){
			$check_value = $check['value'];
		}else if (isset($check['checked'])){
			$check_value = $check['checked']?1:0;
		}
		if (is_array($values)){
			foreach ($values as &$value){
				if (isset($value['value'])){
					//d($value['value'], $field_name);
					if ($value['value'] == $check_value)
						return TRUE;
				}
			}
			return FALSE;
		}
	}
	
	//This function breaks up the states selector string.
	//@param string $string The string to break up.
	// Ex: $string = ':input[name="field_er_most_sen_proj_role[und]"]';
	//@returns array Use this: list($field_name, $lang);
	protected function parse_selector($string){
		if (substr($string, 0, 13) == ':input[name="'){
			$select = substr($string, 13, -2);
			$bracket = strpos($select, '[');
			$field_name = substr($select, 0, $bracket);
			$lang = substr($select, $bracket+1, -1);
			return array($field_name, $lang);
		}else{
			throw new Exception('Unable to parse selector');
		}
	}
	
	//Attempt to get the display title of a field, used in displaying error messages.
	protected function get_field_title($field_name, $lang = 'und'){
		try{
			return $this->form[$field_name][$lang]['#title'];
		}catch(Exception $e){
			return $field_name;
		}
	}
}

function _states_validate(&$form, &$form_state){
	new states_validate($form, $form_state);
}


// User Profile
function er_form_user_profile_form_alter(&$form, &$form_state, $form_id){
	$form['#validate'][] = '_states_validate'; //Automatically try to validate the form based on the #states.
	
	// d($form, 'er_form_user_profile_form_alter');
	$form['field_er_faculty_any_support']['#after_build'] = array('_remove_NA_option');
	
	// Faculty Participant (or Equivalent)
	$faculty_part = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'Faculty participant (or equivalent)'),
		);	  
	
	// Technical support staff
	$tech_support_staff = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'Technical support staff'),
		);
	
	// Non-Technical support staff
	$non_tech_support_staff = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'Non-technical support staff'),
		);
		
	// EPSCoR Leadership Team
	$epscor_leadership_team = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'EPSCoR Leadership Team'),
		);
		
	// Other Staff
	$other_epscor_staff = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'Other'),
		);
	
	// Evaluation Staff
	$evaluation_staff = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'Evaluation'),
		);	
				
	// Undergraduate selection
	
	$undergrad = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'Undergraduate student'),
		);
	
	//anticipated undergraduate completion
	$form['field_er_ant_under']['#states'] = array(
		'visible' => $undergrad, 
		'required' => $undergrad,
	);
	
	// Ask them "Do you or anyone in your research group receive salary support from EPSCoR?" if they're faculty
	//  (this was decided upon in Utah, Summer of 2013)
	$form['field_er_faculty_any_support']['#states'] = array(
		'visible' => $faculty_part,
		'required' => $faculty_part,
	);	
	
	// Graduate Student
	$grad_student = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array(
		'value' => 'Graduate student'),
	);
	//anticipated masters completion
	$form['field_er_ant_masters']['#states'] = array(
		'visible' => $grad_student,
	);
		
	// Post Doctorate selection
	$post_doc = array(
		':input[name="field_er_most_sen_proj_role[und]"]' => array('value' => 'Post Doc'),
	);
	
	// anticipated doctorate completion
	$form['field_er_ant_doctor']['#states'] = array(
		'visible' => $grad_student,
	);
	
	////// Actual Undergrad, masters, post-doc completion fields /////
	
	// actual undergraduate completion
	$form['field_er_act_under']['#states'] = array(
	//'visible' => array($undergrad, $grad_student), <--- this is the original ER code, 
	// NM removes the field 'undergrad' graduation date for Graduate Students
	// mjdaniel@nmepscor.unm.edu informed NM developers that this field is unnecessary for data collection in the outputs table.
	'visible' => $undergrad,	 
	);
	// actual masters completion
	$form['field_er_act_masters']['#states'] = array(
		'visible' => $grad_student,
	);
	
	// actual doctorate completion
	$form['field_er_act_doctor']['#states'] = array(
		'visible' => $grad_student,
	);	
	
	$form['field_er_mentoring_plan_ul']['#states'] = array(
			'visible' => $post_doc,
	);	
	
	//Certifications fieldsets only available to Graduates, Undergraduates	

	//This is a markup field, don't make it required. It'll screw up the validation.
	$form['field_er_have_you_comp_the_rcr']['#states'] = array(
		'visible' => array($undergrad, $grad_student),	
	);

	// RCR completion is only for undergrads and grads
	$form['field_er_rcr_completion_bool']['#states'] = array(
		'visible' => array($undergrad, $grad_student),
		//'required' => array($undergrad, $grad_student),
	);	
	
	// RCR Certification fields only if "I have completed RCR certifications" is checked
	$completed_rcr = array(
		':input[name="field_er_rcr_completion_bool[und]"]' => array('checked' => TRUE),
	);
	
	$form['field_er_rcr_online']['#states'] = 
	$form['field_er_rcr_in_person']['#states'] =
	$form['field_er_additional_doc']['#states'] =
	$form['field_er_rcr_online_ul']['#states'] = 
	$form['field_er_rcr_inperson_ul']['#states'] = array(
		'visible' => $completed_rcr,
		'required' => $completed_rcr,
	);
	
	// Data collection Tab
	// show all the fields if they click the checkbox that " I do/not plan to be collecting data through my research. "
	$collection = array(
		':input[name="field_er_collecting_data_bool[und]"]' => array('checked' => TRUE),
	);
	
	$form['field_er_data_frequency']['#states'] = 
	$form['field_er_data_url']['#states'] = 
	$form['field_er_data_submit_date']['#states'] = array(
		'visible' => $collection,
	);
	
	// required fields "Description of Data"
	$form['field_er_type_of_data']['#states'] = array(
	'required' => $collection, 
	'visible' => $collection);
	
	// change labels on participant date range
	$form['field_er_participating_date']['#after_build'] = array('er_date_label');  //field name of the date
	
	_quick_add_inst($form, $form_state, 'field_er_inst_ref');	
	
	$form['field_er_inst_ref']['und']['#type'] = 'radios'; //make this field single select.
	$form['field_er_inst_ref']['und']['#default_value'] = array_pop($form['field_er_inst_ref']['und']['#default_value']); //fixes the selection
	
	// supervisors Field only available to Graduates, Undergraduates
	$form['field_er_supervisor']['#states'] = array(
		'visible' => array($undergrad, $grad_student),
		'required' => array($undergrad, $grad_student),
	);
		  
	// advisor field only available to Students
		$form['field_er_advisor']['#states'] = array(
		'visible' => array($post_doc, $undergrad, $grad_student),
		'required' => array($post_doc, $undergrad, $grad_student),
	);
	
	// mentoring plan fields only available to Post-Docs
	$form['field_er_mentoring_sign_off']['#states'] = 
	$form['field_er_mentoring_plan_ul']['#states'] = array(
		'visible' => $post_doc,
		'required' => $post_doc,
	);		
	
	// Set up Paid by EPSCoR Variables
	$paid_by_epscor = array(
	 ':input[name="field_er_paid[und]"]' => array('value' => '1'),
	);
		
	$not_paid_by_epscor = array(
	 ':input[name="field_er_paid[und]"]' => array('value' => '0'),
	);
		
	// if Faculty, and Paid = Yes ---> Show "How many months of direct funding ..." field		
	// used 'invisible' here to rule out all Roles other than Faculty AND not Paid by EPSCoR
	$form['field_er_months_of_effort']['#states'] = array(
		'invisible' => array(
		$tech_support_staff, $undergrad, $grad_student, $epscor_leadership_team, 
		$other_epscor_staff, $post_doc, $non_tech_support_staff, $evaluation_staff, $not_paid_by_epscor),
	);	
	
	// if Faculty, and Paid = No ---> Show "Provide a List of all funding sources ..." field
	// used 'invisible' here to rule out all Roles other than Faculty AND Paid by EPSCoR
	$form['field_er_list_funding_sources']['#states'] = array(
		'invisible' => array(
		$tech_support_staff, $undergrad, $grad_student, $epscor_leadership_team, 
		$other_epscor_staff, $post_doc, $non_tech_support_staff, $evaluation_staff, $paid_by_epscor),
	);	
		
	// if Participate in EPSCoR > 160 Hours display "Please select nearest person.."" field
	$participant_more_than_sixty = array(
		':input[name="field_er_160hours[und]"]' => array('value' => '1'),
	);
		
	$form['field_er_person_month']['#states'] = array(
		'visible' => $participant_more_than_sixty,
		'required' => $participant_more_than_sixty,
	);
	
	// $form['#validate'][] = 'user_profile_validate';
}

/*
function user_profile_validate(&$form, &$form_state){
	d($form_state, 'fs');
	$fs = $form_state['values'];
	
	// if they're an undergrad and have not filled in anticipated graduation date, throw validation error	
	if (fs_val($fs, 'field_er_most_sen_proj_role') == 'Undergraduate student' && fs_empty($fs, 'field_er_ant_under')){		
		form_set_error('field_er_ant_under', t('Please enter anticipated graduation date.'));
	}

	// if they're a grad student, make sure they have anticipated graduation date
	if (fs_val($fs, 'field_er_most_sen_proj_role') == 'Graduate student' && fs_empty($fs, 'field_er_ant_masters')){		
		form_set_error('field_er_ant_masters', t('Do you or anyone in your research group receive salary support from EPSCoR?'));
	}
	
	// if they're a post-doc student, make sure they have anticipated graduation date
	if (fs_val($fs, 'field_er_most_sen_proj_role') == 'Graduate student' && fs_empty($fs, 'field_er_ant_masters')){		
		form_set_error('field_er_ant_masters', t('Do you or anyone in your research group receive salary support from EPSCoR?'));
	}
		
		
	// if they're faculty, ask if they receive salary support from EPSCoR
	if (fs_val($fs, 'field_er_most_sen_proj_role') == 'Faculty participant (or equivalent)' && fs_empty($fs, 'field_er_faculty_any_support')){		
		form_set_error('field_er_faculty_any_support', t('Do you or anyone in your research group receive salary support from EPSCoR?'));
	}
	
}
 */

// this function returns form_state values, to cut down on repetition
function fs_val(&$fs, $field){
	return $fs[$field][LANGUAGE_NONE][0]['value'];
}

function fs_empty($fs, $field){
	return empty($fs[$field][LANGUAGE_NONE][0]['value']);
}

/*
 * This function is used because we want our "Smart Forms" to be able to hide/show an entire fieldset, and this is needed
 */
function er_field_group_build_pre_render_alter(&$element){
	// d($element, 'element');
	
	// This IF block was used for the User Profile
	if (isset($element['group_er_certs']) && isset($element['group_er_data'])){
		$element['group_er_certs']['#states'] = array(
	    	'visible' => array(
	      	':input[name="field_er_rcr_completion_bool[und]"]' => array('value' => TRUE),
	    	)
	    );	
		$element['group_er_data_collection']['#states'] = array(
	    	'visible' => array(
	      	':input[name="field_er_collecting_data_bool[und]"]' => array('checked' => TRUE),
	    	)
	    );			
	}
	
	// if (isset($element['group_er_profile']['group_er_inst'])){
		// d('findthis');
		// $element['group_er_profile']['group_er_inst']['#attributes']['class'][] = 'findthis';
		// $element['group_er_profile']['group_er_inst']['#states'] = array(
	    	// 'required' => array(
	      	// ':input[name="field_er_inst_add_bool[und]"]' => array('checked' => TRUE),
	    	// )
	    // );			
		// d($element['group_er_profile']['group_er_inst'], 'inst fieldgroup');
	// }
		
	// This IF block is used for the Publications Content Type		
	if (isset($element['group_er_journal'])){
		// Make Publication Info group visible/invisible depending on the selection
		
		// values for Publication Type field
		$none = array(
			':input[name="field_er_publication_type[und]"]' => array(
				'value' => '_none'),
		);
		$abstract = array(
			':input[name="field_er_publication_type[und]"]' => array(
				'value' => 'Abstract'),
		); 
		$thesis = array(
			':input[name="field_er_publication_type[und]"]' => array(
				'value' => 'Thesis/Dissertation'),
		); 
		$internet = array(
			':input[name="field_er_publication_type[und]"]' => array(
				'value' => 'Internet Dissemination'),
		); 
		// 'Publication Information' group is invisible if:
		// ... 'Publication Type' contains values:
		// ...     '- None -', Abstract, Thesis, I-net Diss.
		// 'Publication Information' group is visible if:
		// ... 'Publication Type' contains values other values:
		// ...     (eg. Journal Article, Book)
		
			// This was the old version that worked with field_group version 1.3
		// The patch includes the 'id' element that gets stripped for the fieldset 'fieldgroup' cck field
				
		//$element['group_er_journal']['#states'] = array(
			//'invisible'=> array($none, $abstract, $thesis, $internet),
		//);

		$element['group_er_journal'] += array(
		'#states' => array(
			'invisible'=> array($none, $abstract, $thesis, $internet),
			),
			'#id' => 'group_er_journal',
		);
	}

}


// Todd asked that we have more specific labels, so here they are
function er_date_label($form, &$form_state) {
  $form['und'][0]['value']['date']['#title'] = 'Start Date';
  $form['und'][0]['value2']['date']['#title'] = 'End Date';
  return $form;
}

/*
 * This function removes the N/A option for the list of Institutions. We don't want it, and can only removing by making the field required,
 * 		however, we can't do that because #states are involved here, and it's not properly validating. Buggy otherwise.
 * 
 * Called from er_form_er_collaboration_node_form_alter
 */
function _remove_NA_option($form, &$form_state){
	// this renames the N/A
	// $form['und']['_none']['#title'] = "Institution is not listed (add a new Institution)";
	// this removes the N/A option
	if (isset($form['und']['_none'])) unset($form['und']['_none']);
	return $form;
}

// Logic used to auto-fill the username into entity reference fields on a number of forms...
// @author Andrew Wessels (awessels@hawaii.edu)
function er_set_default_user_entity_reference_field(&$form, &$form_state, $form_id){
	//d($form, 'er_form_er_news_node_form_alter');
	
	// Autofill the user's name in the Entity Reference field...
	if (empty($form['field_er_user_entity_reference']['und'][0]['target_id']['#default_value'])){ //only fill it if the field is blank.
		$form['field_er_user_entity_reference']['und'][0]['target_id']['#default_value'] = get_real_name(true);
		$form['field_er_user_entity_reference']['und']['#description'] = '<em style="color:red;display:block;">Notice: Your name has been automatically attached.</em>'.$form['field_er_user_entity_reference']['und']['#description'] ;//notify the user that we autofilled your name in.
	}
}

/**
 * Gets a user's full name (first + last)
 *
 * @author Rusty Keele <rkeele@uen.org>
 * @param bool $add_uid Should the function return the user ID in parenthesis? (example: John Doe (5) )
 * @return string Returns full name, or full name with user ID
 */
function get_real_name($add_uid = false) {
	$realname = '';
	$ent_ref_val = '';
	global $user;
	$profile = user_load($user->uid);
	
	if( isset($profile->field_er_realname['und']['0']['value']) ) {
		$realname = (string) $profile->field_er_realname['und']['0']['value'];
		$ent_ref_val = "$realname ({$profile->uid})";
	}
	
	if($add_uid) {
		return $ent_ref_val;
	} else {
		return $realname;
	}
}

function usertest_form_alter(&$form, &$form_state, $form_id) { 
	drupal_set_message('THE FORM ID IS:'.$form_id);
 } 



/**
 * Hide the title field, and generate a generic title.
 * hook_node_presave is then used to create a title based on the data given in the form.
 *
 * @author Brent Sadler <brent.sadler@unh.edu>
 * Modified by Andrew W. <shixish@gmail.com> 7-15-13
 *
 * See: file:node_logic.inc function:er_node_presave
 */
function _form_autogen_title(&$form, &$form_state, $form_id) {
	$form['title']['#type'] = 'hidden';
	$form['title']['#required'] = FALSE;
	$form['title']['#default_value'] = "Generic title {$form['created']['#value']}";
}


/*
 *
 * This function will blank out a form, and replace it with a brief error message.
 * This is used to direct the user to the correct workflow in cases where fields should be automatically filled out for them.
 *
 */
function _deny_direct_form_access(&$form){
	switch($form['#bundle']){
		case 'er_engagement':
			$reference_field = 'field_er_cal_entity_reference';
			$link = '<strong>'.l('Calendar Event', 'node/add/er-event').'</strong>';
			$message = t("External Enagement content cannot be created stand-alone. You must first create a !link, then attach External Engagement content to the event. See the documentation above.", array('!link'=>$link));
			break;
		case 'er_collaborators':
			$reference_field = 'field_er_collab_ref';
			$link = '<strong>'.l('Collaboration', 'node/add/er-collaboration').'</strong>';
			$message = t("External Collaborators content cannot be created stand-alone. You must first create a !link, then attach External Collaborators to the Collaboration. See the documentation above.", array('!link'=>$link));
			break;
	}
	if (
		$reference_field &&
		//it could be in one of two places...
		!$form[$reference_field]['und']['#default_value'] &&
		!$form[$reference_field]['und'][0]['target_id']['#default_value']
	){//If the calendar event isn't filled in, don't even show the form.
		$form = array(
			'#form_id' => $form['#form_id'],
			'#build_id' => $form['#build_id'],
			'#id' => $form['#id'],
			'#token' => $form['#token'],
			'#markup' => $message,
		);
		return true;
	}
	else
		return false;
}

