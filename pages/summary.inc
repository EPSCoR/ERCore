<?php
//require_once ER_MODULE_DIR . '/includes/models.inc';
//This file builds the summary table.
//It includes a series of views in order to populate the counts, and produce the excel sheets.
//The list of content types can be set through the ER Module's settings page located at: admin/config/epscor/er
//See file er/includes/er.admin.inc for settings regarding that setup form.
class er_summary extends er_page{  
  static private $ranges = Array();
  
  public function __construct(){
    drupal_session_start();
    parent::__construct();
		
    //This is used to impose a date range restriction. See the form definition at the bottom of this file.
    if (array_key_exists('er_start_date', $_SESSION) && array_key_exists('er_end_date', $_SESSION)){
      self::$ranges[] = array($_SESSION['er_start_date'], $_SESSION['er_end_date']);
    }else{
      //provide the default ranges
      //this code takes the start date provided in er.module file, and creates an array of date ranges.
      //it is assumed that the reporting periods start with whatever month the grant was awarded on.
			$start_date = variable_get('er_start_date');
			//dsm(mktime(0, 0, 0, $start_date['month'], $start_date['day'], $start_date['year']));
      $cur_month = date('n');
      $start_month = $start_date['month'];
      $adjusted_date = mktime(0, 0, 0, $cur_month-$start_month+7);//+half a year into the future (6) +1 offset
      //$adjusted_date = mktime(0, 0, 0, $cur_month+6);
      $cur_year = date('Y', $adjusted_date);
      $start_year = $start_date['year'];
      for ($y = $cur_year; $y>=$start_year;$y--){
        if ($y == $start_year)
          self::$ranges[] = Array(mktime(0, 0, 0, $start_date['month'], $start_date['day'], $start_date['year']), mktime(0, 0, 0, $start_month, 0, $y+1));
        else
          self::$ranges[] = Array(mktime(0, 0, 0, $start_month, 1, $y), mktime(0, 0, 0, $start_month, 0, $y+1));
        //self::$ranges[] = Array(($y-1)."-09-00", $y."-08-31");
      }
    }
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
   * Produces the data used in both html and excel displays.
   */
  public function data(){
    $debug = false;
    $ranges = Array();
    $ret = Array();
    //$content_types = er_content_types();
		//$ct_info = node_type_get_types();
    $display_date = "M j, Y";
    $ret['header'][] = "Categories of Accomplishments";
    $ret['header'][] = "Inception<br>through<br>" . date($display_date);
    foreach (self::$ranges as $k=>$v)
      $ret['header'][] = date($display_date, $v[0])."<br>through<br>".date($display_date, $v[1]);
    
		$types = variable_get('er_summary_types');//see er/includes/er.admin.inc
		if (isset($types) && count($types)){
			foreach ($types as $k=>$type){
				$data = array();
				$view_name = "er_summary_$type";//$type.'_summary_view';
				if ($view = views_get_view($view_name)){
					$view->set_display('count');				
					$view->execute();
					//dsm($view);
					$data[] = $this->format_label($type, $view);//$ct_info[$type]->name;
					if (isset($view->result[0]->nid))
						$data[] = $view->result[0]->nid;//this is the node count //old way: $view->total_rows;
					else
						$data[] = $view->result[0]->uid;//need this for the participants row...
					foreach (self::$ranges as $k=>$v){
						$view = views_get_view($view_name);
						$view->set_display('count');
						//dsm(date('Y-m-d', $v[0]) . ' through '. date('Y-m-d', $v[1]));
						$view->exposed_input['start_date']['value'] = date('Y-m-d', $v[0]);
						$view->exposed_input['end_date']['value'] = date('Y-m-d', $v[1]);
						//$view->set_items_per_page(1);
						$view->execute();
						if (isset($view->result[0]->nid))
							$data[] = $view->result[0]->nid;//this is the node count 
						else
							$data[] = $view->result[0]->uid;//need this for the participants row...
					}
				}else{
					drupal_set_message(t('View not found: @name', array('@name'=>$view_name)), 'error');
				}
				$ret['rows'][] = $data;
			}
		}else{
			$link = l('click here', '/admin/config/epscor/er');
			drupal_set_message(t('An administrator needs to select which content types you want to see on the summary table!')." $link", 'error');
		}
		//dsm($view);
		//dsm(memory_get_usage(true));
    return $ret;
  }
	
	//this is the label shown on the accomplishments table. ex: "publications", "current and pending", "posters"
	private function format_label($type, &$view){
		//global $user;
		$view_title = isset($view->display['default']->handler->options['title'])?$view->display['default']->handler->options['title']:$type;
		if (isset($view->display['page']->display_options['path']))
			$ret = l('<i class="icon-download-alt"></i>', $view->display['page']->display_options['path'].'/download', array('html'=>true)). ' ' . l($view_title, $view->display['page']->display_options['path']);
		else
			$ret = $view_title;
		return $ret;
	}
  
  /*
   * Function returns the final html markup for this page.
   */
  public function html(){
    $content = '';
		//$content = drupal_get_form('er_date_range_form');
    if (count($_POST) > 0){ //this occurs when the form is submitted... we should probably submit somewhere else...?
      //d($_POST, 'FORM POSTED');
      return $content;
    }
    //global $user;
    $data = $this->data();
		$content .= theme('table', $data);
    $content .= "* Current and Pending Support counts include all grants regardless of status.";
    if (user_access(ER_DOWNLOAD_PERMISSION))
      $content .= '<p style="text-align:center;">'.$this->download_link('Download Reporting Tables', 'excel')."</p>";
    return $content;
  }
    
    /*
     *
     * START OF EXCEL STUFF
     *
     */

  //use this to reorder the fields
  /*private function used_field_keys($type, &$fields){//just pass by ref to use less memory
    $keys = array_keys($fields);
    switch($type){
    case 'er_engagement':
    sort($keys, SORT_STRING);
    $keys = array_reverse($keys);
    $front = array("field_er_cal_event_reference", "field_er_estimated");
    $keys = array_unique(array_merge($front, $keys)); //hehe nifty trick
      break;
    }
    return $keys;
  }*/
  
  /*
  private function used_field_labels($type, $sql_name, $default){
    switch($type){
      case 'er_engagement':
        if (false === array_search($sql_name, array("cal_event_reference", "estimated")))
          return $sql_name;
      default:
        return $default;
    }
  }*/
  
  
  /*
   * Function produces the data needed to fill an excel sheet.
   */
  public function download_accomplishment($type){
    //$type = str_replace('-', '_', substr($path, 0, -5));//take off "-view"
    //$content_types = er_content_types();
    //$class = $content_types[$type];
    if ($class = er_content_types($type)){
      
      $ret = new $class($type, self::$ranges);
      $ret->set_excel_data();
      
      if (ER_DEBUG_EXCEL) d($ret);
      return $ret;
    }else{
      d('Bad accomplishment request.');
      return null;
    }
  }
  
  /*
   * Function takes data from download_accomplishment method, and fills an empty excel sheet, user then downloads the excel sheet.
   */
  public function write_excel(&$objPHPExcel, $sheet = 0){
    //$content_types = er_content_types();
    if ($data = $this->download_accomplishment(arg(3))){
      $display_date = "M j, Y";
      //$data = $this->download_accomplishment(arg(3));
      //$data = $ret->data;
      $worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
      $title = $data->name;
      if (count($data->ranges)==1)
        $title .= " (".date($display_date, $data->ranges[0][0]).' - '.date($display_date, $data->ranges[0][1]).')';
      else
        $title .= " (All)";
      drupal_set_title("Download " . $title);
      $worksheet->setCellValue("A1", str_replace('er_', '', $title));
      $worksheet->getStyle('A1')->applyFromArray(array('font' => array('bold' => true)));
      //$row = 2;
      ////Create labels
      //$worksheet->setCellValue('A'.$row, '#');
      //$worksheet->getColumnDimension('A')->setWidth(4);
      //$column = 1;
      //foreach ($data->order as $field){//draw the headers
      //  $label = $data->labels[$field];
      //  $col = $this->itol($column);
      //  if (!$label)//simple fallback incase there is no label available
      //    $label = ucfirst(str_replace('_', ' ', $field));
      //  $worksheet->setCellValue($col.$row, $label);
      //  $column++;
      //}
      //$row++;
      $column = 0;
      $row = 3;
      $this->write_excel_columns($worksheet, $column, $row, $data);
      //foreach ($data->data as $count=>$result){
        
        //$result = $data->fields;
        //$worksheet->setCellValue('A'.$row, $count+1);
        //$column = 1;
        ////foreach ($result as $fk=>$field){       
        //foreach($data->order as $field){
        //  //$col = chr(ord('A')+$column);
        //  $col = $this->itol($column);
        //  //$output = $this->format_type($result[$field], $data->field_type[$field]);
        //  $field_info = $data->field_info($field);
        //  //need to use special excel library methods for certain types of content.
        //  if ($field_info['module'] == 'date'
        //    && strpos($result[$field], ',') === FALSE //cannot have a comma. the query sometimes produces multiple values, comma separated. I can only deal with 1 date here. 
        //    && strpos($result[$field], '-') !== FALSE //we want to see a dash because this doesn't work well if it's a date field but only has a year.
        //  ){ 
        //    $worksheet->setCellValue($col.$row, PHPExcel_Shared_Date::stringToExcel($result[$field]));
        //    $worksheet->getStyle($col.$row)->applyFromArray(array('numberformat' => array('code' => 'MM/DD/YYYY')));
        //    $worksheet->getColumnDimension($col)->setWidth(12);
        //    //$worksheet->getStyle($col.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_DATE_YYYYMMDDSLASH);
        //  }else if ($field_info['module'] == 'number' && $field_info['prefix'] == '$'){ //assumed to be money, format it as such.
        //    $number = number_format($result[$field], 2, '.', '');
        //    $worksheet->setCellValue($col.$row, $number);
        //    $worksheet->getStyle($col.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_CURRENCY_USD);
        //    $worksheet->getColumnDimension($col)->setWidth(15);
        //  }else{//otherwise use the simple value formatter contained in the summary_data class...
        //    $worksheet->setCellValue($col.$row, $data->format_field($field, $result[$field]));
        //  }
        //  $column++;
        //}
        //$row++;
      //}
    }
  }
  
  private function write_excel_columns(&$worksheet, &$column, &$row, &$data){
    $start_column = $column;
    //Create labels
    $worksheet->setCellValue($this->itol($column++).'2', '#');
    $worksheet->getColumnDimension('A')->setWidth(4);
    foreach ($data->order as $field){//draw the headers
      $label = $data->labels[$field];
      $col = $this->itol($column++);
      if (!$label)//simple fallback incase there is no label available
        $label = ucfirst(str_replace('_', ' ', $field));
      $worksheet->setCellValue($col.'2', $label);
    }
    foreach ($data->data as $count=>$result){
      //$result = $data->fields;
      $column = $start_column;
      $worksheet->setCellValue($this->itol($column++).$row, $count+1);
      //$column = 1;
      //foreach ($result as $fk=>$field){       
      foreach($data->order as $field){
        $col = $this->itol($column);
        /*if (is_array($result[$field])){
          $worksheet->setCellValue($col.$row, 'values');//$result[$field]);
          $row+=4;
        }else */if (is_object($result[$field])){
          $this->write_excel_columns($worksheet, $column, $row, $result[$field]);
        }else{
          //$output = $this->format_type($result[$field], $data->field_type[$field]);
          $field_info = $data->field_info($field);
          //need to use special excel library methods for certain types of content.
          
          if ($field_info['module'] == 'date'
            && strpos($result[$field], ',') === FALSE //cannot have a comma. the query sometimes produces multiple values, comma separated. I can only deal with 1 date here. 
            && strpos($result[$field], '-') !== FALSE //we want to see a dash because this doesn't work well if it's a date field but only has a year.
          ){ 
            $worksheet->setCellValue($col.$row, PHPExcel_Shared_Date::stringToExcel($result[$field]));
            $worksheet->getStyle($col.$row)->applyFromArray(array('numberformat' => array('code' => 'MM/DD/YYYY')));
            $worksheet->getColumnDimension($col)->setWidth(12);
            //$worksheet->getStyle($col.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_DATE_YYYYMMDDSLASH);
          }else if ($field_info['module'] == 'number' && $field_info['prefix'] == '$'){ //assumed to be money, format it as such.
            $number = number_format($result[$field], 2, '.', '');
            $worksheet->setCellValue($col.$row, $number);
            $worksheet->getStyle($col.$row)->getNumberFormat()->setFormatCode(PHPExcel_Style_NumberFormat::FORMAT_CURRENCY_USD);
            $worksheet->getColumnDimension($col)->setWidth(15);
          }else{//otherwise use the simple value formatter contained in the summary_data class...
            $worksheet->setCellValue($col.$row, $data->format_field($field, $result[$field]));
          }
        }
        $column++;
      }
      $row++;
    }
    if (count($data->data) != 0)
      $row--;//remove the extra row...
  }
}

/*
 * Constructs the date range form seen at the top of the accomplishments table.
 */
function er_date_range_form($form_state) {
  drupal_session_start();
  $date_year_range = "2008:+1";
  
  $form['dates'] = array(
    '#type' => 'fieldset',
    //'#title' => t('Pick date range'),
    '#tree' => TRUE,
    '#attributes' => array('class'=>'clearfix fake-inline', 'style'=>"border:none;margin:0;padding:0;"),
    //'#collapsible' => TRUE,
    //'#collapsed' => $_SESSION['er_dates_collapsed'],
  );
  $form['dates']['start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start Date'),
    '#date_format' => 'm/d/Y',//variable_get('date_format_short', 'm/d/Y - H:i'),
    '#date_timezone' => date_default_timezone(), //date_default_timezone_name(),
    '#default_value' => date('Y-m-d', array_key_exists('er_start_date', $_SESSION)?$_SESSION['er_start_date']:time()),
    '#date_year_range' => $date_year_range
  );
  
