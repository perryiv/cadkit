#!/usr/bin/perl
################################################################################
#
# Renames the input files with zero-padded sequential numbers.
# Used as a pre-processor when making movies.
#
################################################################################

use strict;

( @ARGV > 1 ) || die "Usage: $0 <first_number> <file1> [file2] ... [fileN]\n";

my $output = shift @ARGV;

while ( @ARGV )
{
  my $file = shift @ARGV;
  my @parts = split ( /\./, $file );
  my $ext = $parts [ @parts - 1 ];
  my $command = sprintf ( "mv \"$file\" %09d.%s", $output, $ext );
  print "$command\n";
  `$command`;
  $output++;
}
