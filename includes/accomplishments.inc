<?php
function er_accomplishments(){
	return drupal_get_form('er_date_range_form');
}

function _reporting_range_is_set(){
	return array_key_exists('er_start_date', $_SESSION) && array_key_exists('er_end_date', $_SESSION);
}

/*
 * Generates a nested array of date ranges corresponding to the start and end of a valid reporting period.
 * This is used primarily on the summary table.
 */
function _summary_table_ranges(){
	if ($range = er_get_reporting_range(false)){//only if explicitly specified.
		return array($range);
	}else{//only need to do this once.
		return er_get_reporting_ranges();
	}
}

/*
 * This theme function (see theme.inc) generates the summary table.
 * @param $ranges = a nested a array of date ranges
 * @param $debug = a flag which turns on debug messages
 */
function er_summary_table($variables){
	$ranges = $variables['ranges'];
	$debug = $variables['debug'];
	$ret = array('#theme'=>'table');
	//$content_types = er_content_types();
	//$ct_info = node_type_get_types();
	$ret['#header'][] = "Categories of Accomplishments";
	$ret['#header'][] = "Inception<br>through<br>" . date(ER_DATE_FORMAT);
	foreach ($ranges as $k=>$v)
		$ret['#header'][] = date(ER_DATE_FORMAT, $v[0])."<br>through<br>".date(ER_DATE_FORMAT, $v[1]);
	
	$types = variable_get('er_summary_types');//see er/includes/er.admin.inc
	if (isset($types) && count($types)){
		foreach ($types as $k=>$type){
			$data = array();
			$view_name = "er_summary_$type";//$type.'_summary_view';
			if ($view = views_get_view($view_name)){
				$view->set_display('count');
				$view->execute();
				//dsm($view);
				$data[] = er_format_label($type, $view, $ranges);//$ct_info[$type]->name;
				if (isset($view->result[0]->nid))
					$data[] = $view->result[0]->nid;//this is the node count //old way: $view->total_rows;
				else
					$data[] = $view->result[0]->uid;//need this for the participants row...
				foreach ($ranges as $k=>$v){
					$view = views_get_view($view_name);
					$view->set_display('count');
					//dsm(date('Y-m-d', $v[0]) . ' through '. date('Y-m-d', $v[1]));
						$view->exposed_input['start_date']['value']['date'] = date('Y-m-d', $v[0]);
					$view->exposed_input['end_date']['value']['date'] = date('Y-m-d', $v[1]);
					//$view->set_items_per_page(1);
					$view->execute();
					$count = isset($view->result[0]->nid)?$view->result[0]->nid:$view->result[0]->uid;//count nodes or users
					$data[] = $count>0?$count:0;
				}
			}else{
				drupal_set_message(t('View not found: @name', array('@name'=>$view_name)), 'error');
			}
			$ret['#rows'][] = $data;
			//d($data, '$data');
		}
	}else{
		$link = l('click here', 'admin/config/epscor/er');
		$message = "An administrator needs to select which content types you want to see on the summary table! $link";
		drupal_set_message($message, 'error');
	}
	
	$range = er_get_reporting_range();
	$date_label = ' ('.date('n/j/y', $range[0]).' - '.date('n/j/y', $range[1]).')';
	
	$user_can_download = user_access(ER_DOWNLOAD_PERMISSION);
	$ret = array(
		'table'=>$ret,
		'asterisk' => array(
			'#type' => 'markup',
			'#markup' => "* Grant/Proposal counts include all grants regardless of status."
		),
		'download' => array(
			'#type' => 'markup',
			'#access' => $user_can_download,
			'#markup' => '<p style="text-align:center;">'.er_create_download_link('Download EPSCoR Reporting Core Tables'.$date_label, 'excel')."</p>",
		)
	);
	return render($ret);
}

//this is the label shown on the accomplishments table. ex: "publications", "current and pending", "posters"
//We could make this into a proper hook in the future...
function er_format_label($type, &$view, &$ranges){
	//global $user;
	$view_title = isset($view->display['default']->handler->options['title'])?$view->display['default']->handler->options['title']:$type;
	if (count($ranges)==1){
		$view_link_with_range = array(
			'query'=>array(
				'start_date[value][date]'=>date('Y-m-d', $ranges[0][0]),
				'end_date[value][date]'=>date('Y-m-d', $ranges[0][1]),
			)
		);
	}else{
		$view_link_with_range = array();
	}
	if (isset($view->display['page']->display_options['path']))
		$ret = l('<i class="icon-download-alt"></i>', $view->display['page']->display_options['path'].'/download', array('html'=>true)). ' ' . l($view_title, $view->display['page']->display_options['path'], $view_link_with_range);
	else
		$ret = $view_title;
	return $ret;
}

/*
 * Ajax callack function for er_date_range_form.
 * This gets fired when the select box is changed.
 */
function er_change_reporting_year($form, $form_state){
	$ranges = er_get_reporting_ranges();
	$selected = $form_state['input']['reporting-year']['range']-1;
	//watchdog("testing er_change_reporting_year", 'regenerating summary table. range:'.$selected, array(), WATCHDOG_ALERT);
  if ($selected == -1){//The "Select" option
    unset($_SESSION['er_start_date']);
		unset($_SESSION['er_end_date']);
  }else{
    $_SESSION['er_start_date'] = $ranges[$selected][0];
    $_SESSION['er_end_date'] = $ranges[$selected][1];
    $ranges = array($ranges[$selected]);
  }
	
	$form['table']['#ranges'] = $ranges;
	return $form['table'];
}

