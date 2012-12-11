<?php

//This file defines some of our custom views. These are the defaults, and can be extended by the individual epscor states.
//These defaults should function out-of-the-box, and so if something gets messed up, you should be able to revert back to these defaults.
//The views are each defined in individual files, ex: /er/views/[name of view].view
function er_views_default_views() {
/* Mike commented out some stuff so it loops through a directory, not hardcoded */
/*
  $views = array(
    //Summary views
    "er_summary_er_award",
    "er_summary_er_book",
    "er_summary_er_engagement",
    "er_summary_er_event",
    "er_summary_er_internet",
    "er_summary_er_other_product",
    "er_summary_er_patent",
    "er_summary_er_poster",
    "er_summary_er_presentation",
    //"er_summary_er_project", //deprecated, no longer necessary
    "er_summary_er_proposal",
    "er_summary_er_publication",
    "er_summary_er_institution",
    "er_summary_er_collaboration",
    "er_summary_er_news",
    "er_summary_participants",
    
    //NSF Reports
    "er_report_collaborations",
    "er_report_external_engagement",
    "er_report_salary_support",
    "er_report_participants",
    "er_report_outputs",
  );
  */
  $export = array();
  
	$dir = new DirectoryIterator(ER_MODULE_DIR . '/views');
	// loop through all view files in the directory
	foreach ($dir as $fileinfo) {
		if (!$fileinfo->isDot()) {
			$view_filename = $fileinfo->getFilename();
			require_once(ER_MODULE_DIR."/views/$view_filename");
			$export[$view->name] = $view;			
			//er_attach_view($export, $view_filename);//load in the individual views
		}
	}  
  
  
  /*
  foreach ($views as $filename)
    er_attach_view($export, $filename);//load in the individual views
	*/
  return $export;
}
/*
function er_attach_view(&$export, $filename){
  require_once(ER_MODULE_DIR."/views/$filename.view");
  $export[$view->name] = $view;
  
}
*/
