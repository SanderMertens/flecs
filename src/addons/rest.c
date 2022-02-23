#include "../private_api.h"

#ifdef FLECS_REST

typedef struct {
    ecs_world_t *world;
    ecs_entity_t entity;
    ecs_http_server_t *srv;
    int32_t rc;
} ecs_rest_ctx_t;

static ECS_COPY(EcsRest, dst, src, {
    ecs_rest_ctx_t *impl = src->impl;
    if (impl) {
        impl->rc ++;
    }

    ecs_os_strset(&dst->ipaddr, src->ipaddr);
    dst->port = src->port;
    dst->impl = impl;
})

static ECS_MOVE(EcsRest, dst, src, {
    *dst = *src;
    src->ipaddr = NULL;
    src->impl = NULL;
})

static ECS_DTOR(EcsRest, ptr, { 
    ecs_rest_ctx_t *impl = ptr->impl;
    if (impl) {
        impl->rc --;
        if (!impl->rc) {
            ecs_http_server_fini(impl->srv);
            ecs_os_free(impl);
        }
    }
    ecs_os_free(ptr->ipaddr);
})

static char *rest_last_err;

static 
void rest_capture_log(
    int32_t level, 
    const char *file, 
    int32_t line, 
    const char *msg)
{
    (void)file; (void)line;

    if (!rest_last_err && level < 0) {
        rest_last_err = ecs_os_strdup(msg);
    }
}

static
char* rest_get_captured_log(void) {
    char *result = rest_last_err;
    rest_last_err = NULL;
    return result;
}

static
void reply_error(
    ecs_http_reply_t *reply,
    const char *msg)
{
    ecs_strbuf_append(&reply->body, "{\"error\":\"%s\"}", msg);
}

static
void rest_bool_param(
    const ecs_http_request_t *req,
    const char *name,
    bool *value_out)
{
    const char *value = ecs_http_get_param(req, name);
    if (value) {
        if (!ecs_os_strcmp(value, "true")) {
            value_out[0] = true;
        } else {
            value_out[0] = false;
        }
    }
}

static
void rest_parse_json_ser_entity_params(
    ecs_entity_to_json_desc_t *desc,
    const ecs_http_request_t *req)
{
    rest_bool_param(req, "path", &desc->serialize_path);
    rest_bool_param(req, "base", &desc->serialize_base);
    rest_bool_param(req, "values", &desc->serialize_values);
    rest_bool_param(req, "private", &desc->serialize_private);
    rest_bool_param(req, "type_info", &desc->serialize_type_info);
}

static
void rest_parse_json_ser_iter_params(
    ecs_iter_to_json_desc_t *desc,
    const ecs_http_request_t *req)
{
    rest_bool_param(req, "term_ids", &desc->serialize_term_ids);
    rest_bool_param(req, "ids", &desc->serialize_ids);
    rest_bool_param(req, "subjects", &desc->serialize_subjects);
    rest_bool_param(req, "variables", &desc->serialize_variables);
    rest_bool_param(req, "is_set", &desc->serialize_is_set);
    rest_bool_param(req, "values", &desc->serialize_values);
    rest_bool_param(req, "entities", &desc->serialize_entities);
    rest_bool_param(req, "entity_labels", &desc->serialize_entity_labels);
    rest_bool_param(req, "variable_labels", &desc->serialize_variable_labels);
    rest_bool_param(req, "duration", &desc->measure_eval_duration);
    rest_bool_param(req, "type_info", &desc->serialize_type_info);
}

