<?php
//d(get_defined_vars());
$name_map = array(
  'ari'=>'Academic Research Institutions (without Minority Serving Institution status)',
  'pui'=>'Primarily Undergraduate Institutions',
  'msi'=>'Minority Serving Institutions',
  'k12i'=>'K-12 Institutions',
  'oth'=>'Other',
  'tot'=>'total',
  
  'fac'=>'Faculty',
  'stu'=>'Students',
  'tec'=>'Teachers',
  'stud'=>'Students Reached Directly',
  'stut'=>'Students Reached via Teacher Training',
  
  'total'=>'Total',
  'male'=>'Male',
  'female'=>'Female',
  'und'=>'Undeclared Gender',
  'urm'=>'Underrepresented Minority',
);
$cols = array("ari_fac", "ari_stu", "pui_fac", "pui_stu", "msi_fac", "msi_stu", "k12i_tec", "k12i_stud", "k12i_stut", "oth", "tot");
$rows = array("total", "male", "female", "und", "urm");
$n = entity_metadata_wrapper('node', $node);
//d($node, '$node');
echo '<table class="er-external-engagements-view-table">';
  echo '<tr>';
    echo '<td></td>';//spacer
    //headers:
    foreach (array("ari", "pui", "msi") as $inst)
      echo '<td colspan="2" class="header">'.$name_map[$inst].'</td>';
    echo '<td colspan="3" class="header">'.$name_map["k12i"].'</td>';
    foreach (array("oth", "tot") as $inst)
      echo '<td rowspan="2" class="header">'.$name_map[$inst].'</td>';
  echo '</tr>';
  echo '<tr>';
    echo '<td></td>';//spacer
    //sub-headers:
    foreach (array("ari", "pui", "msi") as $inst)
      foreach (array("fac", "stu") as $type)
        echo '<td class="sub-header">'.$name_map[$type].'</td>';
    foreach (array("tec", "stud", "stut") as $type)
      echo '<td class="sub-header">'.$name_map[$type].'</td>';
  echo '</tr>';
  foreach ($rows as $attr){
    echo '<tr>';
      echo '<td class="sub-header">'.$name_map[$attr].'</td>';
      foreach ($cols as $inst){
        $field_name = 'field_er_'.$inst.'_'.$attr;
        $value = $n->$field_name->value();
        echo '<td class="table-'.$field_name.'">'.$value.'</td>';
      }
    echo '</tr>';
  }
echo '</table>';