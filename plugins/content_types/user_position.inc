<?php
/**
* This plugin array is more or less self documenting
*/
$plugin = array(
	'title' => t('Position - Admin Fields Highlighted'),// the title in the admin
	'category' => array(t('User Account Tabs'), -9),
	'description' => t('This will highlight the admin-only permissions in blue'),
	'single' => true,	// no one knows if "single" defaults to FALSE...
	'edit form' => 'er_user_position_edit_form', // no need for settings
	'render callback' => 'er_user_position_render',
	//'admin title' => 'silly_function',
);

/*
function silly_function($subtype, $conf, $context = NULL) {
  dsm($conf);
  $conf['override_title_text'] = 'ATTEMPT';
  $output = t('HopingWorks');
  if ($conf['override_title'] && !empty($conf['override_title_text'])) {
    $output = filter_xss_admin($conf['override_title_text']);
  }
  //return $output;
  return $conf['override_title_text'];
}
 */

function er_user_position_edit_form(&$form, &$form_state) {
	return $form;
}

/**
* Run-time rendering of the body of the block (content type)
* See ctools_plugin_examples for more advanced info
*/
function er_user_position_render($subtype, $conf, $context = NULL) {
	// the below function is called in overrides.inc under the includes folder in er
	$block->content = user_profile_fields_highlighted('group_er_position', 'view');
	return $block;
}
