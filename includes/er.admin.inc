<?php
/*
 * This file contains the settings form for the er module.
 * We can use this form to allow our users to modify the functionality of the module.
 *
 */
function er_settings() {
	//these are the default content types that show up on the list:
	$default_types = array(//it apparently has to be weird like this in order for it to work well with variable_get...
		////PARTICIPANTS
		//"er_project"=>"er_project",
		//"er_book"=>"er_book",
		"er_collaboration"=>"er_collaboration",
		"er_proposal"=>"er_proposal",
		"er_engagement"=>"er_engagement",
		"er_institution"=>"er_institution",
		//"er_internet"=>"er_internet",
		"er_other_product"=>"er_other_product",
		"er_patent"=>"er_patent",
		//"er_poster"=>"er_poster",
		"er_presentation"=>"er_presentation",
		"er_publication"=>"er_publication",
		"er_award"=>"er_award",
		"er_event"=>"er_event",
		"er_news"=>"er_news",
		"participants"=>"participants",
	);
	$types = variable_get('er_summary_types');
  if (!$types) $types = array();
	//dsm($types);
  
  $form['status'] = array(
    '#type' => 'item',
    '#title'=> 'ER-Core Status',
    '#markup' => t('Access the <b>!link</b> to diagnose problems, update your installation, or reset something if it\'s broken.', array('!link'=>l('ER-Core status page', 'admin/config/epscor/er/status')))
  );
	$form['help_description'] = array(
    '#type' => 'item',
    '#title'=> 'Notice:',
    '#markup' => t('You may want to take a look at ER-Core\'s !link!', array('!link'=>l('documentation', 'admin/help/er')))
  );
  
  $start_date = variable_get('er_start_date');
  //dsm(mktime(0, 0, 0, $start_date['month'], $start_date['day'], $start_date['year']));
  $form['er_start_date'] = array(
    '#type' => 'date',
    '#title' => t('Start Date of EPSCoR Grant'),
    '#default_value' => $start_date?$start_date:array('month'=>'9', 'day'=>'15', 'year'=>'2009'),//date('Y-m-d', $start_date?$start_date:mktime(0, 0, 0, 9, 15, 2009)),
    '#description' => t('This date will be used as the start of the grant. This effects the date ranges shown on the accomplishments table.')
  );
  
	$form['er_summary_types'] = array(
		'#type' => 'select_or_other',
		'#select_type' => 'checkboxes',
		'#title' => t('Summary Table Content:'),
		'#default_value' => $types,
		'#options' => array_merge($default_types, $types),
		'#multiple' => TRUE,
		'#description' => t("Select the content types that you want to be displayed on the summary table. <br>Each content type must have a corresponding view set up in order for it to be displayed in the summary table."),
	);
  
	if (module_exists('er_encryption')){
		$form['er_use_encryption'] = array(
		  '#type' => 'checkbox',
		  '#title' => t('Use encryption.'),
		  '#default_value' => variable_get('er_use_encryption'),
		  '#description' => t('This applies to gender, disabilities, citizenship, ethnicity, and race.'),
		);			
	}
	  
	  // this below function will get deleted
	function er_import_fields_temp($form, &$form_state){
		d($form_state, 'this is being called from er_import_fields_temp');
		watchdog('er', 'This is a test, ignore ~Mike',NULL,WATCHDOG_CRITICAL);
	}
	
	return system_settings_form($form);
}