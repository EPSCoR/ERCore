<?php
/*
 * This file contains the tests for the er module.
 */
function er_testing() {
	////this may be useful for checking taxonomy or content types.
	//d(field_info_bundles(), 'field_info_bundles()');
	$link_options = array(
		'query' => array('destination' => 'admin/config/epscor/er/status'),
	);
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
			'title' => "Taxonomy: $name missing!",
			'value' => l("Reset this taxonomy", "admin/config/epscor/er/reset/taxonomy/$machine_data"),
		);
	}
	foreach ($taxonomy_status['extra'] as $name=>$taxonomy_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Taxonomy: $name is extraneous",
			'description' => 'This taxonomy is unknown to this version of the EPSCoR Reporting module, and may be removed.',
			'value' => l('Delete', 'admin/structure/taxonomy/manage/'.$machine_name.'/delete', $link_options),
		);
	}
	foreach ($taxonomy_status['found'] as $name=>$taxonomy_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Taxonomy: $name",
			'value' => "Taxonomy " . l('found', 'admin/structure/taxonomy/'.$machine_name),
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
			'value' => l('Delete', 'admin/structure/flags/manage/'.$name.'/delete', $link_options),
		);
	}
	foreach ($flags_status['found'] as $name=>$flag_node){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Flag: $name",
			'value' => "Flag " . l('found', 'admin/structure/flags/manage/'.$name),
		);
	}
	
	//Roles
	$role_status = er_test_role_perms();
	//d($role_status, '$role_status');
	foreach ($role_status['missing'] as $rid=>$name){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Role: $name role is missing!",
			'value' => l("Reset this role", "admin/config/epscor/er/reset/roles/$rid"),
		);
	}
	foreach ($role_status['extra'] as $rid=>$role){
		$name = $role->name;
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Role: $name role is extraneous",
			'description' => 'This user role is unknown to this version of the EPSCoR Reporting module, and may be removed if desired.',
			'value' => l('Delete', 'admin/people/permissions/roles/delete/'.$rid, $link_options),
		);
	}
	foreach ($role_status['found'] as $rid=>$role){
		$name = $role->name;
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Role: $name",
			'value' => 'Role '.($rid>2?l('found', 'admin/people/permissions/roles/edit/'.$rid):'found'),
			'description' => l('View Permissions', 'admin/people/permissions/'.$rid) . ', ' . l("Reset Permissions", "admin/config/epscor/er/reset/roles/$rid"),
		);
	}

	//Rules
	$rules_status = er_test_rules();
	//d($rules_status, '$rules_status');
	foreach ($rules_status['found'] as $rules_filename=>$rules_filename){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Rule: $rules_filename",
			'value' => "Rule " . l('found', '/admin/config/workflow/rules'.$rules_filename),
		);
	}	
	foreach ($rules_status['missing'] as $rules_filename=>$rules_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Rule Missing: $rules_filename",
			'value' => l("Reset this Rule", "admin/config/epscor/er/reset/rules/$rules_filename"),
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
				'value' => 'Bundle '. l('found', 'admin/structure/types/manage/'.$bundle),
			);
		}
		
	}
	//d($field_status, '$field_status');
	$form['reset'] = array(
		'#type'=>'fieldset',
		'#title'=>'Bulk Reset',
		'#collapsible'=>true,
		'#collapsed'=>true,
	);
	$installation_types = er_get_install_types();
	foreach ($installation_types as $type){
		$form['reset'][$type] = array(
			'#type' => 'link',
			'#title' => 'Reset '.$type,
			'#href' => 'admin/config/epscor/er/reset/'.$type,
			//'#options' => $l_options,
			'#prefix' => '<p>',
			'#suffix' => '</p>',
		);
	}
	
	//Display the table
	$form['status'] = array(
		"#theme"=>"er_status_report",
		'#requirements'=>$requirements,
	);
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
	
	$internal_rules = entity_load('rules_config', FALSE);		
	$drupal_path = ER_INSTALL_DIR . '/rules';
	$dir = new DirectoryIterator($drupal_path);
	$return = array('found'=>array(), 'missing'=>array());
	
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {
		$file_path = $drupal_path . '/' . $fileinfo->getFilename();
 		if (($contents = prep_file_contents($file_path)) !== FALSE){			
						
					$json = '{$internal_rules}';
					
						
					var_dump(json_decode($json));
					
						
					if (var_dump(json_decode($json) == true)){
						$return['found'][$internal_rules] = $internal_rules[$internal_rules];
						
					}else{
						$return['missing'][$name] = $rules;
					}
				
			}
			
			
			else{
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
		
	// loop through all rule files in the directory
	$return = array('missing'=>array(), 'extra'=>array(), 'found'=>array());
	
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {
			$taxonomy_filename = $fileinfo->getFilename();
			include($drupal_path . '/' . $taxonomy_filename);
			$data = db_select('data');
			
// Note to Jeannine: $taxonomy is not defined anywhere, however; 
// after the include() call $data should be defined.

			$data = taxonomy_get_vocabularies();
			if (isset($data) && is_array($data)){
				foreach ($data as $name=>$taxonomy){
					if (isset($data[$name])){
						$return['found'][$name] = $data[$name];
						
						unset($data[$name]);
					}else{
						$return['missing'][$name] = $data;
					}
				}

				}else{
				drupal_set_message('Bad installation file detected, see: ' .  $taxonomy_filename, 'error');
			}
		}
	}
	
	return $return;
}

//function er_test_roles(){
//	$drupal_path = ER_INSTALL_DIR . '/roles_perms';
//	$user_roles = user_roles();
//	unset($user_roles[1], $user_roles[2], $user_roles[3]);//anonymous, authenticated, administrator
//	
//	$return = array('found'=>array(), 'missing'=>array(), 'extra'=>array());
//	$filename = 'role.php';
//	include($drupal_path.'/'.$filename);
//	if (isset($role) && is_array($role)){
//		foreach ($role as $r){
//			$rid = $r['rid'];
//			if (isset($user_roles[$rid])){
//				$return['found'][$name] = $user_roles[$rid];
//				unset($user_roles[$rid]);
//			}else{
//				$return['missing'][$rid] = $r;
//			}
//		}
//	}else{
//		drupal_set_message('Bad installation file detected, see: ' .  $filename, 'error');
//	}
//
//	foreach ($user_roles as $rid=>$name){//iterate over the remaining flags, and look for er_ prefix.
//		$return['extra'][$rid] = $name;
//	}
//	return $return;
//}

function er_test_role_perms(){
	$drupal_path = ER_INSTALL_DIR . '/roles';
	$dir = new DirectoryIterator($drupal_path);
	//$user_roles = user_roles();
	$user_roles = db_select('role')
    ->fields('role')
    ->execute()
    ->fetchAllAssoc('rid');
	//d($user_roles, '$user_roles');
	
	$return = array('found'=>array(), 'missing'=>array(), 'extra'=>array());
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {//Determine if current DirectoryIterator item is '.' or '..'
			$filename = $fileinfo->getFilename();
			include($drupal_path . '/' . $filename);//$role and $role_permission variables should be set now.
			if (isset($role)){
				$rid = $role['rid'];
				if (isset($user_roles[$rid])){
					$return['found'][$rid] = $user_roles[$rid];
					unset($user_roles[$rid]);
				}else{
					$return['missing'][$rid] = $role['name'];
				}
				
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $filename, 'error');
			}
		}
	}
	$return['extra'] = $user_roles;
	return $return;

}