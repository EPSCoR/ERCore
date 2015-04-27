<?php
require_once(ER_MODULE_DIR.'/er_page.inc');
//I'm going to use this file to collect the other excel functions, so they all modify the same excel sheet.
//This system currently assumes that there are "pages" linked into the er module using the names stored in $sheets

class er_collaborations extends er_page{
  static private  $types = array(
    'local'=>'Jurisdictional',
    'domestic'=>'External-Domestic',
    'foreign'=>'External-Foreign',
    'total'=>'Total'
  );
  static private $categories = array(
    'academic'=>'Academic Research Institutions',
    'undergrad'=>'Primarily Undergraduate Institutions',
    'black'=>'Historically Black Colleges and Universities',
    'hispanic'=>'Hispanic Serving Institutions',
    'tribal'=>'Tribal Colleges and Universities',
    'lab'=>'National Laboratories',
    'industry'=>'Industry',
    'none'=>'Other',
    'total'=>'Total'
  );
  
  public function __construct(){ 
      parent::__construct();
  }
  
  public function title(){
    return "Collaborations";
  }
  
  public function data($count = true){
    $inst_ref = self::$field_aliases['collab_inst_ref'];
    $collab_dept = self::$field_aliases['collab_department_unit'];
    $inst_category = self::$field_aliases['inst_category'];
    $inst_type = self::$field_aliases['inst_type'];
    $participating = self::$field_aliases['participating'];
    $collab_user_status = self::$field_aliases['collab_user_status'];
    $collab_name = self::$field_aliases['collab_name'];
    $collab_ref = self::$field_aliases['collab_ref'];
    
    // Query #1: Get the collaborators and collaborator count
    $query = db_select('node', 'collab');
    $query->condition('collab.type', 'er_collaborators');
    $query->leftJoin("field_data_{$collab_dept}", 'dept', 'collab.nid = dept.entity_id');
    $query->innerJoin("field_data_{$inst_ref}", 'inst_ref', 'collab.nid = inst_ref.entity_id');
    $query->innerJoin("node", 'inst', "inst.nid = inst_ref.{$inst_ref}_target_id");
    $query->innerJoin("field_data_{$inst_category}", 'category', "inst.nid = category.entity_id");
    $query->innerJoin("field_data_{$inst_type}", 'type', "inst.nid = type.entity_id");
    $query->innerJoin("field_data_{$participating}", 'participating', "inst.nid = participating.entity_id");
    //$query->condition("participating.{$participating}_value", '0');//must be an external institution
    $query->innerJoin("field_data_{$collab_user_status}", 'status', "collab.nid = status.entity_id");
    $query->condition("status.{$collab_user_status}_value", 'Collaborator');//Person must be marked as a collaborator
    $query->innerJoin("field_data_{$collab_ref}", 'collab_ref', "collab.nid = collab_ref.entity_id");
    
    $inst_expression = "(CASE WHEN dept.{$collab_dept}_value IS NOT NULL THEN CONCAT(dept.{$collab_dept}_value, ' @ ', inst.title) ELSE inst.title END)";
    $query->addField('category', "{$inst_category}_value", 'category');
    $query->addField('type', "{$inst_type}_value", 'type');
    //$query->addField('status', "{$collab_user_status}_value", 'status');
   
    if ($count){
      $query->addExpression("COUNT(DISTINCT collab.nid)", 'collaborators');
      //$query->addExpression("COUNT(DISTINCT $inst_expression)", 'institutions');
      $query->groupBy("category");
      $query->groupBy("type");
    }else{
      $query->leftJoin("field_data_{$collab_name}", 'name', 'collab.nid = name.entity_id');
      $query->innerJoin("node", 'collaboration', "collaboration.nid = collab_ref.{$collab_ref}_target_id");
      $query->addField('name', "{$collab_name}_value", 'name');
      $query->addField("collab", 'nid', 'collaborator_nid');
      $query->addField("collaboration", 'nid', 'collaboration_nid');
      $query->addField("collaboration", 'title', 'collaboration_title');
      $query->addExpression($inst_expression, 'institution');
      $query->addField("inst", 'nid', 'institution_nid');
      //$query->groupBy('institution');
    }
    
    $data = array();
    $result = $query->execute();

    // populate $data with collaboration counts
    while ($record = $result->fetchAssoc()){
      $category = $record['category'];
      $type = $record['type'];
      unset($record['category']);
      unset($record['type']);
      if ($count){
        //$data[$category][$type] = $record;
        $data[$category][$type] = array();
        $data[$category][$type]['collaborators'] = $record['collaborators'];
      }else{
        //$institution = array(
        //  'nid'=>$record['institution_nid'],
        //  'title'=>$record['institution']
        //);

        $data['total']['total']['institutions'][$record['institution']] = $record['institution_nid'];
        $data['total'][$type]['institutions'][$record['institution']] = $record['institution_nid'];
        $data[$category][$type]['institutions'][$record['institution']] = $record['institution_nid'];
        $data['total']['total']['collaborators'][] = $record;
        $data['total'][$type]['collaborators'][] = $record;
        $data[$category][$type]['collaborators'][] = $record;
      }
    }

    // Now rerun query with additional condition that removes participating institutions
    $query->condition("participating.{$participating}_value", '0');//must be an external institution
    
    // and count up institutions if $count=true
    if ($count){
      $query->addExpression("COUNT(DISTINCT $inst_expression)", 'institutions');
    }

    $result = $query->execute();
    while ($record = $result->fetchAssoc()){
      $category = $record['category'];
      $type = $record['type'];
      if ($count){
        if ($record['institutions']) {
          $data[$category][$type]['institutions'] = $record['institutions'];
        }
        else {
          $data[$category][$type]['institutions'] = 0;
        }
      }
      else {
        $data['total']['total']['institutions_counted'][$record['institution']] = $record['institution_nid'];
        $data['total'][$type]['institutions_counted'][$record['institution']] = $record['institution_nid'];
        $data[$category][$type]['institutions_counted'][$record['institution']] = $record['institution_nid']; 
      }
    }
     
    return $data;
  }
  
