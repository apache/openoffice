#!/usr/bin/perl
#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************

# Bazel driver for one images*.zip.  It only PREPARES what pack/makefile.mk
# prepares and then runs upstream's own scripts unchanged:
#
#   1. the image lists: every rsc .ilst, normalised from this build's
#      "%images%\<entry>" (rsc_res's -subimages prefix) to upstream's
#      "%MODULE%/<entry>", plus commandimagelist.ilst generated the way the
#      makefile's find|sed does it;
#   2. sorted.lst: solenv/bin/image-sort.pl over image-sort.lst, reading the
#      toolbar XML from a uiconfig/modules/... tree (the solver xml layout);
#   3. the theme directory, if any: hicontrast-to-theme.pl output, or the
#      classic tarball unpacked;
#   4. solenv/bin/packimages.pl.
#
# Everything is written under --work, a directory Bazel declares, so no step
# reads anything a previous build left behind.

use strict;
use warnings;
use File::Basename;
use File::Copy;
use File::Find;
use File::Path qw(mkpath);
use Getopt::Long;

my ($packimages, $imagesort, $hicontrast_pl, $default, $sortlst, $uiconfig,
    $work, $out, $ilst_list, $classic_tar, $hicontrast);
my @custom;
GetOptions(
    'packimages=s'  => \$packimages,
    'imagesort=s'   => \$imagesort,
    'hicontrast-pl=s' => \$hicontrast_pl,
    'default=s'     => \$default,
    'sortlst=s'     => \$sortlst,
    'uiconfig=s'    => \$uiconfig,
    'work=s'        => \$work,
    'out=s'         => \$out,
    'ilst-list=s'   => \$ilst_list,
    'custom=s'      => \@custom,
    'classic-tar=s' => \$classic_tar,
    'hicontrast'    => \$hicontrast,
) or die "packimages_driver: bad arguments\n";

sub run { system(@_) == 0 or die "packimages_driver: '@_' failed: $?\n"; }

mkpath("$work/ilst");

# ── 1. image lists ───────────────────────────────────────────────────────────
open(my $lh, '<', $ilst_list) or die "can't open $ilst_list: $!\n";
my $n = 0;
while (my $ilst = <$lh>) {
    $ilst =~ s/[\r\n]+$//;
    next if $ilst eq '';
    open(my $in, '<', $ilst) or die "can't open $ilst: $!\n";
    # Prefixed with a counter: two packages may name their rsc_res alike.
    my $dst = sprintf("%s/ilst/%03d_%s", $work, $n++, basename($ilst));
    open(my $o, '>', $dst) or die "can't write $dst: $!\n";
    while (<$in>) {
        s/^%images%[\\\/]/%MODULE%\//;
        print $o $_;
    }
    close($o);
    close($in);
}
close($lh);

# pack/makefile.mk: find $(RSCDEFIMG)/res/commandimagelist -name "*.png"
#   | sed "s#$(RSCDEFIMG)/res#%GLOBALRES%#" | sort
my @cmd;
find({ no_chdir => 1, wanted => sub {
    return unless /\.png$/ && -f $_;
    (my $rel = $File::Find::name) =~ s#^\Q$default\E/res/##;
    push @cmd, "%GLOBALRES%/$rel";
} }, "$default/res/commandimagelist");
open(my $ch, '>', "$work/ilst/commandimagelist.ilst") or die $!;
print $ch "$_\n" for sort @cmd;
close($ch);

# ── 2. sorted.lst ────────────────────────────────────────────────────────────
open(my $sh, '<', $sortlst) or die "can't open $sortlst: $!\n";
while (<$sh>) {
    s/[\r\n]+$//;
    next unless m#^uiconfig/modules/(.+)$#;
    my $src = "$uiconfig/modules/$1";
    my $dst = "$work/sortbase/$_";
    -f $src or die "packimages_driver: image-sort.lst names $_, but $src does not exist\n";
    mkpath(dirname($dst));
    copy($src, $dst) or die "copy $src: $!\n";
}
close($sh);
run($^X, $imagesort, $sortlst, "$work/sortbase", "$work/sorted.lst");

# ── 3. theme directory ───────────────────────────────────────────────────────
if ($hicontrast) {
    run($^X, $hicontrast_pl, $default, "$work/hicontrast");
    unshift @custom, "$work/hicontrast";
}
if ($classic_tar) {
    require Archive::Tar;
    my $tar = Archive::Tar->new($classic_tar, 1) or die "can't read $classic_tar\n";
    for my $f ($tar->get_files) {
        next unless $f->is_file;
        my $dst = "$work/" . $f->full_path;
        mkpath(dirname($dst));
        $tar->extract_file($f->full_path, $dst) or die "extract " . $f->full_path . "\n";
    }
    unshift @custom, "$work/classic";
}

# ── 4. packimages.pl ─────────────────────────────────────────────────────────
# packimages.pl refuses to run without at least one -c.  Upstream always passes
# -c $(RSCCUSTOMIMG), the packimages project dir, which holds no image at all;
# an empty directory is the same thing.
mkpath("$work/rsccustomimg");
push @custom, "$work/rsccustomimg";
run($^X, $packimages,
    '-g', $default, '-m', $default,
    (map { ('-c', $_) } @custom),
    '-l', "$work/ilst",
    '-s', "$work/sorted.lst",
    '-o', $out);
