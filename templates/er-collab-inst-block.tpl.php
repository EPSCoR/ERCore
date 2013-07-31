<?php
  $node = menu_get_object();
  if ($node->type == 'er_collaboration'){
		$view = views_get_view('er_collab_inst_block');
		$view->set_arguments(array($node->nid));
		$view_output = $view->render();
		$title = isset($view->build_info['title'])?$view->build_info['title']:'Collaborators';
		echo '<fieldset class="er-collab-inst-block-view"><legend>'.$title.'</legend>'. $view_output . '</fieldset>';
  }