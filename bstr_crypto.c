#include <openssl/md5.h>
#include <errno.h>
#include "bstr_crypto.h"
#include "blog.h"

#define BSTR_MD5_SIZE  20

int
bstr_md5_readable(bstr_t *buf, bstr_t *rethash)
{
	/* Returns the MD5 hash of buf, in human-readable form. Result will
	 * be appended to rethash. */

	char    hash[BSTR_MD5_SIZE];
	int     i;

	if(bstrempty(buf) || rethash == NULL)
		return EINVAL;

	memset(hash, 0, BSTR_MD5_SIZE);

	(void) MD5((unsigned char *) bget(buf), bstrlen(buf),
	    (unsigned char *) hash);

	/* Convert to human-readable. */
	for(i = 0; i < 16; ++i) {
		bprintf(rethash, "%0x", (unsigned char)hash[i]);
	}

	return 0;
}

