/**
 * @file addons/rest.c
 * @brief Rest addon.
 */

#include "../private_api.h"

#ifdef FLECS_REST

/* Retain captured commands for one minute at 60 FPS */
#define FLECS_REST_COMMAND_RETAIN_COUNT (60 * 60)

static ECS_TAG_DECLARE(EcsRestPlecs);

typedef struct {
    ecs_world_t *world;
    ecs_http_server_t *srv;
    int32_t rc;
    ecs_map_t cmd_captures;
} ecs_rest_ctx_t;

typedef struct {
    char *cmds;
    ecs_time_t start_time;
    ecs_strbuf_t buf;
} ecs_rest_cmd_sync_capture_t;

typedef struct {
    ecs_vec_t syncs;
} ecs_rest_cmd_capture_t;

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
            ecs_rest_server_fini(impl->srv);
        }
    }
    ecs_os_free(ptr->ipaddr);
})

static char *rest_last_err;
static ecs_os_api_log_t rest_prev_log;

static 
void flecs_rest_capture_log(
    int32_t level, 
    const char *file, 
    int32_t line, 
    const char *msg)
{
    (void)file; (void)line;

#ifdef FLECS_DEBUG
    if (level < 0) {
        /* Also log to previous log function in debug mode */
        if (rest_prev_log) {
            ecs_log_enable_colors(true);
            rest_prev_log(level, file, line, msg);
            ecs_log_enable_colors(false);
        }
    }
#endif

    if (!rest_last_err && level < 0) {
        rest_last_err = ecs_os_strdup(msg);
    }
}

static
char* flecs_rest_get_captured_log(void) {
    char *result = rest_last_err;
    rest_last_err = NULL;
    return result;
}

static
void flecs_reply_verror(
    ecs_http_reply_t *reply,
    const char *fmt,
    va_list args)
{
    ecs_strbuf_appendlit(&reply->body, "{\"error\":\"");
    ecs_strbuf_vappend(&reply->body, fmt, args);
    ecs_strbuf_appendlit(&reply->body, "\"}");
}

static
void flecs_reply_error(
    ecs_http_reply_t *reply,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    flecs_reply_verror(reply, fmt, args);
    va_end(args);
}

static
void flecs_rest_bool_param(
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
void flecs_rest_int_param(
    const ecs_http_request_t *req,
    const char *name,
    int32_t *value_out)
{
    const char *value = ecs_http_get_param(req, name);
    if (value) {
        *value_out = atoi(value);
    }
}

static
void flecs_rest_string_param(
    const ecs_http_request_t *req,
    const char *name,
    char **value_out)
{
    const char *value = ecs_http_get_param(req, name);
    if (value) {
        *value_out = ECS_CONST_CAST(char*, value);
    }
}

static
void flecs_rest_parse_json_ser_entity_params(
    ecs_world_t *world,
    ecs_entity_to_json_desc_t *desc,
    const ecs_http_request_t *req)
{
    flecs_rest_bool_param(req, "path", &desc->serialize_path);
    flecs_rest_bool_param(req, "label", &desc->serialize_label);
    flecs_rest_bool_param(req, "brief", &desc->serialize_brief);
    flecs_rest_bool_param(req, "link", &desc->serialize_link);
    flecs_rest_bool_param(req, "color", &desc->serialize_color);
    flecs_rest_bool_param(req, "ids", &desc->serialize_ids);
    flecs_rest_bool_param(req, "id_labels", &desc->serialize_id_labels);
    flecs_rest_bool_param(req, "base", &desc->serialize_base);
    flecs_rest_bool_param(req, "values", &desc->serialize_values);
    flecs_rest_bool_param(req, "private", &desc->serialize_private);
    flecs_rest_bool_param(req, "type_info", &desc->serialize_type_info);
    flecs_rest_bool_param(req, "matches", &desc->serialize_matches);
    flecs_rest_bool_param(req, "alerts", &desc->serialize_alerts);

    char *rel = NULL;
    flecs_rest_string_param(req, "refs", &rel);
    if (rel) {
        desc->serialize_refs = ecs_lookup(world, rel);
    }
}

static
void flecs_rest_parse_json_ser_iter_params(
    ecs_iter_to_json_desc_t *desc,
    const ecs_http_request_t *req)
{
    flecs_rest_bool_param(req, "term_ids", &desc->serialize_term_ids);
    flecs_rest_bool_param(req, "term_labels", &desc->serialize_term_labels);
    flecs_rest_bool_param(req, "ids", &desc->serialize_ids);
    flecs_rest_bool_param(req, "id_labels", &desc->serialize_id_labels);
    flecs_rest_bool_param(req, "sources", &desc->serialize_sources);
    flecs_rest_bool_param(req, "variables", &desc->serialize_variables);
    flecs_rest_bool_param(req, "is_set", &desc->serialize_is_set);
    flecs_rest_bool_param(req, "values", &desc->serialize_values);
    flecs_rest_bool_param(req, "private", &desc->serialize_private);
    flecs_rest_bool_param(req, "entities", &desc->serialize_entities);
    flecs_rest_bool_param(req, "entity_labels", &desc->serialize_entity_labels);
    flecs_rest_bool_param(req, "variable_labels", &desc->serialize_variable_labels);
    flecs_rest_bool_param(req, "variable_ids", &desc->serialize_variable_ids);
    flecs_rest_bool_param(req, "colors", &desc->serialize_colors);
    flecs_rest_bool_param(req, "duration", &desc->measure_eval_duration);
    flecs_rest_bool_param(req, "type_info", &desc->serialize_type_info);
    flecs_rest_bool_param(req, "field_info", &desc->serialize_field_info);
    flecs_rest_bool_param(req, "query_info", &desc->serialize_query_info);
    flecs_rest_bool_param(req, "query_plan", &desc->serialize_query_plan);
    flecs_rest_bool_param(req, "query_profile", &desc->serialize_query_profile);
    flecs_rest_bool_param(req, "table", &desc->serialize_table);
    flecs_rest_bool_param(req, "rows", &desc->serialize_rows);
    bool results = true;
    flecs_rest_bool_param(req, "results", &results);
    desc->dont_serialize_results = !results;
}

static
bool flecs_rest_reply_entity(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    char *path = &req->path[7];
    ecs_dbg_2("rest: request entity '%s'", path);

    ecs_entity_t e = ecs_lookup_path_w_sep(
        world, 0, path, "/", NULL, false);
    if (!e) {
        ecs_dbg_2("rest: entity '%s' not found", path);
        flecs_reply_error(reply, "entity '%s' not found", path);
        reply->code = 404;
        return true;
    }

    ecs_entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;
    flecs_rest_parse_json_ser_entity_params(world, &desc, req);
    if (ecs_entity_to_json_buf(world, e, &reply->body, &desc) != 0) {
        ecs_strbuf_reset(&reply->body);
        reply->code = 500;
        reply->status = "Internal server error";
        return true;
    }
    return true;
}

static
bool flecs_rest_reply_world(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    (void)req;
    if (ecs_world_to_json_buf(world, &reply->body, NULL) != 0) {
        ecs_strbuf_reset(&reply->body);
        reply->code = 500;
        reply->status = "Internal server error";
        return true;
    }
    return true;
}

static
ecs_entity_t flecs_rest_entity_from_path(
    ecs_world_t *world,
    ecs_http_reply_t *reply,
    const char *path)
{
    ecs_entity_t e = ecs_lookup_path_w_sep(
        world, 0, path, "/", NULL, false);
    if (!e) {
        flecs_reply_error(reply, "entity '%s' not found", path);
        reply->code = 404;
    }
    return e;
}

static
bool flecs_rest_set(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply,
    const char *path)
{
    ecs_entity_t e;
    if (!(e = flecs_rest_entity_from_path(world, reply, path))) {
        return true;
    }

    const char *data = ecs_http_get_param(req, "data");
    ecs_from_json_desc_t desc = {0};
    desc.expr = data;
    desc.name = path;
    if (ecs_entity_from_json(world, e, data, &desc) == NULL) {
        flecs_reply_error(reply, "invalid request");
        reply->code = 400;
        return true;
    }
    
    return true;
}

static
bool flecs_rest_delete(
    ecs_world_t *world,
    ecs_http_reply_t *reply,
    const char *path)
{
    ecs_entity_t e;
    if (!(e = flecs_rest_entity_from_path(world, reply, path))) {
        return true;
    }

    ecs_delete(world, e);
    
    return true;
}

static
bool flecs_rest_enable(
    ecs_world_t *world,
    ecs_http_reply_t *reply,
    const char *path,
    bool enable)
{
    ecs_entity_t e;
    if (!(e = flecs_rest_entity_from_path(world, reply, path))) {
        return true;
    }

    ecs_enable(world, e, enable);
    
    return true;
}

static
bool flecs_rest_script(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    (void)world;
    (void)req;
    (void)reply;
#ifdef FLECS_PLECS
    const char *data = ecs_http_get_param(req, "data");
    if (!data) {
        flecs_reply_error(reply, "missing data parameter");
        return true;
    }

    bool prev_color = ecs_log_enable_colors(false);
    rest_prev_log = ecs_os_api.log_;
    ecs_os_api.log_ = flecs_rest_capture_log;

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "scripts.main" }),
        .str = data
    });

    if (!script) {
        char *err = flecs_rest_get_captured_log();
        char *escaped_err = ecs_astresc('"', err);
        flecs_reply_error(reply, escaped_err);
        reply->code = 400; /* bad request */
        ecs_os_free(escaped_err);
        ecs_os_free(err);
    }

    ecs_os_api.log_ = rest_prev_log;
    ecs_log_enable_colors(prev_color);

    return true;
