<?php
function er_block_info() {
  $blocks['er_content_links'] = array(
    'info'=>t('Create new EPSCoR Reporting content links'),
    'title'=>'',
    //'cache'=>DRUPAL_NO_CACHE
  );
  $blocks['er_collab_inst_block'] = array(
    'info'=>t('Block used to attach Collaborators to Collaboration nodes'),
    'title'=>'',
    //'cache'=>DRUPAL_NO_CACHE
  );
  return $blocks;
}

function er_block_view($delta = '') {
  // This example is adapted from node.module.
  $block = array();

  switch ($delta) {
    case 'er_content_links':
      $block['subject'] = t('Create new EPSCoR Reporting content');
      $block['content'] = array(
        '#theme' => 'er_content_links', 
        '#title' => '',//t('Create new EPSCoR Reporting content - title'),
      );
      break;
    case 'er_collab_inst_block':
      $block['subject'] = t('Institutions');
      $block['content'] = array(
        '#theme' => 'er_collab_inst_block', 
        '#title' => '',
      );
      break;

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