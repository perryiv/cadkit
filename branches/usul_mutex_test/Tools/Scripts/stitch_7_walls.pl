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
  my $file3 = shift @ARGV;
  my $file4 = shift @ARGV;
  my $file5 = shift @ARGV;
  my $file6 = shift @ARGV;
  my $command = sprintf ( "stitch_images --input $file6 $file4 $file2 $file0 $file1 $file3 $file5 --output %09d.jpg", $output );
  print "$command\n";
  `$command`;
  $output++;
}
