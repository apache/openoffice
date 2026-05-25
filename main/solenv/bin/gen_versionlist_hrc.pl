#!/usr/bin/perl
# Reads main/solenv/inc/version.lst and writes a versionlist.hrc
# that satisfies the #include "versionlist.hrc" in version.hrc.
# Usage: perl gen_versionlist_hrc.pl <version.lst> <versionlist.hrc>

use strict;
use warnings;

my ($in, $out) = @ARGV;
open my $fin,  '<', $in  or die "Cannot read $in: $!";
open my $fout, '>', $out or die "Cannot write $out: $!";

my %v;
while (<$fin>) {
    $v{$1} = $2 if /^(OOOBASEVERSION\w+)=(\d+)/;
}
close $fin;

print $fout <<END;
/* Generated from version.lst by gen_versionlist_hrc.pl -- do not edit. */
#ifndef _VERSIONLIST_HRC_
#define _VERSIONLIST_HRC_
#define VERSION    $v{OOOBASEVERSIONMAJOR}
#define SUBVERSION $v{OOOBASEVERSIONMINOR}
#define VER_YEAR   $v{OOOBASEVERSIONYEAR}
#define VER_MONTH  $v{OOOBASEVERSIONMONTH}
#define VER_DAY    $v{OOOBASEVERSIONDAY}
#endif
END
close $fout;
