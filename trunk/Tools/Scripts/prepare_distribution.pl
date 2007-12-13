#!/usr/bin/perl
###############################################################################
#
# Copy Helios DLLs and dependencies to a distribution directory.
#
###############################################################################

use strict;

use File::Basename;

( @ARGV == 2 ) || die "Usage: $0 <program> <destination directory>\n";

my $program = shift @ARGV;
my $destination = shift @ARGV;

( -f $program ) || die "Error: '$program' is not a file\n";
( -f $program ) || die "Error: '$program' is not readable, skipping it\n";

if ( !( -d $destination ) )
{
  ( 0 == `mkdir $destination` ) || die "Error: Failed to make directory: $destination\n";
}

my @ldd_output = `ldd $program`;
my $copy_command;

while ( @ldd_output )
{
  my $line = shift @ldd_output;
  chomp ( $line );
  my @parts = split ( / /, $line );

  while ( @parts )
  {
    my $part = shift @parts;
    chomp ( $part );

    if ( -f $part )
    {
      my @letters = split ( /\./, $part );

      $copy_command = "cp $part $destination";
      print "$copy_command\n";
      `$copy_command`;

      my $extension = $letters[@letters-1];
      if ( !( $extension eq "so" ) )
      {
        my $file = basename ( $part );
        my @base = split ( /\./, $file );
        my $link_command = "cd $destination ; ln -s ./$file $base[0].so ; cd ..";
        print "$link_command \n";
        `$link_command`;
      }
    }
  }
}

$copy_command = "cp $program $destination";
print "$copy_command\n";
`$copy_command`;
