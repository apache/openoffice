/* Module configuration — minimal Windows build for OpenOffice pyuno.
 *
 * Removed from the stock PC/config.c:
 *   audioop, imageop          — deprecated media modules (not needed for pyuno)
 *   _hotshot, _lsprof         — profiling tools (not needed for pyuno)
 *   mmap                      — not needed for pyuno
 *   zlib                      — not compiled in; zipimport works with stored zips
 *   _multibytecodec + CJK     — not compiled in; CJK codecs are optional for pyuno
 *
 * Python/getopt.c provides _PyOS_GetOpt/_PyOS_optind etc. (required by main.c).
 * Python/sigcheck.c and Python/intrcheck.c are excluded (stubs superseded by
 * Modules/signalmodule.c).
 * Python/frozenmain.c is excluded (frozen-exe launcher, not needed in the DLL).
 * Parser/pgenmain.c, pgen.c, tokenizer_pgen.c, printgrammar.c are excluded
 * (parser-generator build tools; not part of the interpreter DLL).
 */

#include "Python.h"

extern void initarray(void);
extern void initbinascii(void);
extern void initcmath(void);
extern void initerrno(void);
extern void initfuture_builtins(void);
extern void initgc(void);
extern void initmath(void);
extern void init_md5(void);
extern void initnt(void);
extern void initoperator(void);
extern void initsignal(void);
extern void init_sha(void);
extern void init_sha256(void);
extern void init_sha512(void);
extern void initstrop(void);
extern void inittime(void);
extern void initthread(void);
extern void initcStringIO(void);
extern void initcPickle(void);
extern void initmsvcrt(void);
extern void init_locale(void);
extern void init_codecs(void);
extern void init_weakref(void);
extern void init_random(void);
extern void inititertools(void);
extern void init_collections(void);
extern void init_heapq(void);
extern void init_bisect(void);
extern void init_symtable(void);
extern void init_csv(void);
extern void init_sre(void);
extern void initparser(void);
extern void init_winreg(void);
extern void init_struct(void);
extern void initdatetime(void);
extern void init_functools(void);
extern void init_json(void);
extern void init_subprocess(void);
extern void init_ast(void);
extern void init_io(void);
/* Upstream CPython ships _socket as a separate _socket.pyd (PCbuild/_socket.vcproj).
   We have no .pyd extension-module targets, so it is linked into python27.dll and
   listed here instead.  Required: uno.py does "import socket" at module scope, so
   without it "import uno" -> "import pythonloader" fails and the whole Python UNO
   loader is dead (ImportError: No module named _socket). */
extern void init_socket(void);
extern void _PyWarnings_Init(void);
extern void initxxsubtype(void);
extern void initzipimport(void);

extern void PyMarshal_Init(void);
extern void initimp(void);

struct _inittab _PyImport_Inittab[] = {

    {"array",          initarray},
    {"_ast",           init_ast},
    {"binascii",       initbinascii},
    {"cmath",          initcmath},
    {"errno",          initerrno},
    {"future_builtins",initfuture_builtins},
    {"gc",             initgc},
    {"math",           initmath},
    {"_md5",           init_md5},
    {"nt",             initnt},   /* Use the NT os functions, not posix */
    {"operator",       initoperator},
    {"signal",         initsignal},
    {"_sha",           init_sha},
    {"_sha256",        init_sha256},
    {"_sha512",        init_sha512},
    {"_socket",        init_socket},
    {"strop",          initstrop},
    {"time",           inittime},
#ifdef WITH_THREAD
    {"thread",         initthread},
#endif
    {"cStringIO",      initcStringIO},
    {"cPickle",        initcPickle},
    {"msvcrt",         initmsvcrt},
    {"_locale",        init_locale},
    {"_subprocess",    init_subprocess},

    {"_codecs",        init_codecs},
    {"_weakref",       init_weakref},
    {"_random",        init_random},
    {"_bisect",        init_bisect},
    {"_heapq",         init_heapq},
    {"itertools",      inititertools},
    {"_collections",   init_collections},
    {"_symtable",      init_symtable},
    {"_csv",           init_csv},
    {"_sre",           init_sre},
    {"parser",         initparser},
    {"_winreg",        init_winreg},
    {"_struct",        init_struct},
    {"datetime",       initdatetime},
    {"_functools",     init_functools},
    {"_json",          init_json},
    {"xxsubtype",      initxxsubtype},
    {"zipimport",      initzipimport},

    /* marshal and imp live in the interpreter core */
    {"marshal",        PyMarshal_Init},
    {"imp",            initimp},

    /* These entries are here for sys.builtin_module_names */
    {"__main__",       NULL},
    {"__builtin__",    NULL},
    {"sys",            NULL},
    {"exceptions",     NULL},
    {"_warnings",      _PyWarnings_Init},

    {"_io",            init_io},

    /* Sentinel */
    {0, 0}
};
