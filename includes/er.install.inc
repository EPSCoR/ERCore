<?php
function install_all(){
	//Set up some default values for er-core settings; these are used on the summary table.
  variable_set('er_summary_types', array());//This is used on the summary table, see: includes/accomplishments.inc
  variable_set('er_start_date', array('month'=>'9', 'day'=>'15', 'year'=>'2009'));//This is the internal start date of the grant. see: includes/er.admin.inc
  
  //run batch api, install everything.
  er_batch_reset();
	
	//TODO: Taxonomy and Rules need to be integrated into the er_batch_reset procedure.
	
	//er_import_rules();
	//er_import_perms();
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

//THIS SHOULD BE DEPRECATED IN FAVOR OF er_installer_reset_form
//* this function is called from er.admin.inc when they reset fields, rules, flags, etc. */
function er_revert($form, &$form_state){
/* The order of this install is as such:
	- create taxonomy terms, they will be used by Entity Reference fields when we create Content Types
	- create Content Types and Fields, create User Profile Fields
	- create flags, that may now attach to the proper bundles (content types)
	- create rules
*/

	//Jeannine's attempt at taxonomy import	
	if ($form_state['values']['redo_checkboxes']['Taxonomy'] !== 0){ 
	    // you need the explicit equals btw
		// Create taxonomy terms
		// d('you checked Taxonomy');
		er_import_taxonomy();	
	}	
	// This is the one MIKE is assigned to
	if ($form_state['values']['redo_checkboxes']['Fields'] !== 0){
		// Create Content Types and Field Settings, User Profile Fields
		// d('you checked Fields');
		er_import_fields();	
	}	
	
	if ($form_state['values']['redo_checkboxes']['Flags'] !== 0){	
		// Create flags
		// d('you checked Flags');
		er_import_flags();
	}
	
	if ($form_state['values']['redo_checkboxes']['Rules'] !== 0){	
		// Create rules
		// d('you checked Rules');
		er_import_rules();
	}
	
	if ($form_state['values']['redo_checkboxes']['Permissions'] !== 0){	
		// Reset permissions
		// d('you checked Permissions');
		er_import_perms();
		drupal_set_message('Roles and permissions have been reset.');
	}
		
}

/* this function is called from er.admin.inc when they reset the permissions */
function er_import_perms(){
	$er_roles = array('student', 'administrative staff', 'faculty');
	// these are the two dumps from phpMyAdmin, by including them, 
	//   we load the arrays ($role_permission and $role) and can use them
	$perms_path = ER_INSTALL_DIR . '/roles_perms/';
	include $perms_path . 'role.php';
	include $perms_path . 'role_permission.php';

	foreach ($er_roles as $er_role){
		// note: searchRIDdb and searchRIDfile are defined in this same file, not API calls
		if (!is_null($student_rid = searchRIDdb($er_role))){
			// Remove role, this is a Drupal API call 
			// http://api.drupal.org/api/drupal/modules!user!user.module/function/user_role_delete/7
			user_role_delete($er_role);
			watchdog('er', 'Deleted existing role '.$er_role);
		}
		
		// find indexes for permissions involving role
		$ridArr = array();	
		foreach ($role_permission as $row){
			$ridArr[] = $row['rid'];
		}
	
		// find out RID from database
		$ridIdx = searchRIDfile($role, $er_role);
		$rid = $role[$ridIdx]['rid'];
		
		$justIdx = array_keys($ridArr, $rid);
		
		// create role, remember we deleted it if it existed earlier
		$newrole = new stdClass();
		$newrole->name = $er_role;
		user_role_save($newrole);
		
		// Role to grant the permissions to
		$newrole = user_role_load_by_name($er_role);
		$newrole_rid = $newrole->rid;
		// Define our $er_role role permissions
		$array_perms = array();
		// Grant permissions to our $er_role role		
		// loop through all of those indexes, telling what permissions they are		
		
		foreach ($justIdx as $idx){
			// This next call has to do with avoiding a Fatal Error when you try to add a permission
			//	that's not associated with an enabled module.		
			// see http://drupal.org/node/737816 and http://drupal.org/node/1249952 for more info	
			$modules = user_permission_get_modules();
			if (!is_null($modules[$role_permission[$idx]['permission']])){
				// there is an enabled/recognized module associated with this permission
				$array_perms[] = $role_permission[$idx]['permission'];
			}
		}
		
		// Add that array of permissions to the role. a Drupal API call
		user_role_grant_permissions($newrole_rid, $array_perms);
		watchdog('er', 'Created and granted permissions to role '.$er_role);			
	}	
}

function searchRIDdb($name){
	$result = db_query("SELECT rid, name FROM {role} WHERE name = :name", array(
	  ':name' => $name,
	));
	$rid = $result->fetchAll();
	if (empty($rid)){
		// d($name . ' does not exist.');
		return NULL;
	}		
	else {
		$rid = $rid[0]->rid;
		// d($rid, $name . ' exists as ');
		return $rid;
	}	
}

function searchRIDfile($array, $name) {
   foreach ($array as $key => $val) {
       if ($val['name'] === $name) {
           return $key;
       }
   }
   return null;
}

//SHOULD BE DEPRECATED
function er_import_rules(){
	// basically I'll be mimicking functionality in the import function, check to make sure it exists, for good measure
	$ret = module_load_include('inc', 'rules', 'ui/ui.forms'); // otherwise it can't see the file
	 //require_once(drupal_get_path('module', 'rules_admin') .'/ui/ui.forms.inc');
	if (function_exists('rules_ui_import_form_validate')){
		watchdog('er', 'Begin creating rules, found the rules_ui_import_form_validate function');

		$drupal_path = ER_INSTALL_DIR . '/rules';
		$dir = new DirectoryIterator($drupal_path);
		// loop through all rule files in the directory
		foreach ($dir as $fileinfo) {
			if (!$fileinfo->isDot()) {
				$rule_filename = $fileinfo->getFilename();
				watchdog('er', 'Looping rule '.$rule_filename);
				// this grabs the text from the file and puts it in a variable
				$rules_config = file_get_contents($drupal_path . '/' . $rule_filename);
				
				$form = array();
				$form_state = array();
				$form_state['values']['import'] = $rules_config;
				$form_state['values']['overwrite'] = 1;
				
				// for these imports, work gets done in the _validate and _submit functions to do the actual import
				rules_ui_import_form_validate($form, $form_state);
				rules_ui_import_form_submit($form, $form_state);				
			}
		}
	}
	else{		
		watchdog('er', 'No rules_ui_import_form_validate function found in er.install', NULL, WATCHDOG_CRITICAL);
		drupal_set_message('No rules_ui_import_form_validate function found in er.install');
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
			drupal_set_message($error);
		}
	}else{		
		$error = 'No flag_import_form_validate function found, installation cannot proceed.';
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error);
	}
}

