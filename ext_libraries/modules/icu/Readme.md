# Windows VS2008 and newer ICU Versions
VS2008 will never compile a more recent ICU, we either need a much newer MSVC (2015+ probably), or another compiler we use for modules like ICU and Python, such as Clang.

Clang is highly MSVC compatible, but relies on MSVC's standard library, which means using Clang with VS2008 would support new C++ language features, but not new C++ headers. However, Clang does have its own C++ library, libc++ (https://libcxx.llvm.org/), that can be used instead, but it's really unclear how to set it up, and whether it can even be set up with VS2008.

The upstream Clang binaries do not ship with libc++. On https://libcxx.llvm.org/VendorDocumentation.html#support-for-windows they vaguely describe how you build it from source. When using Visual Studio as the compiler, it needs at least VS2017, but it can also use Clang in MinGW instead. Also we would decrease _WIN32_WINNT to the minimum Windows version we want to support. So then supposedly downloading MinGW and Clang, then rebuilding the whole LLVM project with libc++ enabled and our preferred _WIN32_WINNT (a huge long process, it's about as big as our whole source tree), should produce a Clang compiler and headers that can compile newer ICU versions, Python 3, etc.

However I don't know how binary compatible C++ is between Clang and VS2008. If C++ was only used internally, and modules communicated with each other using C calls, it would probably work, but since ICU is accessed using C++ calls as well, that's much less certain... If the whole of AOO was built with that libc++ Clang, it would probably work, but with some modules using Clang and some using MSVC, we are not so sure.

Is using newer MSVC better? Possibly, it's easier to install than our custom Clang, and more widely available, which matters for 3rd party extensions. And like Clang, newer MSVC can also target older Windows, such as Windows XP, by using a custom toolset (https://learn.microsoft.com/en-us/cpp/build/configuring-programs-for-windows-xp?view=msvc-170).
# Analysis for ICU
----------------------------------------
Unnecessary dependencies
----------------------------------------
cui didn't actually need ICU as a dependency, and ICU has been deleted from
its makefile and precompiled header.

sc does not really need the ICU layout engine, and this was removed from
its makefiles.

There may still be other modules that specify ICU in their prj/build.lst,
header files, and/or makefiles, but don't actually need it.

-----------------------
GNU make bugs
-----------------------
Building with system ICU broke in i18npool, and probably never really
worked since I ported it to gbuild, due to what looks like a bug in GNU
make, because "-include" isn't really optional like it should be. I patched
it to use a conditional "include" instead.

---------------------------------------------------------------
ICU layout engine ("icule") no longer exists
---------------------------------------------------------------
Building vcl broke for several reasons, one of which is that the ICU layout
engine stopped existing! It was poorly maintained and previously deprecated:
https://web.archive.org/web/20160130092624/http://userguide.icu-project.org/layoutengine
https://unicode-org.atlassian.net/browse/ICU-10530
Then in 2016 it was completely dropped from the ICU project:
https://unicode-org.atlassian.net/browse/ICU-12708
They recommend HarfBuzz as a replacement, and HarfBuzz does have a ICU
Layout Engine compatibility API we could use (of unknown quality and
completeness). HarfBuzz has become quite a popular library. Many
applications and frameworks use it, including LibreOffice and Java. It's
MIT licensed.

However at the moment we don't need to do anything. Just commenting out
this line in main/vcl/source/glyphs/gcach_layout.cxx:
#define ENABLE_ICU_LAYOUT
and removing "icule" from vcl makefiles, was enough to get AOO to build and
run. I think it falls back to using Freetype.

Another good thing about the layout engine removal is that the ICU patch to
misc/icu/source/layout/ArabicShaping.cpp will no longer be necessary - that
file was in the layout engine, and whatever problem that patch fixed,
either won't exist in whatever is doing layout now (Freetype?), or will be
that project's responsibility. Also it's worth noting: HarfBuzz was
developed by an Iranian developer, so correct rendering of Arabic fonts was
probably a top priority.

------------------------------------
i18npool
------------------------------------
i18npool needed more patching than everything else put together, and had to
be broken up into several sections:

