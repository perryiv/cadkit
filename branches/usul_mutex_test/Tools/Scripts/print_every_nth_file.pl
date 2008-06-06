#!/usr/bin/perl
################################################################################
#
# Prints every n'th command-line argument.
#
################################################################################

use strict;

( @ARGV > 1 ) || die "Usage: $0 <n> <file1> [file2] ... [fileN]\n";

my $num = shift @ARGV;

while ( @ARGV )
{
  my $file = shift @ARGV;

  my $count = $num - 1;
  while ( $count > 0 )
  {
    shift @ARGV;
    $count = $count - 1;
  }

  print "$file ";
}

print "\n";
