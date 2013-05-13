<?php
/*
 *  Generic form alter
 * 		See: http://api.drupal.org/api/drupal/modules!system!system.api.php/function/hook_form_alter/7
 */

function er_form_alter(&$form, &$form_state, $form_id){
	// d($form_id, 'form id in er_form_alter');
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
}

// External Collaborators
function er_form_er_collaborators_node_form_alter(&$form, &$form_state, $form_id){
	_form_autogen_title($form, $form_state, $form_id);//Hide the title + autogenerate a title.
	$form['field_er_collab_ref']['#disabled'] = TRUE;
	$form['field_er_collab_inst_ref']['#disabled'] = TRUE;
	//d($form, 'er_form_er_collaborators_node_form_alter');
}

// Collaboration
function er_form_er_collaboration_node_form_alter(&$form, &$form_state, $form_id){
	// d($form, 'er_form_er_collaboration_node_form_alter');
	_set_states_for_inst($form);	  
	// $form['#submit'][] = '_create_inst';
	// Using array_unshift makes sure this goes at the very beginning
	//	Which is useful because we're unsetting items
	array_unshift($form['#submit'], '_create_inst');
	$form['field_er_inst_ref']['#after_build'] = array('_remove_NA_inst');
	
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

//This function is used to create the "smart form" functionality necessary
// for creating new institutions while editing another node (like a collaboration)
function _set_states_for_inst(&$form){
	  $addInst = array(
			':input[name="field_er_inst_add_bool[und]"]' => array('checked' => TRUE),
		); 
	  $addInstUnchecked = array(
			':input[name="field_er_inst_add_bool[und]"]' => array('checked' => FALSE),
		); 
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
	  $form['field_er_inst_ref']['#states']  	  
			= array(
	      'optional' => $addInst,
	      'required' => $addInstUnchecked,
	  );
}

// Current and Pending Support
function er_form_er_proposal_node_form_alter(&$form, &$form_state, $form_id){
	// d($form, 'er_form_er_proposal_node_form_alter');

	// Setting up variables that will be used
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
	// Pending
	$form['field_er_proposal_pending']['#states'] = array(
	  'visible'=> array($pending, $awarded, $denied, $expired),
	  'required' => array($pending, $awarded, $expired),
	); 
	
	// Award Dates
	$form['field_er_proposal_date']['#states'] = array(
	  'visible' => array($awarded, $expired),
	  'required' => array($awarded, $expired),
	);
	// Award Amount (Textfield)
	$form['field_er_award_amount']['#states'] = array(
	  'visible' => array($awarded, $expired),
	  'required' => array($awarded, $expired),
	);
	
	// Denied Date
	$form['field_er_proposal_denied']['#states'] = array(
	  'visible' => $denied,
	  'required' => $denied,
	);	

	// This automatically attaches the user's name to this field...	
	//er_set_default_user_entity_reference_field($form, $form_state, $form_id);
}

// Current and Pending Support - Effort
function er_form_er_effort_node_form_alter(&$form, &$form_state, $form_id){
	_form_autogen_title($form, $form_state, $form_id);//Hide the title + autogenerate a title.
	// d($form, 'er_form_er_paid_node_form_alter');
}

// Current and Pending Support - Funded Effort
function er_form_er_paid_node_form_alter(&$form, &$form_state, $form_id){
	_form_autogen_title($form, $form_state, $form_id);//Hide the title + autogenerate a title.
	// d($form, 'er_form_er_paid_node_form_alter');
}

// External Engagements
function er_form_er_engagement_node_form_alter(&$form, &$form_state, $form_id){
	_form_autogen_title($form, $form_state, $form_id);//Hide the title + autogenerate a title.
	if (!$form['field_er_cal_entity_reference']['und']['#default_value'])//If the calendar event isn't filled in, don't even show the form.
		$form['#access'] = false;
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
}

// Presentation
function er_form_er_presentation_node_form_alter(&$form, &$form_state, $form_id){
	er_set_default_user_entity_reference_field($form, $form_state, $form_id);
	
	// smart form to show "Other" text box...
	  $presOther = array(
			':input[name="field_er_type_of_presentation[und]"]' => array('value' => 'Other'),
		);
	  // These fields will be visible, and required
	  $form['field_er_other_pres']['#states'] = array(
	      'visible' => $presOther
	  );
	
}

// Publication
// Modified by Brent on 2013-01-09
function er_form_er_publication_node_form_alter(&$form, &$form_state, $form_id){
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
	// ... Publication Status contains value:  Submitted
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
}

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
function er_form_user_register_form_alter(&$form, &$form_state, $form_id){
	// d($form, 'er_form_user_register_form_alter');
}

// User Profile
function er_form_user_profile_form_alter(&$form, &$form_state, $form_id){
	// d($form, 'er_form_user_profile_form_alter');
	
// Faculty Participant (or Equivalent)
	$faculty_part = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'Faculty participant (or equivalent)'),
	  );
	
