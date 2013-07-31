<?php
/*
 * This file contains the tests for the er module.
 */
function er_testing() {
	////this may be useful for checking taxonomy or content types.
	//d(field_info_bundles(), 'field_info_bundles()');
	$link_options = array(
		'query' => array('destination' => 'admin/config/epscor/er/status'),
		'html' => true,
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
			'value' => '<i class="icon-exclamation-sign"></i> Missing',
			'description' => 'PHPExcel library has not been installed! Please refer to the '. l('documentation', 'admin/help/er', array('fragment' => 'post-install')) . '.',
		);
	}
	
	//Check that there is at least one institution in the system
	$num_institutions = _test_institutions();
	$new_institution_link = l('<i class="icon-plus-sign"></i> Add an Institution', 'node/add/er-institution', array('html'=>true));
	if ($num_institutions){// must be greater than 0
		$requirements[] = array(
			'title' => 'More than one Institution in the system',
			'severity' => REQUIREMENT_OK,
			'value' => 'Requirement has been met',
		);
	}else{
		$requirements[] = array(
			'title' => 'More than one Institution in the system',
			'severity' => REQUIREMENT_ERROR,
			'value' => 'No Institutions found!',
			'description' => 'ER Core requires you to add at least one Institution to the system.<br>' . $new_institution_link,
		);
	}
	
	//Check that there is at least one institution in the system
	$num_components = _test_components();
	$new_component_link = l('<i class="icon-plus-sign"></i> Add a Research Component', 'admin/structure/taxonomy/er_components/add', array('html'=>true));
	if ($num_components){// must be greater than 0
		$requirements[] = array(
			'title' => 'More than one Research Component in the system',
			'severity' => REQUIREMENT_OK,
			'value' => 'Requirement has been met',
		);
	}else{
		$requirements[] = array(
			'title' => 'More than one Research Component in the system',
			'severity' => REQUIREMENT_ERROR,
			'value' => 'No Research Components found!',
			'description' => 'ER Core requires you to add at least one research component to the system.<br>' . $new_component_link,
		);
	}
	
	//Taxonomy
	$taxonomy_status = _test_taxonomy();
	//d($taxonomy_status, '$taxonomy_status');
	foreach ($taxonomy_status['missing'] as $name=>$taxonomy_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Taxonomy: $name missing!",
			'value' => create_reset_link('taxonomy', $name, 'taxonomy'),
		);
	}
	foreach ($taxonomy_status['extra'] as $name=>$taxonomy_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Taxonomy: $name is extraneous",
			'description' => 'This taxonomy is unknown to this version of the EPSCoR Reporting module, and may be removed.',
			'value' => l('<i class="icon-trash"></i> Delete', 'admin/structure/taxonomy/'.$name.'/edit', $link_options),
		);
	}
	foreach ($taxonomy_status['found'] as $name=>$taxonomy_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Taxonomy: $name",
			'value' => l('<i class="icon-ok"></i> Found', 'admin/structure/taxonomy/'.$name, array('html'=>true)) . ', ' . create_reset_link('taxonomy', $name),
		);
	}
	
	//Flags
	$flags_status = _test_flags();
	//d($flags_status, '$flags_status');
	foreach ($flags_status['missing'] as $name=>$flags_value){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Flag: $name missing!",
			'value' => create_reset_link('flags', $name, 'flag'),
		);
	}
	foreach ($flags_status['extra'] as $name=>$flag_node){
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Flag: $name is extraneous",
			'description' => 'This flag is unknown to this version of the EPSCoR Reporting module, and may be removed.',
			'value' => l('<i class="icon-trash"></i> Delete', 'admin/structure/flags/manage/'.$name.'/delete', $link_options),
		);
	}
	foreach ($flags_status['found'] as $name=>$flag_node){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Flag: $name",
			'value' => l('<i class="icon-ok"></i> Found', 'admin/structure/flags/manage/'.$name, array('html'=>true)) . ', ' . create_reset_link('flags', $name),
		);
	}
	
	//Roles
	$role_status = _test_role_perms();
	//d($role_status, '$role_status');
	foreach ($role_status['hijack'] as $rid=>$role){
		$name = $role->name;
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Role: $name",
			'description' => "The system found a role by the name of \"$name\" that is in conflict with ER-Core. <br>You can choose to rename the role, or you may hijack the role for use in ER-Core's system. Reserving the role will <u>not</u> automatically reset it's permissions.",
			'value' => l('<i class="icon-refresh"></i> Hijack role'.($label?" $label":""), "admin/config/epscor/er/reset/hijack_roles/$rid", array('html'=>true)),
		);
	}	
	foreach ($role_status['missing'] as $rid=>$name){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Role: $name role is missing!",
			'value' => create_reset_link('roles', $rid, 'this role'),
		);
	}
	foreach ($role_status['extra'] as $rid=>$role){
		$name = $role->name;
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Role: $name role is extraneous",
			'description' => 'This user role is unknown to this version of the EPSCoR Reporting module, and may be removed if desired.',
			'value' => l('<i class="icon-trash"></i> Delete', 'admin/people/permissions/roles/delete/'.$rid, $link_options) . ', ' . l('<i class="icon-file"></i> Export', 'admin/people/permissions/roles/export/'.$rid, $link_options),
		);
	}
	foreach ($role_status['found'] as $rid=>$role){
		$name = $role->name;
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Role: $name",
			'value' => ($rid>2?l('<i class="icon-ok"></i> Found', 'admin/people/permissions/roles/edit/'.$rid, array('html'=>true)):'<i class="icon-ok"></i> Found') . ', ' . create_reset_link('roles', $rid) . ', ' . l('<i class="icon-pencil"></i> View Permissions', 'admin/people/permissions/'.$rid, array('html'=>true)) . ', ' . l('<i class="icon-file"></i> Export', 'admin/people/permissions/roles/export/'.$rid, $link_options),
		);
	}

	//Rules
	$rules_status = _test_rules();
	//d($rules_status, '$rules_status');
	foreach ($rules_status['missing'] as $name=>$value){
		$requirements[] = array(
			'severity' => REQUIREMENT_ERROR,
			'title' => "Rule: $name missing!",
			'value' => create_reset_link('rules', $name, 'this rule'),
		);
	}
	foreach ($rules_status['extra'] as $name=>$value){
		$requirements[] = array(
			'severity' => REQUIREMENT_WARNING,
			'title' => "Rule: $name is extraneous",
			'description' => 'This rule is unknown to this version of the EPSCoR Reporting module, and may be removed.',
			'value' => l('<i class="icon-trash"></i> Delete', 'admin/config/workflow/rules/reaction/manage/'.$value->id.'/delete', $link_options),
		);
	}
	foreach ($rules_status['found'] as $name=>$value){
		$requirements[] = array(
			'severity' => REQUIREMENT_OK,
			'title' => "Rule: $name",
			'value' => l('<i class="icon-ok"></i> Found', 'admin/config/workflow/rules/reaction/manage/'.$value->id, array('html'=>true)) . ', ' . create_reset_link('rules', $name),
		);
	}

	//Fields and fieldgroups
	$field_status = _test_fields();
	foreach ($field_status as $bundle=>$results){
		$title = "Bundle: ".$results['name']." ($bundle)";
		$has_missing = false;
		$has_extra = false;
		$description = null;
		if (count($results['extra_fields']) || count($results['extra_groups'])){//if missing fields or groups were returned
			$has_extra = true;
			if (count($results['extra_fields'])){
				$description .= "Extra fields: ".implode(", ", array_keys($results['extra_fields'])).'<br>';
			}
			if (count($results['extra_groups'])){
				$description .= "Extra field groups: ".implode(", ", array_keys($results['extra_groups'])).'<br>';
			}
			$description .= "<i>Extra fields may be removed without breaking ER Core's functionality. You may need to transfer data out of an old field into a new field. Please consult the changelog if available.</i><br>";
		}
		if (count($results['missing_fields']) || count($results['missing_groups'])){//if missing fields or groups were returned
			$has_missing = true;
			if (count($results['missing_fields'])){
				$description .= '<strong>Missing fields:</strong> '.implode(", ", array_keys($results['missing_fields'])).'<br>';
			}
			if (count($results['missing_groups'])){
				$description .= '<strong>Missing field groups:</strong> '.implode(", ", array_keys($results['missing_groups'])).'<br>';
			}
			$description .= "<i>New fields may have been added to ER Core, in which case it is suggested that you manually reset this bundle.</i><br>";
		}
		
		$severity = REQUIREMENT_OK;
		$value = l('<i class="icon-ok"></i> Found', 'admin/structure/types/manage/'.$bundle, array('html'=>true)).', '.create_reset_link('bundles', $bundle, 'this bundle');
		if ($has_missing){
			$severity = REQUIREMENT_ERROR;
			$value = create_reset_link('bundles', $bundle, 'this bundle');
		}else if($has_extra){
			$severity = REQUIREMENT_WARNING;
		}
		
		$requirements[] = array(
			'severity' => $severity,
			'title' => $title,
			'description' => $description,
			'value' => $value,
		);
	}
	
	//d($field_status, '$field_status');
	$form['reset'] = array(
		'#type'=>'fieldset',
		'#title'=>'Bulk Reset',
		'#collapsible'=>true,
		'#collapsed'=>true,
	);
	
	$installation_types = er_get_install_types();//don't include the special functions...
	foreach ($installation_types as $type){
		$form['reset'][$type] = array(
			'#type' => 'link',
			'#title' => 'Reset '.$type,
			'#href' => 'admin/config/epscor/er/reset/'.$type,
			//'#options' => $l_options,
			'#prefix' => '<p>',
			'#suffix' => '</p>',
			'#html' => true,
		);
	}
	
	//Devel
	if (function_exists('dpm')){
		$requirements[] = array(
			'title' => 'Development Tools',
			'severity' => REQUIREMENT_OK,
			'value' => 'Devel is enabled',
			'description' => 'This module should be enabled in a development environment, but should not be necessary for a live site.',
		);
	}
	
	//Display the table
	$form['status'] = array(
		"#theme"=>"er_status_report",
		'#requirements'=>$requirements,
	);
	return $form;
}

