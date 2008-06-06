
################################################################################
#
#  Perl program to translate files.
#
################################################################################

# Force all variables to be declared.
use strict;


################################################################################
#
#  Print usage string.
#
################################################################################

sub usage()
{
    print "Usage: $0 <target format> [options] <filename> [filename] ...\n";
    print "examples:\n";
    print "  $0 osg filename.jt\n";
    die   "  $0 stl *.jt\n";
}


################################################################################
#
#  Main function.
#
################################################################################
{
    # Check arguments.
    ( @ARGV > 1 ) || usage();

    # The first argument is the target format.
    my $target = shift @ARGV;

    # Arguments that are not filenames are options.
    my $options = "";
    my @filenames;
    my $arg;
    foreach $arg ( @ARGV )
    {
        if ( -f $arg )
        {
            $filenames[@filenames] = $arg;
        }
        else
        {
            $options .= "$arg ";
        }
    }

    # Make sure you have some filenames.
    ( @filenames ) || &usage();

    # Loop through each filename.
    my $filename;
    foreach $filename ( @filenames )
    {
        # Get the file extension.
        my $source = substr ( $filename, rindex ( $filename, "." ) + 1 );

        # Make the command.
        my $executable = "$source" . '2' . "$target";
        my $command = $executable . " $options$filename";

        # Print the command.
        print "$command\n";

        # Call the command. Note, use "system" so that stdout 
        # from the called executable goes to the shell.
        system $command;
    }
}
