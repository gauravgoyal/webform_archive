<?php

/**
 * @file
 * Drush integration for webform_archive.
 *
 * drush webform-archive
 */

/**
 * Implements hook_drush_help().
 */
function webform_archive_drush_help($section) {
  switch ($section) {
    case 'meta:webfrom_archive:title':
      return dt('Webfrom archive commands');

    case 'drush:ard-webforms-details':
      return dt('Show the list of archived webforms.');

    case 'drush:ar-active-webforms':
      $help = dt('Archive all the active webform submissions till today.');
      $help .= dt('If no webform is spacified all active webforms will be archived till today.');
      return $help;

    case 'drush:ar-closed-webforms':
      $help = dt('Archive all the closed webform submissions.');
      $help .= dt('If no webform is spacified all closed webforms will be archived.');
      return $help;
  }
}

/**
 * Implements hook_drush_command().
 */
function webform_archive_drush_command() {
  $items = array();
  $items['ard-webforms-details'] = array(
    'drupal dependencies' => array('webform_archive'),
    'callback' => 'webform_archive_archived_webform_details',
    'description' => 'Get the list of archived webforms',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'aliases' => array('ardwfd'),
  );
  $items['ar-active-webform'] = array(
    'drupal dependencies' => array('webform_archive'),
    'callback' => 'webform_archive_archive_active_webform',
    'description' => 'Archive active webforms submission',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'aliases' => array('aracwf'),
    'options' => array(
      'date' => 'Unix timestamp for archiving of webforms, till this date webfrom submissions will be archived.',
      'webform' => 'Webfrom nid in comma separated format',
    ),
    'examples' => array(
      'drush aracwf' => 'Archive all active webform submissions',
      'drush aracwf --date=3122342' => 'Archive all webforms till 3122342',
      'drush aracwf --date=3122342 --webform=1234,1235' => 'Archive 1234, 1235 webforms till date 3122342',
    ),
  );
  $items['ar-closed-webform'] = array(
    'drupal dependencies' => array('webform_archive'),
    'callback' => 'webform_archive_archive_closed_webform',
    'description' => 'Archive all closed webform submissions',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'aliases' => array('arcdwf'),
    'options' => array(
      'webform' => 'Webfrom nid in comma separated format',
    ),
    'examples' => array(
      'drush arcdwf' => 'Archive all active webform submissions',
      'drush arcdwf --webform=1234,1235' => 'Archive 1234, 1235 webforms',
    ),
  );
  return $items;
}

/**
 * List out all the archived webforms.
 */
function webform_archive_archived_webform_details() {
  $archived_webforms = webform_archive_get_archived_webforms();
  if (!empty($archived_webforms)) {
    // Set the table headers.
    $header = array(
      dt('NID'),
      dt('Archived till'),
    );
    $rows = array();
    foreach ($archived_webforms as $archived_webform) {
      $row = array();
      $row[] = $archived_webform->nid;
      $row[] = date('d-M-Y', $archived_webform->archived);
      $rows[] = $row;
    }
    array_unshift($rows, $header);
    drush_print_table($rows, TRUE);
  }
  else {
    drush_print('No Webforms Submissions are Archived.');
  }
}

/**
 * Archive all the active webform submissions.
 */
function webform_archive_archive_active_webform() {
  // Get the date argument.
  $archived_date_timepstamp = drush_get_option_list('date');

  // Get the webform arguments.
  $webform_nids = drush_get_option_list('webform');
  $text = '';
  if (empty($archived_date_timepstamp) && empty($webform_nids)) {
    $text = dt('All the webform submissions will be archived till date');
  }
  else {
    if (empty($archived_date_timepstamp)) {
      $webform_nids_str = implode(',', $webform_nids);
      $text = dt('Submissions of given webforms (@nids) will be Archived till date', array('@nids' => $webform_nids_str));
      $archived_date_timepstamp = REQUEST_TIME;
    }
    else {
      if (empty($webform_nids)) {
        $archived_date = date('d M Y, H : i', array_pop($archived_date_timepstamp));
        $text = dt('All webform submissions will be archived till given @date', array('@date' => $archived_date));
      }
    }
  }

  $archive = TRUE;
  if (!empty($text)) {
    if (!drush_confirm($text)) {
      $archive = FALSE;
    }
  }

  if ($archive) {
    webform_archive_start_archiving($webform_nids, $archived_date_timepstamp);
  }
}

/**
 * Starts batch process in the background.
 *
 * @param array $webform_nids
 *   Nid(s) of webform to archive.
 * @param int $archived_date_timepstamp
 *   Unix timestamp for archiving.
 */
function webform_archive_start_archiving($webform_nids, $archived_date_timepstamp) {
  $nid_not_exists = array();
  foreach ($webform_nids as $nid) {
    if (is_object(node_load($nid))) {
      continue;
    }
    $nid_not_exists[] = $nid;
  }

  if (!empty($nid_not_exists)) {
    $webform_nids = array_diff($webform_nids, $nid_not_exists);
    $nid_not_exists = implode(',', $nid_not_exists);
    drush_print(dt('Webform nid(s) @nid does not exists, so these will not be archived', array('@nid' => $nid_not_exists)));
  }

  $operations = array(
    array('webform_archive_batch_process_start',
      array($webform_nids, $archived_date_timepstamp)),
  );
  $batch = array(
    'title' => t('Archive Webform Submissions'),
    'operations' => $operations,
    'file' => drupal_get_path('module', 'webform_archive') . '/includes/webform_archive.batch.inc',
    'finished' => 'webform_archive_batch_finished',
    'init_message' => t('Archive Batch is starting.'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Archive Batch has encountered an error.'),
  );
  batch_set($batch);
  // Start processing the batch operations.
  drush_backend_batch_process();
}

/**
 * @TODO Function which will archive all the closed webform submissions.
 */
