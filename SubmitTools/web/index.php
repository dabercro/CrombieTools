<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
mysqli_report(MYSQLI_REPORT_STRICT);

// Connect to MySQL database
$conn = new mysqli('t3serv015.mit.edu', 'submit', 'submitter', 'submit_queue');

if (isset($_GET['id'])) {
  $id = $_GET['id'];
  $stmt = $conn->prepare('SELECT exe, input_dir, output_dir, input_files, output_file, scram_arch, base, cmssw comments FROM queue WHERE id=?');
  $stmt->bind_param('s', $id);
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

  $old_stat = 'submitted';
  $new_stat = 'running';
  $stmt = $conn->prepare('UPDATE queue SET status=?, last_checked=NOW() WHERE id=? AND status=?');
  $stmt->bind_param('sss', $new_stat, $id, $old_stat);
  $stmt->execute();
  $stmt->close();
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
  echo '<!DOCTYPE html><head><style>table {border-collapse: collapse;} td, th {padding: 5px;}</style></head>';
  echo '<body><center><h3><a href="?">Home</a> <a href="http://t3serv007.mit.edu/condormon/">Condormon</a> <a href="logs">All Logs</a></h3></center>';

  if (isset($_GET['file']) and isset($_GET['dir'])) {
    $path = join('/', array_slice(explode('/', $_GET['dir']), -2, 2));
    $base = pathinfo($_GET['file']);
    $files = glob('logs/' . $path . '/' . $base['filename'] . '*');
    $stmt = $conn->prepare('SELECT id FROM queue WHERE output_dir=? AND output_file=?');
    $stmt->bind_param('ss', $_GET['dir'], $_GET['file']);
    $stmt->execute();
    $stmt->bind_result($id);
    $stmt->fetch();
    echo 'ID: ' . $id . ' <br>';
    $stmt->close();
    foreach($files as $file) {
      echo '<a href="' . $file . '">' . $file . '</a> size: ' . filesize($file) . ' <br>';
    }
  }
  else {
    $first_head = ' In progress';
    $second_head = ' Finished';
    if (isset($_GET['dir'])) {
      $match = $_GET['dir'] . '%';
      if (isset($_GET['logs'])) {
        $stmt = $conn->prepare('SELECT output_file, status, attempts FROM queue WHERE output_dir LIKE ?');
        $link = 'dir=' . $_GET['dir'] . '&file=';
        $first_head = ' Status';
        $second_head = ' Attempts';
      }
      else {
        $stmt = $conn->prepare('SELECT output_dir, SUM(status != "finished") AS not_fin, SUM(status = "finished") AS finished FROM queue WHERE output_dir LIKE ? GROUP BY output_dir');
        $link = 'logs=yep&dir=';
      }
      $stmt->bind_param('s', $match);
    }
    else {
      $stmt = $conn->prepare('SELECT SUBSTRING(output_dir, 1, LOCATE(SUBSTRING_INDEX(output_dir, "/", -1), output_dir) - 1) AS out_dir, SUM(status != "finished") AS not_fin, SUM(status = "finished") AS finished FROM queue GROUP BY out_dir ORDER BY entered DESC;');
      $link = 'dir=';
    }
    $stmt->execute();
    $stmt->bind_result($output, $first, $second);
    printf('<center><table border="2px"><tr><th>Output</th><th>%s</th><th>%s</th></tr>', $first_head, $second_head);
    while ($stmt->fetch()) {
      printf('<tr><td><a href="?%s">%s</a></td><td><font color="%s">%s</font></td><td>%s</td></tr>', $link . $output, $output,
             (($first == '0' or $first == 'finished') ? '#00ff00' : '#ff0000'), $first, $second);
    }
    echo '</table></center></body>';
    $stmt->close();
  }
}

$conn->close();

?>
