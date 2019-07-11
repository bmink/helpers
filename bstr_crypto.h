#ifndef BSTR_CRYPTO_H
#define BSTR_CRYPTO_H

/* This has been moved to a helper file so libcrypto doesn't have to be
 * linked to by everyone who uses libb. */

#include "bstr.h"

int bstr_md5_readable(bstr_t *, bstr_t *);

#endif

