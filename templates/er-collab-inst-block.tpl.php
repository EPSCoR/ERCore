<?php
  $node = menu_get_object();
  if ($node->type == 'er_collaboration'){
    //d($node, 'Collab node');
    if (isset($node->field_er_inst_ref) && $node->field_er_inst_ref['und'])
    foreach ($node->field_er_inst_ref['und'] as $k=>$v){
      $inst = $v['entity'];
      echo '<h3 style="font-weight:bold;">'.$inst->title . ' ' . l('<i class="icon-share"></i>', 'node/'.$inst->nid, array('html'=>true)). '</h3>';
      $view = views_get_view('er_collab_inst_block');
      $view->set_arguments(array($node->nid, $inst->nid));
      //d($view, 'view');
      //$view->agruments['er_collab_ref']->value[0] = $node->nid;
      //$view->agruments['er_collab_inst_ref']->value[0] = $inst->nid;
      //$view->set_display('count');				
			echo $view->render();
      
    }
  }
?>
