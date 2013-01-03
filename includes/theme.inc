<?php
//hook_theme
function er_theme($existing, $type, $theme, $path){
	$path = ER_MODULE_DIR.'/templates';
	$theme = array(
		'er_help'=> array(//produces a block, and a page: node/add2
			'variables' => array(),
			'render element' => 'links',
			'template' => 'er-help',
			'path' => $path,
		),
		//'er_formatter_node_reference_full_node' => array(
		//	'arguments' => array('element' => NULL),
		//),
		'er_content_links'=> array(//produces a block, and a page: node/add2
			'variables' => array('full_view' => NULL),
			'render element' => 'links',
			'template' => 'er-content-links',
			'path' => $path,
		),
		'er_add_core_content'=> array(//produces node/add3
			'variables' => array('full_view' => NULL),
			'render element' => 'links',
			'template' => 'er-add-core-content',
			'path' => $path,
		),
		'er_collab_inst_block'=> array(//produces node/add3
			'variables' => array(),
			'render element' => 'links',
			'template' => 'er-collab-inst-block',
			'path' => $path,
		),
		'er_formatter_format_custom_date' => array(
			'variables' => array('element' => NULL),
			'function' => 'theme_er_format_custom_date'
		),
		'er_summary_table' => array(
			'variables' => array('ranges' => array(), 'debug'=>false),
			'function' => 'er_summary_table'
		),
	);
	return $theme;
}

/**
 * Theme a format interval for a date element
 *
 *	@param $field = the field settings
 *	@param $node = node information, this is not always available and not
 *		 always the full node, it depends on what value was provided to the formatter.
 *		 Only the nid is always guaranteed to be available.
 *	@param $dates - an array of date information, see explanation for date_field_object for details.
 *	@return a formatted display
 *
 */
function theme_er_format_custom_date($element) {
	//global $conf;
	//d($conf, '$conf');
	$element['#formatter'] = 'short';//added this to make the dates come out pretty...
	$node = $element['#node'];
	$field_name = $element['#field_name'];
	$context = !empty($node->content) ? $node->content[$field_name]['#context'] : 'full';
	$type_name = $element['#type_name'];
	$fields = content_fields();
	$field = $fields[$field_name];
	$item = $element['#item'];
	
	

	// Get the formatter settings, either the default settings for this node
	// type or the View settings stored in $node->date_info.
	$options = date_formatter_get_settings($field_name, $type_name, $context);
	if (!empty($node->date_info) && !empty($node->date_info->formatter_settings)) {
		$options = $node->date_info->formatter_settings;
	}

	// If date_id is set for this field and the delta doesn't match, don't display it.
	if (!empty($node->date_id)) {
		foreach ((array) $node->date_id as $key => $id) {
			list($module, $nid, $field_name, $delta, $other) = explode('.', $id);
			if ($field_name == $field['field_name'] && isset($item['#delta']) && $delta != $item['#delta']) {
				return;
			}
		}
	}

	// If this is not coming from Views, it is the full node.
	// If we aren't retrieving a specific value, adjust the node values
	// to match the formatter settings, removing values we should not see.
	if (!empty($node->content) && empty($node->date_id)) {
		$node = date_prepare_node($node, $field, $type_name, $context, $options);

		// Did the current value get removed by formatter settings?
		if (empty($node->{$field_name}[$item['#delta']])) {
			return;
		}
		// Adjust the $element values to match the changes.
		$element['#node'] = $node;
	}
	$dates = date_formatter_process($element);
	if ($debug){
		d($options, '$options');
		d($element, '$element');
		d($dates, '$dates');
		d($context, '$context');
		d($item, '$item');
		d($options['fromto']['fromto'], 'fromto');
	}
	
	$date1 = $dates['value']['formatted'];
	$date2 = $dates['value2']['formatted'];
	if (!empty($element['#item']['value']) && empty($element['#item']['value2']))
		$date2 = 'Today';
	
	// Pull the timezone, if any, out of the formatted result and tack it
	// back on at the end, if it is in the current formatted date.
	$timezone = $dates['value']['formatted_timezone'];
	if ($timezone) {
		$timezone = ' ' . $timezone;
	}
	$date1 = str_replace($timezone, '', $date1);
	$date2 = str_replace($timezone, '', $date2);
	// No date values, display nothing.
	if (empty($date1) && empty($date2)) {
		return 'none';
	}
	elseif($options['fromto']['fromto'] == 'value'){
		return theme('date_display_single', $date1, $timezone);
	}
	elseif($options['fromto']['fromto'] == 'value2'){
		return theme('date_display_single', $date2, $timezone);
	}
	//// From and To dates match or there is no To date, display a complete single date.
	//elseif ($date1 == $date2 || empty($date2)) {
	//	return theme('date_display_single', $date1, $timezone);
	//}
	// Same day, different times, don't repeat the date but show both From and To times.
	elseif (date_has_time($field['granularity']) && $dates['value']['formatted_date'] == $dates['value2']['formatted_date']) {
		// Replace the original time with the from/to time in the formatted start date.
		// Make sure that parentheses or brackets wrapping the time will be retained in the
		// final result.
		$time1 = preg_replace('`^([\(\[])`', '', $dates['value']['formatted_time']);
		$time1 = preg_replace('([\)\]]$)', '', $time1);
		$time2 = preg_replace('`^([\(\[])`', '', $dates['value2']['formatted_time']);
		$time2 = preg_replace('([\)\]]$)', '', $time2);
		$time = theme('date_display_range', $time1, $time2);
		$replaced = str_replace($time1, $time, $date1);
		return theme('date_display_single', $replaced, $timezone);
	}
	// Different days, display both in their entirety.
	else {
		return theme('date_display_range', $date1, $date2, $timezone);
	}
}


