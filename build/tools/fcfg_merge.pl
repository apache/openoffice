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
#   ui
#   /abs/path/fragment_ui.xcu
#   loaders
#   handlers
#   /abs/path/fragment.xcu
#
# xmlpackage: Types | Filter | Misc | GraphicFilter
# Output wraps all non-empty sections in <oor:component-data oor:package=
#   "org.openoffice.TypeDetection" oor:name="<xmlpackage>"> with
#   <node oor:name="Types|Filters|FrameLoaders|ContentHandlers"> children.
#
# The "ui" section carries the filter UIName fragments (filters/<name>_ui.xcu).
# Unlike type fragments (which embed <prop oor:name="UIName"> inline), filter
# fragments keep the localized UIName in a SEPARATE <name>_ui.xcu file.  The
# upstream FCFGMerge tool folds those into the filter node; we do the same here:
# each ui fragment's <prop oor:name="UIName"> block is injected into the filter
# node with the matching oor:name.  Without this the filters' UIName is empty,
# the Save dialog shows " (.ext)" with no name, and
# FileDialogHelper::getRealFilter -> SfxFilterMatcher::GetFilter4UIName returns
# NULL -> guisaveas CheckFilter never reaches STATUS_SAVE -> the Save dialog
# loops forever (the file is never written).

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

# Read manifest.  "ui" is a pseudo-section: not emitted as its own <node>, but
# used to inject UIName props into the filter nodes.
my %sections;
my $current;
open(my $mf, '<', $manifest) or die "Cannot read '$manifest': $!\n";
while (<$mf>) {
    chomp;
    s{\\}{/}g;
    s/^\s+|\s+$//g;
    next unless length;
    if (exists $SETNAME{$_} || $_ eq "ui") {
        $current = $_;
    } elsif (defined $current && /\S/) {
        push @{$sections{$current}}, $_;
    }
}
close($mf);

# Build the UIName map: oor:name -> "<prop oor:name="UIName">...</prop>" block.
my %uiname;
for my $frag (@{$sections{ui} || []}) {
    open(my $ff, '<', $frag) or die "Cannot read ui fragment '$frag': $!\n";
    local $/ = undef;
    my $txt = <$ff>;
    close($ff);
    my ($name)  = $txt =~ m{<node\s+oor:name="([^"]+)"};
    my ($block) = $txt =~ m{(<prop\s+oor:name="UIName".*?</prop>)}s;
    $uiname{$name} = $block if defined $name && defined $block;
}

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
        local $/ = undef;
        my $txt = <$ff>;
        close($ff);

        # For filter nodes, inject the matching UIName prop (if any) just
        # before the node's closing </node>.
        if ($sec eq "filters") {
            my ($name) = $txt =~ m{<node\s+oor:name="([^"]+)"};
            if (defined $name && exists $uiname{$name}) {
                my $inject = "\t\t" . $uiname{$name} . "\n";
                $txt =~ s{(\s*</node>)}{"\n" . $inject . $1}se;
            }
        }

        print $fh $txt;
        print $fh "\n";
    }
    print $fh "\t</node>\n";
}

print $fh "</oor:component-data>\n";
close($fh);
