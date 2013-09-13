<?php
//Purpose: We must alter queries for cases where the end date is null. We want this to behave as if the event goes on indefinitely.
function er_views_query_alter(&$view, &$query){
	$summary_types = variable_get('er_summary_types');
	$found = false;
	//this checks to see if it's one of the views used in the summary table...
	if (is_array($summary_types)){
		foreach ($summary_types as $type){
			if ($view->name == 'er_summary_'.$type){
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
					preg_match("/^DATE_FORMAT\(([A-Z_]+\()*([^,\.]+\.[^,\.]+),/", $cond['field'], $matches);
					//d($matches, 'matches');
					if (count($matches) == 3){
						$field = $matches[2];
						$query->where[1]['conditions'][$c]['field'] .= " OR $field IS NULL";
					}
					//d($query->where[1]['conditions'][$c]['field'], 'after');
				}
			}
		}
	}
}




/**
 * This function gets called from within er_collab_inst_block view.
 * It generates a hyperlink.
 */
function er_collaborations_view_link(&$view){
	global $user;
	$node = menu_get_object();
	if ($node->type == "er_collaboration" && node_access("update", $node, $user) === TRUE) {
		$collab_id = $view->argument['field_er_collab_ref_target_id']->value[0];
		$query = array('field_er_collab_ref' => $collab_id, 'destination'=>"node/".$node->nid);
		$label = '<i class="icon-plus-sign"></i> '.(count($view->result)?
						'Add another collaborator':
						'<strong>Add a name to this collaboration</strong>');
		return l($label, 'node/add/er-collaborators', array('html'=>true, 'query' => $query) );
	}
}