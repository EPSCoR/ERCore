<?php
/**
 * Implementation of hook_theme().
 *
 * Declare the theme function we will use for our custom formatter. This is a
 * Drupal requirement because it likes to know about all the theme functions.
 */
function er_theme(){
	$path = ER_MODULE_DIR.'/templates';
	$theme = array(
		'er_formatter_node_reference_full_node' => array(
			'arguments' => array('element' => NULL),
		),
		'er_chart_block'=> array(
			'arguments' => array('series'=>array(), 'info' => array()),
			'template' => 'er-chart-block',
			'path' => $path,
		),
		'er_timeline'=> array(
			'arguments' => array('series'=>array(), 'info' => array()),
			'template' => 'er-timeline',
			'path' => $path,
		),
		'er_new_content_button'=> array(
			'arguments' => array(),
			'template' => 'er-new-content-button',
			'path' => $path,
			),
		'er_formatter_format_custom_date' => array(
			'arguments' => array('element' => NULL),
			'function' => 'theme_er_format_custom_date'
		),
	);
	return $theme;
}

/*
function theme_er_block_test(&$vars){
	dsm($vars);
	return 'something';
}
*/

/**
 * Theming function for our formatters
 *
 * This is where we actually include and format the node. Note that there is
 * already a "full node" formatter for nodereference fields, but we want to
 * display ours a bit differently.
 */
function theme_er_formatter_node_reference_full_node($element){
	return '<div style="background-color:#eee">'
			. node_view(node_load($element['#item']['nid']),false,false,false)
			. '</div>';
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