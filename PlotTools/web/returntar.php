<?php

header('Content-type: application/gzip');
header('Content-Disposition: attachment; filename="CrombiePlots.tar.gz"');

$onlyget = $_GET['only'];
$filesarray = explode(',',$_GET['files']);

$arrlength = count($filesarray);

$filescommand = '';

for ($iFile = 0; $iFile < $arrlength; $iFile++) {

  $fn = 'plots/' . $filesarray[$iFile];

  if ($onlyget == 'pdf') {

    $filescommand = $filescommand . ' ' . $fn . '.pdf';

  } elseif ($onlyget == 'png') {

    $filescommand = $filescommand . ' ' . $fn . '.png';

  } else {

    $filescommand = $filescommand . ' ' . $fn . '.C ' . $fn . '.png ' . $fn . '.pdf';

  }
}

passthru('tar -cz --directory /home/dabercro/public_html' . $filescommand);
exit(0);

?>
