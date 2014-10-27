<?php

/**
 * @file
 * Drush integration for webform_archive.
 *
 * drush webform-archive
 */

/**
 * Implement hook_drush_help().
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
 * Implement hook_drush_command().
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
    'aliases' => array('aravwf'),
    'options' => array(
      'date' => 'Unix timestamp for archiving of webforms, till this date webfrom submissions will be archived.',
      'webform' => 'Webfrom nid in comma separated format',
    ),
    'examples' => array(
      'drush aracwf' => 'Archive all active webform submissions',
      'drush aracwf --date=3122342' => 'Archive all webforms till 3122342',
      'drush aravwf --date=3122342 --webform=1234,1235' => 'Archive 1234, 1235 webforms till date 3122342',
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
 * @TODO Function which will archive all the active webform submissions.
 */

/**
 * @TODO Function which will archive all the closed webform submissions.
 */