  $form['dates']['end_date'] = array( 
    '#type' => 'date_popup',
    '#title' => t('End Date'),
    '#date_format' => 'm/d/Y',//variable_get('date_format_short', 'm/d/Y - H:i'),
    '#default_value' => date('Y-m-d', array_key_exists('er_end_date', $_SESSION)?$_SESSION['er_end_date']:mktime(0, 0, 0, date('n')+1)),
    '#date_timezone' => date_default_timezone(), //date_default_timezone_name(),
    '#date_year_range' => $date_year_range
  );
  $form['dates']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array('style'=>"margin-top:28px;"),
  );
  $form['dates']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#attributes' => array('style'=>"margin-top:28px;"),
  );
  return $form;
}

/*
 * Handles the form data when the form is submitted.
 * Sets two session variables 'er_start_date' and 'er_end_date'.
 * When these session variables are set, the accomplishments table overrides it's default date ranges, and uses this range instead.
 */
function er_date_range_form_submit($form, &$form_state){
  //$_SESSION['er_dates_collapsed'] = $form['dates']['#collapsed'];
  if ($form['#post']['op'] == t('Reset')){
    $_SESSION['er_start_date'] = $_SESSION['er_end_date'] = NULL;
  }else{
    $s = strtotime($form['dates']['start_date']['#value']['date']);
    $e = strtotime($form['dates']['end_date']['#value']['date']);
    if ($s <= $e){
      $_SESSION['er_start_date'] = $s;
      $_SESSION['er_end_date'] = $e;
    }else{
      drupal_set_message("Start date must be earlier than end date.", 'error');
    }
  }
}