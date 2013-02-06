<?php
/*
 * This file contains the settings form for the er module.
 * We can use this form to allow our users to modify the functionality of the module.
 *
 */
function er_settings() {
	$library = libraries_detect('PHPExcel');
	if ($library['installed'])
		drupal_set_message('PHPExcel library version '.$library['version'].' detected.');
	else
		drupal_set_message('PHPExcel library has not been installed! Please refer to the '. l('documentation', 'admin/help/er', array('fragment' => 'phpexcel')) . '.', 'error');
	
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
		'#description' => t("Select the content types that you want to be displayed on the summary table. Each content type must have a corresponding view set up in order for it to be displayed in the summary table."),
	);
	
  $form['er_admin_container_reset'] = array(
    '#type' => 'fieldset',
    '#title' => 'Reset to default ER Setting(s)',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,      
  );	

  $form['er_admin_container_reset']['redo_checkboxes'] = array(
    '#type' => 'checkboxes',
    '#options' => drupal_map_assoc(array(t('Rules'), t('Taxonomy'), t('Fields'), t('Flags'), t('Permissions'))),
    '#title' => t('Setting'),
  );  

  $form['er_admin_container_reset']['redo_fields'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('er_revert'),
  );
  
// start

	
  $form['er_admin_container_reset_bundle'] = array(
    '#type' => 'fieldset',
    '#title' => 'Individual Content Types',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Under construction ~Mike'),      
  );
  
  // $form['er_admin_container_reset_bundle']['bundle_checkboxes'] = array(
    // '#type' => 'checkboxes',
    // '#options' => drupal_map_assoc($bundle_list),
    // '#title' => t('Individual Content Types'),
  // );  	
  
	$drupal_path = drupal_get_path('module', 'er') . '/er_install/bundles';
	$dir = new DirectoryIterator($drupal_path);
	// loop through all rule files in the directory
	$bundle_list = array();
	// dpm($dir, 'what?');
	foreach ($dir as $fileinfo) {
		// d($fileinfo, '$fileinfo');
		if (!$fileinfo->isDot()) {
			// d('this is a file '. $fileinfo);
			$bundle_filename = $fileinfo->getFilename();
			// d('this file', $bundle_filename);
			$result = db_select('node_type', 'n')
			    ->fields('n')
			    ->condition('orig_type', $bundle_filename ,'=')
			    ->execute()
			    ->fetchAssoc();
				if (empty($result)){
				   // dsm('nothing');
					$form['er_admin_container_reset_bundle']['bundle'] = array(
					  '#type' => 'checkbox',
					  '#title' => $bundle_filename,
					  '#description' => t('<b>Content type DOES NOT exists.</b>'),
					);					
				}
				else{
					// dsm($result, 'result');
					$form['er_admin_container_reset_bundle']["$fileinfo"] = array(
					  '#type' => 'checkbox',
					  '#title' => t("{$result['name']} ($fileinfo)"),
					  '#description' => t('Content type exists.'),
					);				
				}
			
			// check to see if it's in the database
			//$bundle_list[] = $bundle_filename;		
		}
	}
	
  $form['er_admin_container_reset_bundle']['redo_bundle'] = array(
    '#type' => 'submit',
    '#value' => t('Create / update Content Type(s)'),
    '#submit' => array('er_revert_bundle'),
  );

	
// end  
  
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
  
  
  
  
  //// ...nah, not necessary:
  //$root_name = variable_get('er_root_name');
  //$form['er_root_name'] = array(
  //  '#type' => 'textfield',
  //  '#title' => t('Location of summary table and associated views'),
  //  '#default_value' => $root_name?$root_name:'reporting',
  //  '#description' => t('It should not be necessary to change this field. It is suggested that ')
  //);
	
	//UNUSED:
	//
	//$form['er_teams'] = array(
	//	'#type' => 'textarea',
	//	'#title' => t('Research Components (ie., Research Teams)'),
	//	'#default_value' => variable_get('er_teams'),
	//	'#multiple' => TRUE,
	//	'#description' => t("Enter a list of teams names, one per line."),
	//);
	//
	//$default_teams = array(
	//	"CAC (Community Advisory Committee)"=>"CAC (Community Advisory Committee)",
	//	"MAP (Monitoring Assessment Panel)"=>"MAP (Monitoring Assessment Panel)",
	//	"SWC (Statewide Committee)"=>"SWC (Statewide Committee)",
	//	"Administration"=>"Administration",
	//	"Guest"=>"Guest",
	//);
	//
	//$committees = variable_get('er_committees');
	//$form['er_committees'] = array(
	//	'#type' => 'select_or_other',
	//	'#select_type' => 'checkboxes',
	//	'#title' => t('Extra Components (ie., Committees)'),
	//	'#default_value' => $committees,
	//	'#options' => $default_teams+$committees,
	//	'#multiple' => TRUE,
	//	'#description' => t("Enter a list of teams names, one per line."),
	//);
	
	return system_settings_form($form);
}