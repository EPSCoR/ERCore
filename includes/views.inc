<?php
//Purpose: We must alter queries for cases where the end date is null. We want this to behave as if the event goes on indefinitely.
function er_views_query_alter(&$view, &$query){
	$summary_types = variable_get('er_summary_types');
	$found = false;
	//this checks to see if it's one of the views used in the summary table...
	if (is_array($summary_types)){
		foreach ($summary_types as $type){
			if ($view->name == $type.'_summary_view'){
				$found = true;
				break;
			}
		}
	}
	if ($found || $view->name == 'participants' || $view->name == 'All_user_events') {
		// d($view, 'view object');	
		// d($query, 'query object');
		if (count($query->where)){
			foreach ($query->where[1]['conditions'] as $c=>$cond){
				//d($cond);
				if ($cond['operator'] == 'formula' && strpos($cond['field'], 'value2') !== false){
					//d($query->where[1]['conditions'][$c]['field'], 'before');
					//preg_match("/\(([^(,\.]+\.[^,\.]+),/", $cond['field'], $matches);
					preg_match("/^DATE_FORMAT\(STR_TO_DATE\(([^,\.]+\.[^,\.]+),/", $cond['field'], $matches);
					//d($matches, 'matches');
					if (count($matches) == 2){
						$field = $matches[1];
						$query->where[1]['conditions'][$c]['field'] .= " OR $field IS NULL";
					}
					//d($query->where[1]['conditions'][$c]['field'], 'after');
				}
			}
		}
	}
}