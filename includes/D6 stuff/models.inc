<?php
/*************
 * er_model class
 * 
 * This class defines methods used to retrieve the node counts (used to populate the accomplishments table),
 *  and it is also used to generate the excel downloads which come off of each content type.
 * There are many interconnected things going on with this system, and it is a bit of a kludge at the moment.
 * This class defines the "base case", assuming the content type is relatively simple, this class er_should handle everything.
 * It assumes that the date field will be named something along the lines of [content_type]_date.
 *
 * Should a content type need to break away from the "base case" for such reasons as; the date field is named improperly, or is contained in another -- related node, etc,
 * you may need to extend the model class and override the appropriate methods in order to customize it to your needs. This behaves as a type of "hook" system, but is really just OOP in action.
 *
 * See below for the various extensions of this class.
 */
class er_model{
  protected $q, $t, $rev, $n, $debug = false, $excel = false, $options = Array(), $front_fields = Array(), $exclude_fields = Array();
  public		$type, $counts = Array(), $data = Array(), $ranges = Array();
  
  public function __construct($type, $ranges = null, $debug = false){
    $this->debug = $debug;
        $this->type = $this->id = $type; //need id for collaboration case
    $this->ranges = $ranges;
    $this->front_fields = array('nid', 'title');
    //$this->excel = $excel;
        //$this->set_values($ranges);
    //$this->select_fields();
  }
  
  //gets called every time a new request is started. Initializes the tables, and sets up some of the logistics
  protected function initialize(){
    //$this->q = new EntityFieldQuery();
    //$this->q->entityCondition('entity_type', 'node')
    //        ->entityCondition('bundle', $this->type)
    //        //->propertyCondition('status', 1)
    //        ->fieldCondition('field_er_user_reference', 'uid', $account->uid, '=');
    //$results = $query->execute();
		$this->q = db_select('node', 'n')->condition('n.type', 'er_engagement');
		//$this->q->addExpression('COUNT(*)', 'count');
		
		//$field_table_alias = $this->joinField($query, 'n', $field);
		////$query->leftJoin('field_data_field_er_cal_event_reference', 'ref_field', "n.nid = ref_field.entity_id");
		////$query->innerJoin('field_data_field_er_event_date', 'date_field', "ref_field.field_er_cal_event_reference_nid = date_field.entity_id");
		////$query->fields('date_field');
		
    //$this->q = new Query();
    //$this->t = new Table("content_type_{$this->type}", 't');
    //$this->t->field('nid');//->count_distinct('nid');
    //$this->n = new Table('{node}', 'n');
    //$this->n->field('nid');
    //$this->q = new Query($this->t);
    //$this->q->join($this->n);
    //if ($this->excel){
    //  //$this->t->select('nid', array('distinct'=>True))->group('nid');
    //  $this->t->select('nid')->group('nid');
    //  $this->select_fields();
    //}else{
    //  $this->n->field('nid')->field('status')->where('status', 1);
    //  //$this->t->count_distinct($this->constrain);
    //  //$this->count_distinct();
    //  $field = $this->distinct_field();
    //  $field->parent_table->count_distinct($field->name);
    //}
    //$this->constraints();
    if (!isset($this->name)) $this->name = 'name';//$this->t->get_name();
  }
  
  //This method sets some field with the name 'date'. This is used later by the system in order to set date ranges.
  //It must return the table which initializes the date field (it's not always $t).
	protected function date_field(){
		$field_name = 'field_'.$this->type.'_date';
		$this->q->leftJoin('field_data_'.$field_name, 'date_field', "n.nid = date_field.entity_id");
		return $field_name;
		//return $this->t->field("field_{$this->type}_date", 'date');
	}
  
  //extra "where" clause info
  protected function constraints(){
    //d('old constraints');
    if (!$this->excel){
      $this->n->field('status')->where('status', 1);
    }
    //intentionally empty
  }
  
  //which field should be used for grouping
  //protected function distinct_field(){
  //  return $this->t->get_field('nid');
  //}
	
	protected function count_field(){
		$this->q->addExpression('COUNT(*)', 'count');
	}
  
  /*
   *
   * Beginning of COUNT methods
   *
   */
  
