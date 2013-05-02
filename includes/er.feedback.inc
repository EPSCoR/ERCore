<?php
function er_feedback_form($form, &$form_state){
  //Your new best friend: http://api.drupal.org/api/drupal/developer!topics!forms_api_reference.html/7

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