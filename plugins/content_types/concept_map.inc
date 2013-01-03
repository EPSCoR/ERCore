<?php
/*
 * Andrew Wessels 2012
 * This panel is no longer in use. I believe it still works reasonably well,
 *  but we chose not to use it primarily because the view's version is easier
 *  to maintain, and already had all of the necessary functionality.
 * THIS PANEL IS UNUSED -- DEPRECATED
 * SEE: View named "concept_map_nodes"
 */

/**
* This plugin array is more or less self documenting
*/
$plugin = array(
	'category' => array(t('EPSCoR'), -9),
	'title' => t('EPSCoR Concept Map'),// the title in the admin
	'description' => t('Generates the Concept Map'),
	'single' => true,	// no one knows if "single" defaults to FALSE...
	'edit form' => 'er_concept_map_edit_form',
	'render callback' => 'er_concept_map_render'
);

/**
* Run-time rendering of the body of the block (content type)
* See ctools_plugin_examples for more advanced info
*/
function er_concept_map_render($subtype, $conf, $context = NULL) {
	/*// a simple GMap array
	$map_array1 = array(
		'id' => "my-map",				 // id attribute for the map
		'width' => "100%",				// map width in pixels or %
		'height' => "400px",			// map height in pixels
		'latitude' => 41.9023,		// map center latitude
		'longitude' => -87.5391,	// map center longitude
		'zoom' => 7,							// zoom level
		'maptype' => "Map",			 // baselayer type
		'controltype' => "Small"	// size of map controls
	);
	$output1 = theme('gmap', array('#settings' => $map_array));*/

	// a more elaborate example
	$map_array2 = array(
		'id' => 'example',
		'maptype' => 'Terrain',
		'width' => '100%',
		'height' => '700px',
		'longitude' => -70.675048828125,
		'latitude' => 43.667871610117494,
		'zoom' => 4,
		'align' => 'left',
		'controltype' => 'Small',
		'mtc' => 'standard',

		'behavior' => array(
			'locpick' => FALSE,
			'nodrag' => FALSE,
			'nokeyboard' => TRUE,
			//'overview' => TRUE,
			'scale' => TRUE,
		),

		'markers' => array(
			array(
				'text' => 'First Marker',
				'longitude' => -70.675048828125,
				'latitude' => 43.667871610117494,
				'markername' => 'orange',
				'text'=>'something',
			),
			array(
				'text' => 'Second Marker',
				'longitude' => 44.205835001,
				'latitude' => -70.3674316406,
				'markername' => 'orange',
				'text'=>'something',
			),
		),

		'shapes' => array(
			array(
				'type' => "polygon",
				'style' => array("000000", 3, 25, "ffff00", 45),
				'points' => array(
					array(43.667871610117494,-70.675048828125),
					array(43.43696596521823,-70.0927734375),
					array(43.9058083561574,-69.202880859375),
					array(44.512176171071054,-69.796142578125),
					array(43.667871610117494,-70.675048828125),
				),
			),
			array(
				'type' => "circle",
				'style' => array("000000", 3, 25, "ffff00", 45),
				'radius' => 25.7622248729082767,
				'center' => array(43.667871610117494,-70.675048828125),
			),
		),
		'center_on_nodearg' => 1,

		/*'feeds' => array(
			array(
				'url' => 'http://earthquake.usgs.gov/eqcenter/catalogs/eqs7day-M5.xml',
				'markername' => 'red',
			),
		),*/
	);

	$block->content = theme('gmap', array('#settings' => $map_array2));
	//$block->content = "something...";
	/*foreach (array("title", "subtitle", "caption", "type", "style") as $c){
		$block->$c = $conf[$c];
		$block->content .= "$c: {$conf[$c]}<br>\n";
	}*/
	//$block->data = er_concept_map_data($block);
	//dsm($conf, 'conf');
	//dsm($block, 'block');
	return $block;
}

/**
* 'Edit form' callback for the content type.
*/
function er_concept_map_edit_form(&$form, &$form_state) {
	$conf = $form_state['conf'];
	/*$form['type'] = array(
		'#type' => 'radios', 
		'#title' => t('Graph type'), 
		'#default_value' => $conf['type'], 
		'#options' => array(t('Race'), t('Gender'), t('Disabilities'), t('Institution'), t('Involvement'), t('Team')),
		'#description' => t('Choose what data you would like to graph'),
		'#required' => true,
		);
	$form['title'] = array(
		'#type' => 'textfield',
		'#title' => t('Title'),
		'#size' => 40,
		'#description' => t('Provide a title for the graph'),
		'#default_value' => !empty($conf['title'])?$conf['title']:'Participants',
	);
	$form['subtitle'] = array(
		'#type' => 'textfield',
		'#title' => t('Sub-title'),
		'#size' => 40,
		'#description' => t('Optional sub-title for the graph'),
		'#default_value' => $conf['subtitle'],
	);
	$form['caption'] = array(
		'#type' => 'textfield',
		'#title' => t('Caption'),
		'#size' => 120,
		'#description' => t('Optional caption displayed under the graph'),
		'#default_value' => $conf['caption'],
	);
	$form['style'] = array(
		'#type' => 'textfield', 
		'#title' => t('Custom css'),
		'#size' => 40, 
		'#default_value' => $conf['style'], 
		);*/
	return $form;
}


/**
* Submit function, note anything in the formstate[conf] automatically gets saved 
*/
function er_concept_map_edit_form_submit(&$form, &$form_state) {
	//this stores the values so that i can use them later on...
	foreach (array("title", "subtitle", "caption", "type", "style") as $c){
		$form_state['conf'][$c] = $form_state['values'][$c];
	}
}

/*
 * This generates the graph data
 */
function er_concept_map_data($block){
	$graph_fields = array('field_er_race', 'field_er_gender', 'field_er_disabilities', 'field_er_institution', 'field_er_involvement', 'field_er_teams');
	$q = new Query1();

	$t = new Table1('content_type_er_user_profile', 'up');
	$t	->field($graph_fields[$block->type], 'data')
			->group('data')
			->select('data', array('fn'=>'count'), 'y')
			->select('data', array(), 'name')
			->where('data', 'null', array('op'=>"!="));
		//->order('data', array('order'=>"DESC"));
	$q->addTable($t);
	return $q->getData();
}