  //get the counts used in the accomplishments table
    public function set_counts($ranges = Array()){
    $this->excel = false;
    if (isset($ranges)) $this->ranges = $ranges;
        if (!isset($this->path)) $this->path = str_replace("_", "-", strtolower($this->type));
    if (!isset($this->view)) $this->view = $this->path."-view";
        //$this->set_total();
        $this->set_ranges($this->ranges);
    return $this;
    }
  
//  //get the counts used in the accompishments table, this is only sets the "inception to today" data.
//  protected function set_total(){
//        $this->initialize();
//    $field = $this->distinct_field();
//    $field->parent_table->count_distinct($field->name);
//    $this->counts["total"] = $this->q->getObject($this->debug)->count;
//    }
  
  protected function set_range($r1, $r2, $return = false){
		//$this->initialize();
		$field_name = $this->date_field();
		//$from_opt = $this->options+array('fn2'=>'date', 'op'=>'>=', 'or_is_null'=>true);
		//$to_opt = $this->options+array('fn2'=>'date', 'op'=>'<=');
		$y1 = date(DATE_ATOM, $r1);
		$y2 = date(DATE_ATOM, $r2);
		//Super ghetto but neccessary. We need to search from day 00 instead of day 01 in order to get all of the nodes.
		// this happens when date field only accepts month, and not day granularity.
		if (date('j', $r1) == '1')
			$y1 = str_replace('-01T', '-00T', $y1);
		//$date = $date2 = 'date';
		//if ($table->field_isset('date2')) //if there is a "to" date, use it.
		//	$date2 = 'date2';
		
		$this->q->condition("date_field.{$field_name}_value", $y1, '>=');
		$this->q->condition("date_field.{$field_name}_value2", $y2, '<=');
		
		//if ($return){
		//	return array( $table->where($date2, $y1, $from_opt+array('return'=>true)), $table->where($date, $y2, $to_opt+array('return'=>true)) );
		//}else{
		//	$table->where($date2, $y1, $from_opt)->where($date, $y2, $to_opt);
		//}
	}
    
  public function set_ranges($ranges){
    $this->excel = false;
    //set the total counts
    $this->initialize();
		$this->count_field();
    //$field = $this->distinct_field();
    //$field->parent_table->count_distinct($field->name);
    //$this->constraints();
    //if ($_GET['dbg'] == $this->type)
    //  drupal_set_message($this->q->inEnglish());
		//$result = $this->q->execute();
    $this->counts["total"] = intval($this->q->execute()->fetchObject()->count);
    //d($this);
    //do each of the ranges
		foreach ($ranges as $k=>$v){
			$this->initialize();
			$this->count_field();
			//$this->n->field('status')->where('status', 1); //moved to constraints
			//$field = $this->distinct_field();
			//$field->parent_table->count_distinct($field->name);
			//$this->constraints();
			$this->set_range($v[0], $v[1]);
			$this->counts["{$v[0]} - {$v[1]}"] = intval($this->q->execute()->fetchObject()->count);
			//if ($_GET['dbg'] == $this->type)
			//	drupal_set_message($this->q->inEnglish());
		}
    return $this;
  }
  
  /*
   *
   * Beginning of EXCEL methods
   *
   */
  
    //get all of the data needed for the excel sheets
  public function set_excel_data(){
    if ($this->debug == false && ER_DEBUG_EXCEL) $this->debug = true;
    $this->excel = true;
    $this->initialize();
    //$this->t->select('nid', array('distinct'=>True))->group('nid');
    $this->t->select('nid')->group('nid');
    $this->select_fields();
    $this->constraints();
    //user selected a single date range, restrict the data to that range:
    if (count($this->ranges) == 1) $this->set_range($this->ranges[0][0], $this->ranges[0][1]);
    $this->data = $this->q->getData($this->debug);
    //$this->fix_excel_data();
    
    //$this->order = $this->field_order(array_keys($this->q->getSelectFields()));//$this->q->fields
    $this->order = $this->field_order(array_keys($this->labels));//$this->q->fields
    $this->post_process();
    if ($this->debug) d($this, 'output');
    return $this;
  }
  
  protected function post_process(){
    //intentionally empty
  }
  
