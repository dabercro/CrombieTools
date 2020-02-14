<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

include 'include/ls.php';

function rev_name($a, $b) {

  $exp_a = explode('/', $a);
  $exp_b = explode('/', $b);

  if ($exp_a[1] == $exp_b[1])
    return ($exp_a[0] < $exp_b[0]) ? -1 : 1;

  return (strrev($a) < strrev($b)) ? -1 : 1;

}

function no_rev_name($a, $b) {

  $exp_a = explode('/', $a);
  $exp_b = explode('/', $b);

  if ($exp_a[1] == $exp_b[1])
    return ($exp_a[0] < $exp_b[0]) ? -1 : 1;

  return ($exp_a[1] < $exp_b[1]) ? -1 : 1;

}

$dirs = isset($_GET['dirs']) ? $_GET['dirs'] : array();
$share = isset($_GET['share']) ? $_GET['share'] : 'Nope';
$exprs = isset($_GET['expr']) ? $_GET['expr'] : '';
$raw_width = isset($_GET['width']) ? $_GET['width'] : '4';
$rev_name = isset($_GET['rev']) ? $_GET['rev'] : 'yes';

$regs = '/' . str_replace(' ', '|', $exprs) . '/';
    
$file_list = array();

foreach ($dirs as $directory) {

  if (!in_array($directory, $allDirs))
    continue;

  foreach(scandir($plotdir . $directory) as $file_name) {

    if (strpos($file_name, '.pdf') !== FALSE) {

      $file_sep = explode('.', $file_name);
      array_pop($file_sep);
      $file_base = implode('.', $file_sep);

      if ($exprs and !preg_match($regs, $file_base))
        continue;

      array_push($file_list, $directory . '/' . $file_base);

    }

  }

}

if ($rev_name == 'yes')
  uasort($file_list, 'rev_name');
else
  uasort($file_list, 'no_rev_name');

$checked = $dirs;
$width = 100.0/floatval($raw_width);

$joined_files = implode(',', $file_list);

include 'body.html';
?>
