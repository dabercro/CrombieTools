#! /usr/bin/env perl

use v5.10.1;
use strict;

my $database = 'mysql -N -ht3serv015.mit.edu -usubmit -psubmitter -Dsubmit_queue';

for (`echo 'select file_name, id from check_these;' | $database`) {
    my ($pandav, $dataset, $filename, $id) = m{/paus/([^/]+/\d{3})/([^/]+)/(\S+)\s+(\d+)};

    my $thisjob = `echo 'select total_events, input_files from queue where id = $id;' | $database`;
    my ($total_evts, $infiles) = ($thisjob =~ /(\d+)\s+(\S+)/);

    $infiles =~ s/$filename// || next;             # pop out the bad file or run away if it's not there
    $infiles =~ s/^,|,$// || $infiles =~ s/,,/,/;  # Take extra commas or at beginning or end

    open(my $fh, '<', "/home/cmsprod/catalog/t2mit/$pandav/$dataset/Files") || die "No files for $pandav $dataset: $!";
    for (<$fh>) {
        if (/$filename\s+(\d+)/) {
            my $numevents = $1;
            my $newevents = $total_evts - $numevents;
            say qq(update queue set input_files = '$infiles', total_events = $newevents where id = $id);
            `echo "update queue set input_files = '$infiles', total_events = $newevents where id = $id;" | $database`;
        }
    }
}
