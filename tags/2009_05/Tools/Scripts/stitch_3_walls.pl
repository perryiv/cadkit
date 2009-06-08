#!/usr/bin/perl
################################################################################
#
#  Stitch 3 walls together.
#
################################################################################

use strict;
my $output = 0;

while ( @ARGV )
{
  my $file0 = shift @ARGV;
  my $file1 = shift @ARGV;
  my $file2 = shift @ARGV;
  my $command = sprintf ( "stitch_images --input $file2 $file0 $file1 --output %09d.jpg", $output );
  print "$command\n";
  `$command`;
  $output++;
}
