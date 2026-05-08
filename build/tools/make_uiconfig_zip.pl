#!/usr/bin/perl
# make_uiconfig_zip.pl — create uiconfig.zip from an explicit file list
#
# Usage: make_uiconfig_zip.pl <output.zip> <filelist.txt>
#
# filelist.txt contains one exec-root-relative path per line.
# ZIP entry name = everything after the last /uiconfig/ segment in the path,
# with backslashes normalised to forward slashes.
# Falls back to the bare filename if /uiconfig/ is not in the path.

use strict;
use warnings;
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);
use File::Basename;

die "Usage: $0 <output.zip> <filelist.txt>\n" unless @ARGV == 2;

my ($output, $listfile) = @ARGV;

open(my $lf, '<', $listfile) or die "Cannot read '$listfile': $!\n";
my @files = grep { /\S/ } map { chomp; $_ } <$lf>;
close($lf);

my $zip = Archive::Zip->new();

for my $file (@files) {
    $file =~ s{\\}{/}g;
    my $entry;
    if ($file =~ m{/uiconfig/(.+)$}) {
        $entry = $1;
    } else {
        $entry = basename($file);
    }
    my $member = $zip->addFile($file, $entry);
    die "Cannot add '$file' to zip\n" unless defined $member;
}

my $status = $zip->writeToFileNamed($output);
die "Cannot write '$output': Archive::Zip status=$status\n"
    unless $status == AZ_OK;