//This function is used by Batch API; it installs a rule from rules directory.
function er_batch_import_rules($file_path, &$context){
	$name = basename($file_path);
	
	echo t("rules here!");
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
			drupal_set_message($error);
		}
	}else{
		$error = 'No bundle_copy_import_submit function found, Taxonomy Vocabulary installation cannot proceed.';
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error);
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
			drupal_set_message($error);
		}
	}else{
		$error = 'No bundle_copy_import_submit function found, bundle installation cannot proceed.';
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error);
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
						//drupal_set_message($error);
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
			drupal_set_message($error);
		}
	}else{
		$error = "Role $name was not imported! File not found: $file_path";
		watchdog('er', $error, NULL, WATCHDOG_CRITICAL);
		drupal_set_message($error);
	}
}

/**
 * Implements hook_user_role_insert().
 * This function adjusts the RID of newly created roles, this will allow us to use the same RID on all installations of er-core.
 * This allows the user to change the human-readible name of the role without breaking our testing/update/reset suite.
 * I based this aproach on the role_export contrib module. - Andrew
 */
function er_user_role_insert($role) {
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

/*
 * This menu callback gets called for URI: admin/config/epscor/er/reset/*
 * See er_install_reset_form function
 */
function er_installer_reset_callback($type, $filename){
	return drupal_get_form("er_installer_reset_form", $type, $filename);
}

//This just returns the list of acceptable installation types...
function er_get_install_types(){
	static $acceptable = array("all", "flags", "bundles", "rules", "taxonomy", "roles");//only accept these expected installation types
	return $acceptable;
}

/*
 * This sets up a confirmation form which is presented to the user before running batch operations
 *  that will reset/re-run some part of the installation process.
 */
function er_installer_reset_form($form, &$form_state){
	$acceptable = er_get_install_types();
	list($type, $filename) = $form_state['build_info']['args'];//Note: $type comes directly from user input. Don't trust it.
	if (in_array($type, $acceptable)){//only accept certain known values.
		if (isset($filename) && $filename != "" && $type != "all"){//if you specify the name of the specific thing you want (bundle, flag, etc)
			$label = "$filename ($type)";
			$title = t('Are you sure you want to reset @type?', array('@type'=>$label));
			$cancel_path = 'admin/config/epscor/er/status';
		}else{
			//d(get_batch_operations($type), 'batch operations');
			$label = $type=="all"?"everything imported by er-core":"all ER-core $type";
			$title = t('Are you sure you want to reset @type?', array('@type'=>$label));
			$cancel_path = 'admin/config/epscor/er/status';
		}
		return confirm_form($form, $title, $cancel_path, t('This action cannot be undone.'), t('Reset'), t('Cancel'));
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
function er_batch_reset($type = "all", $filename = ""){
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
		static $fn_map = array(
		"taxonomy"=>"er_batch_import_taxonomy",
		"roles"=>"er_batch_import_role",
		"bundles"=>"er_batch_import_fields",
		"flags"=>"er_batch_import_flag",
		"rules"=>"er_batch_import_rules",
	);
	
	$operations = array();
	if ($type == "all"){//indicates that we want to install everything
		foreach ($fn_map as $type => $fn){//loop through each of the installation types
			$drupal_path = ER_INSTALL_DIR . '/' . $type;
			$dir = new DirectoryIterator($drupal_path);
			foreach ($dir as $fileinfo) {// loop through each file in the directory
				if (!$fileinfo->isDot()) { // make sure its not . or ..
					$operations[] = array($fn, array($drupal_path . '/' . $fileinfo->getFilename())); //;
				}
			}
		}
	}else{//We want to reset a specific installation type
		if (isset($fn_map[$type]) && $fn = $fn_map[$type]){//get the corresponding installation function if it exists.
			$drupal_path = ER_INSTALL_DIR . '/' . $type;
			if (isset($filename) && $filename != ""){//if a specific filename was given...
				$operations[] = array($fn, array($drupal_path . '/' . $filename));//just return the one file
			}else{// .. otherwise loop over each file in the directory belonging to this installation type (ex: 'bundles', 'flags', ...)
				$dir = new DirectoryIterator($drupal_path);
				foreach ($dir as $fileinfo) {// loop through each file in the directory
					if (!$fileinfo->isDot()) { // make sure its not . or ..
						$operations[] = array($fn, array($drupal_path . '/' . $fileinfo->getFilename())); //;
					}
				}
			}
		}
	}
	return $operations;
}