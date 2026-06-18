#!/usr/bin/perl
#**************************************************************
# Normalize ImageList IdList blocks in an rsc .srs (stdin -> stdout).
#
# WHY: rsc2's PreprocessSrsFile() (main/rsc/source/rsc/rsc.cxx) resolves the
# Prefix+Id images of an `ImageList { Prefix="sx"; IdList={...}; }` resource by
# scanning the .srs line-by-line and accepting ONLY entries that are a single
# pure integer per line (String::IsNumericAscii) before forming the image file
# name (e.g. "sx" + "0" + "3123" -> sx03123.png) and looking it up via -lip.
#
# Our pipeline produces the .srs with `rscpp` (a plain C preprocessor), which
# leaves the IdList entries as UNEVALUATED expressions all on one line, e.g.
#     IdList =
#     {
#     (3076 + 47) ; (3076 + 50) ; ... ;
#     };
# IsNumericAscii rejects every one of these, so rsc2 embeds an EMPTY image list
# and every Prefix+Id icon (svtools document-type icons, navigator/toolbox
# images in ~44 .src files) silently vanishes from the .res.  Upstream's rsc
# first pass evaluates the expressions and writes one integer per line; this
# script reproduces that for the rscpp+rsc2 split.
#
# WHAT: inside each `IdList = { ... }` block, evaluate every additive numeric
# expression "(BASE + N [+ ...])" to a plain integer and emit one entry per
# line.  Non-numeric tokens (e.g. leftover cpp line-markers like "# 137") are
# passed through untouched — rsc2 already skips them.  The rsc2 COMPILE pass
# evaluated these same expressions itself, so this is behaviour-preserving for
# compilation and only repairs the image pre-pass.
#**************************************************************
use strict;
use warnings;

local $/;                       # slurp whole file
my $text = <STDIN>;

$text =~ s{(IdList\s*=\s*\{)(.*?)(\})}{ normalize($1, $2, $3, $`) }ges;

print $text;

sub normalize {
    my ($head, $body, $tail, $pre) = @_;
    # Only repair IdLists whose entries are the UNEVALUATED arithmetic that rscpp
    # leaves behind, e.g. "(3076 + 47)".  Those always contain a '+'.  Plain
    # numeric or symbolic-already-resolved IdLists ("1; 2;", "36; 37;") were never
    # affected by that bug, and many appear inline inside macros next to a
    # Color{...} mask whose stray '}' would desync rsc2's line scanner if we
    # reformatted them — so leave such blocks BYTE-FOR-BYTE unchanged.
    return $head . $body . $tail unless $body =~ /\+/;

    # Only act when the enclosing ImageList uses capital "Prefix".  rsc2's image
    # resolver scans case-sensitively for "Prefix"; some .src (dbaccess,
    # reportdesign GroupsSorting/CondFormat) write lowercase "prefix", which it
    # never sees → it would form an empty-prefix name like "30768" and
    # FatalError(ERR_NOIMAGE).  Upstream leaves those image lists empty too, so
    # if the nearest prefix keyword in THIS ImageList is lowercase, don't touch
    # the block.  Bound the search to the current ImageList (text after the last
    # "ImageList" keyword) so a previous block's Prefix can't leak in.
    my $block = $pre;
    $block =~ s/.*\bImageList\b//s;
    my $kw = '';
    $kw = $1 while $block =~ /([Pp])refix\s*=/g;
    return $head . $body . $tail unless $kw eq 'P';

    my @out;
    for my $tok (split /;/, $body) {
        $tok =~ s/^\s+//;
        $tok =~ s/\s+$//;
        next if $tok eq '';
        if ($tok =~ /^[\d\s()+]+$/) {       # additive numeric expression
            my $sum = 0;
            $sum += $_ for ($tok =~ /(\d+)/g);
            push @out, $sum;
        } else {                            # leave anything else as-is
            push @out, $tok;
        }
    }
    # Always emit CANONICAL form: a leading newline so "IdList" starts on its
    # own line (some .src use macros that put "MaskColor=Color{...}; IdList={...}"
    # on ONE line — rsc2's PreprocessSrsFile scans for the "IdList" line and stops
    # at the FIRST '}', so a stray '}' from an inline Color{} on that same line
    # breaks it).  "IdList =" / "{" / one-int-per-line / "}" is the form rsc2's
    # scanner reliably consumes.
    return "\nIdList =\n{\n" . join('', map { "$_;\n" } @out) . "}";
}
