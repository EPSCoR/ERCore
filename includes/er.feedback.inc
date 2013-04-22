<?php
function er_feedback_form($form, &$form_state){
  //Your new best friend: http://api.drupal.org/api/drupal/developer!topics!forms_api_reference.html/7
  $form['location'] = array(
  	'#type' => 'textfield',
	'#title' => 'Location',
	'#description' => 'Enter the url of the problem',
	'#required' => TRUE
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Feedback'),
    '#description' => 'Describe the problem',
	'#required' => TRUE
  );
  $form['screenshot'] = array(
  	'#type' => 'file',
	'#title' => 'Screenshot',
	'#description' => 'Upload a screenshot of the page/problem/error message (Optional)'
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form; 
}

function er_feedback_form_validate($form, &$form_state){
  //This function may not be necessary.
  d('Feedback form is being validated.');
}

function er_feedback_form_submit($form, &$form_state){
  d('Feedback form has passed validation and is now being submitted!');
  d($form_state['values']['description'], 'description');
  //Process the form, submit the email message.
}

//Push Test