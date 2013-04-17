<?php
function install_all(){
	//Set up some default values for er-core settings; these are used on the summary table.
  variable_set('er_summary_types', array());//This is used on the summary table, see: includes/accomplishments.inc
  variable_set('er_start_date', array('month'=>'9', 'day'=>'15', 'year'=>'2009'));//This is the internal start date of the grant. see: includes/er.admin.inc
  
  //run batch api, install everything.
  er_batch_reset();
}

/**
* Batch 'finished' callback
*/
function batch_has_finished($success, $results, $operations) {
  if ($success) {
  	//TODO: why are the $results and $operations not working
    // Here we do something meaningful with the results.
    // d($success, 'success');
    // d($results, 'results');
    // d($operations, 'operations');
    // $message = count($results) .' processed.';
    $message = 'Batch processed.';
    // $message .= theme('item_list', $results);  // D6 syntax
    drupal_set_message($message);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array('%error_operation' => $error_operation[0], '@arguments' => print_r($error_operation[1], TRUE)));
    drupal_set_message($message, 'error');
  }
}

//This function uses a shortcut to get the contents of an install file then remove the <?php part and return the result.
//Function returns FALSE if the file could not be read
function prep_file_contents($file_path){
	if (file_exists($file_path)){
		$file_contents = file_get_contents($file_path);
		if (substr($file_contents, 0, 5) == '<?php')
			return substr($file_contents, 5);
		else
			return $file_contents;
	}else
		return FALSE;
}

//This function is used by Batch API; it installs a flag from file.
function er_batch_import_flag($file_path, &$context){
	$name = basename($file_path);
	$context['message'] = t('Now processing flag: @name', array('@name' => $name));
	module_load_include('inc', 'flag', 'includes/flag.export');
	if (function_exists('flag_import_form_validate')){
		//watchdog('er', 'Importing flag: ' . $file_path);
		if (($contents = prep_file_contents($file_path)) !== FALSE){
			$form = array();
			$form_state = array();
			$form_state['values']['import'] = $contents;
			// for these imports, work gets done in the _validate and _submit functions to do the actual import
			flag_import_form_validate($form, $form_state);
			flag_import_form_submit($form, $form_state);
			watchdog('er', "Flag $name imported successfully.");
		}else{
			$error = "Flag $name was not imported! File not found or is invalid: $file_path";
			watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
			drupal_set_message($error, 'error');
		}
	}else{		
		$error = 'No flag_import_form_validate function found, installation cannot proceed.';
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error, 'error');
	}
}

//This function is used by Batch API; it installs a rule from rules directory.
function er_batch_import_rules($file_path, &$context){
	$name = basename($file_path);
	$context['message'] = t('Now processing rule: @name', array('@name' => $name));
	// basically I'll be mimicking functionality in the import function, check to make sure it exists, for good measure
	module_load_include('inc', 'rules', 'ui/ui.forms'); // otherwise it can't see the file
	if (function_exists('rules_ui_import_form_validate')){
		//watchdog('er', 'Begin creating rules, found the rules_ui_import_form_validate function');		
		if (($contents = prep_file_contents($file_path)) !== FALSE){			
			$form = array();
			$form_state = array();
			$form_state['values']['import'] = $contents;
			$form_state['values']['overwrite'] = 1;
			
			// for these imports, work gets done in the _validate and _submit functions to do the actual import
			rules_ui_import_form_validate($form, $form_state);
			rules_ui_import_form_submit($form, $form_state);
			watchdog('er', "Rule $name imported successfully.");
		}else{
			$error = "Flag $name was not imported! File not found or is invalid: $file_path";
			watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
			drupal_set_message($error, 'error');
		}
	}else{
		watchdog('er', 'No rules_ui_import_form_validate function found in er.install', NULL, WATCHDOG_CRITICAL);
		drupal_set_message('No rules_ui_import_form_validate function found in er.install');
	}
}

