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
hiredis_set(const char *key, bstr_t *val)
{
	redisReply	*r;
	int		err;
	bstr_t		*cmd;

	if(rctx == NULL)
		return ENOEXEC;

	if(xstrempty(key) || bstrempty(val))
		return EINVAL;

	r = NULL;
	err = 0;

	cmd = binit();
	if(cmd == NULL) {
		err = ENOMEM;
		blogf("Couldn't initialize cmd");
		goto end_label;
	}

	bprintf(cmd, "SET %s %%b", key);

	r = redisCommand(rctx, bget(cmd), bget(val), bstrlen(val));

	if(r->type == REDIS_REPLY_ERROR) {
		if(!xstrempty(r->str)) {
			blogf("Error while sending command to redis: %s",
			    r->str);
		} else {
			blogf("Error while sending command to redis,"
			    " and no error string returned by redis!");
		}

		err = ENOEXEC;
		goto end_label;

	} else
	if(r->type == REDIS_REPLY_STATUS) {
		if(!xstrempty(r->str)) {
			if(!xstrbeginswith(r->str, "OK")) {
				blogf("Redis error on SET: %s", r->str);
				err = ENOEXEC;
				goto end_label;
			}
		} else {
			blogf("Error while sending SET to redis, and no"
			    " error string returned by redis!");
		}

	} else {
		blogf("Redis didn't respond with STATUS");
		err = ENOEXEC;
		goto end_label;
	}

end_label:

	if(cmd != NULL) {
		buninit(&cmd);
	}

	if(r != NULL) {
		freeReplyObject(r);
		r = NULL;
	}

	return err;
}


int
hiredis_sadd(const char *key, bstr_t *memb, int *nadded)
{
	int		err;
	redisReply	*r;

	if(rctx == NULL)
		return ENOEXEC;

	if(xstrempty(key) || bstrempty(memb))
		return EINVAL;

	err = 0;
	r = NULL;

	r = redisCommand(rctx, "SADD %s %s", key, bget(memb));

	if(r->type == REDIS_REPLY_ERROR) {
		if(!xstrempty(r->str)) {
			blogf("Error while sending command to redis: %s",
			    r->str);
		} else {
			blogf("Error while sending command to redis,"
			    " and no error string returned by redis!");
		}

		err = ENOEXEC;
		goto end_label;

	} else
	if(r->type == REDIS_REPLY_INTEGER) {
		if(nadded != NULL) {
			*nadded = r->integer;
		}
	} else {
		blogf("Redis didn't respond with integer");
		err = ENOEXEC;
		goto end_label;
	}

end_label:

	if(r != NULL) {
		freeReplyObject(r);
		r = NULL;
	}

	return err;
}


int
hiredis_sismember(const char *key, bstr_t *memb, int *ismemb)
{
	int		err;
	redisReply	*r;

	if(rctx == NULL)
		return ENOEXEC;

	if(xstrempty(key) || bstrempty(memb) || ismemb == NULL)
		return EINVAL;

	err = 0;
	r = NULL;

	r = redisCommand(rctx, "SISMEMBER %s %s", key, bget(memb));

	if(r->type == REDIS_REPLY_ERROR) {
		if(!xstrempty(r->str)) {
			blogf("Error while sending command to redis: %s",
			    r->str);
		} else {
			blogf("Error while sending command to redis,"
			    " and no error string returned by redis!");
		}

		err = ENOEXEC;
		goto end_label;

	} else
	if(r->type == REDIS_REPLY_INTEGER) {
		*ismemb = r->integer;
	} else {
		blogf("Redis didn't respond with integer");
		err = ENOEXEC;
		goto end_label;
	}

end_label:

	if(r != NULL) {
		freeReplyObject(r);
		r = NULL;
	}

	return err;
}


