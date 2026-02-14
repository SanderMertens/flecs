#include <rest.h>

typedef struct {
    double value;
} Mass;

static
bool custom_rest_reply(
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply,
    void *ctx);

static
bool custom_rest_get_sources(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply);

int main(int argc, char *argv[]) {
    // Passing in the command line arguments will allow the explorer to display
    // the application name.
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsUnits);
    ECS_IMPORT(world, FlecsStats); // Collect statistics periodically

    // Mass component
    ECS_COMPONENT(world, Mass);

    ecs_struct(world, {
        .entity = ecs_id(Mass),
        .members = {
            { .name = "value", .type = ecs_id(ecs_f64_t), .unit = EcsKiloGrams}
        }
    });

    // Simple hierarchy
    ecs_entity_t Sun = ecs_entity(world, { .name = "Sun" });
    ecs_set(world, Sun, Mass, {1.988500e31});

    ecs_set_scope(world, Sun);
    ecs_entity_t Earth = ecs_entity(world, { .name = "Earth" });
    ecs_set(world, Earth, Mass, {5.9722e24});

    ecs_set_scope(world, Earth);
    ecs_entity_t Moon = ecs_entity(world, { .name = "Moon" });
    ecs_set(world, Moon, Mass, {7.34767309e22});

    // Restore default hierarchy scope to root
    ecs_set_scope(world, 0);

    /* Set target FPS for main loop */
    ecs_set_target_fps(world, 60);

    // Run application with REST interface and a custom "sources/" route
    // If you don't need custom routes, please see "examples/c/explorer" instead.
    // When the application is running, navigate to
    // https://flecs.dev/explorer to inspect it!
    //
    // See docs/FlecsRemoteApi.md#explorer for more information.
    ecs_http_server_desc_t *desc = &(ecs_http_server_desc_t) {
        //.ipaddr = "0.0.0.0", 
        //.port = 27750,
        .cache_timeout = 0.2,
        .callback = custom_rest_reply
    };

    // taken from distr/flecs.c, flecs_on_set_rest()
    if (!desc->port) {
        desc->port = ECS_REST_DEFAULT_PORT;
    }
    ecs_http_server_t *srv = ecs_rest_server_init(world, desc);
    if (!srv) {
        const char *ipaddr = desc->ipaddr ? desc->ipaddr : "0.0.0.0";
        ecs_err("failed to create REST server on %s:%u", 
            ipaddr, desc->port);
        return 1;
    }
    ecs_rest_ctx_t *ctx = ecs_http_server_ctx(srv);

    ecs_http_server_start(srv);

    // inspired by: distr/flecs.c, DequeueRest(ecs_iter_t *it);
    const ecs_world_info_t *wi = ecs_get_world_info(world);
    double last_time = wi->world_time_total_raw;
    while ( ecs_progress(world, 0)) {
        // When using EcsRest component, the DequeueRest system is ran in EcsPostFrame pipeline phase
        float elapsed = (float)(wi->world_time_total_raw - last_time);
        ecs_http_server_dequeue(ctx->srv, (ecs_ftime_t)elapsed);
        //FIXME private: flecs_rest_server_garbage_collect(world, ctx);
        last_time = wi->world_time_total_raw;
    }

    ecs_http_server_stop(srv);
    ecs_rest_server_fini(srv);
    return ecs_fini(world);
}

bool custom_rest_reply(
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply,
    void *ctx)
{
    ecs_rest_ctx_t *impl = ctx;
    ecs_world_t *world = impl->world;

    if (req->path == NULL) {
        ecs_dbg("rest: bad request (missing path)");
        //FIXME private flecs_reply_error(reply, "bad request (missing path)");
        reply->code = 400;
        return false;
    }

    if (req->method == EcsHttpGet) {
        /* Sources endpoint */
        if (!ecs_os_strcmp(req->path, "sources") || !ecs_os_strncmp(req->path, "sources/", 8)) {
            return custom_rest_get_sources(world, req, reply);
        }
        // ...
    } else if (req->method == EcsHttpPut) {
        // ...
    } else if (req->method == EcsHttpDelete) {
        // ...
    }

    return flecs_rest_reply(req, reply, ctx);
}

static
bool custom_rest_get_sources(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    (void)world;
    /*
    char *path = &req->path[7];
    ecs_dbg_2("rest: request entity '%s'", path);

    ecs_entity_t e = ecs_lookup_path_w_sep(
        world, 0, path, "/", NULL, false);
    if (!e) {
        ecs_dbg_2("rest: entity '%s' not found", path);
        //FIXME private: flecs_reply_error(reply, "entity '%s' not found", path);
        reply->code = 404;
        return true;
    }

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    //FIXME private: flecs_rest_parse_json_ser_entity_params(world, &desc, req);
    if (ecs_entity_to_json_buf(world, e, &reply->body, &desc) != 0) {
        ecs_strbuf_reset(&reply->body);
        reply->code = 500;
        reply->status = "Internal server error";
        return true;
    }*/

    ecs_strbuf_append(&reply->body, "Hello %s\n", "world");
    return true;
}

/*
static
void CustomDequeueRest(ecs_iter_t *it) {
    CustomEcsRest *rest = ecs_field(it, CustomEcsRest, 0);

    if (it->delta_system_time > (ecs_ftime_t)1.0) {
        ecs_warn(
            "detected large progress interval (%.2fs), REST request may timeout",
            (double)it->delta_system_time);
    }

    const ecs_world_info_t *wi = ecs_get_world_info(it->world);

    int32_t i;
    for(i = 0; i < it->count; i ++) {
        ecs_rest_ctx_t *ctx = rest[i].impl;
        if (ctx) {
            float elapsed = (float)(wi->world_time_total_raw - ctx->last_time);
            ecs_http_server_dequeue(ctx->srv, (ecs_ftime_t)elapsed);
            flecs_rest_server_garbage_collect(it->world, ctx);
            ctx->last_time = wi->world_time_total_raw;
        }
    } 
}
*/