// Technical support staff
	$tech_support_staff = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'Technical support staff'),
	  );

// Non-Technical support staff
	$non_tech_support_staff = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'Non-technical support staff'),
	  );
	  
// EPSCoR Leadership Team
	$epscor_leadership_team = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'EPSCoR Leadership Team'),
	  );
	  
// Other Staff
	$other_epscor_staff = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'Other'),
	  );

// Evaluation Staff
	$evaluation_staff = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'Evaluation'),
	  );	
	    	
// Undergraduate selection
	$undergrad = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'Undergraduate student'),
	  );
	//anticipated undergraduate completion
	$form['field_er_ant_undergrad_ca1']['#states'] = array(
	  'visible' => $undergrad, 
	  'required' => $undergrad,
	);
	
// Graduate Student
	$grad_student = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array(
		'value' => 'Graduate student'),
	);
	$form['field_er_ant_mast_grad_ca1']['#states'] = array(
	  'visible' => $grad_student,
	  'required' => $grad_student,
	);
	//anticipated masters completion
	$form['field_er_ant_mast_grad_ca1']['#states'] = array(
	  'visible' => $grad_student,
	);
		
// Post Doctorate selection
	$post_doc = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => array('value' => 'Post Doc'),
	);
	//anticipated doctorate completion
	$form['field_er_ant_doct_completion_ca1']['#states'] = array(
	  	'visible' => $post_doc,$grad_student,
	  	'required' => $post_doc,$grad_student,
	);
	
	////// Actual Undergrad, masters, post-doc completion fields /////

	//actual undergraduate completion
	$form['field_er_act_undergrad_grad_ca1']['#states'] = array(
	  	'visible' => array($undergrad, $grad_student, $post_doc),
	);	
	//actual masters completion
	$form['field_er_act_mast_grad_ca1']['#states'] = array(
	 	'visible' => array($grad_student,$post_doc),
	);
	//actual doctorate completion
	$form['field_er_act_doctl_complet_ca1']['#states'] = array(
	 	'visible' => $post_doc,
	);
	$form['field_er_ant_doct_completion_ca1']['#states'] = 
	$form['field_er_mentoring_plan_ul']['#states'] = 
	$form['field_er_mentoring_plan_ul']['#states'] = array(
	  	'visible' => $post_doc,
	);
	
	//Certifications fieldsets only available to Graduates, Undergraduates	

	$form['field_er_have_you_comp_the_rcr']['#states'] = array(
		'visible' => array($undergrad, $grad_student),
		'required' => array($undergrad, $grad_student),
	);
	
	$form['field_er_rcr_completion_bool']['#states'] = array(
		'visible' => array($undergrad, $grad_student),
		//'required' => array($undergrad, $grad_student),
	);
	
	// RCR Certification fields only if "I have completed RCR certifications" ic checked
	$completed_rcr = array(
		'input:[name="field_er_rcr_completion_bool[und]"]' => array('checked' => TRUE),
	);
		$form['field_er_rcr_online_ca1']['#states'] = 
		$form['field_er_rcr_in_person_ca1']['#states'] =
		$form['field_er_additional_doc']['#states'] =
		$form['field_er_rcr_online_ul']['#states'] = 
		$form['field_er_rcr_inperson_ca1']['#states'] = 
		$form['field_er_nsf_grant_signatory']['#states'] = 
		$form['field_er_rcr_inperson_ul']['#states'] 
		= array(
			'visible' => $completed_rcr, $grad_student, $undergrad,
			'required' => $completed_rcr, $grad_student, $undergrad,
	);
	
	// Data collection Tab -- 
	// show all the fields if they click the checkbox that "I will be collecting data"
	$collection = array(
		':input[name="field_er_collecting_data_bool[und]"]' => array('checked' => TRUE),
	  ); 

	$form['field_er_data_frequency']['#states'] = 
	$form['field_er_data_url']['#states'] = 
	$form['field_er_data_submit_date']['#states'] = array(
	  'visible' => $collection,
	);
	
	//required fields
	$form['field_er_type_of_data']['#states'] = array(
	'required' => $collection, 
	'visible' => $collection);
	
	// change labels on participant date range
	$form['field_er_participant_date']['#after_build'] = array('er_date_label');  //field name of the date
	
	_set_states_for_inst($form);		  	  
	// if (empty($form['#submit'])) 
		// $form['#submit'][] = '_create_inst';
	// else
	
	// Using array_unshift makes sure this goes at the very beginning
	//	Which is useful because we're unsetting items		
	array_unshift($form['#submit'], '_create_inst');
	
	// $form['#validate'][] = '_create_inst';
	// array_push($form['#validate'], '_create_inst');
	$form['field_er_inst_ref']['#after_build'] = array('_remove_NA_inst');
	
	$form['field_er_inst_ref']['und']['#type'] = 'radios';//make this field single select.
	
