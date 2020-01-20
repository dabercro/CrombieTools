#! /usr/bin/env perl

use strict;
use warnings;
use v5.10;

use Data::Dumper;

my @infiles;
my $firstfile = shift;

push @infiles, $firstfile;
my @included;
push @included, $firstfile;

while (scalar @infiles) {

    my $infile = pop @infiles;

    my @lines;
    open (my $handle, '<', $infile) or die "Can't open $infile";
    push @lines, <$handle>;
    close $handle;

    for (grep {/#include\s.(\w+\.h)/} @lines) {
        my ($checkfile) = /(\w+\.h)/;

        if (-f "include/$checkfile" && ! grep {/include\/$checkfile/} @included) {
            push @infiles, "include/$checkfile";
            push @included, "include/$checkfile";
        }

    }

    for (grep {/INCLUDE\s(treedefs\/\w+\.txt)/} @lines) {
        my ($checkfile) = /(treedefs\/\w+\.txt)/;

        if (-f $checkfile && ! grep {/$checkfile/} @included) {
            push @infiles, $checkfile;
            push @included, $checkfile;
        }

    }

}

for (@included) {
    say $_;
}
