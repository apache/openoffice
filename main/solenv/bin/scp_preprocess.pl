#!/usr/bin/perl -w
# scp_preprocess.pl — Run cl.exe /EP /Tc on a .scp file and write output to a file.
#
# Uses cmd.exe to redirect cl.exe stdout to the output file.
# cl.exe is invoked by NAME (not full path) so that cmd.exe finds it via PATH,
# side-stepping MSYS2 Perl's fork/exec failure for PE binaries in paths with spaces.
# Caller must set PATH to include VC\bin (done by the Bazel action via _MSVC_ENV_PATH).
#
# SCP2 files use '#Keyword ...' as line comments (not C preprocessor directives).
# cl.exe rejects these with C1021; GNU cpp (used by the original build) ignores them.
# We pre-filter the input, replacing such lines with blank lines before running cl.exe.

use strict;
use warnings;
use Getopt::Long qw(:config no_auto_abbrev);

my ($cl, $include_env, $out);
my (@idirs, @defines);

GetOptions(
    'cl=s'      => \$cl,
    'include=s' => \$include_env,
    'out=s'     => \$out,
    'I=s'       => \@idirs,
    'D=s'       => \@defines,
) or die "Usage: scp_preprocess.pl --cl CL --include INC --out OUT.pre "
       . "[-I DIR]... [-D DEF]... INPUT.scp\n";

my ($input) = @ARGV;
die "--cl, --out, and an input file are required\n"
    unless defined $cl && defined $out && defined $input;

# Pass INCLUDE to cl.exe via the environment.
local $ENV{INCLUDE} = $include_env if defined $include_env;

# Pre-filter: SCP2 uses '#Keyword' as line comments (e.g. #File, #End, #Module).
# These are not valid C preprocessor directives and cause cl.exe C1021 errors.
# The original build used GNU cpp.lcc which silently ignores unknown # directives.
# We replace such lines with blank lines to keep line numbers intact.
my $valid_directives = qr/define|undef|include|ifdef|ifndef|if|elif|else|endif|error|pragma|line|warning/;
my $filtered = $out . ".flt";
{
    open my $in,  '<', $input    or die "Cannot read $input: $!\n";
    open my $tmp, '>', $filtered or die "Cannot write $filtered: $!\n";
    while (my $line = <$in>) {
        if ($line =~ /^\s*#\s*([A-Za-z_]\w*)/ &&
            $1 !~ /^(?:$valid_directives)$/) {
            print $tmp "\n";
        } else {
            print $tmp $line;
        }
    }
    close $in;
    close $tmp;
}

# Use cl.exe by name only — cmd.exe finds it via PATH (VC\bin).
# This avoids MSYS2 fork/exec failing on the space-containing full path.
my @parts = (
    'cl.exe',
    '/nologo', '/EP', '/Tc', $filtered,
    (map { "/I$_" } @idirs),
    (map { "/D$_" } @defines),
);

# Build a single command string for cmd /c; append stdout redirect.
# No paths here contain spaces, so no quoting is needed — this keeps the
# cmd.exe command line free of embedded double-quotes, avoiding MSYS2/cmd
# quoting mismatch issues.
my $cmd = join(' ', @parts) . ' > ' . $out;

# Disable MSYS2's POSIX→Windows argument conversion so that '/c' is passed
# to cmd.exe verbatim (MSYS2 would otherwise silently rewrite it to 'C:',
# causing cmd.exe to start in interactive mode instead of running our command).
local $ENV{MSYS2_ARG_CONV_EXCL} = '*';
local $ENV{MSYS_NO_PATHCONV}    = 1;

# cmd.exe has no spaces in its path so MSYS2 Perl's fork/exec handles it fine.
my $comspec = $ENV{COMSPEC} // 'C:\\Windows\\System32\\cmd.exe';
my $rc = system($comspec, '/c', $cmd);
unlink $filtered;
$rc == 0 or die sprintf("cl.exe preprocessing failed (exit %d)\n", $? >> 8);
