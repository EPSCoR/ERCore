<?php
/* 
 *
 * Start of related content sutff
 * 
 */
function theme_er_related_content($options){
  //dsm(func_get_args());
  $conf = $options['conf'];
  //dsm($conf, '$conf');
  drupal_add_css(drupal_get_path('module', 'er_profile').'/css/er_profile.css');
	global $user;
	$addHelp = false;
	$anonymous = isset($user->roles[1]);//!in_array('anonymous user', array_values($user->roles));//$conf['links'];
	$show_links = !$anonymous;
  $needs_review = flag_get_flag('er_needs_review');
  
	$content = '';
  //if (arg(0) == 'user')
  //  $account = user_load(arg(1));
  //else
  //  $account = clone $user;
  $account =  $conf['type']==0?user_load(arg(1)):
              ($conf['type']==1?user_load($conf['static_uid']):
              clone $user);
  //dsm($account);
  
  $hide = module_invoke_all('related_content_hide_types');//a list of content types that should not appear in this display
  $hide = array_unique($hide);
  
  $fields = field_info_fields(); //every field, and it's info. note: cant use field_read_fields() because it doesn't contain "bundles" attribute
  $nodes = array();
	foreach ($fields as $field_name => $info) {
    if ($info['type'] == 'entityreference' && $info['settings']['target_type'] == 'user' && isset($info['bundles']['node'])){
      //dsm($info, 'field info');
      foreach ($info['bundles']['node'] as $type){
        if (!in_array($type, $hide)){ //TODO:: we can use variable_get/set for this list...
          $query = new EntityFieldQuery();
          $query  ->entityCondition('entity_type', 'node')
                  ->entityCondition('bundle', $type)
                  //->propertyCondition('status', 1)
                  ->fieldCondition($info['field_name'], 'target_id', $account->uid, '=');
          $results = $query->execute();
          if ($results && isset($results['node'])) {
            $data = node_load_multiple(array_keys($results['node']));// Load the node objects based on the node ids returned from the query.
            if (array_key_exists($type, $nodes))//maybe a content type has two or more user references...? it's possible...
              $nodes[$type] = array_merge($nodes[$type], $data);//merge it with whatever is already there.
            else
              $nodes[$type] = $data;
          }
        }
      }
    }
	}
    
  $node_types = node_type_get_types();
  foreach ($nodes as $type=>$data){
    //$content .= '<h2 class="title">'. $node_types[$type]->name . '</h2>';
    $items = array();
    foreach ($data as $node){
			$d = module_invoke_all('related_content_'.$type.'_item_theme', $node, $show_links);
      $d = array_pop($d);
      if (!$d) $d = l($node->title, 'node/'.$node->nid); //otherwise use the default
      $flagged = $needs_review->is_flagged($node->nid);
      $items[] = array(
          'data' => (!$node->status?'<span class="unpublished">(Draft)</span> ':'') . ($flagged?'<span class="er-needs-review-label">Needs Review</span> ':'') . $d,
          'id' => $node->nid
      );
    }
    $content .= theme_item_list(array(
      'items' => $items,
      'title' => $node_types[$type]->name,
      'type' => 'ul',
      'attributes' => array(
          'class' => 'related_content_'.$type,
      )
    ));
  }
//	$linktoHelp = l(
//		  t('My link'),
//		  'content/updates-needed',
//		  array(
//		    'attributes' => array(
//		      'rel' => 'lightframe',
//		      'query' => 'format=simple'
//		    	)
//		  	)
//			);
//			
//	$linktoHelp = $url = url('content/updates-needed', array('query'=>'format=simple'));
//	$fullLink = '<a href="' . $linktoHelp . '" rel="lightframe[group|height:500px]">What does this mean?</a>';
//	// if ($addHelp) drupal_set_message('Attention: some content on this page needs updating. '.$fullLink);
//	
	return $content;
}

//used to hide certain content types
function er_related_content_hide_types(){
  return array('er_user_event', 'er_user_event_instance');
}