#else
    return false;
#endif
}

static
void flecs_rest_reply_set_captured_log(
    ecs_http_reply_t *reply)
{
    char *err = flecs_rest_get_captured_log();
    if (err) {
        char *escaped_err = ecs_astresc('"', err);
        flecs_reply_error(reply, escaped_err);
        ecs_os_free(escaped_err);
        ecs_os_free(err);
    }

    reply->code = 400;
}

static
void flecs_rest_iter_to_reply(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply,
    ecs_poly_t *query,
    ecs_iter_t *it)
{
    ecs_iter_to_json_desc_t desc = {0};
    desc.serialize_entities = true;
    desc.serialize_variables = true;
    flecs_rest_parse_json_ser_iter_params(&desc, req);
    desc.query = query;

    int32_t offset = 0;
    int32_t limit = 1000;

    flecs_rest_int_param(req, "offset", &offset);
    flecs_rest_int_param(req, "limit", &limit);

    if (offset < 0 || limit < 0) {
        flecs_reply_error(reply, "invalid offset/limit parameter");
        return;
    }

    ecs_iter_t pit = ecs_page_iter(it, offset, limit);
    if (ecs_iter_to_json_buf(world, &pit, &reply->body, &desc)) {
        flecs_rest_reply_set_captured_log(reply);
    }

    flecs_rest_int_param(req, "offset", &offset);
}

static
bool flecs_rest_reply_existing_query(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply,
    const char *name)
{
    ecs_entity_t q = ecs_lookup(world, name);
    if (!q) {
        flecs_reply_error(reply, "unresolved identifier '%s'", name);
        reply->code = 404;
        return true;
    }

    ecs_poly_t *poly = NULL;
    const EcsPoly *poly_comp = ecs_get_pair(world, q, EcsPoly, EcsQuery);
    if (!poly_comp) {
        poly_comp = ecs_get_pair(world, q, EcsPoly, EcsObserver);
        if (poly_comp) {
            poly = &((ecs_observer_t*)poly_comp->poly)->filter;
        } else {
            flecs_reply_error(reply, 
                "resolved identifier '%s' is not a query", name);
            reply->code = 400;
            return true;
        }
    } else {
        poly = poly_comp->poly;
    }

    if (!poly) {
        flecs_reply_error(reply, "query '%s' is not initialized", name);
        reply->code = 400;
        return true;
    }

    ecs_iter_t it;
    ecs_iter_poly(world, poly, &it, NULL);

    ecs_dbg_2("rest: request query '%s'", q);
    bool prev_color = ecs_log_enable_colors(false);
    rest_prev_log = ecs_os_api.log_;
    ecs_os_api.log_ = flecs_rest_capture_log;

    const char *vars = ecs_http_get_param(req, "vars");
    if (vars) {
        if (!ecs_poly_is(poly, ecs_rule_t)) {
            flecs_reply_error(reply, 
                "variables are only supported for rule queries");
            reply->code = 400;
            return true;
        }
        if (ecs_rule_parse_vars(poly, &it, vars) == NULL) {
            flecs_rest_reply_set_captured_log(reply);
            return true;
        }
    }

    flecs_rest_iter_to_reply(world, req, reply, poly, &it);

    ecs_os_api.log_ = rest_prev_log;
    ecs_log_enable_colors(prev_color);    

    return true;
}

