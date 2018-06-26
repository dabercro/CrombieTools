#! /usr/bin/env perl

# Get the dependecies for a source file

sub uniq_sort {
    my %seen;
    return sort(grep {! $seen{$_}++ } @_);
}

sub depends {
    my @output;
    open(my $check, '<', $_[0]);
    while(my $line = <$check>) {
        if ($line =~ /#include "(crombie\/.*)"/) {
            my $file = "include/$1";
            push @output, $file;
            push @output, depends($file);
        }
    }
    return uniq_sort @output;
}

foreach my $file (depends $ARGV[0]) {
    print "$ENV{'CROMBIEPATH'}/src/$file ";
}

print "$ARGV[0]\n";