  protected function select_fields(){
    //$this->initialize();
    $this->labels['nid'] = "Node ID";
    if (($label = $this->field_label('title')) !== NULL){
      $this->n->field('title')->select('title');
      $this->labels['title'] = $label;
    }
    if (($label = $this->field_label('status')) !== NULL){
      $this->n->field('status')->select('status')->order('status');
      $this->labels['status'] = $label;
    }
    if ($this->t->has_body() && ($label = $this->field_label('body')) !== NULL){
      $this->rev = new Table('{node_revisions}', 'rev');
      $this->rev->field('nid');
      $this->q->join($this->rev);
      $this->rev->field('body')->select('body');
      $this->labels['body'] = $label;
    }
    $this->select_table_fields();
  }
  
  protected function select_table_fields($t = null, $options = null){
    if ($options == null) $options = array('fn'=>'GROUP_CONCAT', 'distinct'=>True);
    if (!$t) $t = $this->t;
    $fields = $t->get_fields();
    foreach ($fields as $field_name => $field){
      if ($field['type'] != 'markup' && ($label = $this->field_label($field_name, $field)) !== NULL){//markup is just used for display, and there is no data associated with it in the db.
        $this->labels[$field_name] = $label;
        $t->field($field_name)->select($field_name, $options);
        if ($t->date_is_multi($field_name)){
          $this->labels["{$field_name}2"] = $this->field_label($field_name, $field, true); //"To (".$this->labels[$field_name].")";
        }
      }
    }
  }
  
  //protected function count_distinct(){
  //  $this->t->count_distinct('nid');
  //}
  /*
   *
   * Beginning of TIMELINE methods
   *
   */
  public function set_timeline_data($conf, $uid = null){
    $this->conf = $conf;
    //d($conf);
    $this->set_minmax();
    $this->set_timeline_block();
    $this->set_timeline_info();
  }
  
  protected function set_uid(){
    if ($this->conf['show-all'] != 1){
      $uid = arg(1);
      if (empty($uid)){
        global $user;
        $uid = $user->uid;
      }
      //d(arg());
      $this->user_reference_field()->where('ur', $uid);
    }
  }
  
  protected function user_reference_field(){
    return $this->t->field('field_er_user_reference', 'ur');
  }
  
  protected function set_minmax(){
    $getmin = true;
    if ($this->conf['date']!==null){
      $getmin = false;
      $minDate = $this->conf['date'];
    }
    $getmax = true;
    if ($this->conf['date2']!==null){
      $getmax = false;
      $maxDate = $this->conf['date2'];
    }
    if ($getmin || $getmax){
      $this->initialize();
      $this->set_uid();
      $table = $this->date_field();
      $multi = $table->date_is_multi('date');
      $this->constraints();
      if ($getmin) $this->t->select('date', array('fn'=>array('MIN')), 'min');
      if ($getmax) $this->t->select('date'.($multi?'2':''), array('fn'=>array('MAX')), 'max');
      $data = $this->q->getArray($this->debug);
      if ($getmin) $minDate = $data['min'];
      if ($getmax) $maxDate = $data['max'];
    }
    $now = time();
    $minDate = strtotime($minDate);
    $maxDate = strtotime($maxDate);
    if ($maxDate < $now) $maxDate = $now;
      
    $this->block->info['minDate'] = $minDate;
    $this->block->info['maxDate'] = $maxDate;
    $this->block->info['multi'] = $multi;
  }
  
  protected function set_timeline_block(){
    $this->initialize();
    $this->set_uid();
    $this->date_field();
    $this->t->select('date')->select('nid');
    $this->set_timeline_fields();
    $this->set_timeline_order();
    $this->constraints();
    $data = $this->q->getData($this->debug);
    
    foreach($data as $k=>$v){
      $data[$k]['ulink'] = url('user/'.$data[$k]['uid']);
      $data[$k]['nlink'] = url('node/'.$data[$k]['nid']);
      $data[$k]['date'] = $data[$k]['date']?strtotime($data[$k]['date']):null;
      $data[$k]['date2'] = $data[$k]['date2']?strtotime($data[$k]['date2']):null;
      //cant use unix timestamp sql function because it doesn't work on dates where day == 00
    }
    $this->block->data = $data;
  }
  