static
bool flecs_rest_reply_query(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    const char *q_name = ecs_http_get_param(req, "name");
    if (q_name) {
        return flecs_rest_reply_existing_query(world, req, reply, q_name);
    }

    const char *q = ecs_http_get_param(req, "q");
    if (!q) {
        ecs_strbuf_appendlit(&reply->body, "Missing parameter 'q'");
        reply->code = 400; /* bad request */
        return true;
    }

    bool try = false;
    flecs_rest_bool_param(req, "try", &try);

    ecs_dbg_2("rest: request query '%s'", q);
    bool prev_color = ecs_log_enable_colors(false);
    rest_prev_log = ecs_os_api.log_;
    ecs_os_api.log_ = flecs_rest_capture_log;

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = q
    });
    if (!r) {
        flecs_rest_reply_set_captured_log(reply);
        if (try) {
            /* If client is trying queries, don't spam console with errors */
            reply->code = 200;
        }
    } else {
        ecs_iter_t it = ecs_rule_iter(world, r);
        flecs_rest_iter_to_reply(world, req, reply, r, &it);
        ecs_rule_fini(r);
    }

    ecs_os_api.log_ = rest_prev_log;
    ecs_log_enable_colors(prev_color);

    return true;
}

#ifdef FLECS_MONITOR

static
void flecs_rest_array_append_(
    ecs_strbuf_t *reply,
    const char *field,
    int32_t field_len,
    const ecs_float_t *values,
    int32_t t)
{
    ecs_strbuf_list_appendch(reply, '"');
    ecs_strbuf_appendstrn(reply, field, field_len);
    ecs_strbuf_appendlit(reply, "\":");
    ecs_strbuf_list_push(reply, "[", ",");

    int32_t i;
    for (i = t + 1; i <= (t + ECS_STAT_WINDOW); i ++) {
        int32_t index = i % ECS_STAT_WINDOW;
        ecs_strbuf_list_next(reply);
        ecs_strbuf_appendflt(reply, (double)values[index], '"');
    }

    ecs_strbuf_list_pop(reply, "]");
}

#define flecs_rest_array_append(reply, field, values, t)\
    flecs_rest_array_append_(reply, field, sizeof(field) - 1, values, t)

static
void flecs_rest_gauge_append(
    ecs_strbuf_t *reply,
    const ecs_metric_t *m,
    const char *field,
    int32_t field_len,
    int32_t t,
    const char *brief,
    int32_t brief_len)
{
    ecs_strbuf_list_appendch(reply, '"');
    ecs_strbuf_appendstrn(reply, field, field_len);
    ecs_strbuf_appendlit(reply, "\":");
    ecs_strbuf_list_push(reply, "{", ",");

    flecs_rest_array_append(reply, "avg", m->gauge.avg, t);
    flecs_rest_array_append(reply, "min", m->gauge.min, t);
    flecs_rest_array_append(reply, "max", m->gauge.max, t);

    if (brief) {
        ecs_strbuf_list_appendlit(reply, "\"brief\":\"");
        ecs_strbuf_appendstrn(reply, brief, brief_len);
        ecs_strbuf_appendch(reply, '"');
    }

    ecs_strbuf_list_pop(reply, "}");
}

static
void flecs_rest_counter_append(
    ecs_strbuf_t *reply,
    const ecs_metric_t *m,
    const char *field,
    int32_t field_len,
    int32_t t,
    const char *brief,
    int32_t brief_len)
{
    flecs_rest_gauge_append(reply, m, field, field_len, t, brief, brief_len);
}

