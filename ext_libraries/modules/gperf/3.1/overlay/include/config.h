/* gperf 3.1 config.h for Windows / MSVC VS2008 */
#ifndef GPERF_CONFIG_H
#define GPERF_CONFIG_H

/* Tell getopt.c that config.h was found (suppresses fallback guessing) */
#define HAVE_CONFIG_H 1

/* Standard headers available on Windows */
#define HAVE_STRING_H 1

/* VS2008 C++ does NOT support C99 variable-length arrays */
#define HAVE_DYNAMIC_ARRAY 0

/* No libintl / gettext on this build */
#define HAVE_LIBINTL_H 0
#ifndef gettext
#define gettext(s) (s)
#endif
#ifndef ngettext
#define ngettext(s1, s2, n) ((n) == 1 ? (s1) : (s2))
#endif

#endif /* GPERF_CONFIG_H */
