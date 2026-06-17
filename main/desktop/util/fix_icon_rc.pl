#!/usr/bin/perl
#**************************************************************
#
# fix_icon_rc.pl <icondir> <ooverinfo2.rc> <out.rc>
#
# Emits a Windows .rc that binds the soffice.exe window-icon ids to the OOo
# icon set, compilable by the project's configured Windows SDK v7.0 rc.exe.
#
# Two reasons the upstream ooverinfo2.rc can't be fed to rc.exe directly:
#
#  1. Token order.  ooverinfo2.rc uses "ICON <id> \"file\"" (keyword first).
#     MS rc.exe requires "<id> ICON \"file\"" (single-line resource syntax is
#     nameID typeID file).  "ICON 2 \"x.ico\"" is parsed as name=ICON,
#     type=2 (RT_BITMAP) and rc tries to load the .ico as a bitmap ->
#     "RC2170 ... not in 3.00 format".  dmake's PATH rc tolerated the reversed
#     order; the strict v7.0 rc does not.  We swap the tokens.
#
#  2. Stale entries.  ooverinfo2.rc still references per-module *application*
#     icons under their old dash names (ooo-writer-app.ico, ...) that were
#     renamed to ooo3_*_app.ico and are no longer delivered (sysui d.lst ships
#     misc/*.ico verbatim).  rc would fail "RC2135 file not found".  Those are
#     NOT window-title icons (framework's titlebarupdate uses the *document*
#     icon ids 2..15, which all exist), so entries whose file is absent are
#     skipped.
#
# Mirrors dmake's APP5 resource (solenv/inc/_tg_app.mk: "1 ICON <APP5ICON>" +
# #include ooverinfo2.rc), corrected for the strict v7.0 rc.  Icon paths stay
# "icons/<f>.ico" and resolve via rc /I <icondir-parent>.
#
#**************************************************************
use strict;
use warnings;

my ($icondir, $rc, $out) = @ARGV;
die "usage: $0 <icondir> <ooverinfo2.rc> <out.rc>\n" unless defined $out;

open(my $in,  '<', $rc)  or die "open $rc: $!";
open(my $ofh, '>', $out) or die "write $out: $!";

# id 1 = SAL_RESID_ICON_DEFAULT: window-class default / taskbar / .exe icon
# and Start Center (framework DEFAULT_ICON_ID fallback).
print $ofh "1 ICON \"icons/ooo3_main_app.ico\"\n"
    if -e "$icondir/ooo3_main_app.ico";

# ids 2..39 = per-module document-window icons (Writer=2, Calc=4, Draw=6,
# Impress=8, Math=15, ...), token-order-corrected, skipping absent files.
while (my $line = <$in>) {
    next unless $line =~ /^\s*ICON\s+(\d+)\s+"([^"]+)"/;
    my ($id, $file) = ($1, $2);
    (my $base = $file) =~ s#.*/##;
    if (-e "$icondir/$base") {
        print $ofh "$id ICON \"$file\"\n";
    } else {
        print $ofh "// missing icon, skipped: $file\n";
    }
}
close $ofh;
close $in;