//This function is used by Batch API; it installs a Taxonomy Vocabulary + associated fields from file.
//Note: This is function is basically the same as er_batch_import_fields.
function er_batch_import_taxonomy($file_path, &$context){
	$name = basename($file_path);
	$context['message'] = t('Now processing taxonomy: @name', array('@name' => $name));
	module_load_include('inc', 'taxonomy', 'includes/taxonomy.export');
	if (function_exists('bundle_copy_import_submit')){
		if (($contents = prep_file_contents($file_path)) !== FALSE){
			$form = array();
			$form_state = array();
			$form_state['values']['macro'] = $contents;
			// for these imports, work gets done in the _validate and _submit functions to do the actual import
			bundle_copy_import_submit('', $form_state); // function doesn't use first arg
			watchdog('er', "Taxonomy Vocabulary $name imported successfully.");
		}else{
			$error = "Taxonomy Vocabulary $name was not imported! File not found or is invalid: $file_path";
			watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
			drupal_set_message($error, 'error');
		}
	}else{
		$error = 'No bundle_copy_import_submit function found, Taxonomy Vocabulary installation cannot proceed.';
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error, 'error');
	}
}

//This function is used by Batch API; it installs a content type + fields from file.
function er_batch_import_fields($file_path, &$context){
	$name = basename($file_path);
	$context['message'] = t('Now processing bundle: @name', array('@name' => $name));
	if (function_exists('bundle_copy_import_submit')){
		if (($contents = prep_file_contents($file_path)) !== FALSE){
			$form = array();
			$form_state = array();
			$form_state['values']['macro'] = $contents;
			// for these imports, work gets done in the _validate and _submit functions to do the actual import
			bundle_copy_import_submit('', $form_state); // function doesn't use first arg
			watchdog('er', "Bundle $name imported successfully.");
		}else{
			$error = "Bundle $name was not imported! File not found or is invalid: $file_path";
			watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
			drupal_set_message($error, 'error');
		}
	}else{
		$error = 'No bundle_copy_import_submit function found, bundle installation cannot proceed.';
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error, 'error');
	}
}

function er_get_role_by_name($name){
	return array_pop(db_select('role')
		->fields('role')
		->condition('name', $name)
		->execute()
		->fetchAllAssoc('rid'));
}

//This function was created to handle a special case in which the user already has a role by the name of one used in ER-Core.
//This function will hijack the role by setting the role's RID to the RID that is expected by our system.
function er_batch_hijack_roles($file_path, &$context){
	$name = basename($file_path);
	$context['message'] = t('Now hijacking role: @name', array('@name' => $name));
	if (file_exists($file_path)){
		include($file_path);//$role variable becomes available
		$db_role = er_get_role_by_name($role['name']);
		if (isset($db_role->rid) && $internal_role = user_role_load($db_role->rid)){
			$internal_role->static_rid = $role['rid'];//get the system to reset it's rid to the one that is expected.
			$status = user_role_save($internal_role);
			watchdog('er', "Role {$role['name']} successfully hijacked.");
		}else{
			$error = "There is no role with RID:$rid in the system! The system was unable to hijack this role.";
			watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
			drupal_set_message($error, 'error');
		}
	}else{
		$error = "Role $name was not hijacked! File not found: $file_path";
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error, 'error');
	}
}

function er_batch_import_role($file_path, &$context){
	$name = basename($file_path);
	$context['message'] = t('Now processing role: @name', array('@name' => $name));
	if (file_exists($file_path)){
		include($file_path);
		if (isset($role)){
			$rid = $role['rid'];
			$name = $role['name'];
			if (user_role_load($rid) === FALSE){//role does not exist
				if (count(er_get_roles_by_name($role['name']))){
					$error = "Unable to create role! The name \"$name\" was already taken!";
					watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
					drupal_set_message($error, 'error');
					return;
				}else{
					$new_role = (object) array(
						'name' => $role['name'],
						'static_rid' => $role['rid'],
					);
					$status = user_role_save($new_role);
					watchdog('er', "Role {$role['name']} successfully created.");
				}
				
				$new_role = (object) array(
					'name' => $role['name'],
					'static_rid' => $role['rid'],
				);
				$status = user_role_save($new_role);
				watchdog('er', "Role {$role['name']} successfully created.");
			}
			if (isset($role['permissions'])){//now to set permissions...
				$array_perms = array(); //This becomes the final permissions array
				//Get the list all of the available permissions, with the given modules that are currently installed:
				$modules = user_permission_get_modules();
				//Check to make sure the permissions are valid:
				foreach ($role['permissions'] as $permission_name){
					// This next call has to do with avoiding a Fatal Error when you try to add a permission
					//	that's not associated with an enabled module.		
					// see http://drupal.org/node/737816 and http://drupal.org/node/1249952 for more info	
					if (isset($modules[$permission_name])){//This permission currently exists in the system
						$array_perms[] = $permission_name;
					}else{
						$error = "Unable to attach \"$permission_name \" to role $name, module is likely missing!";
						watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
						//drupal_set_message($error, 'error');
					}
				}
				// Add that array of permissions to the role. a Drupal API call
				user_role_grant_permissions($rid, $array_perms);
				//d($array_perms, '$array_perms');
				watchdog('er', 'Granted permissions to role '.$name);
			}
			
		}else{
			$error = "Role $name was not imported! Unable to read data contained in: $file_path";
			watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
			drupal_set_message($error, 'error');
		}
	}else{
		$error = "Role $name was not imported! File not found: $file_path";
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error, 'error');
	}
}


