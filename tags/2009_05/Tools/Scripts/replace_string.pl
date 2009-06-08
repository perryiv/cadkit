#!/usr/bin/perl
################################################################################
#
#  Perl program to replace a substring in one or more files with a new one.
#
################################################################################

# Force all variables to be declared.
use strict;

# Bring in these modules.
use File::stat;
use File::Copy;


################################################################################
#
#  Main program.
#
################################################################################
{
  # Check arguments.
  ( @ARGV >= 3 ) || die "Usage: $0 <old string> <new string> <filename(s)>\n";

  # Get the arguments.
  my $old_string = shift @ARGV;
  my $new_string = shift @ARGV;

  # Loop through each filename.
  my $filename;
  foreach $filename ( @ARGV )
  {
    # Replace the sub-string.
    &replaceSubString ( $old_string, $new_string, $filename );
  }
}


################################################################################
#
#  Subroutine to replace the substring in the file.
#
################################################################################

sub replaceSubString() # $old_string, $new_string, $input_filename.
{
  # Check arguments.
  ( @_ == 3 ) || die "Error: replaceSubString(), bad arguments\n";

  # Get the argument(s).
  my $old_string = shift @_;
  my $new_string = shift @_;
  my $input_filename = shift @_;

  # Local variable(s).
  my $temp_filename = "$input_filename.temp";
  my $line;
  my $found = 0;

  # Make sure it's a file.
  if ( !( -f $input_filename ) ) { print "Warning: '$input_filename' is not a file\n"; return; }

  # Make sure it's readable.
  if ( !( -r $input_filename ) ) { print "Warning: '$input_filename' is not readable, skipping it\n"; return; }

  # Open the input file for reading.
  open ( IN, $input_filename ) || die "Error: cannot open file: $input_filename\n";

  # Open the temporary output file for writing.
  open ( OUT, ">$temp_filename" ) || die "Error: cannot open temporary output file: $input_filename.temp\n";

  # For each line in the input file.
  foreach $line ( <IN> )
  {
    # Replace the old string with the new one.
    if ( $line =~ s/$old_string/$new_string/g ) { $found = 1; }

    # Write the new line to the output file.
    print OUT $line;
  }

  # Close the input file.
  close ( IN ) || die "Error: failed to close input file: $input_filename\n";

  # Close the temporary output file.
  close ( OUT ) || die "Error: failed to close temporary output file: $temp_filename\n";

  # Delete the original.
  unlink ( $input_filename ) || die "Error: failed to remove original file: $input_filename\n";

  # Move the temporary output file to the original name.
  rename ( $temp_filename, $input_filename ) || print "Error: failed to rename temporary output file to the original name: $input_filename\n";

  # Let the user know if we changed something.
  if ( $found ) { print "Changed: $input_filename\n"; }
}
