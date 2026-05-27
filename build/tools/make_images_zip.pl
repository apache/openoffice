#!/usr/bin/perl
# make_images_zip.pl — pack image files into a zip archive.
#
# Usage: make_images_zip.pl <output.zip> <strip_prefix> <filelist.txt>
#
# Each file in <filelist.txt> is added to the zip.  The zip entry name is
# the file's path with everything up to and including <strip_prefix>/ removed.
# If the prefix is not found the bare filename is used as the entry name.

use strict;
use warnings;
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);

die "Usage: $0 <output.zip> <strip_prefix> <filelist.txt>\n" unless @ARGV == 3;
my ($output, $strip_prefix, $listfile) = @ARGV;

$strip_prefix =~ s{\\}{/}g;
$strip_prefix =~ s{/$}{};          # remove trailing slash if present

open(my $fh, '<', $listfile) or die "Cannot open '$listfile': $!\n";
my @files = grep { /\S/ } map { chomp; $_ } <$fh>;
close $fh;

my $zip = Archive::Zip->new();
for my $f (@files) {
    $f =~ s{\\}{/}g;
    my $entry;
    my $pos = index($f, $strip_prefix . '/');
    if ($pos >= 0) {
        $entry = substr($f, $pos + length($strip_prefix) + 1);
    } else {
        ($entry) = ($f =~ m{([^/]+)$});
    }
    next unless defined $entry && $entry =~ /\S/;
    $zip->addFile($f, $entry) or warn "Cannot add '$f': skipped\n";
}

my $rc = $zip->writeToFileNamed($output);
die "Failed to write '$output'\n" unless $rc == AZ_OK;
