<?php
/**
* This plugin array is more or less self documenting
*/
$plugin = array(
	'category' => array(t('User Account Tabs'), -9),
	'title' => t('Certifications - Admin Fields Highlighted'),// the title in the admin
	'description' => t('This will highlight the admin-only permissions in blue'),
	'single' => true,	// no one knows if "single" defaults to FALSE...
	'edit form' => 'er_user_certs_edit_form',
	'render callback' => 'er_user_certifications_render'
);

function er_user_certs_edit_form(&$form, &$form_state) {
	return $form;
}

/**
* Run-time rendering of the body of the block (content type)
* See ctools_plugin_examples for more advanced info
*/
function er_user_certifications_render($subtype, $conf, $context = NULL) {
	// the below function is called in overrides.inc under the includes folder in er
	$block->content = user_profile_fields_highlighted('group_er_certifications', 'view');
	return $block;
}
