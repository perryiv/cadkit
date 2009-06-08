#!/usr/bin/perl
################################################################################
#
# Makes all the file extensions lower case.
#
################################################################################

use strict;

( @ARGV > 0 ) || die "Usage: $0 <file1> [file2] ... [fileN]\n";

my $temp_extension = "some_long_file_extension";

while ( @ARGV )
{
  my $file = shift ( @ARGV );
  my @parts = split ( /\./, $file );

  my @rparts = reverse ( @parts );
  my $ext = shift ( @rparts );
  $ext =~ tr/[A-Z]/[a-z]/;

  my $base = shift ( @parts );

  print ( sprintf ( "mv %s %s.%s\n", $file, $base, $ext ) );

  my $command = sprintf ( "mv %s %s.%s", $file, $base, $temp_extension );
  `$command`;

  $command = sprintf ( "mv %s.%s %s.%s", $base, $temp_extension, $base, $ext );
  `$command`;
}