function create_reset_link($type, $name, $label = ''){
	return l('<i class="icon-refresh"></i> Reset'.($label?" $label":""), "admin/config/epscor/er/reset/$type/$name", array('html'=>true));
}

function _test_institutions(){
	$query = new EntityFieldQuery();
  $count = $query
		->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'er_institution')
		->count()->execute();
	return $count;
}

function _test_components(){
	$taxa = taxonomy_vocabulary_machine_name_load('er_components');//need this to get the vid of the vocabulary
	$query = new EntityFieldQuery();
  $count = $query
		->entityCondition('entity_type', 'taxonomy_term')
    ->entityCondition('vid', $taxa->vid)
		->count()->execute();
	return $count;
}

/**
 * Check the status of fields and fieldgroups
 */
function _test_fields(){
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
			
			$installed_fields = field_info_instances($entity_type, $bundle_type);
			if (isset($data['fields'])){ //Make sure there are fields
				foreach ($data['fields'] as $field_name=>$field){ //Check each field
					if (isset($installed_fields[$field_name])){//the field exists
						unset($installed_fields[$field_name]);
						unset($data['fields'][$field_name]);
					}
				}
			}
			
			$installed_groups = field_group_info_groups($entity_type, $bundle_type, 'form', TRUE);//get the list of groups in this bundle.
			
			//This will remove duplicates and change the key to be the group_name and not the weirdo composite id that it gets packaged with...
			$fieldgroups = array();
			if (isset($data['fieldgroups'])) //if this bundle has fieldgroups...
				foreach ($data['fieldgroups'] as $group_id=>$group){
					$fieldgroups[$group->group_name] = $group;
				}
			
			foreach ($fieldgroups as $group_name=>$group){ //Check each fieldgroup
				if (isset($installed_groups[$group_name])){//the field exists
					unset($installed_groups[$group_name]);
					unset($fieldgroups[$group_name]);
				}
			}
			
			$return[$bundle_type] = array(
				'missing_fields' => $data['fields'],
				'extra_fields' => $installed_fields,
				'missing_groups' => $fieldgroups,
				'extra_groups' => $installed_groups,
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
function _test_flags(){
	$drupal_path = ER_INSTALL_DIR . '/flags';
	$dir = new DirectoryIterator($drupal_path);
	$internal_flags = flag_get_flags();
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
function _test_rules(){	
	$drupal_path = ER_INSTALL_DIR . '/rules';
	$dir = new DirectoryIterator($drupal_path);
	$return = array('found'=>array(), 'missing'=>array());
	
	$internal_rules_raw = entity_load('rules_config', FALSE);
	$internal_rules = array();//construct a associative array, where the key is the name of the rule
	foreach ($internal_rules_raw as $k=>$r)
		$internal_rules[$r->name] = $r;
	
	//d($internal_rules, '$internal_rules');
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {
			$file_path = $drupal_path . '/' . $fileinfo->getFilename();
			if (($contents = prep_file_contents($file_path)) !== FALSE){
				$data = json_decode($contents, TRUE);//convert the JSON string into a php array
				foreach ($data as $name=>$value){//should just be one value
					if (isset($internal_rules[$name])){
						$return['found'][$name] = $internal_rules[$name];
						unset($internal_rules[$name]);
					}else{
						$return['missing'][$name] = $value;
					}
				}
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $rules_filename, 'error');
			}
		}
	}
	$return['extra'] = $internal_rules;
	return $return;
}

/*
 * This function tests to see if all of the Rules are up to date with what's in the Rules install directory.
 * @author Jeannine Haney
 */
function _test_taxonomy(){
	$drupal_path = ER_INSTALL_DIR . '/taxonomy';
	$dir = new DirectoryIterator($drupal_path);
	 
	// loop through all rule files in the directory
	$return = array('missing'=>array(), 'extra'=>array(), 'found'=>array());
	$internal_taxonomy = array();
	foreach (taxonomy_get_vocabularies() as $vocab)
		$internal_taxonomy[$vocab->machine_name] = $vocab;
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {
			$taxonomy_filename = $fileinfo->getFilename();
			include($drupal_path . '/' . $taxonomy_filename);
			//d($data, 'data');
			if (isset($data) && is_array($data)){
				foreach ($data['bundles'] as $name=>$install_taxa){
					if (isset($internal_taxonomy[$name])){//the taxonomy exists in the system
						$return['found'][$name] = $install_taxa;
						unset($internal_taxonomy[$name]);
					}else{
						$return['missing'][$name] = $install_taxa;
					}
				}
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $taxonomy_filename, 'error');
			}
		}
	}
	foreach ($internal_taxonomy as $name => $node){//iterate over the remaining flags, and look for er_ prefix.
		if (substr($name, 0, 3) == "er_")
			$return['extra'][$name] = $node;
	}
	// d($return, '$return');
	return $return;
}


function _test_role_perms(){
	$drupal_path = ER_INSTALL_DIR . '/roles';
	$dir = new DirectoryIterator($drupal_path);
	//$user_roles = user_roles();
	$user_roles = db_select('role')
    ->fields('role')
    ->execute()
    ->fetchAllAssoc('rid');
	
	$user_roles_by_name = array();
	foreach ($user_roles as $key=>$obj) $user_roles_by_name[$obj->name] = $key;
	
	$return = array('found'=>array(), 'missing'=>array(), 'extra'=>array(), 'hijack'=>array());
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
					if ($key = $user_roles_by_name[$role['name']]){
						$return['hijack'][$rid] = $user_roles[$key];
						unset($user_roles[$key]);
					}else{
						$return['missing'][$rid] = $role['name'];
					}
				}
				
			}else{
				drupal_set_message('Bad installation file detected, see: ' .  $filename, 'error');
			}
		}
	}
	$return['extra'] = $user_roles;
	return $return;
}
