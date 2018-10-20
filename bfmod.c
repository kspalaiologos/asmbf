
#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "util_script.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define ARR_SIZE 30000

module MODULE_VAR_EXPORT bf_module;

static int bf_handler(request_rec * r);
static void interpret(char * c);
static int set_post_input(request_rec * r);

static request_rec * req;
static char a[ARR_SIZE], *post_input, *cur;
static int p, method;

static int bf_handler(request_rec * r) {
    FILE * f;
    char * c;
    size_t fsize;
    int ret;
    if ((method = r->method_number) != M_GET && method != M_POST)
        return DECLINED;
    if (r->finfo.st_mode == 0)
        return NOT_FOUND;
    if ((f = ap_pfopen (r->pool, r->filename, "r")) == NULL)
        return FORBIDDEN;
    c = ap_palloc (r->pool, (fsize = r->finfo.st_size) + 1);
    fread (c, 1, fsize, f);
    c[fsize] = '\0';
    ap_pfclose (r->pool, f);
    req = r;
    memset (a, 0, ARR_SIZE);
    p = 0;
    ap_send_http_header (r);
    ap_hard_timeout ("mod_bf", r);
    if (method == M_POST) {
        if ((ret = set_post_input(r)) != OK)
            return ret;
        cur = post_input;
    }
    if (!r->header_only)
        interpret (c);
    if (method == M_POST)
        free(post_input);
    ap_kill_timeout (r);
    return OK;
}

static void interpret(char * c) {
    int b;
    char * d;
    for (; *c; c++) {
        switch (*c) {
            case '+':
                a[p]++;
                break;
            case '-':
                a[p]--;
                break;
            case '>':
                p++;
                break;
            case '<':
                p--;
                break;
            case '.':
                if (ap_rputc (a[p], req) == EOF)
                    return;
                ap_rflush (req);
                ap_reset_timeout (req);
                break;
            case ',':
                if (method == M_GET) {
                    if ((a[p] = *req->args) == EOF || a[p] == CR)
                        a[p] = 0;
                    req->args++;
                } else if ((a[p] = *cur++) == EOF || a[p] == CR)
                    a[p] = 0;
                ap_reset_timeout (req);
                break;
            case '[':
                /* Warning, unreadable code incoming */
                /* I feel like a mathematican myself */
                d = ++c;
                for (b = 1; b && *c; c++)
                    b += (*c == '[' ? 1 : (*c == ']' ? -1 : 0));
                if (!b) {
                    *--c = 0;
                    while (a[p])
                        interpret (d);
                    *c = ']';
                }
                break;
        }
    }
}

static int set_post_input(request_rec * req) {
    int ret;
    if ((ret = ap_setup_client_block (req, REQUEST_CHUNKED_ERROR)) != OK)
        return ret;
    if (ap_should_client_block(req)) {
        char argbuf[512];
        int nread, size, pos = 0;
        long len = req->remaining;
        post_input = ap_pcalloc(req->pool, len + 1);
        while ((nread = ap_get_client_block(req, argbuf, sizeof(argbuf))) > 0) {
            ap_reset_timeout(req);
            if ((pos + nread) > len)
                size = len - pos;
            else
                size = nread;
            memcpy(post_input + pos, argbuf, size);
            pos += size;
        }
    }
    return ret;
}

static const handler_rec bf_handlers[] = {
    {"bf-handler", bf_handler},
    {NULL}
};

#if APACHE_RELEASE >= 20000000
module MODULE_VAR_EXPORT bf_module = {
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    bf_handlers,
    NULL,
};
#else
module MODULE_VAR_EXPORT bf_module = {
    STANDARD_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    bf_handlers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif
