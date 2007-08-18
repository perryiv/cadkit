#!/usr/bin/perl
################################################################################
#
# Renames the input files with zero-padded sequential numbers.
# Used as a pre-processor when making movies.
#
################################################################################

use strict;

( @ARGV > 1 ) || die "Usage: $0 <file1> [file2] ... [fileN]\n";

my $output = 0;
my $digits = length ( @ARGV );
print "digits = $digits\n";
my $format = "\%0".$digits."d.jpg";
print "format = $format\n";

while ( @ARGV )
{
  my $file = shift @ARGV;
  my $command = sprintf ( "mv $file $format", $output );
  print "$command\n";
  `$command`;
  $output++;
}
