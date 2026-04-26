#ifndef OPENSSL_ERR_H
#define OPENSSL_ERR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*ERR_print_errors_cb_fn)(const char *str, size_t len, void *u);
void ERR_print_errors_cb(ERR_print_errors_cb_fn cb, void *u);

#ifdef __cplusplus
}
#endif

#endif /* OPENSSL_ERR_H */
