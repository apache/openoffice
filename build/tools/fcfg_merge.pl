#!/usr/bin/perl
# fcfg_merge.pl — merge filter-config fragments into a combined XCU
#
# Usage: fcfg_merge.pl <output.xcu> <xmlpackage> <manifest.txt>
#
# manifest.txt has section-header lines followed by absolute fragment paths:
#   types
#   /abs/path/fragment.xcu
#   filters
#   /abs/path/fragment.xcu
#   loaders
#   handlers
#   /abs/path/fragment.xcu
#
# xmlpackage: Types | Filter | Misc | GraphicFilter
# Output wraps all non-empty sections in <oor:component-data oor:package=
#   "org.openoffice.TypeDetection" oor:name="<xmlpackage>"> with
#   <node oor:name="Types|Filters|FrameLoaders|ContentHandlers"> children.

use strict;
use warnings;

die "Usage: $0 <output.xcu> <xmlpackage> <manifest.txt>\n" unless @ARGV == 3;

my ($output, $xmlpackage, $manifest) = @ARGV;

my %SETNAME = (
    types    => "Types",
    filters  => "Filters",
    loaders  => "FrameLoaders",
    handlers => "ContentHandlers",
);
my @SECTIONS = qw(types filters loaders handlers);

# Read manifest
my %sections;
my $current;
open(my $mf, '<', $manifest) or die "Cannot read '$manifest': $!\n";
while (<$mf>) {
    chomp;
    s{\\}{/}g;
    s/^\s+|\s+$//g;
    next unless length;
    if (exists $SETNAME{$_}) {
        $current = $_;
    } elsif (defined $current && /\S/) {
        push @{$sections{$current}}, $_;
    }
}
close($mf);

open(my $fh, '>', $output) or die "Cannot write '$output': $!\n";
print $fh qq{<?xml version="1.0" encoding="UTF-8"?>\n};
print $fh qq{<oor:component-data xmlns:oor="http://openoffice.org/2001/registry" };
print $fh qq{xmlns:xs="http://www.w3.org/2001/XMLSchema" };
print $fh qq{oor:package="org.openoffice.TypeDetection" oor:name="$xmlpackage">\n};

for my $sec (@SECTIONS) {
    my $frags = $sections{$sec} or next;
    next unless @$frags;
    print $fh "\t<node oor:name=\"$SETNAME{$sec}\">\n";
    for my $frag (@$frags) {
        open(my $ff, '<', $frag) or die "Cannot read fragment '$frag': $!\n";
        print $fh $_ while <$ff>;
        close($ff);
        print $fh "\n";
    }
    print $fh "\t</node>\n";
}

print $fh "</oor:component-data>\n";
close($fh);
