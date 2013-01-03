<?php

class er_handler_filter_view extends views_handler_filter {
  function admin_summary() { }
  function operator_form() { }
  function query() {
    $table = $this->ensure_my_table();
    $join = new views_join();
    $join->construct('er_user_notify', $this->table_alias, 'uid', 'uid');
    $this->query->ensure_table('er_user_notify', $this->relationship, $join);
    $join = new views_join();
    $join->construct('users', 'er_user_notify', 'uid', 'uid');
    $this->query->ensure_table('users', $this->relationship, $join);
    $this->query->add_where($this->options['group'], "er_user_notify.ignore = 1");
  }
}