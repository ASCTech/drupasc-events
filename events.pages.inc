<?php

/**
 * @file
 * Page callback file for the events module.
 */

/**
 * Menu callback; displays a Drupal page containing recent events entries of a given user.
 */
function events_page_user($account) {
  global $user;

  drupal_set_title($title = t("@name's events", array('@name' => $account->name)));

  $items = array();

  if (($account->uid == $user->uid) && user_access('create events entries')) {
    $items[] = l(t('Post new events entry.'), "node/add/events");
  }
  else if ($account->uid == $user->uid) {
    $items[] = t('You are not allowed to post a new events entry.');
  }

  $output = theme('item_list', $items);

  $result = pager_query(db_rewrite_sql("SELECT n.nid, n.sticky, n.created FROM {node} n WHERE n.type = 'events' AND n.uid = %d AND n.status = 1 ORDER BY n.sticky DESC, n.created DESC"), variable_get('default_nodes_main', 10), 0, NULL, $account->uid);
  $has_posts = FALSE;
  
  while ($node = db_fetch_object($result)) {
    $output .= node_view(node_load($node->nid), 1);
    $has_posts = TRUE;
  }
  
  if ($has_posts) {
    $output .= theme('pager', NULL, variable_get('default_nodes_main', 10));
  }
  else {
    if ($account->uid == $user->uid) {
      drupal_set_message(t('You have not created any events entries.'));
    }
    else {
      drupal_set_message(t('!author has not created any events entries.', array('!author' => theme('username', $account))));
    }
  }
  drupal_add_feed(url('events/'. $account->uid .'/feed'), t('RSS - !title', array('!title' => $title)));

  return $output;
}

/**
 * Menu callback; displays a Drupal page containing recent events entries of all users.
 */
function events_page_last() {
  global $user;

  $output = '';
  $items = array();

  if (user_access('edit own events')) {
    $items[] = l(t('Create new events entry.'), "node/add/events");
  }

  $output = theme('item_list', $items);

  $result = pager_query(db_rewrite_sql("SELECT n.nid, n.created FROM {node} n WHERE n.type = 'events' AND n.status = 1 ORDER BY n.sticky DESC, n.created DESC"), variable_get('default_nodes_main', 10));
  $has_posts = FALSE;

  while ($node = db_fetch_object($result)) {
    $output .= node_view(node_load($node->nid), 1);
    $has_posts = TRUE;
  }
  
  if ($has_posts) {
    $output .= theme('pager', NULL, variable_get('default_nodes_main', 10));
  }
  else {
    drupal_set_message(t('No events entries have been created.'));
  }
  drupal_add_feed(url('events/feed'), t('RSS - events'));

  return $output;
}

/**
 * Menu callback; displays an RSS feed containing recent events entries of a given user.
 */
function events_feed_user($account) {
  $result = db_query_range(db_rewrite_sql("SELECT n.nid, n.created FROM {node} n  WHERE n.type = 'events' AND n.uid = %d AND n.status = 1 ORDER BY n.created DESC"), $account->uid, 0, variable_get('feed_default_items', 10));
  $channel['title'] = $account->name ."'s events";
  $channel['link'] = url('events/'. $account->uid, array('absolute' => TRUE));

  $items = array();
  while ($row = db_fetch_object($result)) {
    $items[] = $row->nid;
  }
  node_feed($items, $channel);
}

/**
 * Menu callback; displays an RSS feed containing recent events entries of all users.
 */
function events_feed_last() {
  $result = db_query_range(db_rewrite_sql("SELECT n.nid, n.created FROM {node} n WHERE n.type = 'events' AND n.status = 1 ORDER BY n.created DESC"), 0, variable_get('feed_default_items', 10));
  $channel['title'] = variable_get('site_name', 'Drupal') .' events';
  $channel['link'] = url('events', array('absolute' => TRUE));

  $items = array();
  while ($row = db_fetch_object($result)) {
    $items[] = $row->nid;
  }

  node_feed($items, $channel);
}
