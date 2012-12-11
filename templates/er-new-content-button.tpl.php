<?php
$ci = _content_type_info();
$content_types = $ci['content types'];
//List of content types: 'er_user_event', 'er_user_profile', 'cimes_news_item', 'ddblock_news_item', 'er_collaborator', 'er_paid', 'er_user_event_instance', 'mike_ahah_test', 'popup', 'webform'
//d(menu_tree_all_data('menu-create-content-menu')); //hooks into the menu system...

//NOTICE: "new_user_link" is an exception. This is not the name of a content type, it will instead create a "New User" link at that location.
$links = array(
    //"Reporting Content" => array('er_publication', 'er_proposal', 'er_poster', 'er_presentation', 'er_book', 'er_internet', 'er_patent', 'er_other_product'),
    //"Reporting" => array('er_project', 'er_proposal', 'er_patent', 'er_other_product', 'er_nsf_fastlane', 'er_institution', 'er_user_event', 'er_user_event_instance'),
    "Reporting" => array('er_project', 'er_proposal', 'er_patent', 'er_other_product', 'er_nsf_fastlane'),
    "Outreach" => array('er_publication', 'er_poster', 'er_presentation', 'er_book', 'er_internet', 'ddblock_news_item'),
    "Web Content" => array('er_event', 'page', 'blog', 'node_gallery_gallery', 'node_gallery_image'),
    "Admin Content" => array('er_institution', 'er_user_event', 'er_user_event_instance', 'new_user_link'),
    //"Admin Content" => array('er_institution', 'er_nsf_fastlane')
);
?>
<div>
    <div id="create-content-links">
        <?
        $total = count($links);
        foreach ($links as $label=>$names){
            $counter++;
            $ul = '';
            foreach ($names as $c){
                if ($c == 'new_user_link'){
                    if (user_access("administer users"))
                        $ul .= "<li>".l("New User", 'admin/user/user/create')."</li>";
                }else{
                    if (user_access("create $c content"))
                        $ul .= "<li>".l($content_types[$c]['name'], 'node/add/'.$content_types[$c]['url_str'])."</li>";
                }
            }
            if ($ul){?>
            <nav class="<?=(!$counter?'first':'')?><?=($counter==$total?'last':'')?>">
                <h3><?=$label?></h3>
                <ul>
                    <?=$ul?>
                </ul>
            </nav>
            <? }
        } ?>
    </div>
    <div style="margin:auto;padding:5px 0 5px 0;width:150px;">
    <button id="create-content-button" class="cupid-green">Create new content</button>
    <!--<button class="minimal">Create new content</button>-->
    <!--<button class="clean-gray">Create new content</button>-->
    <!--<button class="cupid-blue">Create new content</button>-->
    <!--<button class="blue-pill">Create new content</button>-->
    <!--<button class="download-itunes">Create new content</button>-->
    <!--<button class="skip">New content</button>-->
    </div>
    <script>
        (function($, window, document, undefined){
            var $links = $('div#create-content-links').hide().addClass('create-content-popout').click(function(e){
                    e.stopPropagation();
                }),
                $button = $('button#create-content-button').click(function(e){
                    e.stopPropagation();
                    $links.show();
                });
            $(window).click(function(){
               $links.hide(); 
            });
        })(jQueryNew, this, document);
    </script>
</div>