//adds customizations for er_project links
//PROJECTS ARE NO LONGER PART OF ER-CORE
function er_related_content_er_project_item_theme($node, $show_links){
  $label = $node->title;
  if ($node->field_er_proj_inst['und'][0]['nid'] != null){ //this happens when no institutions are attached
    $append = '<ul>';
    foreach ($node->field_er_proj_inst['und'] as $inst_nids){
      $append .= '<li>';
      $inst = node_load($inst_nids['nid']);
      $append .= "{$inst->title}<br>";
      $query = new EntityFieldQuery();
      $query  ->entityCondition('entity_type', 'node')
          ->entityCondition('bundle', 'er_collaborator')
          //->propertyCondition('status', 1)
          ->fieldCondition('field_er_collab_project', 'nid', $node->nid, '=')
          ->fieldCondition('field_er_collab_inst', 'nid', $inst->nid, '=');
      $results = $query->execute();
      if ($results){
        $first_collab = array_pop($results['node']);
        //$collab = node_load($first_node->nid);
        //dsm($collab, '$collab');
        $append .= l('<span class="edit button16"></span>Add/Edit collaboration details', 'node/'.$first_collab->nid.'/edit', array('html'=>true));
        if (count($results['node']) > 1){
          $append .= '<div style="color:red">ERROR! Multiple collaborator nodes detected! Please inform an administrator of this occurence. Thank you.</div>';
        }
      }else{
        $append .= l('<span class="add button16"></span>Add person to this Institution', 'node/add/er-collaborator', array('query'=>array('er_collab_project'=>$node->nid, 'er_collab_inst'=>$inst->nid), 'html'=>true));
        //display link to add new collab node
      }
      $append .= '</li>';
    }
    $append .= '<ul>';
  }
  return l($label, 'node/'.$node->nid, array('html'=>true)) . $append;
}

//adds customizations for er_event links
function er_related_content_er_event_item_theme($node, $show_links){
  $label = "{$node->title}";
  $append = '';
  if (array_key_exists('und', $node->field_er_event_reminders) && $node->field_er_event_reminders['und'][0]['value']){ //then this node should have an EE
    //check if one exists:
    $query = new EntityFieldQuery();
    $query  ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'er_engagement')
        //->propertyCondition('status', 1)
        ->fieldCondition('field_er_cal_entity_reference', 'target_id', $node->nid, '=');
    $results = $query->execute();
    if ($results){
      $ee = array_pop($results['node']);
      $append .= '<ul><li>'.l('<i class="icon-list-alt"></i> Attendee data', 'node/'.$ee->nid, array('html'=>TRUE))."</li></ul>";
    }else{
      $append .= "<ul><li>".add_external_engagement_link($node)."</li></ul>";
    }
  }
  return l($label, 'node/'.$node->nid, array('html'=>true)) . $append;
}

//function returns the status of the node given one of proposal, patent, publication, or book CTs
function er_related_content_get_status_string($node){
  $field = $node->{"field_{$node->type}_status"};
  if ($field && array_key_exists('und', $field))
    return ' - <span class="ct_field_status">'.$field['und'][0]['value'].'</span>';
  return '';
}

//adds customizations for er_proposal links
function er_related_content_er_proposal_item_theme($node, $show_links){
  $label = "{$node->title}".er_related_content_get_status_string($node);
  return l($label, 'node/'.$node->nid, array('html'=>true));
}

//adds customizations for er_patent links
function er_related_content_er_patent_item_theme($node, $show_links){
  $label = "{$node->title}".er_related_content_get_status_string($node);
  return l($label, 'node/'.$node->nid, array('html'=>true));
}

//adds customizations for er_publication links
function er_related_content_er_publication_item_theme($node, $show_links){
  $label = "{$node->title}".er_related_content_get_status_string($node);
  return l($label, 'node/'.$node->nid, array('html'=>true));
}

//adds customizations for er_book links
function er_related_content_er_book_item_theme($node, $show_links){
  $label = "{$node->title}".er_related_content_get_status_string($node);
  return l($label, 'node/'.$node->nid, array('html'=>true));
}