static
bool rest_reply(
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply,
    void *ctx)
{
    ecs_rest_ctx_t *impl = ctx;
    ecs_world_t *world = impl->world;

    if (req->path == NULL) {
        ecs_dbg("rest: bad request received (no URL)");
        reply_error(reply, "bad request");
        reply->code = 400;
        return false;
    }

    ecs_strbuf_appendstr(&reply->headers, "Access-Control-Allow-Origin: *\r\n");

    if (req->method == EcsHttpGet) {
        /* Entity endpoint */
        if (!ecs_os_strncmp(req->path, "entity/", 7)) {
            char *path = &req->path[7];
            ecs_dbg("rest: request entity '%s'", path);

            ecs_entity_t e = ecs_lookup_path_w_sep(
                world, 0, path, "/", NULL, false);
            if (!e) {
                e = ecs_lookup_path_w_sep(
                    world, EcsFlecsCore, path, "/", NULL, false);
                if (!e) {
                    ecs_dbg("rest: requested entity '%s' does not exist", path);
                    reply_error(reply, "entity not found");
                    return true;
                }
            }

            ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
            rest_parse_json_ser_entity_params(&desc, req);

            ecs_entity_to_json_buf(world, e, &reply->body, &desc);
            return true;
        
        /* Query endpoint */
        } else if (!ecs_os_strcmp(req->path, "query")) {
            const char *q = ecs_http_get_param(req, "q");
            if (!q) {
                ecs_strbuf_appendstr(&reply->body, "Missing parameter 'q'");
                reply->code = 400; /* bad request */
                return true;
            }

            ecs_dbg("rest: request query '%s'", q);
            bool prev_color = ecs_log_enable_colors(false);
            ecs_os_api_log_t prev_log_ = ecs_os_api.log_;
            ecs_os_api.log_ = rest_capture_log;

            ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t) {
                .expr = q
            });
            if (!r) {
                char *err = rest_get_captured_log();
                char *escaped_err = ecs_astresc('"', err);
                reply_error(reply, escaped_err);
                ecs_os_free(escaped_err);
                ecs_os_free(err);
            } else {
                ecs_iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;
                rest_parse_json_ser_iter_params(&desc, req);
                
                ecs_iter_t it = ecs_rule_iter(world, r);
                ecs_iter_to_json_buf(world, &it, &reply->body, &desc);
                ecs_rule_fini(r);
            }

            ecs_os_api.log_ = prev_log_;
            ecs_log_enable_colors(prev_color);

            return true;
        }
    }
    if (req->method == EcsHttpOptions) {
        return true;
    }

    return false;
}

static
void on_set_rest(ecs_iter_t *it)
{
    EcsRest *rest = it->ptrs[0];

    int i;
    for(i = 0; i < it->count; i ++) {
        if (!rest[i].port) {
            rest[i].port = ECS_REST_DEFAULT_PORT;
        }

        ecs_rest_ctx_t *srv_ctx = ecs_os_malloc_t(ecs_rest_ctx_t);
        ecs_http_server_t *srv = ecs_http_server_init(&(ecs_http_server_desc_t){
            .ipaddr = rest[i].ipaddr,
            .port = rest[i].port,
            .callback = rest_reply,
            .ctx = srv_ctx
        });

        if (!srv) {
            const char *ipaddr = rest[i].ipaddr ? rest[i].ipaddr : "0.0.0.0";
            ecs_err("failed to create REST server on %s:%u", 
                ipaddr, rest[i].port);
            ecs_os_free(srv_ctx);
            continue;
        }

        srv_ctx->world = it->world;
        srv_ctx->entity = it->entities[i];
        srv_ctx->srv = srv;
        srv_ctx->rc = 1;

        rest[i].impl = srv_ctx;

        ecs_http_server_start(srv_ctx->srv);
    }
}

static
void DequeueRest(ecs_iter_t *it) {
    EcsRest *rest = ecs_term(it, EcsRest, 1);

    if (it->delta_system_time > (FLECS_FLOAT)1.0) {
        ecs_warn(
            "detected large progress interval (%.2fs), REST request may timeout",
            (double)it->delta_system_time);
    }

    int32_t i;
    for(i = 0; i < it->count; i ++) {
        ecs_rest_ctx_t *ctx = rest[i].impl;
        if (ctx) {
            ecs_http_server_dequeue(ctx->srv, it->delta_time);
        }
    } 
}

void FlecsRestImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsRest);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsRest);

    ecs_set_component_actions(world, EcsRest, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsRest),
        .copy = ecs_copy(EcsRest),
        .dtor = ecs_dtor(EcsRest),
        .on_set = on_set_rest
    });

    ECS_SYSTEM(world, DequeueRest, EcsPostFrame, EcsRest);
}

#endif
