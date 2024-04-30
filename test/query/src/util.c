#include <query.h>
#include <stdio.h>

void probe_system_w_ctx(
    ecs_iter_t *it,
    Probe *ctx) 
{
    if (!ctx) {
        return;
    }

    ctx->param = it->param;
    ctx->system = it->system;
    ctx->event = it->event;
    ctx->event_id = it->event_id;
    ctx->offset = 0;
    ctx->term_count = it->field_count;
    ctx->term_index = it->term_index;

    int i;
    for (i = 0; i < ctx->term_count; i ++) {
        ctx->c[ctx->invoked][i] = it->ids[i];
        ctx->s[ctx->invoked][i] = ecs_field_src(it, i);

        ecs_id_t e = ecs_field_id(it, i);
        test_assert(e != 0);
    }

    for (i = 0; i < it->count; i ++) {
        if (i + ctx->count < 256) {
            ctx->e[i + ctx->count] = it->entities[i];
        } else {
            /* can't store more than that, tests shouldn't rely on
                * getting back more than 256 results */
        }
    }
    ctx->count += it->count;

    ctx->invoked ++;
}

void probe_iter(
    ecs_iter_t *it) 
{
    Probe *ctx = ecs_get_ctx(it->world);
    if (!ctx) {
        ctx = it->ctx;
    }
    probe_system_w_ctx(it, ctx);
}

void probe_has_entity(Probe *probe, ecs_entity_t e) {
    int i;
    for (i = 0; i < probe->count; i ++) {
        if (probe->e[i] == e) {
            break;
        }
    }

    test_assert(i != probe->count);
}

void install_test_abort(void) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-5);
}
