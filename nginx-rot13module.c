
/* Nginx ROT13 filter module. Written by Michiel Overtoom, motoom@xs4all.nl */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_flag_t enabled;
    } rot13conf;

static ngx_command_t  rot13commands[]={
    {ngx_string("rot13"),
     NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
     ngx_conf_set_flag_slot, NGX_HTTP_LOC_CONF_OFFSET, offsetof(rot13conf, enabled),
     NULL},
    ngx_null_command
};

static ngx_http_output_body_filter_pt nextfilter;
ngx_module_t rot13module;

static ngx_int_t rot13filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_connection_t *c=r->connection;
    rot13conf  *conf;
    char *p;

    if (ngx_http_get_module_ctx(r, rot13module)!=NULL) return NGX_DECLINED;
    conf = ngx_http_get_module_loc_conf(r, rot13module);
    if (in && conf->enabled==1) {
        for (p=(char *)in->buf->start;p<=(char *)in->buf->end;p++) {
            if (*p>='a' && *p<='m') *p=*p-'a'+'n';
            else if (*p>='n' && *p<='z') *p=*p-'n'+'a';
            else if (*p>='A' && *p<='M') *p=*p-'A'+'N';
            else if (*p>='N' && *p<='Z') *p=*p-'N'+'A';
            }
        }
    if (in || c->buffered) return nextfilter(r->main, in);
    return NGX_OK;
}

static rot13init(ngx_conf_t *cf)
{
    nextfilter=ngx_http_top_body_filter;
    ngx_http_top_body_filter=rot13filter;
    return NGX_OK;
}

static void *createconf(ngx_conf_t *cf)
{
    rot13conf  *conf=ngx_pcalloc(cf->pool, sizeof(rot13conf));
    if (!conf) return NGX_CONF_ERROR;
    conf->enabled=NGX_CONF_UNSET;
    return conf;
}

static char *mergeconf(ngx_conf_t *cf, void *parent, void *child)
{
    rot13conf *prev=parent, *conf=child;
    ngx_conf_merge_off_value(conf->enabled, prev->enabled, NGX_CONF_UNSET);
    return NGX_CONF_OK;
}

static ngx_http_module_t  rot13module_ctx={
    NULL,
    rot13init,
    NULL, NULL, NULL, NULL,
    createconf, mergeconf
};

ngx_module_t rot13module={
    NGX_MODULE_V1, &rot13module_ctx, rot13commands, NGX_HTTP_MODULE,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NGX_MODULE_V1_PADDING
};
