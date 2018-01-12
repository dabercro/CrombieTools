#! /usr/bin/env perl

use strict;
use warnings;

# Check arguments and print help function if needed
sub print_use {
    print "Usage: $0 NAME=SOURCE [NAME=SOURCE] ... OUTPUTHEAD\n\n";
    print "NAME        Name of the string variable to create in the header\n";
    print "SOURCE      Source file(s) that you would like to analyze\n";
    print "OUTPUTHEAD  Place where you would like to put the output header\n";
    return "\nYou did something wrong with arguments\n";
}

# Check presence of arguments
my $out_file = pop @ARGV or die print_use;
if (not @ARGV) {
    die print_use;
}

# Check that each source file exists
my %out_vars;
foreach my $arg (@ARGV) {
    my @holder = split(/=/, $arg);

    if (not -e $holder[1]) {
        print "File $holder[1] does not exist!\n\n";
        die print_use;
    }

    $out_vars{$holder[0]} = $holder[1];
}

# Check first line of header
if (-e $out_file) {
    open (my $handle, '<', $out_file);
    chomp (my $first = <$handle>);
    close $handle;
    if ($first ne '#ifndef CROMBIE_BCALSTRINGS_H') {
        print "First line of $out_file looks suspicious! I don't want to overwrite:\n";
        print "$first\n";
        die print_use;
    }
}


# Start output file
open (my $out, '>', $out_file);

print $out <<HEAD;
#ifndef CROMBIE_BCALSTRINGS_H
#define CROMBIE_BCALSTRINGS_H 1

#include <string>

namespace bcalstrings {
HEAD

while ((my $varname, my $csvfile) = each(%out_vars)) {
    print $out "\n  const std::string $varname = \"\"\n";
    open(my $input, '<', $csvfile);
    while(<$input>) {
        chomp;
        s/\r//g;
        s/"/\\"/g;
        print $out "    \"$_\\n\"\n";
    }
    print $out "  ;\n";
    close $input;
}

print $out <<HEAD;
}

#endif
HEAD

close $out;
