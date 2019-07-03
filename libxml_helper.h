#ifndef LIBXML_HELPER_H
#define LIBXML_HELPER_H

#include <libxml/tree.h>
#include "bstr.h"

int libxml_get_childstr(xmlDocPtr, xmlNodePtr, const char *, bstr_t *);
int libxml_get_child_attrstr(xmlDocPtr, xmlNodePtr, const char *, const char *,
	bstr_t *);


#endif

