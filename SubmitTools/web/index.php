<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
mysqli_report(MYSQLI_REPORT_STRICT);

$conn = new mysqli('t3serv015.mit.edu', 'submit', 'submitter', 'submit_queue');

if (isset($_GET['id'])) {
  // Connect to MySQL database

  $stmt = $conn->prepare('SELECT exe, input_dir, output_dir, input_files, output_file, scram_arch, base, cmssw comments FROM queue WHERE id=?');
  $stmt->bind_param('s', $_GET['id']);
  $stmt->execute();
  $stmt->bind_result($exe, $in_dir, $out_dir, $in_file, $out_file, $arch, $base, $cmssw);
  $stmt->fetch();
  $stmt->close();

  $result = array(
    'exe' => $exe,
    'input_dir' => $in_dir,
    'output_dir' => $out_dir,
    'input_files' => explode(',', $in_file),
    'output_file' => $out_file,
    'arch' => $arch,
    'base' => $base,
    'cmssw' => $cmssw
  );

  header('Content-Type: application/json');
  echo str_replace('\/', '/', json_encode($result));
}

if (isset($_GET['report'])) {

  $status = isset($_GET['status']) ? $_GET['status'] : 'missing';

  $stmt = $conn->prepare('INSERT IGNORE INTO check_these (file_name, reported, status) VALUES (?, NOW(), ?)');
  $stmt->bind_param('ss', str_replace(' ', '+', $_GET['report']), $status);
  $stmt->execute();
  $stmt->close();

  echo 'Reported ' . $status . ' file to check: ' . $_GET['report'];
  
}

$conn->close();

?>
