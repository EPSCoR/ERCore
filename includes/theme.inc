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
		'er_summary_table' => array(
			'variables' => array('ranges' => array(), 'debug'=>false),
			'function' => 'er_summary_table'
		),
		'er_status_report' => array(
			'variables' => array('requirements'=>array()),
			//'render element' => 'requirements',
			'function' => 'theme_er_status_report',
		),
		'node__er_collaboration' => array(
			//'variables' => array('requirements'=>array()),
			'template' => 'node-er_collaboration',
			'path' => $path,
		),
		'er_related_content' => array(
			'variables' => array('conf' => array('links'=>1, 'type'=>2)),
			'function' => 'theme_er_related_content',
			'file' => 'includes/related_content.inc',
		),
		
		'field__field_er_user_entity_reference'=>array(
			'variables' => array(),
			'function' => 'theme_field_er_user_entity_reference',
		),
	);
	return $theme;
}

/**
 * Implements hook_field_formatter_info().
 */
function er_field_formatter_info() {
  return array(
    'date_custom' => array(
      'label' => t('Custom date format'),
      'field types' => array('date'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function er_field_formatter_view($entity_type, $entity, $field, $instance, $lang_code, $items, $display) {
	$format = 'F j, Y';
  $elements = array();
  foreach ($items as $delta => $item) {
		$date_settings = array(
			'timezone'=>$item['timezone'],
			'timezone_db'=>$item['timezone_db'],
			'date_type'=>$item['date_type'],
		);
		$date1 = field_view_value('node', $entity, $field['field_name'], array('value' => $item['value'])+$date_settings);
		if (!is_null($item['value2']))
			$date2 = field_view_value('node', $entity, $field['field_name'], array('value' => $item['value2'])+$date_settings);
		else
			$date2 = array('#markup'=>'Present');
    $elements[$delta] = array(
      '#markup' => render($date1) . " - " . render($date2),
    );
  }
  return $elements;
}

/*
 * @purpose This function re-themes the field_er_user_entity_reference entity reference field.
 * 		It splits up the users referenced by whether they are participants or collaborators.
 * @author Andrew Wessels
 */
function theme_field_er_user_entity_reference($variables){
	$grouped_items = array('participant' => array(), 'collaborator'=>array());
	foreach ($variables['element']['#items'] as $delta => $item){
		$participating = check_user_participating($item['entity']);
		$grouped_items[$participating][] = $variables['items'][$delta];
	}
	
	$label = explode('|', $variables['label']);
	if (count($label) < 2)
		$label[] = 'Collaborators'; //if no alternative label was given, use this.
	
	$output = '';
	foreach ($grouped_items as $type => $items){
		$this_output = '';
		// Render the label, if it's not hidden.
		if (!$variables['label_hidden'] && count($items)) {
			$this_output .=
				'<div class="field-label"' . $variables['title_attributes'] . '>' .
					($type == 'participant'?$label[0]:$label[1]) .
				': </div>';
		}
	 
		// Render the items.
		$this_output .= '<div class="field-items"' . $variables['content_attributes'] . '>';
		foreach ($items as $delta => $item) {
			$classes = 'field-item ' . ($delta % 2 ? 'odd' : 'even');
			$this_output .= '<div class="' . $classes . '"' . $variables['item_attributes'][$delta] . '>' . drupal_render($item) . '</div>';
		}
		$this_output .= '</div>';
	 
		// Render the top-level DIV.
		$output .= '<div class="' . $variables['classes'] . '"' . $variables['attributes'] . '>' . $this_output . '</div>';
	}
 
  return $output;
}

/**
 * @author Andrew
 */
function theme_er_status_report($variables){
	//d($variables, '$variables');
  $requirements = $variables['requirements'];
  return theme('status_report', array('requirements' => $requirements));
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