int
hiredis_zadd(const char *key, int score, bstr_t *memb, int *nadded)
{
	int		err;
	redisReply	*r;

	if(rctx == NULL)
		return ENOEXEC;

	if(xstrempty(key) || bstrempty(memb))
		return EINVAL;

	err = 0;
	r = NULL;

	r = redisCommand(rctx, "ZADD %s %d %s", key, score, bget(memb));

	if(r->type == REDIS_REPLY_ERROR) {
		if(!xstrempty(r->str)) {
			blogf("Error while sending command to redis: %s",
			    r->str);
		} else {
			blogf("Error while sending command to redis,"
			    " and no error string returned by redis!");
		}

		err = ENOEXEC;
		goto end_label;

	} else
	if(r->type == REDIS_REPLY_INTEGER) {
		if(nadded != NULL) {
			*nadded = r->integer;
		}
	} else {
		blogf("Redis didn't respond with integer");
		err = ENOEXEC;
		goto end_label;
	}

end_label:

	if(r != NULL) {
		freeReplyObject(r);
		r = NULL;
	}

	return err;
}


int
hiredis_zcount(const char *key, bstr_t *rmin, bstr_t *rmax, int *count)
{
	/* Instead of taking numgers for the range, we take bstr. This
	 * is so the caller can use "+/-inf", and "(number" notations. */
	int		err;
	redisReply	*r;

	if(rctx == NULL)
		return ENOEXEC;

	if(xstrempty(key) || bstrempty(rmin) || bstrempty(rmax) ||
	    count == NULL)
		return EINVAL;

	err = 0;
	r = NULL;

	r = redisCommand(rctx, "ZCOUNT %s %s %s", key, bget(rmin), bget(rmax));

	if(r->type == REDIS_REPLY_ERROR) {
		if(!xstrempty(r->str)) {
			blogf("Error while sending command to redis: %s",
			    r->str);
		} else {
			blogf("Error while sending command to redis,"
			    " and no error string returned by redis!");
		}

		err = ENOEXEC;
		goto end_label;

	} else
	if(r->type == REDIS_REPLY_INTEGER) {
		*count = r->integer;
	} else {
		blogf("Redis didn't respond with integer");
		err = ENOEXEC;
		goto end_label;
	}

end_label:

	if(r != NULL) {
		freeReplyObject(r);
		r = NULL;
	}

	return err;
}

#define ZRANGE_FMT		"ZRANGE %s %d %d"
#define ZRANGE_WITHSCORES_FMT	"ZRANGE %s %d %d WITHSCORES"

int
hiredis_zrange(const char *key, int start, int stop, int withscores,
	barr_t *resp)
{
	/* resp should be a barr of (bstr_t *). Caller responsible for freeing
	 * the returned elements. */
	int		err;
	redisReply	*r;
	redisReply	*elem;
	bstr_t		*str;
	int		i;

	if(rctx == NULL)
		return ENOEXEC;

	if(xstrempty(key) ||  resp == NULL)
		return EINVAL;

	err = 0;
	r = NULL;

	r = redisCommand(rctx, withscores==0?ZRANGE_FMT:ZRANGE_WITHSCORES_FMT,
	    key, start, stop);

	if(r->type == REDIS_REPLY_ERROR) {
		if(!xstrempty(r->str)) {
			blogf("Error while sending command to redis: %s",
			    r->str);
		} else {
			blogf("Error while sending command to redis,"
			    " and no error string returned by redis!");
		}

		err = ENOEXEC;
		goto end_label;

	} else
	if(r->type == REDIS_REPLY_ARRAY && r->elements > 0 &&
	    r->element != NULL) {
		for(i = 0; i < r->elements; ++i) {
			elem = r->element[i];
			if(elem->type != REDIS_REPLY_STRING) {
				blogf("Element is not string");
				continue;
			}
			if(xstrempty(elem->str)) {
				blogf("Element is invalid string");
				continue;
			}
			str = binit();
			if(str == NULL) {
				blogf("Couldn't allocate str");
				continue;
			}
			bstrcat(str, elem->str);
			barr_add(resp, (void *) str);
		}
	} else {
		blogf("Redis didn't respond with valie array");
		err = ENOEXEC;
		goto end_label;
	}

end_label:

	if(r != NULL) {
		freeReplyObject(r);
		r = NULL;
	}

	return err;
}



