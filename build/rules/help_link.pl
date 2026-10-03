#!/usr/bin/perl
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

# One help module, the way helpcontent2/util/target.pmk links it:
#
#   1. zip text/<module>/** into the module's .jar (upstream ZIP1TARGET xhp_<m>)
#   2. HelpLinker   -> <m>.db/.ht/.key + the -add files + caption/ content/
#   3. HelpIndexerTool -extension -> <m>.idxl/ from caption/ content/, which
#      it then deletes (the flag only changes the epilogue: index in place, no
#      zip; the index itself is built exactly as in upstream's zip mode)
#   4. copy every produced file to its declared output, and FAIL on any file
#      the rule did not declare, or any declared file not produced -- that is
#      how a change in HelpLinker's or Lucene's output becomes a loud error
#      instead of a silently incomplete help directory.
#
# Everything happens in a private work directory: HelpLinker writes caption/
# and content/ into its -zipdir, so modules sharing an output directory would
# race on them.

use strict;
use warnings;
use Getopt::Long;
use File::Path qw(make_path remove_tree);
use File::Copy qw(copy);
use File::Find;
use File::Basename;
use Archive::Zip qw(:ERROR_CODES);

my (%o, @adds, @outs);
GetOptions(
    "helplinker=s" => \$o{helplinker}, "tools-dir=s"  => \$o{tools},
    "java=s"       => \$o{java},       "indexer=s"    => \$o{indexer},
    "module=s"     => \$o{module},     "lang=s"       => \$o{lang},
    "src=s"        => \$o{src},        "sty=s"        => \$o{sty},
    "idxcaption=s" => \$o{idxcaption}, "idxcontent=s" => \$o{idxcontent},
    "links=s"      => \$o{links},      "work=s"       => \$o{work},
    "jar-list=s"   => \$o{jarlist},    "jar-root=s"   => \$o{jarroot},
    "add=s"        => \@adds,          "out=s"        => \@outs,
) or die "help_link.pl: bad arguments\n";

my $m = $o{module};
my $zipdir = "$o{work}/zipdir";
remove_tree($zipdir);
make_path($zipdir) or die "cannot create $zipdir\n";

# 1. the module's .jar = upstream's ZIP1LIST over the merged pages.  The page
# list is EXPLICIT (--jar-list): never walk the tree on disk, which can hold
# stale outputs of an earlier build that Bazel does not delete.
my $jar = "$o{work}/xhp_$m.zip";
{
    my $root = "$o{src}/$o{lang}";
    my $zip = Archive::Zip->new();
    my @pages;
    open my $jf, "<", $o{jarlist} or die "cannot read $o{jarlist}\n";
    while (my $l = <$jf>) { chomp $l; $l =~ s/\r$//; push @pages, $l if length $l; }
    close $jf;
    # Directory entries only BELOW --jar-root: upstream zips "text/<m>/*", so
    # the shell expands the children and the root itself (and text/) never
    # becomes an entry; a single-page ZIP1LIST gets no directory entries.
    my %dirs;
    for my $rel (sort @pages) {
        if (length $o{jarroot}) {
            my $d = dirname($rel);
            while ($d ne $o{jarroot} && $d ne "." && !$dirs{$d}++) { $d = dirname($d); }
        }
        $zip->addFile("$root/$rel", $rel) or die "cannot add $rel\n";
    }
    $zip->addDirectory("$_/") for sort keys %dirs;
    # Reproducible: a member's time would otherwise be its input's mtime,
    # i.e. when Bazel happened to stage it.  Set the DOS date/time field
    # itself (1980-01-01 00:00, the DOS epoch: date 0x0021, time 0) --
    # setLastModFileDateTimeFromUnix() converts through LOCAL time, so the
    # result would still depend on the build machine's time zone.
    $_->{'lastModFileDateTime'} = 0x00210000 for $zip->members;
    $zip->writeToFileNamed($jar) == AZ_OK or die "cannot write $jar\n";
}

# 2. HelpLinker, driven by a response file like upstream's @$(mktmp ...).
my @args = ("-mod", $m, "-src", $o{src}, "-sty", $o{sty},
            "-zipdir", $zipdir, "-idxcaption", $o{idxcaption},
            "-idxcontent", $o{idxcontent}, "-lang", $o{lang},
            "-add", "$m.jar", $jar);
for my $a (@adds) {
    my ($name, $path) = split /=/, $a, 2;
    push @args, "-add", $name, $path;
}
my $nlinks = 0;
open my $lf, "<", $o{links} or die "cannot read $o{links}\n";
while (my $l = <$lf>) {
    chomp $l; $l =~ s/\r$//;
    next unless length $l;
    push @args, "$o{src}/$o{lang}/$l";
    ++$nlinks;
}
close $lf;
push @args, "-o", "$o{work}/unused.zip";   # required; HelpLinker only logs it
my $rsp = "$o{work}/helplinker.rsp";
open my $rf, ">", $rsp or die "cannot write $rsp\n";
print $rf join("\n", @args), "\n";
close $rf;

$ENV{PATH} = "$o{tools};$ENV{PATH}";
(my $hl = $o{helplinker}) =~ s{/}{\\}g;
system($hl, "\@$rsp") == 0 or die "HelpLinker failed for $m ($?)\n";

# 3. the Lucene index -- only for a module that links pages.  For one that
# links none (shared), upstream's zip-mode run finds no caption/ dir, returns
# -1 and DELETES the index, so no .idxl ships; -extension mode would instead
# leave an empty index (segments.gen + segments_1).  Not running it is the
# same result as upstream, and HelpLinker made no caption/content to remove.
if ($nlinks > 0) {
    system($o{java}, "-cp", $o{indexer}, "com.sun.star.help.HelpIndexerTool",
           "-lang", $o{lang}, "-mod", $m, "-zipdir", $zipdir, "-extension") == 0
        or die "HelpIndexerTool failed for $m ($?)\n";
}

# 4. declared outputs: --out <relative name>=<output path>
my %want;
for my $spec (@outs) {
    my ($rel, $dst) = split /=/, $spec, 2;
    $want{$rel} = $dst;
}
my @produced;
find({ no_chdir => 1, wanted => sub {
    return unless -f $_;
    (my $rel = $File::Find::name) =~ s{^\Q$zipdir\E/}{};
    push @produced, $rel;
}}, $zipdir);
my @extra = grep { !exists $want{$_} } @produced;
my %have = map { $_ => 1 } @produced;
my @missing = grep { !$have{$_} } sort keys %want;
die "help_link.pl: $m produced files the rule does not declare: @extra\n" if @extra;
die "help_link.pl: $m did not produce declared files: @missing\n" if @missing;
for my $rel (sort keys %want) {
    make_path(dirname($want{$rel}));
    copy("$zipdir/$rel", $want{$rel}) or die "cannot copy $rel: $!\n";
}
# The work dir is a declared (tree) output, so it must exist afterwards --
# empty, since nothing in it is a product.
remove_tree($o{work});
make_path($o{work});
