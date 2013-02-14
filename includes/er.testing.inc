<?php
/*
 * This file contains the tests for the er module.
 */
function er_testing() {
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
			//d('this file', $bundle_filename);
			//$bundle_config = file_get_contents($drupal_path . '/' . $bundle_filename);
			include($drupal_path . '/' . $bundle_filename);
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
					$form['er_testing_container']['er_admin_container_reset_bundle_error']["$bundle_filename"] = array(
					  '#type' => 'checkbox',
					  '#title' => t("{$data['bundles'][$bundle_filename]->name} ($bundle_filename)"),
					  '#description' => t('Missing: ') . implode(', ', $missing_list),
					);						
				}else{
					//dsm($exist, 'this field exists');
					$form['er_testing_container']['er_admin_container_reset_bundle']["$bundle_filename"] = array(
					  '#type' => 'checkbox',
					  '#title' => t("{$data['bundles'][$bundle_filename]->name} ($bundle_filename)"),
					);							
				}				
			
			// check to see if it's in the database
			//$bundle_list[] = $bundle_filename;		
			}
		}
	}
	
  $form['er_admin_container_reset_bundle']['redo_bundle'] = array(
    '#type' => 'submit',
    '#value' => t('Create / Update Content Type(s)'),
    '#submit' => array('er_revert_bundle'),
  );

	
// end  

	return $form;
}