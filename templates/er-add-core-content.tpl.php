<?php
$node_types = node_type_get_types(); //we can use this to get the [human readible] name of the content type 
//dsm($node_types, 'node_types');
//dsm($full_view, '$full_view');//variable gets passed in through theme function
//
// content types that appear in "ER Core" section
//
$core = array(
	'er_event', 'er_collaboration', 'er_institution',
	'er_other_product', 'er_patent', 'er_presentation',
	'er_proposal', 'er_publication');

//Hide these content types
$hidden = array('er_collaborators', 'er_engagement', 'er_effort');

$links = array(
  "Drupal Content"=>array(),
  "EPSCoR Reporting Categories"=>array(),//changed from "ERCore" to "EPSCoR Reporting Categories" per admin direction
  "Jurisdictional"=>array(),
//  "Currently Hidden [debugging display]"=>array(),
);
//dsm(array_keys($node_types));
$types = array_keys($node_types);
$nodes = array_values($node_types);
function cmp($a, $b){
  return strcmp($a->name, $b->name);
}
usort($nodes, "cmp");//sort the cts in alpha order.
foreach ($nodes as $n){
    if (substr($n->type, 0, 3) != 'er_') {
			// everything that doesn't start with 'er_' goes in first section
			$links['Drupal Content'][] = $n->type;
    }else{
			if (in_array($n->type, $core)) {
				// types in $core go in second section
				// need to do this in order to get them in alpha order...
				$links["EPSCoR Reporting Categories"][] = $n->type;
			}else if (!in_array($n->type, $hidden)) {
					// types in $juris go in third section
					$links['Jurisdictional'][] = $n->type;
			}
		}
}
$class = $full_view?'full':'small';
?>
<nav class="<?=$class?>">
<?php
  foreach ($links as $label=>$types){
    $links = array();
    foreach ($types as $type){
      $link = '';
      if ($type == 'new_user_link' && user_access("administer users")){
        $link = l("New User", 'admin/user/user/create');
        if ($full_view)
          $link .= '<div class="description">Create a new user account</div>';
      }else if (user_access("create $type content")){
        $link = '<span class="label">'.l($node_types[$type]->name, 'node/add/'.str_replace('_', '-', $type)).'</span>';
        if ($full_view)
          $link .= '<div class="description">'.$node_types[$type]->description.'</div>';
      }
      if ($link)
      $links[] = $link;
    }
    if (count($links)){
      echo theme_item_list(array(
        'items' => $links,
        'title' => $label,
        'type' => 'ul',
        'attributes' => array(
          'class'=>'admin-list'
        )
      )) . '<br>';
    }
  }
?>
</nav>
<div style="clear:both"></div>
