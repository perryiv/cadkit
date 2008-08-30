#!/usr/bin/perl
################################################################################
#
# Removes every n'th file in the argument list.
#
################################################################################

use strict;

( @ARGV > 1 ) || die "Usage: $0 <n> <file1> [file2] ... [fileN]\n";

my $files = `print_every_nth_file.pl @ARGV`;
my $command = "rm $files";
print "$command\n";
`$command`;