//Supervisors Field only available to Graduates, Undergraduates
		$form['field_er_supervisor']['#states'] = array(
		'visible' => array($undergrad, $grad_student),
		'required' => array($undergrad, $grad_student),
	);
	
//Education fieldset only available to Graduates, Undergraduates
		$form['group_er_education']['#states']['visible'] = array(
		':input[name="field_er_most_sen_proj_role_ca1[und]"]' => 
		array('value' => $grad_student, $undergrad, $post_doc),
		);	  
//Advisor field only available to Students
		$form['field_er_advisor']['#states'] = array(
		'visible' => array($post_doc, $undergrad, $grad_student),
		'required' => array($post_doc, $undergrad, $grad_student),
	);
	
//Mentoring plan fields only available to Post-Docs
		$form['field_er_mentoring_sign_off']['#states'] = 
		$form['field_er_ment_plan_post_docs_ca1']['#states'] = 
		$form['field_er_mentoring_plan_ul']['#states'] =
		array(
		'visible' => array($post_doc),
		'required' => array($post_doc),
	);	
	


// Set up Paid by EPSCoR Variables
		$paid_by_epscor = array(
		 ':input[name="field_er_paid[und]"]' => array('value' => '1'),
		);
		
		$not_paid_by_epscor = array(
		 ':input[name="field_er_paid[und]"]' => array('value' => '0'),
		);
		
//If Faculty, and Paid = Yes ---> Show "How many months of direct funding ..." field		
// used 'invisible' here to rule out all Roles other than Faculty AND not Paid by EPSCoR
		$form['field_er_months_of_effort']['#states'] = array(
		'invisible' => array(
		$tech_support_staff, $undergrad, $grad_student, $epscor_leadership_team, 
		$other_epscor_staff, $post_doc, $non_tech_support_staff, $evaluation_staff, $not_paid_by_epscor),
	);
	
	
//If Faculty, and Paid = No ---> Show "Provide a List of all funding sources ..." field
// used 'invisible' here to rule out all Roles other than Faculty AND Paid by EPSCoR
		$form['field_provide_a_list_of_all_fund']['#states'] = array(
		'invisible' => array(
		$tech_support_staff, $undergrad, $grad_student, $epscor_leadership_team, 
		$other_epscor_staff, $post_doc, $non_tech_support_staff, $evaluation_staff, $paid_by_epscor),
	);
	
		
//If Participate in EPSCoR > 160 Hours display "Please select nearest person.."" field
		$participant_more_than_sixty = array(
		':input[name="field_er_160hours[und]"]' => array('value' => '1'),
		);
		
		$form['field_er_select_the_near_ca1']['#states'] = array(
		'visible' => $participant_more_than_sixty,
		'required' => $participant_more_than_sixty,
	);		
}

/*
 * Below are validation, after_build, and other such functions
 */
