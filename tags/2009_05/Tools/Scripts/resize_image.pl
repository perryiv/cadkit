#!/usr/bin/perl
################################################################################
#
#  Resize the images.
#
################################################################################

use strict;
( 4 == @ARGV ) || die "Usage: $0 <input> <output> <width> <height>\n";

my $input  = shift @ARGV;
my $output = shift @ARGV;
my $width  = shift @ARGV;
my $height = shift @ARGV;

my $command = "stitch_images --input $input --output $output --dimensions $width $height";
print "$command\n";
`$command`;
