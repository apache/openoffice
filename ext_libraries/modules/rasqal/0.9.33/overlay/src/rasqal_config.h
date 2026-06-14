/* src/rasqal_config.h.  Generated from rasqal_config.h.in by configure.  */
/* src/rasqal_config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Flex version as a decimal */
#define FLEX_VERSION_DECIMAL 00000

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #define HAVE_DLFCN_H */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the <gcrypt.h> header file. */
/* #undef HAVE_GCRYPT_H */

/* Define to 1 if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* Define to 1 if you have the `gettimeofday' function. */
/* #undef HAVE_GETTIMEOFDAY */

/* Define to 1 if you have the <gmp.h> header file. */
/* #undef HAVE_GMP_H */

/* Have gmp_version constant */
/* #undef HAVE_GMP_VERSION */

/* Define to 1 if you have the `gmtime_r' function. */
/* #undef HAVE_GMTIME_R */

/* Define to 1 if you have the `initstate' function. */
#define HAVE_INITSTATE 1

/* Define to 1 if you have the `initstate_r' function. */
/* #undef HAVE_INITSTATE_R */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <libxml/schemasInternals.h> header file. */
#define HAVE_LIBXML_SCHEMASINTERNALS_H 1

/* Define to 1 if you have the <libxml/xmlschemastypes.h> header file. */
#define HAVE_LIBXML_XMLSCHEMASTYPES_H 1

/* Define to 1 if you have the <libxml/xmlschemas.h> header file. */
#define HAVE_LIBXML_XMLSCHEMAS_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <math.h> header file. */
#define HAVE_MATH_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <mhash.h> header file. */
/* #undef HAVE_MHASH_H */

/* Define to 1 if you have the <mpfr.h> header file. */
/* #undef HAVE_MPFR_H */

/* Define to 1 if you have the `rand' function. */
#define HAVE_RAND 1

/* Define to 1 if you have the `random' function. */
#define HAVE_RANDOM 1

/* Define to 1 if you have the `random_r' function. */
/* #undef HAVE_RANDOM_R */

/* Define to 1 if you have the `rand_r' function. */
#define HAVE_RAND_R 1

/* Define to 1 if you have the <regex.h> header file. */
/* #undef HAVE_REGEX_H */

/* have PCRE regex - Perl Compatible Regular Expressions */
/* #undef HAVE_REGEX_PCRE */

/* have POSIX regex library */
/* #undef HAVE_REGEX_POSIX */

/* Define to 1 if you have the `srand' function. */
#define HAVE_SRAND 1

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
/* #undef HAVE_STDINT_H */

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
/* #undef HAVE_STRCASECMP */

/* Define to 1 if you have the `stricmp' function. */
#define HAVE_STRICMP 1

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `timegm' function. */
/* #undef HAVE_TIMEGM */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* struct tm has tm_gmtoff field */
/* #undef HAVE_TM_GMTOFF */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <uuid/uuid.h> header file. */
/* #undef HAVE_UUID_UUID_H */

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Is __FUNCTION__ available */
#define HAVE___FUNCTION__ 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* need 'extern int optind' declaration? */
/* #undef NEED_OPTIND_DECLARATION */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "rasqal"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugs.librdf.org/"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Rasqal RDF Query Library"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Rasqal RDF Query Library 0.9.33"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "rasqal"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.9.33"

/* Use GMP gmp_randseed_ui() and mpz_urandomm() */
/* #undef RANDOM_ALGO_GMP_RAND */

/* Use internal Mersenne Twister */
/* #undef RANDOM_ALGO_MTWIST */

/* Use POSIX srand() and rand() */
#define RANDOM_ALGO_RAND 1

/* Use BSD initstate() and random() */
/* #undef RANDOM_ALGO_RANDOM */

/* Use glibc initstate_r() and random_r() */
/* #undef RANDOM_ALGO_RANDOM_R */

/* Use POSIX rand_r() */
/* #undef RANDOM_ALGO_RAND_R */

/* Decimal from GNU MP library */
/* #undef RASQAL_DECIMAL_GMP */

/* Decimal from MPFR library */
/* #undef RASQAL_DECIMAL_MPFR */

/* Decimal without a library */
#define RASQAL_DECIMAL_NONE 1

/* Use gcrypt library for message digests */
/* #undef RASQAL_DIGEST_GCRYPT */

/* Use internal routines for message digests */
#define RASQAL_DIGEST_INTERNAL 1

/* Use mhash library for message digests */
/* #undef RASQAL_DIGEST_MHASH */

/* Use no message digest library */
/* #undef RASQAL_DIGEST_NONE */

/* Building LAQRS query */
#define RASQAL_QUERY_LAQRS 1

/* Building SPARQL query */
#define RASQAL_QUERY_SPARQL 1

/* Use no regex library */
/* #undef RASQAL_REGEX_NONE */

/* Use PCRE regex library */
/* #undef RASQAL_REGEX_PCRE */

/* Use POSIX regex library */
/* #undef RASQAL_REGEX_POSIX */

/* Use internal routines for UUID */
#define RASQAL_UUID_INTERNAL 1

/* Use UUID in libc library for UUID */
/* #undef RASQAL_UUID_LIBC */

/* Use libuuid library for UUID */
/* #undef RASQAL_UUID_LIBUUID */

/* Use no UUID library */
/* #undef RASQAL_UUID_NONE */

/* Use OSSP library for UUID */
/* #undef RASQAL_UUID_OSSP */

/* Release version as a decimal */
#define RASQAL_VERSION_DECIMAL 933

/* Major version number */
#define RASQAL_VERSION_MAJOR 0

/* Minor version number */
#define RASQAL_VERSION_MINOR 9

/* Release version number */
#define RASQAL_VERSION_RELEASE 33

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
/* #undef TIME_WITH_SYS_TIME */

/* Version number of package */
#define VERSION "0.9.33"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
