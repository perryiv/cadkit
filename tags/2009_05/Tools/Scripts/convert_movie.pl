#!/usr/bin/perl

use strict;

( 2 == @ARGV ) || die "Usage: $0 <input movie> <output movie>\n";

my $input  = shift @ARGV;
my $output = shift @ARGV;

my $command = "ffmpeg -y -i ".$input." -sameq ".$output;

`$command`;