//This function is fired after form submit on certain content types.
//It checks if it needs to create a new institution, and does so when appropriate.
function _create_inst(&$form, &$form_state){
	// d($form, 'form');
	// d($form_state, 'form state');
	if ($form_state['values']['field_er_inst_add_bool']['und'][0]['value'] == 1 
		&& isset($form_state['values']['field_er_inst_name']['und'][0]['value']) 
		&& isset($form_state['values']['field_er_inst_link']['und'][0]['url'])){
		// They've chosen to add a new Institution
		// d('made it inside if statement');
		  global $user;
		  // d($user, 'user');
		  $newNode = (object) NULL;
		  $newNode->type = 'er_institution';
		  $newNode->uid = $user->uid;
		  $newNode->created = strtotime("now");
		  $newNode->changed = strtotime("now");
		  $newNode->status = 1;
		  $newNode->comment = 0;
		  $newNode->promote = 0;
		  $newNode->moderate = 0;
		  $newNode->sticky = 0;
		  $newNode->language = 'und';
		  $newNode->field_er_inst_name['und'][0]['value'] = 
		  	$form_state['values']['field_er_inst_name']['und'][0]['value'];		  
		  // This if statement is because the abbreviation is put in parenthesis in Andrew's code
		  if (!empty($form_state['values']['field_er_inst_abbreviation']['und'][0]['value']))
		  	$newNode->field_er_inst_abbreviation['und'][0]['value'] = 
		  		$form_state['values']['field_er_inst_abbreviation']['und'][0]['value'];
		  $newNode->field_er_inst_country['und'][0]['value'] = 
		  	$form_state['values']['field_er_inst_country']['und'][0]['value'];
		  $newNode->field_er_inst_link['und'][0]['url'] = 
		  	$form_state['values']['field_er_inst_link']['und'][0]['url'];
		  $newNode->body['und'][0]['value'] = 
		  	$form_state['values']['field_er_inst_description']['und'][0]['value'];
		  
		  // Set the title according to whether there's an abbreviation set
		  if (empty($form_state['values']['field_er_inst_abbreviation']['und'][0]['value'])){
		  	$newNode->title = $form_state['values']['field_er_inst_name']['und'][0]['value'];
		  }
		  else{
		  	$newNode->title = $form_state['values']['field_er_inst_name']['und'][0]['value'] . ' (' . $form_state['values']['field_er_inst_abbreviation']['und'][0]['value'] . ')';
		  }
		  // save it!
		  node_save($newNode);	
		  $nid = $newNode->nid;
		  
		  if ($user->uid == 0){
		  	  // This means someone is creating an Institution and 
	  	  //		is anonymous creating an account, so we need this to be urgent
			  $flag = flag_get_flag('er_needs_review_urgent');
			  $flag->flag('flag', $newNode->nid, null, TRUE);	 		  			  		  	
		  }
		  // flag this as needing Administrator review so they can 
  		  //		add Juristictional information, etc...
		  $flag = flag_get_flag('er_needs_review');
		  $flag->flag('flag', $newNode->nid, null, TRUE);	
		  
			//Add the new node to the list of institutions attached to this node.
			$form_state['values']['field_er_inst_ref']['und'][] = array('target_id'=>$nid);
			
		  // clean up values so when you view it, it's proper
			//$form_state['values']['field_er_inst_ref']['und'][0]['target_id'] = $nid; //this doesn't work for collaborations since we can select multiple institutions.
		  $form_state['values']['field_er_inst_add_bool']['und'][0]['value'] = 0;
		  $form_state['values']['field_er_inst_name']['und'][0]['value'] = '';
		  $form_state['values']['field_er_inst_abbreviation']['und'][0]['value'] = '';
		  $form_state['values']['field_er_inst_link']['und'][0]['url'] = '';
		  $form_state['values']['field_er_inst_country']['und'][0]['value'] = '';
		  $form_state['values']['field_er_inst_description']['und'][0]['value'] = '';
		  
		  // not unsetting for user profile
		  if (isset($form_state['complete form']['field_er_inst_name']['und'][0]['value']['#value']))
		  	$form_state['complete form']['field_er_inst_name']['und'][0]['value']['#value'] = '';
		  if (isset($form['field_er_inst_country']['und'][0]['value']['#value']))
		  	$form['field_er_inst_country']['und'][0]['value']['#value'] = '';
	}
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
		
		$element['group_er_journal']['#states'] = array(
			'invisible'=> array($none, $abstract, $thesis, $internet),
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
function _remove_NA_inst($form, &$form_state){
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
 * Auto Generate a node title for selcted form types
 *
 * @author Brent Sadler <brent.sadler@unh.edu>
 */
function _form_autogen_title(&$form, &$form_state, $form_id) {
	// d($form_id, 'form_id entering form_autogen_title');
	// d($form, 'form entering form_autogen_title');
	
	$form['title']['#type'] = 'hidden';
	$form['title']['#required'] = FALSE;
	
	$referenced_cal_event = $form['field_er_cal_entity_reference']['und']['#default_value'];
	//strip characters except digits of node ID
	$calendarNodeID = preg_replace('[\D]', '', $referenced_cal_event );
	//$calendarNodeID = $form['field_er_cal_entity_reference']['und']['#default_value'];
	//$calendarNodeID = $form['field_er_cal_entity_reference']['target_id'];
	$calendarNode = node_load($calendarNodeID);
	$calendarTitle = $calendarNode->title;

	// Construct a unique title for node creation
	switch ($form_id)
	{
		case 'er_collaborators_node_form';
			$form['title']['#default_value'] = "Collaborators {$form['created']['#value']}";
			break;
		case 'er_engagement_node_form';
			$form['title']['#default_value'] = $calendarTitle;
		 					 
			break;	
		case 'er_effort_node_form';
			$form['title']['#default_value'] = "Effort {$form['created']['#value']}";
			break;
		case 'er_paid_node_form':
			$form['title']['#default_value'] = "Funded effort {$form['created']['#value']}";
			break;
		default:
			$form['title']['#default_value'] = "Generic title {$form['created']['#value']}";
			break;
	}
	// d($form['title']['#default_value'], 'node-title exiting form_autogen_title');
	// d($form, 'form exiting form_autogen_title');


}