#! /usr/bin/env perl

use strict;
use warnings;
use v5.10;

use Data::Dumper;

my @infiles;
push @infiles, shift;
my @included;

while (scalar @infiles) {

    my $infile = pop @infiles;
    push @included, $infile;

    my @lines;
    open (my $handle, '<', $infile) or die "Can't open $infile";
    push @lines, <$handle>;
    close $handle;

    for (grep {/#include\s.(\w+\.h)/} @lines) {
        my ($checkfile) = /(\w+\.h)/;

        if (-f "include/$checkfile" && ! grep {/include\/$checkfile/} @included) {
            push @infiles, "include/$checkfile";
        }

    }

}

for (@included) {
    say $_;
}