  protected function set_timeline_order(){
    $this->t->order('date', array(), 'DESC');
  }
  
  protected function set_timeline_fields(){
    $this->n->field('title')->select('title');
    //intentionally empty
  }
  
  protected function timeline_tooltip_title(){
    return '<a href="{{nlink}}">{{title}}</a>';
  }
  
  protected function timeline_tooltip_body(){
    return '';
  }
  
  protected function timeline_pill_label(){
    return '{{nid}}';
  }
  
  protected function timeline_pill_color(){
    return 'nid';
  }
  
  protected function set_timeline_info(){
    $tpl_types = array(
      "tooltip-title"=>'timeline_tooltip_title',
      "tooltip-body"=>'timeline_tooltip_body',
      "pill-label"=>'timeline_pill_label',
      "pill-color"=>'timeline_pill_color'
    );
    foreach ($tpl_types as $c=>$fn){
      $this->block->info[$c]= $this->conf[$c]=='<none>'?'':
                  $this->conf[$c]?$this->conf[$c]:
                  $this->$fn();
    }
    $this->block->info['name'] = $this->name;
    $this->block->info['type'] = $this->id;
    //$block->info["tooltip-title"] = $conf["tooltip-title"]?$conf["tooltip-title"]:$this->timeline_tooltip_title();
  }
    
  /*
   *
   * Beginning of FORMAT/HELPER methods
   *
   */
  
  public function field_info($field_name){
    return $this->t->field_info($field_name);
  }
  
  public function field_type($field_name){
    $field = $this->t->field_info($field_name);
    return $field['type'];
  }
  
  public function field_module($field_name){
    $field = $this->t->field_info($field_name);
    return $field['module'];
  }
  
  //formats a field by looking at it's data type assigned in the drupal system
  public function format_field($field_name, $raw_value){
    $values = explode(',', $raw_value);
    $field = $this->t->field_info($field_name);
    $ret = '';
    $first = True;
    //d($field, $field_name);
    if ($field_name == 'status')
      return $raw_value?'Published':'Draft';
    foreach ($values as $k=>$value){
      if (!$first)
        $ret .= ', ';
      $first = False;
      if($field['widget']['module'] == 'optionwidgets' && $field['type'] == 'number_integer'){
        if ($value == ''){
          $ret .= 'NULL';
        }else{
          $allowed_values = allowed_values_array($field['allowed_values']);
          $ret .= $allowed_values[$value];
        }
      }else if($field['module'] == 'nodereference'){
        $ret .= get_node_title($value)."[$value]";
      }else if($field['module'] == 'userreference'){
        $ret .= strip_tags(theme('username',user_load($value)));
      }else if($field['module'] == 'number'){
        $ret .= number_format($value);
      }else if($field['module'] == 'text'){
        $ret .= strip_tags($value, '<a>');
      }else{
        $ret .= $value;
      }
      //these are deprecated:
      //}else if ($field['prefix'] == '$' && $field['type'] == 'number_float'){
      //  $ret .= "$".number_format($value);
      //}else if($field['module'] == 'date'){
      //  $ret .= $value?date('Y-n-j', strtotime($value)):'';
      
    }
    //d($raw_value, $field_name.'_raw');
    //d($ret, $field_name.'_ret');
    return $ret;
  }
  
  //if function returns NULL for a given field, the field will not be requested from the database (through the select statement).
  protected function field_label($field_name, $field = null, $toDate = false){
    //d($field_name, $test = ucwords($field_name));
    if ($field){
      if ($toDate) return 'To ('.$field['widget']['label'].')';
      return $field['widget']['label'];
    }else{
      return ucwords($field_name);
    }
  }
  
  //pretty poor way of doing this...
  protected function field_order(&$keys){
    sort($keys, SORT_STRING);
    $keys = array_reverse($keys);
    return array_unique(array_merge($this->front_fields, $keys)); //hehe nifty trick
  }
}

class er_inst_model extends er_model{
  public function __construct($type, $ranges = null){
    parent::__construct("er_institution", $ranges);
    $this->name = "Institutions";
    $this->options = Array('fn'=>"from_unixtime");
  }
    protected function date_field(){
        return $this->n->field("created", 'date');
    }
}

