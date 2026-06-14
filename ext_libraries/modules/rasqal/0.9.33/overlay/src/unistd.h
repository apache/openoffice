/* Dummy unistd.h for the MSVC (Windows) build.
 *
 * Faithful to dmake: src/makefile.mk compiles with -DHAVE_CONFIG_H, so sources
 * do `#ifdef HAVE_UNISTD_H #include <unistd.h>` (rasqal_config.h sets it to 1).
 * dmake satisfies that include via `-I$(PRJ)/$(INPATH)/inc`, the solver include
 * dir, which holds the empty unistd.h stub delivered by main/soltools/winunistd.
 * Cross-repo `-I main/...` is fragile under Bazel, so this overlay reproduces the
 * same empty stub on rasqal's own include path (-Isrc).  The POSIX functions
 * rasqal uses on Windows come from <io.h>/<windows.h> via win32_rasqal_config.h.
 */
