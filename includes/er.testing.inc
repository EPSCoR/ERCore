<?php
/*
 * This file contains the tests for the er module.
 */
function er_testing() {
	////this may be useful for checking taxonomy or content types.
	//d(field_info_bundles(), 'field_info_bundles()');
	
	$requirements = array();
	/* Example usage:
	$requirements[] = array(
		'severity' => REQUIREMENT_OK,//Acceptable values: REQUIREMENT_INFO, REQUIREMENT_OK, REQUIREMENT_WARNING, REQUIREMENT_ERROR
		'title' => 'Test',
		'description' => "This is a test",
		'value' => '1.0',//May be a version number
	);
	*/
	
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
			'value' => 'missing',
			'description' => 'PHPExcel library has not been installed! Please refer to the '. l('documentation', 'admin/help/er', array('fragment' => 'phpexcel')) . '.',
		);
	}

	//Taxonomy
	$taxonomy_status = er_test_taxonomy();
	//d($taxonomy_status, '$taxonomy_status');
	foreach ($taxonomy_status['missing'] as $name=>$taxonomy_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Taxonomy: $machine_name missing!",
			'value' => l("Reset this taxonomy", "admin/config/epscor/er/reset/taxonomy/$machine_name"),
		);
	}
	foreach ($taxonomy_status['extra'] as $name=>$taxonomy_node){
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Taxonomy: $machine_name is extraneous",
			'description' => 'This taxonomy is unknown to this version of the EPSCoR Reporting module, and may be removed.',
			'value' => l('delete', 'admin/structure/taxonomy/manage/'.$machine_name.'/delete'),
		);
	}
	foreach ($taxonomy_status['found'] as $name=>$taxonomy_node){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Taxonomy: $machine_name",
			'value' => "Taxonomy " . l('found', 'admin/structure/taxonomy/manage/'.$machine_name),
		);
	}
		
	//Flags
	$flags_status = er_test_flags();
	//d($flags_status, '$flags_status');
	foreach ($flags_status['missing'] as $name=>$flags_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Flag: $name missing!",
			'value' => l("Reset this flag", "admin/config/epscor/er/reset/flags/$name"),
		);
	}
	foreach ($flags_status['extra'] as $name=>$flag_node){
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Flag: $name is extraneous",
			'description' => 'This flag is unknown to this version of the EPSCoR Reporting module, and may be removed.',
			'value' => l('delete', 'admin/structure/flags/manage/'.$name.'/delete'),
		);
	}
	foreach ($flags_status['found'] as $name=>$flag_node){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Flag: $name",
			'value' => "Flag " . l('found', 'admin/structure/flags/manage/'.$name),
		);
	}
	
	//Fields and fieldgroups
	$field_status = er_test_fields();
	foreach ($field_status as $bundle=>$results){
		if (count($results['fields']) || count($results['groups'])){//if missing fields or groups were returned
			$missing[] = count($results['fields'])?"fields: ".implode($results['fields']):'';//collect the names of the missing fields
			$missing[] = count($results['groups'])?"groups: ".implode($results['groups']):'';//collect the names of the missing fieldgroups
			$requirements[] = array(
				'severity' => REQUIREMENT_ERROR,
				'title' => "Bundle: ".$results['name']." ($bundle) incomplete!",
				'description' => "Missing ".implode($missing),
				'value' => l("Reset this bundle", "admin/config/epscor/er/reset/bundles/$bundle"),
			);
		}else{
			$requirements[] = array(
				'severity' => REQUIREMENT_OK,
				'title' => "Bundle: ".$results['name']." ($bundle)",
				'value' => 'Ok',
			);
		}
		
	}
	//d($field_status, '$field_status');
	
	
	//Display the table
	$form['status'] = array(
		"#theme"=>"er_status_report",
		'#requirements'=>$requirements,
	);
	
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
  
	//THIS SECTION SHOULD BE DEPRECATED IN FAVOR OF THE BROKEN-OUT er_test_fields() FUNCTION (see below)
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

/**
 * Check the status of fields and fieldgroups
 */