class er_ee_model extends er_model{
  private $c;
  public function __construct($type, $ranges = null){
    parent::__construct($type, $ranges);
    $this->front_fields = array('title', 'field_er_event_date', 'field_er_event_date2');
  }
  protected function initialize(){
    parent::initialize();
    $this->c = new Table("content_type_er_event", 'c');
    $this->c->field('nid', 'event_nid');
    $this->t->field("field_er_cal_event_reference", 'event_nid');
    //$this->q->addTable($this->c)->link($this->t, $this->c);//link defaults to linking by 'nid'
    $this->q->join($this->c, $this->t, 'event_nid');
  }
    protected function date_field(){
//        //need to do this because the external engagement's date information is in the event
//    //only need to use this condition when viewing the date view and not the reporting year view.
//    $this->c = new Table("content_type_er_event", 'c');
//    $this->c  ->field("field_er_event_date", 'date')
//          ->field('nid');
//    $this->t->field("field_er_cal_event_reference", 'nid');
//    //$this->q->addTable($this->c)->link($this->t, $this->c);//link defaults to linking by 'nid'
//    $this->q->join($this->c);
        return $this->c->field("field_er_event_date", 'date');
    }
  
  protected function field_label($field_name, $field = null, $toDate = false){
    //d($field_name);
    preg_match('/field_er_([a-z1-9]{3,4})_([a-z]+)_?([a-z]+)?/', $field_name, $matches);
    if ($matches) //the renames the various fields simply using their machine name.
      return implode('-', array_slice($matches, 1));
    return parent::field_label($field_name, $field, $toDate);
  }
  
  protected function select_fields(){
    parent::select_fields();
    $this->labels['field_er_event_date'] = 'Event Date';
    $this->labels['field_er_event_date2'] = 'Event To-Date';
    $this->c->field("field_er_event_date")->select('field_er_event_date');
    //d($this->c);
  }
}

class er_user_model extends er_model{
  private $e;
  public function __construct($type, $ranges = null){
    parent::__construct($type, $ranges);
    //$this->debug = true;
    $this->name = "Participants";
    $this->view = 'participants';
    //$this->front_fields = array("field_er_lname", "field_er_fname", "field_er_institution", "field_er_institution_other", "field_er_teams", "field_er_team_other", "field_er_supervisor");
    $this->front_fields = array("field_er_lname", "field_er_fname", "field_er_involvement", "field_er_teams", "field_er_institution", "field_er_gender", "field_er_supervisor");
  }
    protected function initialize(){
    parent::initialize();
    $this->t->field("field_er_involvement", 'involvement')->order('involvement')
        ->field("field_er_fname", 'first')->order('first')
        ->field("field_er_lname", 'last')->order('last');
    $this->n->field('uid');
    $this->e = new Table("content_type_er_user_event", 'e');
    //$this->select_table_fields($this->e);
    $this->e->field("field_er_event_user", 'uid');
    $on = array();
    if ($this->excel && count($this->ranges) != 1){
      $this->date_field();
      $now = time();
      $on = $this->set_range($now, $now, true);
    }
    $this->q->join($this->e, $this->n, 'uid', 'LEFT', $on);
    if (!$this->excel || count($this->ranges) == 1)
      $this->e->field('field_er_user_event_type', 'type')->where('type', 'Participating');
  }
  protected function select_fields(){
    parent::select_fields();
    //$this->select_table_fields($this->e);
    $opt = array('fn'=>'GROUP_CONCAT', 'distinct'=>True);
    //$this->e->field('field_er_user_event_date', 'event_date')->select('event_date', $opt)->select('event_date2', $opt);
    if (count($this->ranges) != 1){
      $this->e->field('field_er_user_event_type', 'event_type')->select('event_type', $opt);
      $this->labels['event_type'] = "Current Events";
    }/*else{
      $this->labels['event_date'] = "Participation Start";
      $this->labels['event_date2'] = "Participation End";
    }*/
  }
    protected function date_field(){
        return $this->e->field("field_er_user_event_date", 'date');
    }
  protected function distinct_field(){
    return $this->n->field('uid')->get_field('uid');
  }
  //protected function count_distinct(){
  //  d('count_distinct:: TODO:: need to use this field in excel grouping');
  //  $this->n->field('uid')->count_distinct('uid');
  //}
  protected function field_label($field_name, $field = null, $toDate = false){
    //d($field);
    if ($field){
      if ($field['field_name'] == 'field_er_rcr_online')
        return 'RCR Online';
      else if ($field['field_name'] == 'field_er_paid')
        return 'Paid by Grant';
      else if ($field['field_name'] == 'field_er_160hours')
        return '>160h';
    }
    //else if ($field['field_name'] == "field_er_involvement")
    //  return 'Project Role';
    if ($field_name != 'title' && $field_name != 'body')
      return parent::field_label($field_name, $field, $toDate);
  }
}