---------------------------------------------------------------
i18npool: break iterator rule files don't compile
---------------------------------------------------------------
OpenOffice doesn't just use the break iterators provided by ICU, but also
makes custom ones, by compiling its own break rule files in
i18npool/source/breakiterator/data/*.

The file i18npool/source/breakiterator/data/LICENSE_INFO describes how
these files were forked from ICU 3.2 in 2004, and then modified. ICU
changed since then, and now line.txt doesn't compile because of the
"!!LBCMNoChain;" option which ICU dropped, while sent.txt doesn't compile
due to some other error.

Using "diff" to compare our line.txt and sent.txt to ICU's different
versions, they were forked from (or later updated to) what seem more like
ICU 3.6, which used the rules from Unicode version 5, while the latest ICU
are based on Unicode 12 for sent.txt and Unicode 14 for line.txt.

sent.txt was customized by:
- adding "$Thai         = [:Script = Thai:];"
- adding "$LettersEx = [$OLetter $Upper $Lower $Numeric $Close $STerm]
($Extend | $Format)*;"
- adding "$LettersEx* $Thai $LettersEx* ($ATermEx | $SpEx)*;"
- changing the first line of rule 12 to:
"[[^$STerm $ATerm $Close $Sp $Sep $Format $Extend $Thai]{bof}] ($Extend |
$Format | $Close | $Sp)* [^$Thai];"

which seems like quite a hack, adding Thai-specific rules to a file
intended to find general sentence boundaries, which doesn't mention other
languages.

line.txt has more changes, but I still trust the Unicode 14 rules more than
our (questionably) patched Unicode 5 rules.

The code in main/i18npool/source/breakiterator/breakiterator_unicode.cxx
method BreakIterator_Unicode::loadICUBreakIterator() will fall back to
general break iterators when custom ones cannot be loaded, so I've deleted
our sent.txt and line.txt, so that it uses ICU's own ones.

-------------------------------------------------------------------------------------------------
i18npool: unknown data length when loading custom break iterator rules
-------------------------------------------------------------------------------------------------
The custom break rules are stored in ICU's binary rule format, and get
loaded with udata_open(), which returns a UDataMemory*, which OpenOffice
was ultimately passing to the RuleBasedBreakIterator constructor. Now we
can't call that constructor from C, the only RuleBasedBreakIterator
constructor we can call (through ubrk_openBinaryRules()) is a different one
that takes a pointer and a length ("ruleLength"). [ICU is really a C++
library with a C wrapper around some of its features. The C++ API is more
general and flexible.] While it's possible to get the data pointer from
UDataMemory* by using udata_getMemory(), there is no public function to get
the data length.

By reading the ICU code for the RuleBasedBreakIterator constructor, I see
that, at present, that ruleLength is not really used. That constructor only
checks that it is large enough to hold its internal RBBIDataHeader data
structure, and that it is large enough to hold the file data specified in
RBBIDataHeader. Therefore what I've done is pass 1 billion as the
ruleLength, to get past these checks, which has allowed it to work, but
could break later if the ICU internals change.

------------------------------------------------------------------------------------------------------------
i18npool: subclassing RuleBasedBreakIterator to call protected
setBreakType()
------------------------------------------------------------------------------------------------------------
OpenOffice subclasses RuleBasedBreakIterator in order to call the protected
setBreakType() method inside it, an ugly hack that messes with ICU
internals and only happened to work by pure luck. This subclassing cannot
be done from C, but even if it could, in recent ICU, setBreakType() has
been deleted. What was OpenOffice using setBreakType() for? When the custom
break rules are loaded, it uses setBreakType() to set whether to break on
characters, or words, or sentences, etc., presumably allowing one set of
break rules to be used for multiple types of breaks, instead of needing a
unique set of break rules for each type of break.

This was the ICU commit in which fBreakType was removed:
---snip---
commit 8640bee541ac93bb79cef32ad0643b2ffd09dc3a
Merge: 44b2617d449 d7f2cd98d3a
Author: Andy Heninger <an...@gmail.com>
Date:   Wed Feb 21 23:10:10 2018 +0000

    ICU-10688 Remove redundant break type logic from BreakIterators. Merge
to trunk.