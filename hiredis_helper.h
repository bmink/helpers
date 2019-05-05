#ifndef HIREDIS_HELPER_H
#define HIREDIS_HELPER_H

#include "bstr.h"

int hiredis_init();
int hiredis_uninit();

int hiredis_sendcmd_intresp(bstr_t *, int *);

int hiredis_set(const char *, bstr_t *);
int hiredis_sadd(const char *, bstr_t *, int *);
int hiredis_sismember(const char *, bstr_t *, int *);

#endif
