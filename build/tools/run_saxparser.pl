#!/usr/bin/perl
# run_saxparser.pl — wrapper that invokes the i18npool localedata `saxparser`
# tool inside a Bazel action.
#
# saxparser is a UNO-bootstrapping executable: it builds a service manager from
# saxparser.rdb (which registers com.sun.star.xml.sax.Parser → expwrap.dll) plus
# types.rdb, instantiates the SAX parser, parses one locale <lang>_<CC>.xml and
# emits the generated localedata_<lang>_<CC>.cxx.
#
# Three of its inputs must be ABSOLUTE because osl cannot resolve relative paths
# on Windows (the same constraint that forces cppumaker's "./" -O trick):
#   * argv[4]  the saxparser.rdb               → file:/// URL
#   * argv[5]  the types.rdb                   → file:/// URL
#   * -env:OOO_INBUILD_SHAREDLIB_DIR=<dir url> → file:/// URL of the tools dir
#       (the `vnd.sun.star.expand:$OOO_INBUILD_SHAREDLIB_DIR/expwrap.dll` URI in
#        saxparser.rdb expands against this to locate the SAX component DLL)
# The XML input and the .cxx output stay RELATIVE: saxparser opens both with a
# plain fopen() against the action's cwd (= execroot), exactly like dmake's
# `$< $@` arguments.
#
# The tools directory (saxparser.exe + sal3/cppu3/cppuhelper3MSC/salhelper3MSC/
# reg/store/expwrap + CRT) is prepended to PATH so the Windows loader resolves
# saxparser's own imports and expwrap's dependencies.
#
# Usage:
#   perl run_saxparser.pl <saxparser.exe> <tools_dir> <locale> \
#                         <xml_in> <cxx_out> <saxparser.rdb> <types.rdb>

use strict;
use warnings;
use Cwd 'abs_path';

my ($exe, $tools_dir, $locale, $xml_in, $cxx_out, $rdb, $types) = @ARGV;

sub to_url {
    my $p = abs_path($_[0]);
    die "run_saxparser.pl: cannot resolve '$_[0]'\n" unless defined $p;
    $p =~ s{\\}{/}g;               # backslashes → forward slashes for the URL
    return "file:///$p";
}
sub to_win {
    my $p = abs_path($_[0]);
    die "run_saxparser.pl: cannot resolve '$_[0]'\n" unless defined $p;
    $p =~ s{/}{\\}g;               # forward slashes → backslashes for PATH
    return $p;
}

my $exe_abs   = to_win($exe);
my $tools_win = to_win($tools_dir);
my $tools_url = to_url($tools_dir);
my $rdb_url   = to_url($rdb);
my $types_url = to_url($types);

# Prepend the staged tools dir so the loader finds saxparser's DLLs + expwrap.
$ENV{PATH} = "$tools_win;" . ($ENV{PATH} || "");

my @cmd = (
    $exe_abs,
    $locale,
    $xml_in,                       # relative — fopen() against execroot
    $cxx_out,                      # relative — fopen() against execroot
    $rdb_url,
    $types_url,
    "-env:OOO_INBUILD_SHAREDLIB_DIR=$tools_url",
);

my $rc = system(@cmd);
if ($rc != 0) {
    print STDERR "run_saxparser.pl: saxparser failed (rc=$rc) for locale $locale\n";
    exit(1);
}
exit(0);
