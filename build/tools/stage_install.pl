#!/usr/bin/perl
# stage_install.pl — copy Bazel build outputs into a staged install layout.
#
# Usage: stage_install.pl <dest_dir> <manifest.txt>
#
# manifest.txt format (one entry per line):
#   <subdir> TAB <src_path> [TAB <dest_name>]
#
# If dest_name is given the file is copied under that name; otherwise the
# source basename is used.  Blank lines and lines starting with '#' are ignored.

use strict;
use warnings;
use File::Basename qw(basename);
use File::Path    qw(make_path);
use File::Copy    qw(copy);

die "Usage: $0 <dest_dir> <manifest.txt>\n" unless @ARGV == 2;

my ($dest_dir, $manifest) = @ARGV;
$dest_dir =~ s{\\}{/}g;
$dest_dir =~ s{/$}{};

open(my $fh, '<', $manifest) or die "Cannot read '$manifest': $!\n";
my $n = 0;
while (<$fh>) {
    chomp;
    s{\\}{/}g;
    next if /^\s*$/ || /^\s*#/;

    my ($subdir, $src, $destname) = split /\t/, $_, 3;
    next unless defined $src && $src =~ /\S/;
    $destname //= '';
    $destname =~ s/^\s+|\s+$//g;
    $destname = basename($src) unless length $destname;

    my $outdir = "$dest_dir/$subdir";
    make_path($outdir) unless -d $outdir;

    my $dest = "$outdir/$destname";
    copy($src, $dest) or die "Cannot copy '$src' → '$dest': $!\n";
    $n++;
}
close($fh);
print "Staged $n file(s) into $dest_dir\n";
