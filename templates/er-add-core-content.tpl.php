<?php
$node_types = node_type_get_types(); //we can use this to get the [human readible] name of the content type 
//dsm($node_types, 'node_types');
//dsm($full_view, '$full_view');//variable gets passed in through theme function

$core = array('er_institution', 'er_proposal', 'er_patent', 'er_other_product', 'er_nsf_fastlane', 'er_publication', 'er_poster', 'er_presentation', 'er_book', 'er_internet'/*, 'er_news'*/, 'er_engagement', 'er_collaboration', 'er_event', 'er_paid');
$links = array(
  "Drupal Content"=>array(),
  "ER Core"=>array(),
  "Juristictional"=>array(),
);
//dsm(array_keys($node_types));
$types = array_keys($node_types);
$nodes = array_values($node_types);
function cmp($a, $b){
  return strcmp($a->name, $b->name);
}
usort($nodes, "cmp");//sort the cts in alpha order.
foreach ($nodes as $n){
  if (substr($n->type, 0, 3) != 'er_')//everything that doesn't start with 'er_'
    $links['Drupal Content'][] = $n->type;
  else if (in_array($n->type, $core)){//need to do this in order to get them in alpha order...
    $links["ER Core"][] = $n->type;
  }else{
    $links['Juristictional'][] = $n->type;//if it has 'er_' but isn't part of core, add that into the juristictional section
  }
  //dsm($type);
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