/*
 * Ajax callack function for er_date_range_form.
 * This gets fired when the save button is clicked.
 * This updates the summary table when a date range is specified.
 */
function er_change_date_range($form, $form_state){
  $ranges = array();
  if ($form_state['values']['op'] == t('Save')){
    $s = strtotime($form_state['values']['dates']['start_date']);
    $e = strtotime($form_state['values']['dates']['end_date']);
    if ($s <= $e){
      $_SESSION['er_start_date'] = $s;
      $_SESSION['er_end_date'] = $e;
      $ranges = array(array($s, $e));
    }else{
      drupal_set_message("Start date must be earlier than end date.", 'error');
    }
  }else{
		unset($_SESSION['er_start_date']);
    unset($_SESSION['er_end_date']);
    $ranges = _summary_table_ranges();
  }
	$form['table']['#ranges'] = $ranges;
	return $form['table'];
}

/*
 * Constructs the date range form seen at the top of the accomplishments table.
 */
function er_date_range_form($form_state) {
  //d($_SESSION);
	$start_date = variable_get('er_start_date');
    $date_year_range = "{$start_date['year']}:+1";//"2008:+1"; //+1 means 1 year from today
	
	$select_list = array('Select');
	$ranges = er_get_reporting_ranges(); //This returns the list of available date ranges...
	$range = er_get_reporting_range(); //Get the select reporting period, this defaults to the current reporting period if none was selected.
	$range_is_set = _reporting_range_is_set();//Was a reporting period selected, or is $range the default -- current reporting period.
	$selected = 0;
	foreach ($ranges as $k=>$v){
		$select_list[] = date('M d, Y', $v[0]) . ' to ' . date('M d, Y', $v[1]);
		if ($range_is_set && $range == $v) $selected = $k+1;
	}
	
	
  $form['reporting-year'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reporting Year'),
    '#tree' => true,
    '#attributes' => array('class'=>array('fake-inline')),
    '#collapsible' => true,
    '#collapsed' => false,//$_SESSION['er_dates_collapsed'],
  );
  $form['reporting-year']['range'] = array(
		'#type' => 'select',
		'#title' => t('Reporting Period'),
		'#options' => $select_list,
		'#default_value' => $selected,
		//'#description' => t('Select a reporting year to search by.'),
		'#ajax' => array(
      'callback' => 'er_change_reporting_year',
      'wrapper' => 'er_summary_table',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
	
	$date_format = 'm/d/Y';//variable_get('date_format_short', 'm/d/Y - H:i'),
  $form['dates'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date range'),
    '#tree' => true,
    '#attributes' => array('class'=>array('fake-inline')),
    '#collapsible' => true,
    '#collapsed' => true,//$_SESSION['er_dates_collapsed'],
  );
	
	
  $form['dates']['start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start Date'),
    '#date_format' => $date_format,
    '#date_timezone' => date_default_timezone(), //date_default_timezone_name(),
		//'#value' => array('date'=>date('Y-m-d', isset($range[0])?$range[0]:time())),
    '#default_value' => date('Y-m-d', $range[0]),
    '#date_year_range' => $date_year_range,
		'#date_label_position' => 'none',
  );
  
  $form['dates']['end_date'] = array( 
    '#type' => 'date_popup',
    '#title' => t('End Date'),
    '#date_format' => $date_format,
		//'#value' => array('date'=>date('Y-m-d', isset($range[1])?$range[1]:mktime(0, 0, 0, date('n')+1))),
    '#default_value' => date('Y-m-d', $range[1]),
    '#date_timezone' => date_default_timezone(), //date_default_timezone_name(),
    '#date_year_range' => $date_year_range,
		'#date_label_position' => 'none',
  );
  $form['dates']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#ajax' => array(
      'callback' => 'er_change_date_range',
      'wrapper' => 'er_summary_table',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
  $form['dates']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#ajax' => array(
      'callback' => 'er_change_date_range',
      'wrapper' => 'er_summary_table',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
	$form['table'] = array(
		'#theme'=>'er_summary_table',
		'#ranges'=>_summary_table_ranges(),//returns the full list of reporting ranges if a range wasn't selected, otherwise just returns the selected range.
		'#prefix'=>'<div id="er_summary_table">',
		'#suffix'=>'</div>',
		//'#access'=>$access,
	);
  return $form;
}

/*
 * Handles the form data when the form is submitted.
 * Sets two session variables 'er_start_date' and 'er_end_date'.
 * When these session variables are set, the accomplishments table overrides it's default date ranges, and uses this range instead.
 */
function er_date_range_form_submit($form, &$form_state){
	//d($form_state);
  if ($form_state['clicked_button']['#id'] == 'edit-dates-reset'){
		unset($_SESSION['er_start_date']);
		unset($_SESSION['er_end_date']);
  }else{
    $s = strtotime($form['dates']['start_date']['#value']['date']);
    $e = strtotime($form['dates']['end_date']['#value']['date']);
    if ($s <= $e){
      $_SESSION['er_start_date'] = $s;
      $_SESSION['er_end_date'] = $e;
    }else{
      drupal_set_message("Start date must be earlier than end date.", 'error');
    }
  }
}