/*
 * This function is for making sure that people who are NOT referenced in a node cannot edit it.
 * 	This is bypassing the permissions in a sense, but it seems like the only way to make sure 
 * 		only the proper people are editing the node.
 */
function er_preprocess_page(&$vars, $hook){
	// drupal_set_message('in er_preprocess_page');
	// remove Edit tab if User is not Admin and not referenced as a co-author
	if (isset($vars['user']->roles)){
		// if they're NOT an administrator
		if (!in_array('administrator', $vars['user']->roles)){
			// they're not an administrator			
			// See if they're looking at a node with Field Entity for a User
			$check_edit_permission = list_cts_having_field('field_er_user_entity_reference');
			
			if (isset($vars['node']) && in_array($vars['node']->type, $check_edit_permission)){
				// this is one of the content types we check
				//	does this user have permission to edit it? 
				//		Are they the Creator, Referenced in the Node, or an Admin?
				if (!isUserRefCreatedAdmin($vars['node'])){
					// Person is NOT ALLOWED to Edit
					er_removetab('Edit', $vars); // get rid of the tab labeled 'Edit'
				}				
			}			
		}
	}
}

/*
 * Called by er_preprocess_page, this will actually remove the tab whose label is $label
 * 		$vars is simply passed by reference from hook_preprocess_HOOK
 * 	http://api.drupal.org/api/drupal/modules!system!theme.api.php/function/hook_preprocess_HOOK/7
 */	
function er_removetab($label, &$vars) {
	// d($vars, 'vars');
	// Remove from primary tabs
	$i = 0;
	if (is_array($vars['tabs']['#primary'])) {
		foreach ($vars['tabs']['#primary'] as $primary_tab) {
		  if ($primary_tab['#link']['title'] == $label) {
			unset($vars['tabs']['#primary'][$i]);
		  }
		  ++$i;
		}
	}
	
	// If we only have the 'View' tab left, get rid of it
	if (count($vars['tabs']['#primary']) == 1) array_pop($vars['tabs']['#primary']);
}

/*
 *	This is called by er_preprocess_page
 * 		It determines if the given User has access to edit the node.
 * 		That is, whether they're referenced in the Entity Reference field, they're the node creator, or an administrator
 */
function isUserRefCreatedAdmin($node){
	global $user;
	// d($node, 'node isuser');
	$ref = $node->field_er_user_entity_reference['und'];
	$refArray = array();
	if (is_array($ref)) {
		foreach ($ref as $k){
			$refArray[] = $k['target_id'];
		}
	}	
	if (!in_array($user->uid, $refArray) && !user_access('administer nodes') && $node->uid != $user->uid){
		// this person is NOT referenced and does NOT have admin privs and they're NOT the creator of the node
		return FALSE;
	} else {
		// They CAN EDIT the node
		return TRUE;
	}
}

// function er_theme(&$existing, $type, $theme, $path) {
	// // drupal_set_message('mike');
	// // echo 'whereisthis';
	// // $hooks['er_proposal_node_form'] = array('variables' => array('form' => NULL),); 
	// // return $hooks;
// 	
  // return array(
    // 'er_proposal_node_form'  => array(
      // 'render element' => 'form',
    // ),
  // );	
// }
// 
// function bartik_er_proposal_node_form($form){
	// echo'mike2';
	// return $form;
// }
// 
// /**
// * Preprocessor for theme('article_node_form').
// */
// function template_preprocess_er_proposal_node_form(&$variables) {
	// d($variables, '$variables in template_preprocess_article_node_form');
// }
// function er_proposal_node_form($form, &$form_state){
	// echo'mike3';
	// d($form, 'form in er_proposal_node_form');
	// return $form;
// }
// function er_er_proposal_node_form($form){
	// echo'mike4';
// 
	// // d($form, '$form in er_er_proposal_node_form');
// 	
	// // $table_data = array();
    // // $output = '';
    // // /* HARDCODED */ 
	// // $cnpTypes = array('Submitted' => 'submit', 'Pending' => 'pending', 'Awarded' => 'date', 'Denied' => 'denied', 'Expired' => '');
// // 	
	// // foreach ($cnpTypes as $type => $val) {
		// // $table_data[] = array(
		    // // drupal_render($form['field_er_proposal_status']['value'][$type]),
		    // // drupal_render($form['field_er_proposal_'.$val])
		    // // /* or drupal_render($form['some_options']['other']).' '.drupal_render($form['other']) to put it in one cell*/
		// // );		
	// // }
// // 
    // // /* put your headers or leave it empty */
    // // $headers = array();
    // // $table = theme_table($headers,$table_data, array('class'=>'smallLineHeight'));
// //    
    // // // NOTE: the weight here will depend upon where it lies in your content type
	// // $form['cnpInTable'] = array(
		// // '#value' => $table,
		// // '#weight' => 9,		
	// // );
	// // the weight got all messed up and started appearing at the top. definitely a bug
	// // had to use a form alter instead of here to set the weight, see er_form_er_proposal_node_form_alter
    // /* to render all others element user drupal_render($form) */
// 	
    // // return drupal_render($form);
//     
    // return $form;
// }