/**
 * This function adjusts the RID of newly created roles, this will allow us to use the same RID on all installations of er-core.
 * This allows the user to change the human-readible name of the role without breaking our testing/update/reset suite.
 * I based this aproach on the role_export contrib module. - Andrew
 */
function er_user_role_static_rid($role){
  // Update the numeric id.
	if ($role->static_rid){//if a statid rid is specified, use that, otherwise just let drupal generate one.
		db_update('role')
			->fields(array(
				'rid' => $role->static_rid//change the rid to use the static id provided
			))
			->condition('rid', $role->rid)
			->execute();
		// Set the new role id in the role object, so that it is available for
		// other modules.
		$role->rid = $role->static_rid;
	}
}

/**
 * Implements hook_user_role_insert().
 * See function er_user_role_static_rid
 */
function er_user_role_insert($role) {
	er_user_role_static_rid($role);
}

/**
 * Implements hook_user_role_update().
 * See function er_user_role_static_rid
 */
function er_user_role_update($role) {
	er_user_role_static_rid($role);
}

/*
 * This menu callback gets called for URI: admin/config/epscor/er/reset/*
 * See er_install_reset_form function
 */
function er_installer_reset_callback($type, $filename){
	return drupal_get_form("er_installer_reset_form", $type, $filename);
}

/*
 * This sets up a confirmation form which is presented to the user before running batch operations
 *  that will reset/re-run some part of the installation process.
 */
function er_installer_reset_form($form, &$form_state){
	$acceptable = er_get_install_types();
	list($type, $filename) = $form_state['build_info']['args'];//Note: $type comes directly from user input. Don't trust it.
	if (in_array($type, $acceptable)){//only accept certain known values.
		$button_label = t('Reset');
		if (isset($filename) && $filename != "" && $type != "all"){//if you specify the name of the specific thing you want (bundle, flag, etc)
			if ($type == 'hijack_roles'){
				$title = t('Are you sure you want to hijack the role?');
				$cancel_path = 'admin/config/epscor/er/status';
				$button_label = t('Hijack');
			}else{
				$label = "$filename ($type)";
				$title = t('Are you sure you want to reset @type?', array('@type'=>$label));
				$cancel_path = 'admin/config/epscor/er/status';
			}
		}else{
			//d(get_batch_operations($type), 'batch operations');
			$label = $type=="all"?"everything imported by er-core":"all ER-core $type";
			$title = t('Are you sure you want to reset @type?', array('@type'=>$label));
			$cancel_path = 'admin/config/epscor/er/status';
		}
		return confirm_form($form, $title, $cancel_path, t('This action cannot be undone.'), $button_label, t('Cancel'));
	}
	return array('#disabled'=>true);//invalid input, just return a junk form.
}

function er_installer_reset_form_submit($form, &$form_state){
	list($type, $filename) = $form_state['build_info']['args'];
	er_batch_reset($type, $filename);
}

/*
 * This function prepares the Batch API calls used in er-core installation, and begins the process.
 * This is being used by the reset form, it allows you to begin a batch operation to reset some part of the install.
 * see get_batch_operations function below
 */
