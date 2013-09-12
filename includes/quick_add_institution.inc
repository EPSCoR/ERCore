<?php
function _quick_add_inst_validate(&$form, &$form_state){
  //d('validation!');d($form, '$form');d($form_state, '$form_state');
  $inst_field_name = $form_state['quick_add_inst_field_name'];
  //form_set_error($inst_field_name, t('You must either select an institution, or create a new institution by clicking on the checkbox below the institution selection.'));
  if ($form_state['values']['field_er_inst_add_bool']['und'][0]['value'] == 1){//creating a new inst
    if (strlen($form_state['values']['field_er_inst_name']['und'][0]['value']) < 4){
      form_set_error('field_er_inst_name', t('Please specify the full name of the institution you wish to add to the system.'));
    }
  }else{
    if (!$form_state['values'][$inst_field_name]['und'][0]['target_id']){
      form_set_error($inst_field_name, t('You must either select an institution, or create a new institution by clicking on the checkbox below the institution selection.'));
    }
  }
}

function _quick_add_inst_process(&$form, &$form_state){
  $inst_field_name = $form_state['quick_add_inst_field_name'];
	$prepared_data = _quick_add_inst_prepare_data($form, $form_state);
	_quick_add_inst_cleanup($form, $form_state);
  if ($prepared_data){//if all went well...
    $node = _quick_add_inst_create($prepared_data);
    $form_state['values'][$inst_field_name]['und'] = array(array('target_id'=>$node->nid));
  }
}

function _quick_add_inst_prepare_data(&$form, &$form_state){
	//name has to be set to something:
	if (
    $form_state['values']['field_er_inst_add_bool']['und'][0]['value'] == 1 &&
    isset($form_state['values']['field_er_inst_name']['und'][0]['value']) &&
    $form_state['values']['field_er_inst_name']['und'][0]['value']
  ){
		return array(
			'name' => $form_state['values']['field_er_inst_name']['und'][0]['value'],
			'link' => isset($form_state['values']['field_er_inst_link']['und'][0]['url'])?
				$form_state['values']['field_er_inst_link']['und'][0]['url']:'',
			'abbr' => isset($form_state['values']['field_er_inst_abbreviation']['und'][0]['value'])?
				$form_state['values']['field_er_inst_abbreviation']['und'][0]['value']:'',
			'country' => isset($form_state['values']['field_er_inst_country']['und'][0]['value'])?
				$form_state['values']['field_er_inst_country']['und'][0]['value']:'',
			'desc' => isset($form_state['values']['field_er_inst_description']['und'][0]['value'])?
				$form_state['values']['field_er_inst_description']['und'][0]['value']:'',
		);
  }else
		return NULL;
}

function _quick_add_inst_cleanup(&$form, &$form_state){
  //$form_state['values'] = array();
	// clean up values so when you view it, it's proper
	unset($form_state['values']['field_er_inst_add_bool']['und']);
	unset($form_state['values']['field_er_inst_name']['und']);
	unset($form_state['values']['field_er_inst_abbreviation']['und']);
	unset($form_state['values']['field_er_inst_link']['und']);
	unset($form_state['values']['field_er_inst_country']['und']);
	unset($form_state['values']['field_er_inst_description']['und']);
	
	// not unsetting for user profile
	if (isset($form_state['complete form']['field_er_inst_name']['und'][0]['value']['#value']))
		$form_state['complete form']['field_er_inst_name']['und'][0]['value']['#value'] = '';
	if (isset($form['field_er_inst_country']['und'][0]['value']['#value']))
		$form['field_er_inst_country']['und'][0]['value']['#value'] = '';
}

function _quick_add_inst_create($data){
	global $user;
	// d($user, 'user');
	$newNode = (object) NULL;
	$newNode->type = 'er_institution';
	$newNode->uid = $user->uid;
	$newNode->created = strtotime("now");
	$newNode->changed = strtotime("now");
	$newNode->status = 1;
	$newNode->comment = 0;
	$newNode->promote = 0;
	$newNode->moderate = 0;
	$newNode->sticky = 0;
	$newNode->language = 'und';
	$newNode->field_er_inst_name['und'][0]['value'] = $data['name'];
	$newNode->field_er_inst_abbreviation['und'][0]['value'] = $data['abbr'];
	$newNode->field_er_inst_country['und'][0]['value'] = $data['country'];
	$newNode->field_er_inst_link['und'][0]['url'] = $data['link'];
	$newNode->body['und'][0]['value'] = $data['desc'];
	
	$newNode->title = $data['name'];
	if (!empty($data['abbr'])) //add the abbreviation to the title if available.
		$newNode->title .= " (".$data['abbr'].")";
	
	node_save($newNode); //save it!
	$nid = $newNode->nid;
	
	// flag this as needing Administrator review so they can 
	//		add Juristictional information, etc...
	$flag = flag_get_flag('er_needs_review');
	$flag->flag('flag', $newNode->nid, null, TRUE);
  
  return $newNode;
}