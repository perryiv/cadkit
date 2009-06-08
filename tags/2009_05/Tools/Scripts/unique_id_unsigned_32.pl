#!/usr/bin/perl
################################################################################
#
#  Generates unsigned 32-bit unique id.
#
################################################################################

use strict;

my $guid = `uuidgen`;
$guid =~ s/-//g;
$guid = substr ( $guid, 0, 8 );
$guid = hex ( $guid );

while ( length $guid < 10 )
{
    $guid = $guid * 10;
}

print "$guid\n";
