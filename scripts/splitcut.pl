#! /usr/bin/env perl

use v5.10;

my $cut = shift @ARGV || die "Usage: $0 cut";

my $balanced = qr/
  (?(DEFINE)
    (?<BAL_PAT>\(
      (?:
        (?> [^()]+ )
      |
        [^()]*(?&BAL_PAT)[^()]*
      )
    \))
  )
/x;

# If starting and ending with () with balanced parentheses inside
# strip the beginning and end
if ($cut =~ /^(?&BAL_PAT)$balanced$/x)
{
    $cut =~ s/^\(|\)$//g;
}

s/\s//g, say for (split /
    \s* && \s*
    (?![^(]*\))
/x, $cut);
