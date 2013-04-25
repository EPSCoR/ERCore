<?php
//THIS FILE IS NOW DEFUNCT. SEE: includes/accomplishments.inc
require_once(ER_MODULE_DIR.'/er_page.inc');
//require_once ER_MODULE_DIR . '/includes/models.inc';
//This file builds the summary table.
//It includes a series of views in order to populate the counts, and produce the excel sheets.
//The list of content types can be set through the ER Module's settings page located at: admin/config/epscor/er
//See file er/includes/er.admin.inc for settings regarding that setup form.
class er_summary extends er_page{  
  public function __construct(){
    parent::__construct();
  }
  
  /**
   * Returns the title of the page which will be used in the H1 and TITLE tags.
   *
   * @return <string>
   */
  public function title(){
    if (arg(2) == "download" && $name = arg(3))//quick way of changing the filename of the excel sheets...
      return $name;
    return 'Summary Table';
  }
  
  /*
   * Function returns the final html markup for this page.
   */
  public function html(){
		//see: includes/accomplishments.inc
		return drupal_get_form('er_date_range_form');
  }
}