#include <errno.h>
#include "libxml_helper.h"

int
libxml_get_childstr(xmlDocPtr doc, xmlNodePtr parent, const char *chnam,
	bstr_t *retval)
{
	xmlNodePtr	node;
	xmlChar		*val;

	if(parent == NULL || xstrempty(chnam) || retval == NULL)
		return EINVAL;

	val = NULL;

	for(node = parent->xmlChildrenNode; node != NULL; node = node->next) {
		if(!xmlStrcmp(node->name, (const xmlChar *) chnam)) {
			val = xmlNodeListGetString(doc, node->xmlChildrenNode,
                            1);
                        if(!xstrempty((char *)val)) {
				bstrcat(retval, (char *)val);
                        }

			if(val) {
				xmlFree(val);
				val = NULL;
			}

			return 0;
		}
	}

	return ENOENT;
}

