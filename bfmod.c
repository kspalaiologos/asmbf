
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

#define MEMORY 30000

module MODULE_VAR_EXPORT bf_module;

static int bf_handler(bf_requestuest_rec * r);
static void bf_run(char * c);
static int bf_post(bf_requestuest_rec * r);

static bf_requestuest_rec * bf_request;
static char ram[MEMORY], *post_input, *cur;
static int p, method;

static int bf_handler(bf_request * r) {
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
    bf_request = r;
    memset (ram, 0, MEMORY);
    p = 0;
    ap_send_http_header (r);
    ap_hard_timeout ("bfmod", r);
    if (method == M_POST) {
        if ((ret = bf_post(r)) != OK)
            return ret;
        cur = post_input;
    }
    if (!r->header_only)
        bf_run (c);
    if (method == M_POST)
        free(post_input);
    ap_kill_timeout (r);
    return OK;
}

static void bf_run(char * c) {
    int b;
    char * d;
    for (; *c; c++) {
        switch (*c) {
            case '+':
                ram[p]++;
                break;
            case '-':
                ram[p]--;
                break;
            case '>':
                p++;
                break;
            case '<':
                p--;
                break;
            case '.':
                if (ap_rputc (ram[p], bf_request) == EOF)
                    return;
                ap_rflush (bf_request);
                ap_reset_timeout (bf_request);
                break;
            case ',':
                if (method == M_GET) {
                    if ((ram[p] = *bf_request->args) == EOF || ram[p] == CR)
                        ram[p] = 0;
                    bf_request->args++;
                } else if ((ram[p] = *cur++) == EOF || ram[p] == CR)
                    ram[p] = 0;
                ap_reset_timeout (bf_request);
                break;
            case '[':
                /* Warning, unreadable code incoming */
                /* I feel like ram mathematican myself */
                d = ++c;
                for (b = 1; b && *c; c++)
                    b += (*c == '[' ? 1 : (*c == ']' ? -1 : 0));
                if (!b) {
                    *--c = 0;
                    while (ram[p])
                        bf_run (d);
                    *c = ']';
                }
                break;
        }
    }
}

static int bf_post(bf_request * bfreq) {
    int ret;
    if ((ret = ap_setup_client_block (bfreq, REQUEST_CHUNKED_ERROR)) != OK)
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
    {"bfmod", bf_handler},
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
    bf_handler,
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
    bf_handler,
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
