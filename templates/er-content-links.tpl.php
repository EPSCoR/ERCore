<?php
//This file may no longer be necessary.

$node_types = node_type_get_types(); //we can use this to get the [human readible] name of the content type 
//dsm($node_types, 'node_types');
//dsm($full_view, '$full_view');//variable gets passed in through theme function

//List of content types: 'er_user_event', 'er_user_profile', 'cimes_news_item', 'ddblock_news_item', 'er_collaborator', 'er_paid', 'er_user_event_instance', 'mike_ahah_test', 'popup', 'webform'
//d(menu_tree_all_data('menu-create-content-menu')); //hooks into the menu system...

//NOTICE: "new_user_link" is an exception. This is not the name of a content type, it will instead create a "New User" link at that location.
$links = array(
  //"Reporting Content" => array('er_publication', 'er_proposal', 'er_poster', 'er_presentation', 'er_book', 'er_internet', 'er_patent', 'er_other_product'),
  //"Reporting" => array('er_project', 'er_proposal', 'er_patent', 'er_other_product', 'er_nsf_fastlane', 'er_institution', 'er_user_event', 'er_user_event_instance'),
  "Reporting" => array(/*'er_project',*/ 'er_proposal', 'er_patent', 'er_other_product', 'er_nsf_fastlane'),
  "Outreach" => array('er_publication', 'er_poster', 'er_presentation', 'er_book', 'er_internet', 'er_news'),
  "Web Content" => array('er_event', 'page', 'blog', 'node_gallery_gallery', 'node_gallery_image'),
  "Admin Content" => array('er_institution', /*'er_user_event', 'er_user_event_instance',*/ 'new_user_link'),
  //"Admin Content" => array('er_institution', 'er_nsf_fastlane')
);

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
      ));
    }
  }
?>
</nav>
<div style="clear:both"></div>
