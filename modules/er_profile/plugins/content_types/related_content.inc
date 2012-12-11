<?php
/*
 * This panel creates the "Related content" section currently seen on the user profiles, and in the user dashboard.
 * Much of this code started it's life as part of the userreference module.
 * Mike made a copy of the code that creates the drupal default "related content" section, then we made modifications to it.
 * The code eventually found a home here, after being integrated with the panels system.
 * Some modifications included removing content we dont want to see, adding extra hyperlinks, and labels such as (Draft) and proposal (CnP) status.
 */
$plugin = array(
	'category' => array(t('EPSCoR'), -9),
	'title' => t('Related Content'),// the title in the admin
	'description' => t('Displays related user content.'),
	'single' => true,	// no one knows if "single" defaults to FALSE...
	'edit form' => 'er_profile_related_content_edit_form',
	//'render callback' => 'related_content_render'
);

function er_profile_related_content_content_type_render($subtype, $conf, $context = NULL) {
  //$block->title = 'Related Content';
  $block->content = theme('er_profile_related_content', array('conf'=>$conf));
  $block->css_id = "related-content";
  //$block->css_class = "related-content";
  return $block;
}

function er_profile_related_content_edit_form(&$form, &$form_state) {
	$conf = $form_state['conf'];
	$form['type'] = array(
		'#type' => 'radios', 
		'#title' => t('UID Selection'), 
		'#default_value' => !empty($conf['type'])?$conf['type']:0,
		'#options' => array('URL Arguement', 'Current user', 'Static UID'),
		'#description' => t('Where should I get the user ID?'),
		'#required' => true,
  );
	$form['static_uid'] = array(
		'#type' => 'textfield',
		'#title' => t('Static uid'),
		'#size' => 40,
		'#description' => t('Manually set a UID to use'),
		'#default_value' => !empty($conf['static_uid'])?$conf['static_uid']:'',
	);
	$form['links'] = array(
		'#type' => 'checkbox',
		'#title' => t('Display the "add new" links'), 
		'#default_value' => !empty($conf['links'])?$conf['links']:0,
  );
	return $form;
}

/**
* Submit function, note anything in the formstate[conf] automatically gets saved 
*/
function er_profile_related_content_edit_form_submit(&$form, &$form_state) {
	//this stores the values so that i can use them later on...
	foreach (array("type", "static_uid", "links") as $c){
		$form_state['conf'][$c] = $form_state['values'][$c];
	}
}