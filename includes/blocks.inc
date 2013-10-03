<?php
function er_block_info() {
  $blocks['er_content_links'] = array(
    'info'=>t('Create new EPSCoR Reporting content links'),
    'title'=>'',
    //'cache'=>DRUPAL_NO_CACHE
  );
  $blocks['er_nsf_acknowledgement'] = array(
    'info'=>t('EPSCoR NSF Acknowledgements'),
    'title'=>'<none>',
    'region'=>'footer',
    'status'=>1,
    'cache'=>DRUPAL_CACHE_GLOBAL,
  );
  return $blocks;
}

function er_block_configure($delta = ''){
  $form = array();
  switch ($delta) {
    case 'er_nsf_acknowledgement':
      $form['er_grant_number'] = array(
        '#type' => 'textfield',
        '#title' => t('NSF Grant Number'),
        '#required' => 1,
        '#default_value' => variable_get('er_grant_number', ''),
      );
      break;
  }
  return $form;
}

function er_block_save($delta = '', $edit = array()) {
  switch ($delta) {
    case 'er_nsf_acknowledgement':
      variable_set('er_grant_number', $edit['er_grant_number']);
      break;
  }
}

function er_block_view($delta = '') {
  
  $block = array();

  switch ($delta) {
    case 'er_content_links':
      $block['subject'] = t('Create new EPSCoR Reporting content');
      $block['content'] = array(
        '#theme' => 'er_content_links', 
        '#title' => '',//t('Create new EPSCoR Reporting content - title'),
      );
      break;
    case 'er_nsf_acknowledgement':
      $grant_num = variable_get('er_grant_number', '#');
      $ack = t('This material is based upon work supported by the National Science Foundation under Grant Number %grantnumber.', array('%grantnumber'=>$grant_num));
      $additional = t("Any opinions, findings, and conclusions or recommendations expressed in this material are those of the author(s) and do not necessarily reflect the views of the National Science Foundation.");
      $block['subject'] = t('EPSCoR NSF Acknowledgements');
      $block['content'] = array(
        '#markup' => $ack.'<br>'.$additional,
        '#title' => '',
      );
      break;
    // This example is adapted from node.module.
    //case 'recent':
    //  if (user_access('access content')) {
    //    $block['subject'] = t('Recent content');
    //    if ($nodes = node_get_recent(variable_get('node_recent_block_count', 10))) {
    //      $block['content'] = array(
    //        '#theme' => 'node_recent_block', 
    //        '#nodes' => $nodes,
    //      );
    //    }
    //    else {
    //      $block['content'] = t('No content available.');
    //    }
    //  }
    //  break;
  }
  return $block;
}