function er_batch_reset($type = "all", $filename = "", $op = ""){
	$batch = array (
		'operations' => get_batch_operations($type, $filename),
		'finished' => 'batch_has_finished',
		'title' => t('ER Core Batch Processing'),
		'init_message' => t('Initializing batch process...'),
		'progress_message' => t('Processed @current out of @total.'),
		'error_message' => t('ER Core Batch Processing has encountered an error.'),
	);
	batch_set($batch); // Standard Batch call
	batch_process('admin/config/epscor/er/status');
}

function er_batch_api_callbacks(){
	static $acceptable_types = array(
		"taxonomy"=>array(
			'directory'=>'taxonomy',
			'callback'=>"er_batch_import_taxonomy",
		),
		"roles"=>array(
			'directory'=>'roles',
			'callback'=>"er_batch_import_role",
		),
		"bundles"=>array(
			'directory'=>'bundles',
			'callback'=>"er_batch_import_fields",
		),
		"flags"=>array(
			'directory'=>'flags',
			'callback'=>"er_batch_import_flag",
		),
		"rules"=>array(
			'directory'=>'rules',
			'callback'=>"er_batch_import_rules",
		),
		
		//Special functions:
		//These do not correspond to installation types, but still may be run through batch api.
		"hijack_roles"=>array(
			'directory'=>'roles',
			'callback'=>"er_batch_hijack_roles",
		),
	);
	return $acceptable_types;
}


//This just returns the list of acceptable installation types...
function er_get_install_types(){
	return array_keys(er_batch_api_callbacks()) + array("all");
}

/**
 * Automatically generate batch operations for the different installation types
 * 
 */
function get_batch_operations($type = "all", $filename = ""){
	/*
	Note: We should install things in this order:
		- create taxonomy terms, they will be used by Entity Reference fields when we create Content Types
		- create Content Types and Fields
		- create User Profile Fields
		- create flags, that may now attach to the proper bundles (content types)
		- create rules
  */
	//This array represents the available "installation types".
	// It maps the name of the directory (from er/install/*) to the function used to import that installation type.
	static $acceptable_types = array(
		"taxonomy"=>array(
			'directory'=>'taxonomy',
			'callback'=>"er_batch_import_taxonomy",
		),
		"roles"=>array(
			'directory'=>'roles',
			'callback'=>"er_batch_import_role",
		),
		"bundles"=>array(
			'directory'=>'bundles',
			'callback'=>"er_batch_import_fields",
		),
		"flags"=>array(
			'directory'=>'flags',
			'callback'=>"er_batch_import_flag",
		),
		"rules"=>array(
			'directory'=>'rules',
			'callback'=>"er_batch_import_rules",
		),
		
		//Special functions:
		//These do not correspond to installation types, but still may be run through batch api.
		"hijack_roles"=>array(
			'directory'=>'roles',
			'callback'=>"er_batch_hijack_roles",
		),
	);
	
	$operations = array();
	if ($type == "all"){//indicates that we want to install everything
		foreach ($acceptable_types as $type => $tdata){//loop through each of the installation types
			$drupal_path = ER_INSTALL_DIR . '/' . $tdata['directory'];
			$dir = new DirectoryIterator($drupal_path);
			foreach ($dir as $fileinfo) {// loop through each file in the directory
				if (!$fileinfo->isDot()) { // make sure its not . or ..
					$operations[] = array($tdata['callback'], array($drupal_path . '/' . $fileinfo->getFilename()));
				}
			}
		}
	}else{//We want to reset a specific installation type
		if (isset($acceptable_types[$type]) && $tdata = $acceptable_types[$type]){//get the corresponding installation function if it exists.
			$drupal_path = ER_INSTALL_DIR . '/' . $tdata['directory'];
			if (isset($filename) && $filename != ""){//if a specific filename was given...
				$operations[] = array($tdata['callback'], array($drupal_path . '/' . $filename));//just return the one file
			}else{// .. otherwise loop over each file in the directory belonging to this installation type (ex: 'bundles', 'flags', ...)
				$dir = new DirectoryIterator($drupal_path);
				foreach ($dir as $fileinfo) {// loop through each file in the directory
					if (!$fileinfo->isDot()) { // make sure its not . or ..
						$operations[] = array($tdata['callback'], array($drupal_path . '/' . $fileinfo->getFilename()));
					}
				}
			}
		}
	}
	return $operations;
}