#include "hiredis_helper.h"
#include "hiredis.h"
#include "blog.h"
#include "errno.h"

#define REDIS_ADDR      "127.0.0.1"
#define REDIS_PORT      6379

static redisContext	*rctx = NULL;

int
hiredis_init()
{

	rctx = redisConnect(REDIS_ADDR, REDIS_PORT);
	if(rctx == NULL) {
		blogf("Could not create redis context\n");
		return ENOEXEC;
	}

	if(rctx->err != 0) {
		blogf("Could not connect to redis: %s\n", rctx->errstr);
		return ENOEXEC;
	}

	return 0;
}


int
hiredis_uninit()
{

	if(rctx != NULL) {
		redisFree(rctx);
		rctx = NULL;
	}

	return 0;
}

int
hiredis_sendcmd_intresp(bstr_t *cmd, int *resp)
{
	redisReply	*r;
	int		err;

	if(rctx == NULL)
		return ENOEXEC;

	if(bstrempty(cmd) || resp == NULL)
		return EINVAL;

	r = NULL;
	err = 0;

	r = redisCommand(rctx, bget(cmd));

	if(r->type == REDIS_REPLY_ERROR) {
		if(!xstrempty(r->str)) {
			blogf("Error while sending command to redis: %s",
			    r->str);
		} else {
			blogf("Error while sending command to redis,"
			    " and no error string returned by redis!");
		}

		err = ENOEXEC;
	} else
	if(r->type == REDIS_REPLY_INTEGER) {
		*resp = r->integer;
	} else {
		blogf("Redis didn't respond with integer");
		err = ENOEXEC;
	}


	if(r != NULL) {
		freeReplyObject(r);
		r = NULL;
	}

	return err;
}


int
hiredis_sendcmd_strresp(bstr_t *cmd, bstr_t **resp)
{
	if(rctx == NULL)
		return ENOEXEC;

	if(bstrempty(cmd) || resp == NULL)
		return EINVAL;


	return 0;
}

