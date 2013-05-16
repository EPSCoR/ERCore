<?php
  $node = menu_get_object();
  if ($node->type == 'er_collaboration'){
    //d($node, 'Collab node');
    if (isset($node->field_er_inst_ref) && $node->field_er_inst_ref['und'])
    foreach ($node->field_er_inst_ref['und'] as $k=>$v){
      $inst = $v['entity'];
			//d($inst, '$inst');
      echo '<h3 style="font-weight:bold;">'.$inst->title . ' ' . l('<i class="icon-share"></i>', 'node/'.$inst->nid, array('html'=>true)). '</h3>';
      $view = views_get_view('er_collab_inst_block');
      $view->set_arguments(array($node->nid, $inst->nid));
			$view_output = $view->render();
			//d($view, '$view');
			if ($inst->field_er_inst_participating['und'][0]['value'] != 1 && !count($view->result))
				echo '<span class="form-required">* At least one collaborator must be added to this institution.</span>';
			
			echo $view_output;
    }
  }
?>