function er_test_fields(){
	$bundle_path = ER_INSTALL_DIR . '/bundles';
	$dir = new DirectoryIterator($bundle_path);
	$bundle_list = array();
	$found_errors = false;
	$return = array();
	foreach ($dir as $fileinfo) {//loop through all files in the directory
		if (!$fileinfo->isDot()) { //Check that it's not . or .. (which are not really files)
			$bundle_filename = $fileinfo->getFilename();
			include($bundle_path . '/' . $bundle_filename);//now all the bundle information is contained in $data
			
			//This will give me the name of the bundle. This should be equivalent to th e filename, but doesn't have to be:
			$bundle_info = end($data['bundles']);//this will be empty for the user profile bundle
			$bundle_type = key($data['bundles']);//this gets the internal name of the bundle
			$bundle_label = isset($bundle_info->name)?$bundle_info->name:'User Profile';
			$entity_type = $bundle_type=='user'?'user':'node';
			
			$groups = field_group_info_groups($entity_type, $bundle_type, 'form', TRUE);//get the list of groups in this bundle.
			//d($data, '$data');
			
			//Note: I think we could use field_info_instances("node",$source_node_type) to determine if a CT has extra fields.
			
			$missing_fields = array();
			if (isset($data['fields'])){ //Make sure there are fields
				foreach ($data['fields'] as $field){ //Check each field
					// see if the field exists
					$field_info = field_info_field($field['field_name']);
					if (is_null($field_info)){
						$missing_fields[] = $field['field_name'];
					}
				}
			}
			
			$missing_groups = array();
			if (isset($data['fieldgroups'])){ //if this bundle has fieldgroups...
				foreach ($data['fieldgroups'] as $group){ //Check each fieldgroup
					if (!isset($groups[$group->group_name])){
						$missing_groups[] = $group->group_name;
					}
				}
			}
			
			$return[$bundle_type] = array(
				'fields' => $missing_fields,
				'goups' => $missing_groups,
				'name' => $bundle_label
			);
			
		}
	}
	return $return;
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
 *		"extra" =>
 *			flag name => flag_node object of any flag that does not belong to er-core but starts with "er_"
 *			...
 */
function er_test_flags(){
	$drupal_path = ER_INSTALL_DIR . '/flags';
	$dir = new DirectoryIterator($drupal_path);
	$internal_flags = flag_get_flags();
	//d($internal_flags, '$internal_flags');
	// loop through all flag files in the directory
	$return = array('found'=>array(), 'missing'=>array(), 'extra'=>array());
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {//Determine if current DirectoryIterator item is '.' or '..'
			$flag_filename = $fileinfo->getFilename();
			// this grabs the text from the file and puts it in a variable
			//$flag_config = file_get_contents($drupal_path . '/' . $flag_filename);
			//eval($flag_config);//This will set the $flags variable as defined in the file.
			include($drupal_path . '/' . $flag_filename);//$flags variable should be set now.
			if (isset($flags) && is_array($flags)){
				foreach ($flags as $name=>$flag){
					if (isset($internal_flags[$name])){
						$return['found'][$name] = $internal_flags[$name];
						//unset this flag from the list of internal flags, that way all that's left will be extras flags that are not necessary for er-core.
						//this will be useful info because we might remove some of these flags in the futuere, this will allow us to display the no-longer-necessary flags to the user.
						unset($internal_flags[$name]);
					}else{
						$return['missing'][$name] = $flags;
					}
				}
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $flag_filename, 'error');
			}
		}
	}
	foreach ($internal_flags as $name => $flag_node){//iterate over the remaining flags, and look for er_ prefix.
		if (substr($name, 0, 3) == "er_")
			$return['extra'][$name] = $flag_node;
	}
	return $return;
}


/*
 * This function tests to see if all of the Rules are up to date with what's in the Rules install directory.
 * @author Chris Allen
 */

function er_test_rules(){
	//This will get the active rules in the system
	$conditions = array('plugin' => 'reaction rule', 'active' => TRUE);
	$entities = entity_load('rules_config', FALSE, $conditions);
	d($entities, '$entities');//check out the results
	
	$drupal_path = ER_INSTALL_DIR . '/rules';
	$dir = new DirectoryIterator($drupal_path);
	$internal_rules = rules_get_rules();
	// loop through all rule files in the directory
	$return = array();
	
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {
			$rules_filename = $fileinfo->getFilename();
			
			include($drupal_path . '/' . $rules_filename);
			if (isset($rules) && is_array($rules)){
				foreach ($rules as $name=>$rules){
					if (isset($internal_rules[$name])){
						$return['found'][$name] = $internal_rules[$name];
						
						unset($internal_rules[$name]);
					}else{
						$return['missing'][$name] = $rules;
					}
				}
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $rules_filename, 'error');
			}
		}
	}

	return $return;
}


/*
 * This function tests to see if all of the Rules are up to date with what's in the Rules install directory.
 * @author Jeannine Haney
 */

function er_test_taxonomy(){
	$drupal_path = ER_INSTALL_DIR . '/taxonomy';
	$dir = new DirectoryIterator($drupal_path);
	$internal_taxonomy = taxonomy_vocabulary_get_names();
	$return = array('found'=>array(), 'missing'=>array(), 'extra'=>array());
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {//Determine if current DirectoryIterator item is '.' or '..'
			$internal_taxonomy = $fileinfo->getFilename();
			// this grabs the text from the file and puts it in a variable
			//$taxonomy_config = file_get_contents($drupal_path . '/' . $taxonomy_vocabulary_get_names);
			//eval($flag_config);//This will set the $taxonomy variable as defined in the file.
			include($drupal_path . '/' . $internal_taxonomy);
			//$taxonomy variable should be set now.
			if (isset($taxonomy) && is_array($taxonomy)){
				foreach ($taxonomy as $name=>$taxonomy){
					if (isset($internal_taxonomy[$name])){
						$return['found'][$name] = $internal_taxonomy[$name];
						
						unset($internal_taxonomy[$name]);
					}else{
						$return['missing'][$name] = $tax;
					}
				}
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $taxonomy_filename, 'error');
			}
		}
	}
	foreach ($internal_taxonomy as $name => $taxonomy){
	//iterate over the remaining taxonomy, and look for er_ prefix.
		if (substr($name, 0, 3) == "er_")
			$return['extra'][$name] = $taxonomy;
	}
	return $return;
}
