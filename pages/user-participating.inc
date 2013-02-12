<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
class er_user_participating extends er_page{

	public function __construct() { 
        parent::__construct(); 
    }
    
    public function title(){
    	return "Manage User Events";
    }
    
	public function data($op = 'withue'){
		$defaultType = 'Employed';
		switch ($op) {
			// case 'withhired':
				// $query = db_query("SELECT uid, name, field_er_user_event_type_value FROM {users} u, {content_type_er_user_event} c WHERE u.uid = c.field_er_event_user_uid AND field_er_user_event_type_value = 'Hired'");
				// $fieldsArr = array('Name', 'Hired Type');
				// $defaultType = 'Employed';
				// break;
			case 'withue':
				$query = db_query("SELECT uid, name, field_er_user_event_type_value FROM {users} u, {content_type_er_user_event} c WHERE u.uid = c.field_er_event_user_uid AND field_er_user_event_type_value LIKE 'Employed%'");
				$fieldsArr = array('Name', 'Employment Type');
				$defaultType = 'Employed';
				break;
			case 'withoutue':
				$query = db_query("SELECT uid, name, mail FROM {users} u WHERE NOT EXISTS (SELECT * FROM {content_type_er_user_event} c WHERE u.uid = c.field_er_event_user_uid AND field_er_user_event_type_value LIKE 'Employed%')");
				$fieldsArr = array('Name', 'Email');
				break;
			case 'part':
				$query = db_query("SELECT uid, name, field_er_user_event_type_value FROM {users} u, {content_type_er_user_event} c WHERE u.uid = c.field_er_event_user_uid AND field_er_user_event_type_value = 'Participating'");
				$fieldsArr = array('Name', 'Type');
				$defaultType = 'Not Participating';															
				break;
			case 'notpart':
				$query = db_query("SELECT uid, name, mail FROM {users} u WHERE NOT EXISTS (SELECT * FROM {content_type_er_user_event} c WHERE u.uid = c.field_er_event_user_uid AND field_er_user_event_type_value = 'Participating')");
				$fieldsArr = array('Name', 'Email');
				$defaultType = 'Participating';
				break;
			
			default:
				$query = db_query("SELECT uid, name, mail FROM {users} u WHERE NOT EXISTS (SELECT * FROM {content_type_er_user_event} c WHERE u.uid = c.field_er_event_user_uid AND field_er_user_event_type_value LIKE 'Employed%')");
				$fieldsArr = array('Name', 'Email');				
				break;
		}

	  
			 
	  $data = array();
	
	  $i = 0;
	  $url = url('node/add/er-user-event', array('query'=>'format=simple'));
	  
	  
	  
	  while ($row = db_fetch_array($query)) {
	  	if ($row['uid']==0) continue; // removes anonymous user from list
	    $data[$i] = $row;
		// $data[$i]['url'] = '<a href="'.$url.'&uid='.$data[$i]['uid']. '" rel="lightframe[group|width:800px;height:750px]">Add Event</a>';
		$data[$i]['url'] = '<a href="'.$url.'&ueType='.$defaultType.'&uid='.$data[$i]['uid']. '" rel="lightframe[group|width:800px;height:750px]">Add \''.$defaultType.'\' Event</a>';
	    
	    //d($row, 'row');
		$account = realname_get_user($data[$i]['uid']);
		if ($account->name != NULL) {
			// $data[$i]['name'] = $account->name;
			$data[$i]['name'] = l($account->name, "user/$account->uid/user-events");
			// d($account, 'account');
		}
		// $data[$i]['field_er_user_event_type_value'] .= '<br/>nextone here';
		// d($data, 'data');
		unset($data[$i]['uid']);
	    $i++;
	  }
	  // d($data, 'data before table');
	  $output = theme_table($fieldsArr, $data);	
		
		return $output;
	}
	
	public function html(){
		
		if (!user_access("administer nodes")){
			$content = 'You do not have access to this page.';			
		} 
		else{
			$content = drupal_get_form('er_user_participating_form');
	        $data = $this->data($_SESSION['uechoice']);
			$content .= $data;					
		}
		return $content;		
	}
}


	function er_user_participating_form($form_state) {
		//d($form_state, 'formstate');
		
		$form['ue'] = array(
			'#type' => 'fieldset',
			'#tree' => TRUE,
			'#attributes' => array('class'=>'clearfix fake-inline', 'style'=>"border:none;margin:0;padding:0;"),
		);		
		
		$form['ue']['uetype'] = array(
		  '#type' => 'select',
		  '#title' => t('List Users from "Participating" or "Employed" User Events'),
		  '#default_value' => isset($_SESSION['uechoice'])?$_SESSION['uechoice']:'withoutue',
		  '#options' => array(
		    'withue' => t('Users with an Employed Event'),
		    'withoutue' => t('Users WITHOUT an Employed Event'),
		    'part'		=> t('Users with Participating Event'),
		    'notpart'	=> t('Users WITHOUT Participating Event'),
		  ),
		  '#description' => t('All users need to be be given user events.'),
		);					
		
		$form['ue']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Search'),
			
		);
		return $form;		
	}	
	
	function er_user_participating_form_submit($form, &$form_state){
		$_SESSION['uechoice'] = $form['ue']['uetype']['#value'];
	}