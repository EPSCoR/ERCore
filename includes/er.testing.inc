<?php
/*
 * This file contains the tests for the er module.
 */
function er_testing() {
	//Available severity values:
	//	REQUIREMENT_INFO, REQUIREMENT_OK, REQUIREMENT_WARNING, REQUIREMENT_ERROR
	
	$requirements = array();
	
	//ER Module info
	$info = drupal_parse_info_file(ER_MODULE_DIR.'/er.info');
	$requirements[] = array(
		'severity' => REQUIREMENT_INFO,
		'title' => $info['name'],
		'value' => $info['version'],
	);
	
	//PHPExcel status
	$library = libraries_detect('PHPExcel');
	if ($library['installed']){
		$requirements[] = array(
			'title' => 'PHPExcel library',
			'severity' => REQUIREMENT_OK,
			'value' => $library['version'],
		);
	}else{
		$requirements[] = array(
			'title' => 'PHPExcel library',
			'severity' => REQUIREMENT_ERROR,
			'description' => 'PHPExcel library has not been installed! Please refer to the '. l('documentation', 'admin/help/er', array('fragment' => 'phpexcel')) . '.',
		);
	}
	
	//Flags
	$flags_status = er_test_flags();
	foreach ($flags_status['missing'] as $name=>$flags_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Flag: $name missing!",
			'value' => 'Fix this issue. Heres a button: ...',
		);
	}
	
	$requirements[] = array(
		'severity' => REQUIREMENT_OK,
		'title' => '',
		'description' => "something",
		'value' => '1.2.3.4',
	);
	//Display the table
	$form['status'] = array(
		"#theme"=>"er_status_report",
		'#requirements'=>$requirements,
	);
	
	
	if ($library['installed'])
		drupal_set_message('PHPExcel library version '.$library['version'].' detected.');
	else
		drupal_set_message('PHPExcel library has not been installed! Please refer to the '. l('documentation', 'admin/help/er', array('fragment' => 'phpexcel')) . '.', 'error');	
  
	//start
  $form['er_testing_container'] = array(
    '#type' => 'fieldset',
    '#title' => 'Individual Content Types',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Under construction ~Mike'),      
  );
	
  $form['er_testing_container']['er_admin_container_reset_bundle'] = array(
    '#type' => 'fieldset',
    '#title' => 'Installed correctly',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['er_testing_container']['er_admin_container_reset_bundle_error'] = array(
    '#type' => 'fieldset',
    '#title' => 'Fields Missing',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  ); 
  
	$bundle_path = ER_INSTALL_DIR . '/bundles';
	$dir = new DirectoryIterator($bundle_path);
	// loop through all rule files in the directory
	$bundle_list = array();
	$found_errors = false;
	// dpm($dir, 'what?');
	foreach ($dir as $fileinfo) {
		// d($fileinfo, '$fileinfo');
		if (!$fileinfo->isDot()) {
			// d('this is a file '. $fileinfo);
			$bundle_filename = $fileinfo->getFilename();
			//d('this file', $bundle_filename);
			//$bundle_config = file_get_contents($bundle_path . '/' . $bundle_filename);
			include($bundle_path . '/' . $bundle_filename);
			// now all the bundle information is contained in $data
			//d($bundle_config, 'bundle config');
			//d($data, 'bundle config');
			if (isset($data['fields'])){ // standard check before going into foreach loop
				$field_not_found = false;
				$missing_list = array();
				foreach ($data['fields'] as $field){
					// see if the field exists
					$exist = field_info_field($field['field_name']);
					if (is_null($exist)){
						dsm('this field doesnt exist: ' . $field['field_name']);
						$field_not_found = true;						
						$missing_list[] = $field['field_name'];
					}
				}
				if ($field_not_found){					
					// Content Types were found where the fields did not work
					$found_errors = true; // set this flag so we can add a message if all are okay
					$form['er_testing_container']['er_admin_container_reset_bundle_error']["$bundle_filename"] = array(
					  '#type' => 'checkbox',
					  '#title' => t("{$data['bundles'][$bundle_filename]->name} ($bundle_filename)"),
					  '#description' => t('Missing: ') . implode(', ', $missing_list),
					);						
				}else{
					//dsm($exist, 'this field exists');
					$form['er_testing_container']['er_admin_container_reset_bundle']["$bundle_filename"] = array(
					  '#type' => 'markup',
					  '#markup' => t("{$data['bundles'][$bundle_filename]->name} ($bundle_filename)<br/>"),
					);							
				}				
			
			// check to see if it's in the database
			//$bundle_list[] = $bundle_filename;		
			}
		}
	}
	// instead of having a blank fieldgroup for the errors, let's add a message if there were no errors
	if (!$found_errors){
		$form['er_testing_container']['er_admin_container_reset_bundle_error']['no_error_msg'] = array(
		  '#type' => 'markup',
		  '#markup' => t('No Content Types had errors.'),
		);		
	}
	
	
  $form['er_admin_container_reset_bundle']['redo_bundle'] = array(
    '#type' => 'submit',
    '#value' => t('Create / Update Content Type(s)'),
    '#submit' => array('er_revert_bundle'),
  );
	
	

	
// end  

	return $form;
}


/*
 * This function tests to see if all of the flags are up to date with what's in the flags install directory.
 * @author Andrew Wessels
 * @return associative array structure:
 * 		"found" =>
 *			flag name => flag_node object
 *			...
 *		"missing" =>
 *			flag name => $flags array code as a string (comes directly from the files) - can then be used to import the flag.
 *			...
 *		"extras" =>
 *			flag name => flag_node object of any flag that does not belong to er-core
 *			...
 */
function er_test_flags(){
	$drupal_path = ER_INSTALL_DIR . '/flags';
	$dir = new DirectoryIterator($drupal_path);
	$internal_flags = flag_get_flags();
	//d($internal_flags, '$internal_flags');
	// loop through all flag files in the directory
	$return = array('found'=>array(), 'missing'=>array(), 'extras'=>array());
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {//Determine if current DirectoryIterator item is '.' or '..'
			$flag_filename = $fileinfo->getFilename();
			// this grabs the text from the file and puts it in a variable
			$flag_config = file_get_contents($drupal_path . '/' . $flag_filename);
			eval($flag_config);//This will set the $flags variable as defined in the file.
			if (isset($flags) && is_array($flags)){
				foreach ($flags as $name=>$flag){
					if (isset($internal_flags[$name])){
						$return['found'][$name] = $internal_flags[$name];
						//unset this flag from the list of internal flags, that way all that's left will be extras flags that are not necessary for er-core.
						//this will be useful info because we might remove some of these flags in the futuere, this will allow us to display the no-longer-necessary flags to the user.
						unset($internal_flags[$name]);
					}else{
						$return['missing'][$name] = $flag_config;
					}
				}
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $flag_filename, 'error');	
			}
		}
	}
	$return['extras'] = $internal_flags;//all that will be left after all the unsets will be any extra flags still in the system
	return $return;
}