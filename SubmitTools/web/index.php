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
elseif (isset($_GET['report'])) {
  $status = isset($_GET['status']) ? $_GET['status'] : 'missing';
  $id = isset($_GET['me']) ? $_GET['me'] : '0';

  $stmt = $conn->prepare('INSERT IGNORE INTO check_these (file_name, reported, status, id) VALUES (?, NOW(), ?, ?)');
  $stmt->bind_param('sss', str_replace(' ', '+', $_GET['report']), $status, $id);
  $stmt->execute();
  $stmt->close();

  echo 'Reported ' . $status . ' file to check: ' . $_GET['report'];
}
else {

  echo '<a href="?">Home</a> <br>';
  echo '<a href="logs">All Logs</a> <br>';

  if (isset($_GET['file']) and isset($_GET['dir'])) {
    $path = join('/', array_slice(explode('/', $_GET['dir']), -2, 2));
    $base = pathinfo($_GET['file']);
    $files = glob('logs/' . $path . '/' . $base['filename'] . '*');
    foreach($files as $file) {
      echo '<a href="' . $file . '">' . $file . '</a> size: ' . filesize($file) . ' <br>';
    }
  }
  elseif (isset($_GET['dir'])) {
    $match = $_GET['dir'] . '%';
    if (isset($_GET['logs'])) {
      $first_head = ' Status: ';
      $second_head = ' Attempts: ';
      $stmt = $conn->prepare('SELECT output_file, status, attempts FROM queue WHERE output_dir LIKE ?');
      $link = '<a href="?dir=' . $_GET['dir'] . '&file=';
    }
    else {
      $first_head = ' In progress: ';
      $second_head = ' Finished: ';
      $stmt = $conn->prepare('SELECT output_dir, SUM(status != "finished") AS not_fin, SUM(status = "finished") AS finished FROM queue WHERE output_dir LIKE ? GROUP BY output_dir');
      $link = '<a href="?logs=yep&dir=';
    }
    $stmt->bind_param('s', $match);
    $stmt->execute();
    $stmt->bind_result($output, $first, $second);
    while ($stmt->fetch()) {
      echo $link . $output . '">' . $output . '</a>' . $first_head . $first . $second_head . $second . ' <br>';
    }
    $stmt->close();
  }
  else {
    $out_dirs = $conn->query('SELECT SUBSTRING(output_dir, 1, LOCATE(SUBSTRING_INDEX(output_dir, "/", -1), output_dir) - 1) AS out_dir, SUM(status != "finished") AS not_fin, SUM(status = "finished") AS finished FROM queue GROUP BY out_dir;');

    while ($row = $out_dirs->fetch_array(MYSQLI_ASSOC)) {
      echo '<a href="?dir=' . $row['out_dir'] . '">' . $row['out_dir'] . '</a> In progress: ' . $row['not_fin'] . ' Finished: ' . $row['finished'] . ' <br>';
    }

    $out_dirs->close();
  }
}

$conn->close();

?>
