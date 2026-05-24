#!/usr/bin/perl
# make_services_input.pl — generate services.input for packcomponents.xslt
#
# Usage: make_services_input.pl <output_file> <component> [<component> ...]
#
# Writes an XML <list> of <filename> elements with absolute file:/// URIs
# built from cwd() (the Bazel exec root) + each exec-root-relative path.
# Absolute URIs are required because packcomponents.xslt's document() call
# resolves relative URIs against the stylesheet's base URI, not the input
# document's base URI.

use strict;
use warnings;
use Cwd qw(cwd);

die "Usage: $0 <output_file> <component>...\n" unless @ARGV >= 2;

my ($output, @components) = @ARGV;

my $here = cwd();
$here =~ s{\\}{/}g;
$here =~ s{/$}{};       # strip trailing slash

open(my $fh, '>', $output) or die "Cannot write '$output': $!\n";
print $fh "<list>\n";
for my $comp (@components) {
    if ($comp =~ /^--dep=(.+)$/) {
        printf $fh "  <dependency file=\"%s\"/>\n", $1;
    } else {
        $comp =~ s{\\}{/}g;
        printf $fh "  <filename>file:///%s/%s</filename>\n", $here, $comp;
    }
}
print $fh "</list>\n";
close($fh);
