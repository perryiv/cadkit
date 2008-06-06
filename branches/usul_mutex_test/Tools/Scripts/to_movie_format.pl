#!/usr/bin/perl

use strict;

( @ARGV > 1 ) || die "Usage: $0 <output extension> <input movie> [input movie] ... [input movie]\n";

my $extension = shift @ARGV;
my $input;

foreach $input ( @ARGV )
{
  my @output = split ( "\\.", $input );
  my $output = "$output[0].$extension";

  my $command = "convert_movie.pl $input $output";

  `$command`;

  if ( "flv" == $extension )
  {
    $command = "flvtool2 -U $output";
    `$command`;
  }
}