#define ECS_GAUGE_APPEND_T(reply, s, field, t, brief)\
    flecs_rest_gauge_append(reply, &(s)->field, #field, sizeof(#field) - 1, t, brief, sizeof(brief) - 1)

#define ECS_COUNTER_APPEND_T(reply, s, field, t, brief)\
    flecs_rest_counter_append(reply, &(s)->field, #field, sizeof(#field) - 1, t, brief, sizeof(brief) - 1)

#define ECS_GAUGE_APPEND(reply, s, field, brief)\
    ECS_GAUGE_APPEND_T(reply, s, field, (s)->t, brief)

#define ECS_COUNTER_APPEND(reply, s, field, brief)\
    ECS_COUNTER_APPEND_T(reply, s, field, (s)->t, brief)

static
void flecs_world_stats_to_json(
    ecs_strbuf_t *reply,
    const EcsWorldStats *monitor_stats)
{
    const ecs_world_stats_t *stats = &monitor_stats->stats;

    ecs_strbuf_list_push(reply, "{", ",");
    ECS_GAUGE_APPEND(reply, stats, entities.count, "Alive entity ids in the world");
    ECS_GAUGE_APPEND(reply, stats, entities.not_alive_count, "Not alive entity ids in the world");

    ECS_GAUGE_APPEND(reply, stats, performance.fps, "Frames per second");
    ECS_COUNTER_APPEND(reply, stats, performance.frame_time, "Time spent in frame");
    ECS_COUNTER_APPEND(reply, stats, performance.system_time, "Time spent on running systems in frame");
    ECS_COUNTER_APPEND(reply, stats, performance.emit_time, "Time spent on notifying observers in frame");
    ECS_COUNTER_APPEND(reply, stats, performance.merge_time, "Time spent on merging commands in frame");
    ECS_COUNTER_APPEND(reply, stats, performance.rematch_time, "Time spent on revalidating query caches in frame");

    ECS_COUNTER_APPEND(reply, stats, commands.add_count, "Add commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.remove_count, "Remove commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.delete_count, "Delete commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.clear_count, "Clear commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.set_count, "Set commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.ensure_count, "Get_mut commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.modified_count, "Modified commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.other_count, "Misc commands executed");
    ECS_COUNTER_APPEND(reply, stats, commands.discard_count, "Commands for already deleted entities");
    ECS_COUNTER_APPEND(reply, stats, commands.batched_entity_count, "Entities with batched commands");
    ECS_COUNTER_APPEND(reply, stats, commands.batched_count, "Number of commands batched");

    ECS_COUNTER_APPEND(reply, stats, frame.merge_count, "Number of merges (sync points)");
    ECS_COUNTER_APPEND(reply, stats, frame.pipeline_build_count, "Pipeline rebuilds (happen when systems become active/enabled)");
    ECS_COUNTER_APPEND(reply, stats, frame.systems_ran, "Systems ran in frame");
    ECS_COUNTER_APPEND(reply, stats, frame.observers_ran, "Number of times an observer was invoked in frame");
    ECS_COUNTER_APPEND(reply, stats, frame.event_emit_count, "Events emitted in frame");
    ECS_COUNTER_APPEND(reply, stats, frame.rematch_count, "Number of query cache revalidations");

    ECS_GAUGE_APPEND(reply, stats, tables.count, "Tables in the world (including empty)");
    ECS_GAUGE_APPEND(reply, stats, tables.empty_count, "Empty tables in the world");
    ECS_COUNTER_APPEND(reply, stats, tables.create_count, "Number of new tables created");
    ECS_COUNTER_APPEND(reply, stats, tables.delete_count, "Number of tables deleted");

    ECS_GAUGE_APPEND(reply, stats, components.tag_count, "Tag ids in use");
    ECS_GAUGE_APPEND(reply, stats, components.component_count, "Component ids in use");
    ECS_GAUGE_APPEND(reply, stats, components.pair_count, "Pair ids in use");
    ECS_GAUGE_APPEND(reply, stats, components.type_count, "Registered component types");
    ECS_COUNTER_APPEND(reply, stats, components.create_count, "Number of new component, tag and pair ids created");
    ECS_COUNTER_APPEND(reply, stats, components.delete_count, "Number of component, pair and tag ids deleted");

    ECS_GAUGE_APPEND(reply, stats, queries.query_count, "Queries in the world");
    ECS_GAUGE_APPEND(reply, stats, queries.observer_count, "Observers in the world");
    ECS_GAUGE_APPEND(reply, stats, queries.system_count, "Systems in the world");

    ECS_COUNTER_APPEND(reply, stats, memory.alloc_count, "Allocations by OS API");
    ECS_COUNTER_APPEND(reply, stats, memory.realloc_count, "Reallocs by OS API");
    ECS_COUNTER_APPEND(reply, stats, memory.free_count, "Frees by OS API");
    ECS_GAUGE_APPEND(reply, stats, memory.outstanding_alloc_count, "Outstanding allocations by OS API");
    ECS_COUNTER_APPEND(reply, stats, memory.block_alloc_count, "Blocks allocated by block allocators");
    ECS_COUNTER_APPEND(reply, stats, memory.block_free_count, "Blocks freed by block allocators");
    ECS_GAUGE_APPEND(reply, stats, memory.block_outstanding_alloc_count, "Outstanding block allocations");
    ECS_COUNTER_APPEND(reply, stats, memory.stack_alloc_count, "Pages allocated by stack allocators");
    ECS_COUNTER_APPEND(reply, stats, memory.stack_free_count, "Pages freed by stack allocators");
    ECS_GAUGE_APPEND(reply, stats, memory.stack_outstanding_alloc_count, "Outstanding page allocations");

    ECS_COUNTER_APPEND(reply, stats, http.request_received_count, "Received requests");
    ECS_COUNTER_APPEND(reply, stats, http.request_invalid_count, "Received invalid requests");
    ECS_COUNTER_APPEND(reply, stats, http.request_handled_ok_count, "Requests handled successfully");
    ECS_COUNTER_APPEND(reply, stats, http.request_handled_error_count, "Requests handled with error code");
    ECS_COUNTER_APPEND(reply, stats, http.request_not_handled_count, "Requests not handled (unknown endpoint)");
    ECS_COUNTER_APPEND(reply, stats, http.request_preflight_count, "Preflight requests received");
    ECS_COUNTER_APPEND(reply, stats, http.send_ok_count, "Successful replies");
    ECS_COUNTER_APPEND(reply, stats, http.send_error_count, "Unsuccessful replies");
    ECS_COUNTER_APPEND(reply, stats, http.busy_count, "Dropped requests due to full send queue (503)");

    ecs_strbuf_list_pop(reply, "}");
}

static
void flecs_system_stats_to_json(
    ecs_world_t *world,
    ecs_strbuf_t *reply,
    ecs_entity_t system,
    const ecs_system_stats_t *stats)
{
    ecs_strbuf_list_push(reply, "{", ",");
    ecs_strbuf_list_appendlit(reply, "\"name\":\"");
    ecs_get_path_w_sep_buf(world, 0, system, ".", NULL, reply);
    ecs_strbuf_appendch(reply, '"');

    if (!stats->task) {
        ECS_GAUGE_APPEND(reply, &stats->query, matched_table_count, "");
        ECS_GAUGE_APPEND(reply, &stats->query, matched_entity_count, "");
    }

    ECS_COUNTER_APPEND_T(reply, stats, time_spent, stats->query.t, "");
    ecs_strbuf_list_pop(reply, "}");
}

static
void flecs_pipeline_stats_to_json(
    ecs_world_t *world,
    ecs_strbuf_t *reply,
    const EcsPipelineStats *stats)
{
    ecs_strbuf_list_push(reply, "[", ",");

    int32_t i, count = ecs_vec_count(&stats->stats.systems), sync_cur = 0;
    ecs_entity_t *ids = ecs_vec_first_t(&stats->stats.systems, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        ecs_entity_t id = ids[i];
        
        ecs_strbuf_list_next(reply);

        if (id) {
            ecs_system_stats_t *sys_stats = ecs_map_get_deref(
                &stats->stats.system_stats, ecs_system_stats_t, id);
            flecs_system_stats_to_json(world, reply, id, sys_stats);
        } else {
            /* Sync point */
            ecs_strbuf_list_push(reply, "{", ",");
            ecs_sync_stats_t *sync_stats = ecs_vec_get_t(
                &stats->stats.sync_points, ecs_sync_stats_t, sync_cur);

            ecs_strbuf_list_appendlit(reply, "\"system_count\":");
            ecs_strbuf_appendint(reply, sync_stats->system_count);

            ecs_strbuf_list_appendlit(reply, "\"multi_threaded\":");
            ecs_strbuf_appendbool(reply, sync_stats->multi_threaded);

            ecs_strbuf_list_appendlit(reply, "\"no_readonly\":");
            ecs_strbuf_appendbool(reply, sync_stats->no_readonly);

            ECS_GAUGE_APPEND_T(reply, sync_stats, 
                time_spent, stats->stats.t, "");
            ECS_GAUGE_APPEND_T(reply, sync_stats, 
                commands_enqueued, stats->stats.t, "");

            ecs_strbuf_list_pop(reply, "}");
            sync_cur ++;
        }
    }

    ecs_strbuf_list_pop(reply, "]");
}

static
bool flecs_rest_reply_stats(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    char *period_str = NULL;
    flecs_rest_string_param(req, "period", &period_str);
    char *category = &req->path[6];

    ecs_entity_t period = EcsPeriod1s;
    if (period_str) {
        char *period_name = ecs_asprintf("Period%s", period_str);
        period = ecs_lookup_child(world, ecs_id(FlecsMonitor), period_name);
        ecs_os_free(period_name);
        if (!period) {
            flecs_reply_error(reply, "bad request (invalid period string)");
            reply->code = 400;
            return false;
        }
    }

    if (!ecs_os_strcmp(category, "world")) {
        const EcsWorldStats *stats = ecs_get_pair(world, EcsWorld, 
            EcsWorldStats, period);
        flecs_world_stats_to_json(&reply->body, stats);
        return true;

    } else if (!ecs_os_strcmp(category, "pipeline")) {
        const EcsPipelineStats *stats = ecs_get_pair(world, EcsWorld, 
            EcsPipelineStats, period);
        flecs_pipeline_stats_to_json(world, &reply->body, stats);
        return true;

    } else {
        flecs_reply_error(reply, "bad request (unsupported category)");
        reply->code = 400;
        return false;
    }
}
#else
static
bool flecs_rest_reply_stats(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    (void)world;
    (void)req;
    (void)reply;
    return false;
}
#endif

static
void flecs_rest_reply_table_append_type(
    ecs_world_t *world,
    ecs_strbuf_t *reply,
    const ecs_table_t *table)
{
    ecs_strbuf_list_push(reply, "[", ",");
    int32_t i, count = table->type.count;
    ecs_id_t *ids = table->type.array;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_list_next(reply);
        ecs_strbuf_appendch(reply, '"');
        ecs_id_str_buf(world, ids[i], reply);
        ecs_strbuf_appendch(reply, '"');
    }
    ecs_strbuf_list_pop(reply, "]");
}

static
void flecs_rest_reply_table_append_memory(
    ecs_strbuf_t *reply,
    const ecs_table_t *table)
{
    int32_t used = 0, allocated = 0;

    used += table->data.entities.count * ECS_SIZEOF(ecs_entity_t);
    allocated += table->data.entities.size * ECS_SIZEOF(ecs_entity_t);

    int32_t i, storage_count = table->column_count;
    ecs_column_t *columns = table->data.columns;

    for (i = 0; i < storage_count; i ++) {
        used += columns[i].data.count * columns[i].ti->size;
        allocated += columns[i].data.size * columns[i].ti->size;
    }

    ecs_strbuf_list_push(reply, "{", ",");
    ecs_strbuf_list_append(reply, "\"used\":%d", used);
    ecs_strbuf_list_append(reply, "\"allocated\":%d", allocated);
    ecs_strbuf_list_pop(reply, "}");
}

static
void flecs_rest_reply_table_append(
    ecs_world_t *world,
    ecs_strbuf_t *reply,
    const ecs_table_t *table)
{
    ecs_strbuf_list_next(reply);
    ecs_strbuf_list_push(reply, "{", ",");
    ecs_strbuf_list_append(reply, "\"id\":%u", (uint32_t)table->id);
    ecs_strbuf_list_appendstr(reply, "\"type\":");
    flecs_rest_reply_table_append_type(world, reply, table);
    ecs_strbuf_list_append(reply, "\"count\":%d", ecs_table_count(table));
    ecs_strbuf_list_append(reply, "\"memory\":");
    flecs_rest_reply_table_append_memory(reply, table);
    ecs_strbuf_list_pop(reply, "}");
}

static
bool flecs_rest_reply_tables(
    ecs_world_t *world,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    (void)req;

    ecs_strbuf_list_push(&reply->body, "[", ",");
    ecs_sparse_t *tables = &world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense_t(tables, ecs_table_t, i);
        flecs_rest_reply_table_append(world, &reply->body, table);
    }
    ecs_strbuf_list_pop(&reply->body, "]");

    return true;
}

static
const char* flecs_rest_cmd_kind_to_str(
    ecs_cmd_kind_t kind)
{
    switch(kind) {
    case EcsCmdClone: return "Clone";
    case EcsCmdBulkNew: return "BulkNew";
    case EcsCmdAdd: return "Add";
    case EcsCmdRemove: return "Remove";
    case EcsCmdSet: return "Set";
    case EcsCmdEmplace: return "Emplace";
    case EcsCmdEnsure: return "Ensure";
    case EcsCmdModified: return "Modified";
    case EcsCmdModifiedNoHook: return "ModifiedNoHook";
    case EcsCmdAddModified: return "AddModified";
    case EcsCmdPath: return "Path";
    case EcsCmdDelete: return "Delete";
    case EcsCmdClear: return "Clear";
    case EcsCmdOnDeleteAction: return "OnDeleteAction";
    case EcsCmdEnable: return "Enable";
    case EcsCmdDisable: return "Disable";
    case EcsCmdEvent: return "Event";
    case EcsCmdSkip: return "Skip";
    default: return "Unknown";
    }
}

static
bool flecs_rest_cmd_has_id(
    const ecs_cmd_t *cmd)
{
    switch(cmd->kind) {
    case EcsCmdClear:
    case EcsCmdDelete:
    case EcsCmdClone:
    case EcsCmdDisable:
    case EcsCmdPath:
        return false;
    case EcsCmdBulkNew:
    case EcsCmdAdd:
    case EcsCmdRemove:
    case EcsCmdSet:
    case EcsCmdEmplace:
    case EcsCmdEnsure:
    case EcsCmdModified:
    case EcsCmdModifiedNoHook:
    case EcsCmdAddModified:
    case EcsCmdOnDeleteAction:
    case EcsCmdEnable:
    case EcsCmdEvent:
    case EcsCmdSkip:
    default:
        return true;
    }
}

static
void flecs_rest_server_garbage_collect_all(
    ecs_rest_ctx_t *impl)
{
    ecs_map_iter_t it = ecs_map_iter(&impl->cmd_captures);

    while (ecs_map_next(&it)) {
        ecs_rest_cmd_capture_t *capture = ecs_map_ptr(&it);
        int32_t i, count = ecs_vec_count(&capture->syncs);
        ecs_rest_cmd_sync_capture_t *syncs = ecs_vec_first(&capture->syncs);
        for (i = 0; i < count; i ++) {
            ecs_rest_cmd_sync_capture_t *sync = &syncs[i];
            ecs_os_free(sync->cmds);
        }
        ecs_vec_fini_t(NULL, &capture->syncs, ecs_rest_cmd_sync_capture_t);
        ecs_os_free(capture);
    }

    ecs_map_fini(&impl->cmd_captures);
}

static
void flecs_rest_server_garbage_collect(
    ecs_world_t *world,
    ecs_rest_ctx_t *impl)
{
    const ecs_world_info_t *wi = ecs_get_world_info(world);
    ecs_map_iter_t it = ecs_map_iter(&impl->cmd_captures);
    ecs_vec_t removed_frames = {0};

    while (ecs_map_next(&it)) {
        int64_t frame = flecs_uto(int64_t, ecs_map_key(&it));
        if ((wi->frame_count_total - frame) > FLECS_REST_COMMAND_RETAIN_COUNT) {
            ecs_rest_cmd_capture_t *capture = ecs_map_ptr(&it);
            int32_t i, count = ecs_vec_count(&capture->syncs);
            ecs_rest_cmd_sync_capture_t *syncs = ecs_vec_first(&capture->syncs);
            for (i = 0; i < count; i ++) {
                ecs_rest_cmd_sync_capture_t *sync = &syncs[i];
                ecs_os_free(sync->cmds);
            }
            ecs_vec_fini_t(NULL, &capture->syncs, ecs_rest_cmd_sync_capture_t);
            ecs_os_free(capture);

            ecs_vec_init_if_t(&removed_frames, int64_t);
            ecs_vec_append_t(NULL, &removed_frames, int64_t)[0] = frame;
        }
    }

    int32_t i, count = ecs_vec_count(&removed_frames);
    if (count) {
        int64_t *frames = ecs_vec_first(&removed_frames);
        if (count) {
            for (i = 0; i < count; i ++) {
                ecs_map_remove(&impl->cmd_captures, 
                    flecs_ito(uint64_t, frames[i]));
            }
        }
        ecs_vec_fini_t(NULL, &removed_frames, int64_t);
    }
}

static
void flecs_rest_cmd_to_json(
    ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_cmd_t *cmd)
{
    ecs_strbuf_list_push(buf, "{", ",");

    ecs_strbuf_list_appendlit(buf, "\"kind\":\"");
        ecs_strbuf_appendstr(buf, flecs_rest_cmd_kind_to_str(cmd->kind));
        ecs_strbuf_appendlit(buf, "\"");

    if (flecs_rest_cmd_has_id(cmd)) {
        ecs_strbuf_list_appendlit(buf, "\"id\":\"");
            char *idstr = ecs_id_str(world, cmd->id);
            ecs_strbuf_appendstr(buf, idstr);
            ecs_strbuf_appendlit(buf, "\"");
            ecs_os_free(idstr);
    }

    if (cmd->system) {
        ecs_strbuf_list_appendlit(buf, "\"system\":\"");
            char *sysstr = ecs_get_fullpath(world, cmd->system);
            ecs_strbuf_appendstr(buf, sysstr);
            ecs_strbuf_appendlit(buf, "\"");
            ecs_os_free(sysstr); 
    }

    if (cmd->kind == EcsCmdBulkNew) {
        /* Todo */
    } else if (cmd->kind == EcsCmdEvent) {
        /* Todo */
    } else {
        if (cmd->entity) {
            ecs_strbuf_list_appendlit(buf, "\"entity\":\"");
                char *path = ecs_get_path_w_sep(world, 0, cmd->entity, ".", "");
                ecs_strbuf_appendstr(buf, path);
                ecs_strbuf_appendlit(buf, "\"");
                ecs_os_free(path);

            ecs_strbuf_list_appendlit(buf, "\"is_alive\":\"");
                if (ecs_is_alive(world, cmd->entity)) {
                    ecs_strbuf_appendlit(buf, "true");
                } else {
                    ecs_strbuf_appendlit(buf, "false");
                }
                ecs_strbuf_appendlit(buf, "\"");

            ecs_strbuf_list_appendlit(buf, "\"next_for_entity\":");
                ecs_strbuf_appendint(buf, cmd->next_for_entity);
        }
    }

    ecs_strbuf_list_pop(buf, "}");
}

static
void flecs_rest_on_commands(
    const ecs_stage_t *stage,
    const ecs_vec_t *commands,
    void *ctx)
{
    ecs_world_t *world = stage->world;
    ecs_rest_cmd_capture_t *capture = ctx;
    ecs_assert(capture != NULL, ECS_INTERNAL_ERROR, NULL);

    if (commands) {
        ecs_vec_init_if_t(&capture->syncs, ecs_rest_cmd_sync_capture_t);
        ecs_rest_cmd_sync_capture_t *sync = ecs_vec_append_t(
            NULL, &capture->syncs, ecs_rest_cmd_sync_capture_t);

        int32_t i, count = ecs_vec_count(commands);
        ecs_cmd_t *cmds = ecs_vec_first(commands);
        sync->buf = ECS_STRBUF_INIT;
        ecs_strbuf_list_push(&sync->buf, "{", ",");
        ecs_strbuf_list_appendlit(&sync->buf, "\"commands\":");
            ecs_strbuf_list_push(&sync->buf, "[", ",");
            for (i = 0; i < count; i ++) {
                ecs_strbuf_list_next(&sync->buf);
                flecs_rest_cmd_to_json(world, &sync->buf, &cmds[i]);
            }
            ecs_strbuf_list_pop(&sync->buf, "]");

        /* Measure how long it takes to process queue */
        sync->start_time = (ecs_time_t){0};
        ecs_time_measure(&sync->start_time);
    } else {
        /* Finished processing queue, measure duration */
        ecs_rest_cmd_sync_capture_t *sync = ecs_vec_last_t(
            &capture->syncs, ecs_rest_cmd_sync_capture_t);
        double duration = ecs_time_measure(&sync->start_time);

        ecs_strbuf_list_appendlit(&sync->buf, "\"duration\":");
        ecs_strbuf_appendflt(&sync->buf, duration, '"');
        ecs_strbuf_list_pop(&sync->buf, "}");

        sync->cmds = ecs_strbuf_get(&sync->buf);
    }
}

static
bool flecs_rest_reply_commands_capture(
    ecs_world_t *world,
    ecs_rest_ctx_t *impl,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    (void)req;
    const ecs_world_info_t *wi = ecs_get_world_info(world);
    ecs_strbuf_appendstr(&reply->body, "{");
    ecs_strbuf_append(&reply->body, "\"frame\":%u", wi->frame_count_total);
    ecs_strbuf_appendstr(&reply->body, "}");
    
    ecs_map_init_if(&impl->cmd_captures, &world->allocator);
    ecs_rest_cmd_capture_t *capture = ecs_map_ensure_alloc_t(
        &impl->cmd_captures, ecs_rest_cmd_capture_t, 
        flecs_ito(uint64_t, wi->frame_count_total));

    world->on_commands = flecs_rest_on_commands;
    world->on_commands_ctx = capture;

    /* Run garbage collection so that requests don't linger */
    flecs_rest_server_garbage_collect(world, impl);

    return true;
}

static
bool flecs_rest_reply_commands_request(
    ecs_world_t *world,
    ecs_rest_ctx_t *impl,
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply)
{
    (void)world;
    char *frame_str = &req->path[15];
    int32_t frame = atoi(frame_str);
    
    ecs_map_init_if(&impl->cmd_captures, &world->allocator);
    const ecs_rest_cmd_capture_t *capture = ecs_map_get_deref(
        &impl->cmd_captures, ecs_rest_cmd_capture_t, 
        flecs_ito(uint64_t, frame));

    if (!capture) {
        ecs_strbuf_appendstr(&reply->body, "{");
        ecs_strbuf_append(&reply->body, 
            "\"error\": \"no capture for frame %u\"", frame);
        ecs_strbuf_appendstr(&reply->body, "}"); 
        reply->code = 404;
        return true;
    }

    ecs_strbuf_appendstr(&reply->body, "{");
    ecs_strbuf_list_append(&reply->body, "\"syncs\":");
    ecs_strbuf_list_push(&reply->body, "[", ",");

    int32_t i, count = ecs_vec_count(&capture->syncs);
    ecs_rest_cmd_sync_capture_t *syncs = ecs_vec_first(&capture->syncs);

    for (i = 0; i < count; i ++) {
        ecs_rest_cmd_sync_capture_t *sync = &syncs[i];
        ecs_strbuf_list_appendstr(&reply->body, sync->cmds);
    }

    ecs_strbuf_list_pop(&reply->body, "]");
    ecs_strbuf_appendstr(&reply->body, "}");

    return true;
}

static
bool flecs_rest_reply(
    const ecs_http_request_t* req,
    ecs_http_reply_t *reply,
    void *ctx)
{
    ecs_rest_ctx_t *impl = ctx;
    ecs_world_t *world = impl->world;

    if (req->path == NULL) {
        ecs_dbg("rest: bad request (missing path)");
        flecs_reply_error(reply, "bad request (missing path)");
        reply->code = 400;
        return false;
    }

    if (req->method == EcsHttpGet) {
        /* Entity endpoint */
        if (!ecs_os_strncmp(req->path, "entity/", 7)) {
            return flecs_rest_reply_entity(world, req, reply);

        /* Query endpoint */
        } else if (!ecs_os_strcmp(req->path, "query")) {
            return flecs_rest_reply_query(world, req, reply);

        /* World endpoint */
        } else if (!ecs_os_strcmp(req->path, "world")) {
            return flecs_rest_reply_world(world, req, reply);

        /* Stats endpoint */
        } else if (!ecs_os_strncmp(req->path, "stats/", 6)) {
            return flecs_rest_reply_stats(world, req, reply);

        /* Tables endpoint */
        } else if (!ecs_os_strncmp(req->path, "tables", 6)) {
            return flecs_rest_reply_tables(world, req, reply);

        /* Commands capture endpoint */
        } else if (!ecs_os_strncmp(req->path, "commands/capture", 16)) {
            return flecs_rest_reply_commands_capture(world, impl, req, reply);

        /* Commands request endpoint (request commands from specific frame) */
        } else if (!ecs_os_strncmp(req->path, "commands/frame/", 15)) {
            return flecs_rest_reply_commands_request(world, impl, req, reply);
        }

    } else if (req->method == EcsHttpPut) {
        /* Set endpoint */
        if (!ecs_os_strncmp(req->path, "set/", 4)) {
            return flecs_rest_set(world, req, reply, &req->path[4]);
        
        /* Delete endpoint */
        } else if (!ecs_os_strncmp(req->path, "delete/", 7)) {
            return flecs_rest_delete(world, reply, &req->path[7]);

        /* Enable endpoint */
        } else if (!ecs_os_strncmp(req->path, "enable/", 7)) {
            return flecs_rest_enable(world, reply, &req->path[7], true);

        /* Disable endpoint */
        } else if (!ecs_os_strncmp(req->path, "disable/", 8)) {
            return flecs_rest_enable(world, reply, &req->path[8], false);

        /* Script endpoint */
        } else if (!ecs_os_strncmp(req->path, "script", 6)) {
            return flecs_rest_script(world, req, reply);
        }
    }

    return false;
}

ecs_http_server_t* ecs_rest_server_init(
    ecs_world_t *world,
    const ecs_http_server_desc_t *desc)
{
    ecs_rest_ctx_t *srv_ctx = ecs_os_calloc_t(ecs_rest_ctx_t);
    ecs_http_server_desc_t private_desc = {0};
    if (desc) {
        private_desc = *desc;
    }
    private_desc.callback = flecs_rest_reply;
    private_desc.ctx = srv_ctx;

    ecs_http_server_t *srv = ecs_http_server_init(&private_desc);
    if (!srv) {
        ecs_os_free(srv_ctx);
        return NULL;
    }

    srv_ctx->world = world;
    srv_ctx->srv = srv;
    srv_ctx->rc = 1;
    srv_ctx->srv = srv;
    return srv;
}

void ecs_rest_server_fini(
    ecs_http_server_t *srv)
{
    ecs_rest_ctx_t *impl = ecs_http_server_ctx(srv);
    flecs_rest_server_garbage_collect_all(impl);
    ecs_os_free(impl);
    ecs_http_server_fini(srv);
}

static
void flecs_on_set_rest(ecs_iter_t *it) {
    EcsRest *rest = it->ptrs[0];

    int i;
    for(i = 0; i < it->count; i ++) {
        if (!rest[i].port) {
            rest[i].port = ECS_REST_DEFAULT_PORT;
        }

        ecs_http_server_t *srv = ecs_rest_server_init(it->real_world,
            &(ecs_http_server_desc_t){ 
                .ipaddr = rest[i].ipaddr, 
                .port = rest[i].port,
                .cache_timeout = 0.2
            });

        if (!srv) {
            const char *ipaddr = rest[i].ipaddr ? rest[i].ipaddr : "0.0.0.0";
            ecs_err("failed to create REST server on %s:%u", 
                ipaddr, rest[i].port);
            continue;
        }

        rest[i].impl = ecs_http_server_ctx(srv);

        ecs_http_server_start(srv);
    }
}

static
void DequeueRest(ecs_iter_t *it) {
    EcsRest *rest = ecs_field(it, EcsRest, 1);

    if (it->delta_system_time > (ecs_ftime_t)1.0) {
        ecs_warn(
            "detected large progress interval (%.2fs), REST request may timeout",
            (double)it->delta_system_time);
    }

    int32_t i;
    for(i = 0; i < it->count; i ++) {
        ecs_rest_ctx_t *ctx = rest[i].impl;
        if (ctx) {
            ecs_http_server_dequeue(ctx->srv, it->delta_time);
            flecs_rest_server_garbage_collect(it->world, ctx);
        }
    } 
}

static
void DisableRest(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    ecs_iter_t rit = ecs_term_iter(world, &(ecs_term_t){
        .id = ecs_id(EcsRest),
        .src.flags = EcsSelf
    });

    if (it->event == EcsOnAdd) {
        /* REST module was disabled */
        while (ecs_term_next(&rit)) {
            EcsRest *rest = ecs_field(&rit, EcsRest, 1);
            int i;
            for (i = 0; i < rit.count; i ++) {
                ecs_rest_ctx_t *ctx = rest[i].impl;
                ecs_http_server_stop(ctx->srv);
            }
        }
    } else if (it->event == EcsOnRemove) {
        /* REST module was enabled */
        while (ecs_term_next(&rit)) {
            EcsRest *rest = ecs_field(&rit, EcsRest, 1);
            int i;
            for (i = 0; i < rit.count; i ++) {
                ecs_rest_ctx_t *ctx = rest[i].impl;
                ecs_http_server_start(ctx->srv);
            }
        }
    }
}

void FlecsRestImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsRest);

    ECS_IMPORT(world, FlecsPipeline);
#ifdef FLECS_PLECS
    ECS_IMPORT(world, FlecsScript);
#endif
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
    ecs_doc_set_brief(world, ecs_id(FlecsRest), 
        "Module that implements Flecs REST API");
#endif

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsRest);

    ecs_set_hooks(world, EcsRest, { 
        .ctor = ecs_default_ctor,
        .move = ecs_move(EcsRest),
        .copy = ecs_copy(EcsRest),
        .dtor = ecs_dtor(EcsRest),
        .on_set = flecs_on_set_rest
    });

    ECS_SYSTEM(world, DequeueRest, EcsPostFrame, EcsRest);

    ecs_system(world, {
        .entity = ecs_id(DequeueRest),
        .no_readonly = true
    });

    ecs_observer(world, {
        .filter = { 
            .terms = {{ .id = EcsDisabled, .src.id = ecs_id(FlecsRest) }}
        },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = DisableRest
    });

    ecs_set_name_prefix(world, "EcsRest");
    ECS_TAG_DEFINE(world, EcsRestPlecs);
}

#endif