class er_cnp_model extends er_model{
  private $p;
  public function __construct($type, $ranges = null){
    parent::__construct($type, $ranges);
    $this->name = "Current and Pending Support *";
    //$this->debug = true;
  }
  //protected function select_fields(){
  //  parent::select_fields();
  //  $this->p = new Table("content_type_er_paid", 'p');
  //  //$this->t->field('nid', 'join');
  //  //$this->p->field('field_er_paid_node', 'join');
  //  //
  //  $this->select_table_fields($this->p, array());
  //  $this->q->join($this->p, $this->t, array('field_er_paid_node', 'nid'), 'LEFT');
  //}
    
  protected function post_process(){
    //$this->labels['paid_nodes'] = 'Paid Nodes';
    $this->order[] = 'paid_nodes';
    foreach ($this->data as $k=>$v){
      $paid = new er_paid_model('er_paid', $this->ranges, $v['nid']);
      $paid->set_excel_data();
      $this->data[$k]['paid_nodes'] = $paid;//array('nid'=>'more data', 'title'=>'and more');
    }
  }
  //we want to see everything.
//    protected function constraints(){
//    //adding this exception because the proposals ("current and pending support") count shouldn't include "expired" or "denied" types
//    $this->t  ->field("field_er_proposal_status", 'type')
//          ->where('type', array("Submitted", "Awarded", "Pending"));
//  }
}

//this one is used with the cnp_model above.
class er_paid_model extends er_model{
  private $p, /*$u, $un,*/ $nid;
  
  public function __construct($type, $ranges, $nid = null){
    parent::__construct('er_paid', $ranges);
    //$this->debug = true;
    $this->nid = $nid;
    $this->front_fields = array('field_er_funding', 'field_er_funded_effort');
  }
  protected function initialize(){
    parent::initialize();
    //$this->u = new Table('content_type_er_user_profile', 'u');
    //$this->un = new Table('{node}', 'un');
    $this->p = new Table('content_type_er_proposal', 'p');
    $this->p->field('nid', 'proposal_nid');
    $this->t->field('field_er_paid_node', 'proposal_nid');
    if ($this->nid)
      $this->p->where('proposal_nid', $this->nid);
    $this->q->join($this->p, $this->t, 'proposal_nid');
  }
    protected function date_field(){
        return $this->p->field("field_er_proposal_date", 'date');
    }
  protected function select_fields(){
    parent::select_fields();
    $this->labels['uid'] = 'User ID';
    $this->n->field('uid')->select('uid');
  }
  protected function field_label($field_name, $field = null, $toDate = false){
    if (($field && $field['field_name'] != 'field_er_paid_node') || $field_name == 'title'){//i dont want to see that field.
      return parent::field_label($field_name, $field, $toDate);
    }
    
  }
}

class er_proj_model extends er_model{
  public function __construct($type, $ranges = null){
    parent::__construct($type, $ranges);
    //$this->debug = true;
  }
    protected function date_field(){
        return $this->t->field("field_er_start_date", 'date');
    }
  protected function field_label($field_name, $field = null, $toDate = false){
    if ($field['field_name'] == 'field_er_start_date' && $toDate)
      return 'Proj End Date';
    return parent::field_label($field_name, $field, $toDate);
  }
}

