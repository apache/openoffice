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

# One language of an extension's help, the way solenv's
# extension_helplink.mk links it.  Unlike the office's own help
# (help_link.pl) the source and destination are ONE directory, the
# language's, and the module is always "help":
#
#   1. lay the pages out as <work>/<lang>/<package id>/*.xhp
#   2. HelpLinker -mod help -extlangsrc <dir> -extlangdest <dir> <pages>
#      -> help.db_/.ht_/.key_ + caption/ content/ in <dir>
#   3. zip <package id>/* into help.jar (upstream: zip -u -r help.jar ...)
#   4. HelpIndexerTool -extension -> help.idxl/, deleting caption/ content/
#   5. copy every product to its declared output, and FAIL on any file the
#      rule did not declare or any declared file not produced.

use strict;
use warnings;
use Getopt::Long;
use File::Path qw(make_path remove_tree);
use File::Copy qw(copy);
use File::Find;
use File::Basename;
use Archive::Zip qw(:ERROR_CODES);

my (%o, @pages, @outs);
GetOptions(
    "helplinker=s" => \$o{helplinker}, "tools-dir=s"  => \$o{tools},
    "java=s"       => \$o{java},       "indexer=s"    => \$o{indexer},
    "lang=s"       => \$o{lang},       "sty=s"        => \$o{sty},
    "idxcaption=s" => \$o{idxcaption}, "idxcontent=s" => \$o{idxcontent},
    "work=s"       => \$o{work},
    "page=s"       => \@pages,         "out=s"        => \@outs,
) or die "help_extension_link.pl: bad arguments\n";

my $dir = "$o{work}/$o{lang}";
remove_tree($o{work});
make_path($dir) or die "cannot create $dir\n";

# 1. the pages
my %page;  # relative name -> source
for my $spec (@pages) {
    my ($rel, $src) = split /=/, $spec, 2;
    $page{$rel} = $src;
    make_path(dirname("$dir/$rel"));
    copy($src, "$dir/$rel") or die "cannot copy $src: $!\n";
}
my @rels = sort keys %page;

# 2. HelpLinker, driven by a response file
my @args = ("-mod", "help", "-extlangsrc", $dir, "-sty", $o{sty},
            "-extlangdest", $dir, "-idxcaption", $o{idxcaption},
            "-idxcontent", $o{idxcontent}, @rels);
my $rsp = "$o{work}/helplinker.rsp";
open my $rf, ">", $rsp or die "cannot write $rsp\n";
print $rf join("\n", @args), "\n";
close $rf;

$ENV{PATH} = "$o{tools};$ENV{PATH}";
(my $hl = $o{helplinker}) =~ s{/}{\\}g;
system($hl, "\@$rsp") == 0 or die "HelpLinker failed for $o{lang} ($?)\n";

# 3. help.jar: the pages by their relative names.  "zip -r help.jar <id>/*"
# expands the files on the command line, so no directory entry is written.
# Reproducible: the DOS date/time field is set to the DOS epoch directly (see
# help_link.pl for why not setLastModFileDateTimeFromUnix).
{
    my $zip = Archive::Zip->new();
    $zip->addFile("$dir/$_", $_) or die "cannot add $_\n" for @rels;
    $_->{'lastModFileDateTime'} = 0x00210000 for $zip->members;
    $zip->writeToFileNamed("$dir/help.jar") == AZ_OK or die "cannot write help.jar\n";
}

# 4. the Lucene index
system($o{java}, "-cp", $o{indexer}, "com.sun.star.help.HelpIndexerTool",
       "-extension", "-lang", $o{lang}, "-mod", "help", "-zipdir", $dir) == 0
    or die "HelpIndexerTool failed for $o{lang} ($?)\n";

# 5. declared outputs: --out <relative name>=<output path>; the pages are
# inputs that stay where they were put, not products.
my %want;
for my $spec (@outs) {
    my ($rel, $dst) = split /=/, $spec, 2;
    $want{$rel} = $dst;
}
my @produced;
find({ no_chdir => 1, wanted => sub {
    return unless -f $_;
    (my $rel = $File::Find::name) =~ s{^\Q$dir\E/}{};
    push @produced, $rel unless exists $page{$rel};
}}, $dir);
my @extra = grep { !exists $want{$_} } @produced;
my %have = map { $_ => 1 } @produced;
my @missing = grep { !$have{$_} } sort keys %want;
die "help_extension_link.pl: produced files the rule does not declare: @extra\n" if @extra;
die "help_extension_link.pl: did not produce declared files: @missing\n" if @missing;
for my $rel (sort keys %want) {
    make_path(dirname($want{$rel}));
    copy("$dir/$rel", $want{$rel}) or die "cannot copy $rel: $!\n";
}
# The work dir is a declared (tree) output, so it must exist afterwards --
# empty, since nothing in it is a product.
remove_tree($o{work});
make_path($o{work});
