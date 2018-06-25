#! /usr/bin/env perl

use v5.10.1;
use strict;

my $database = 'mysql -N -ht3serv015.mit.edu -usubmit -psubmitter -Dsubmit_queue';

for (`echo 'select file_name, queue.id, total_events, input_files from check_these left join queue on queue.id = check_these.id;' | $database`) {
    my ($pandav, $dataset, $filename, $id, $total_evts, $infiles) = m{/paus/([^/]+/\d{3})/([^/]+)/(\S+)\s+(\d+)\s+(\d+)\s+(\S+)};

    $infiles =~ s/$filename// || next;             # pop out the bad file or run away if it's not there
    $infiles =~ s/^,|,$// || $infiles =~ s/,,/,/;  # Take extra commas or at beginning or end

    open(my $fh, '<', "/home/cmsprod/catalog/t2mit/$pandav/$dataset/Files") || die "No files for $pandav $dataset: $!";
    for (<$fh>) {
        if (/$filename\s+(\d+)/) {
            my $numevents = $1;
            my $newevents = $total_evts - $numevents;
            say qq(update queue set input_files = '$infiles' and total_events = $newevents where id = $id);
            `echo "update queue set input_files = '$infiles' and total_events = $newevents where id = $id;" | $database`;
        }
    }
}