class er_collab_base_model extends er_model{
  private $ext, $i;
  public function __construct($type, $ranges, $ext){
    parent::__construct('er_collaborator', $ranges);
    $this->id = $type;
    $this->ext = $ext;
    //$this->debug = true;
  }
  protected function initialize(){
    parent::initialize();
    $this->i = new Table("content_type_er_institution", "i");
    $this->t->field('field_er_collab_inst', 'inst_nid');
    $this->i  ->field('nid', 'inst_nid')
          ->field('field_er_inst_ext', 'ext');
    //$this->q->addTable($this->i)->link($this->i, $this->t, 'inst_nid');
    $this->q->join($this->i, $this->t, 'inst_nid');
  }
    protected function constraints(){
    $this->i->where('ext', $this->ext);
    parent::constraints();
  }
    protected function date_field(){
        return $this->t->field("field_er_collab_date", 'date');
    }
}

class er_partner_model extends er_collab_base_model{
  public function __construct($type, $ranges = null){
    parent::__construct($type, $ranges, 0);
    $this->name = 'Partnerships';
    $this->view = 'er-partnership-view';
  }
}

class er_collab_model extends er_collab_base_model{
  public function __construct($type, $ranges = null){
    parent::__construct($type, $ranges, 1);
    $this->name = 'Collaborations';
    $this->view = 'er-collaboration-view';
  }
}

//class er_partner_model extends er_model{
//  public $i, $ext;
//  public function __construct($type, $ranges = null){
//    parent::__construct('er_collaborator', $ranges);
//    $this->id = $type;
//    $this->ext = 0;
//    $this->name = 'Partnerships';
//    $this->view = 'er-partnership-view';
//    //$this->debug = true;
//  }
//  protected function initialize(){
//    parent::initialize();
//    $this->i = new Table("content_type_er_institution", "i");
//    $this->t->field('field_er_collab_inst', 'inst_nid');
//    $this->i  ->field('nid', 'inst_nid')
//          ->field('field_er_inst_ext', 'ext');
//    //$this->q->addTable($this->i)->link($this->i, $this->t, 'inst_nid');
//    $this->q->join($this->i, $this->t, 'inst_nid');
//    d($this->q);
//    d('not working...?');
//  }
//    protected function constraints(){
//    $this->i->where('ext', $this->ext);
//    //d($this->ext, 'constraints');
//    //d(debug_backtrace());
//    parent::constraints();
//  }
//    protected function date_field(){
//        return $this->t->field("field_er_collab_date", 'date');
//    }
//}
//
//class er_collab_model extends er_model{
//  public $i, $ext;
//  public function __construct($type, $ranges = null){
//    parent::__construct('er_collaborator', $ranges);
//    $this->id = $type;
//    $this->ext = 1;
//    $this->name = 'Collaborations';
//    $this->view = 'er-collaboration-view';
//    //$this->debug = true;
//  }
//  protected function initialize(){
//    parent::initialize();
//    $this->i = new Table("content_type_er_institution", "i");
//    $this->t->field('field_er_collab_inst', 'inst_nid');
//    $this->i  ->field('nid', 'inst_nid')
//          ->field('field_er_inst_ext', 'ext');
//    //$this->q->addTable($this->i)->link($this->i, $this->t, 'inst_nid');
//    $this->q->join($this->i, $this->t, 'inst_nid');
//  }
//    protected function constraints(){
//    $this->i->where('ext', $this->ext);
//    //d($this->ext, 'constraints');
//    //d(debug_backtrace());
//    parent::constraints();
//  }
//    protected function date_field(){
//        return $this->t->field("field_er_collab_date", 'date');
//    }
//}

function er_content_types($type = null){
  static $types = array(
    "er_project" => 'er_proj_model',
    "er_collaborator_1" => 'er_collab_model',
    "er_collaborator_0" => 'er_partner_model',
    "er_publication" => 'er_model',
    "er_proposal" => 'er_cnp_model',
    "er_patent" => 'er_model',
    "er_internet" => 'er_model',
    "er_poster" => 'er_model',
    "er_presentation" => 'er_model',
    "er_book" => 'er_model',
    "er_other_product" => 'er_model',
    "er_event" => 'er_model',
    "er_engagement" => 'er_ee_model',
    "er_institution" => 'er_inst_model',
    "er_user_profile" => 'er_user_model',
    //"er_user_event"=> 
    //"er_paid" => er_paid_model
  );
  if ($type !== null){
    if ($class = $types[$type])
      return $class;
  }else
    return $types;
}
