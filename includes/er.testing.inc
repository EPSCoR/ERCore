<?php
/*
 * This file contains the tests for the er module.
 */
function er_testing() {
	er_test_flags();
	$library = libraries_detect('PHPExcel');
	if ($library['installed'])
		drupal_set_message('PHPExcel library version '.$library['version'].' detected.');
	else
		drupal_set_message('PHPExcel library has not been installed! Please refer to the '. l('documentation', 'admin/help/er', array('fragment' => 'phpexcel')) . '.', 'error');	
  
// start
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
 */
function er_test_flags(){
	$drupal_path = ER_INSTALL_DIR . '/flags';
	$dir = new DirectoryIterator($drupal_path);
	$internal_flags = flag_get_flags();
	//d($internal_flags, '$internal_flags');
	// loop through all flag files in the directory
	$missing = array();
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {//Determine if current DirectoryIterator item is '.' or '..'
			$flag_filename = $fileinfo->getFilename();
			// this grabs the text from the file and puts it in a variable
			$flag_config = file_get_contents($drupal_path . '/' . $flag_filename);
			eval($flag_config);
			if (isset($flags) && is_array($flags)){
				foreach ($flags as $name=>$flag){
					if (isset($internal_flags[$name])){
						//unset this flag from the list of internal flags, that way all that's left will be extras flags that are not necessary for er-core.
						//this will be useful info because we might remove some of these flags in the futuere, this will allow us to display the no-longer-necessary flags to the user.
						unset($internal_flags[$name]);
					}else{
						$missing[$flag->name] = $flag_config;
					}
				}
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $flag_filename, 'error');	
			}
		}
	}
	$return = array('missing'=>$missing, 'extras'=>$internal_flags);
	d($return, 'Flags status');
	return $return;
}