  public function html(){
    if (!user_access(ER_DOWNLOAD_PERMISSION)) return '';//don't let anon see this stuff

    $data = $this->data(false);
    if (isset($_GET['debug'])){
      d($this->data(), 'counts');
      d($data, 'full data');
    }
    
    $ret = $this->download_link();
    //$ret .= print_r($data);
    foreach ($data as $category => $cat_data){
      $ret .= "<h1>".self::$categories[$category]."</h1>";
      foreach($cat_data as $inst_type=>$type_data){
        if (count($type_data) > 0){
          $ret .= "<fieldset><legend>".self::$types[$inst_type]."</legend>";
          $ret .= '<div style="float:left;margin-top:1em;width:45%">';
            $ret .= "<h3 style=\"text-align:center;\">Institutions</h3>";
            $ret .= "<ol>";
            if ($type_data['institutions_counted']) {
              foreach($type_data['institutions_counted'] as $title=>$nid){
                $ret .= "<li><strong>".l($title, 'node/'.$nid)."</strong></li>";
              }
            }
            else {
              $ret .= "(No non-participating institutions in this category)";
            }     
            $ret .= "</ol>";
          $ret .= '</div>';
          $ret .= '<div style="float:left;margin-top:1em;width:55%">';
            $ret .= "<h3 style=\"text-align:center;\">Collaborators</h3>";
            $ret .= "<ol>";
              foreach($type_data['collaborators'] as $k=>$v){
                $ret .= "<li><strong>".$v['name'].'</strong> from ' . l($v['institution'], 'node/'.$v['institution_nid']) . ' on '.l($v['collaboration_title'], 'node/'.$v['collaboration_nid'])."</li>";
              }
            $ret .= "</ol>";
          $ret .= '</div>';
          $ret .= '</fieldset>';
          $ret .= '<div style="clear: both;"></div>';
        }
      }      
    }
    return $ret;
  }
  
  public function write_excel(&$objPHPExcel, $sheet = 0){
    $data = $this->data();
    $worksheet = $objPHPExcel->setActiveSheetIndex($sheet);
    $base_row = 5;
    $row = $base_row;
    $category_pos = array_flip(array('academic','undergrad','black','hispanic','tribal','lab','industry','none'));
    
    foreach ($data as $category => $temp){
      $row = $category_pos[$category]+$base_row;
      //Jurisdictional
      $worksheet->setCellValue('B'.$row, isset($temp['local']["institutions"])?$temp['local']["institutions"]:0);
      $worksheet->setCellValue('C'.$row, isset($temp['local']["collaborators"])?$temp['local']["collaborators"]:0);
      //External-Domestic
      $worksheet->setCellValue('D'.$row, isset($temp['domestic']["institutions"])?$temp['domestic']["institutions"]:0);
      $worksheet->setCellValue('E'.$row, isset($temp['domestic']["collaborators"])?$temp['domestic']["collaborators"]:0);
      //External-Foreign
      $worksheet->setCellValue('F'.$row, isset($temp['foreign']["institutions"])?$temp['foreign']["institutions"]:0);
      $worksheet->setCellValue('G'.$row, isset($temp['foreign']["collaborators"])?$temp['foreign']["collaborators"]:0);
      //$row++;
    }
  }
  
  public function excel_file(){
    $objPHPExcel = PHPExcel_IOFactory::load(ER_MODULE_DIR."/static/files/Collaborations-template.xls");
    return $objPHPExcel;
  }

}
