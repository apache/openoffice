tools is migrated. Here's what was needed:

tl.dll — all sources from Library_tl.mk, plus win/source/dll/toolsdll.cxx

ooopathutils.lib — separate static library for pathutils.cxx (used by some modules without needing the full tl.dll)

Key issues resolved:

tstring.cxx/tustring.cxx use #include <strimp.cxx> (angle-bracket) → added /Imain/tools/source/string to copts + declared those files as textual_hdrs in a helper _tl_textual library
strmsys.cxx includes strmwnt.cxx and comdep.cxx includes wntmsc.cxx (both quoted) → textual_hdrs handles visibility
dirent.cxx uses clock() without #include <time.h> → fixed with /FItime.h force-include
resmgr.cxx uses std::hash_map<OUString,...> → //main/stlport:stlport
zcodec.cxx uses zlib → Z_PREFIX + SYSTEM_ZLIB + @zlib//:zlib