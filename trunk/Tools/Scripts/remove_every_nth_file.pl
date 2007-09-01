#!/usr/bin/perl
################################################################################
#
# Removes every n'th file in the argument list.
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

  my $command = "rm $file";
  print "$command\n";
  #`$command`;
}
