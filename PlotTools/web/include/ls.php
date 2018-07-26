<?php

$plotdir = '../plots/';

# List the directory in the reverse order
$allDirs = scandir($plotdir, 1);

# Remove the listings . and ..
array_pop($allDirs);
array_pop($allDirs);

$passNum = 20;

$firstDirs = array_slice($allDirs, 0, $passNum);
$restDirs = array_slice($allDirs, $passNum);

?>
