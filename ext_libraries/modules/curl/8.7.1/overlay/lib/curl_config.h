/* curl_config.h - generated for Bazel build */
#ifndef CURL_CONFIG_H
#define CURL_CONFIG_H

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#ifndef _WIN32
#define HAVE_SYS_SOCKET_H 1
#endif

/* Define to 1 if you have the <netinet/in.h> header file. */
#ifndef _WIN32
#define HAVE_NETINET_IN_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef _WIN32
#define HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Enable OpenSSL */
#define USE_OPENSSL 1

/* Enable zlib */
#define HAVE_LIBZ 1
#define HAVE_ZLIB_H 1

/* Version */
#define CURL_VERSION "8.7.1"
#define LIBCURL_VERSION "8.7.1"
#define LIBCURL_VERSION_MAJOR 8
#define LIBCURL_VERSION_MINOR 7
#define LIBCURL_VERSION_PATCH 1
#define LIBCURL_VERSION_NUM 0x080701

/* Size of curl_off_t */
#define CURL_SIZEOF_CURL_OFF_T 8
#define CURL_TYPEOF_CURL_OFF_T long long
#define CURL_FORMAT_CURL_OFF_T "lld"
#define CURL_FORMAT_CURL_OFF_TU "llu"
#define CURL_OFF_T_MAX 0x7FFFFFFFFFFFFFFFLL
#define CURL_OFF_T_MIN (-CURL_OFF_T_MAX - 1LL)
#define CURL_SUFFIX_CURL_OFF_T LL
#define CURL_SUFFIX_CURL_OFF_TU ULL

#endif /* CURL_CONFIG_H */
