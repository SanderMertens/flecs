#include <core.h>
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

        ecs_id_t field_id = ecs_field_id(it, i);
        test_assert(field_id != 0);

        if (ecs_field_is_set(it, i)) {
            if (it->trs[i]) {
                if (it->sources[i]) {
                    ecs_table_t *table = ecs_get_table(it->world, it->sources[i]);
                    test_assert(it->trs[i]->hdr.table == table);
                } else {
                    test_assert(it->trs[i]->hdr.table == it->table);
                }
            }
        }
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
    ecs_os_api_t os_api = ecs_os_get_api();
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-5);
}

const ecs_entity_t* bulk_new_w_type(
    ecs_world_t *world, ecs_entity_t type_ent, int32_t count) 
{
    const ecs_type_t *type = ecs_get_type(world, type_ent);
    test_assert(type != NULL);

    ecs_id_t *ids = type->array;
    int i = 0;
    while ((ecs_id_get_flags(world, ids[i]) & EcsIdOnInstantiateDontInherit)) {
        i ++;
    }
    const ecs_entity_t *result = ecs_bulk_new_w_id(world, ids[i], count);
    for (; i < type->count; i ++) {
        for (int e = 0; e < count; e ++) {
            if (ecs_id_get_flags(world, ids[i]) & EcsIdOnInstantiateDontInherit) {
                continue;
            }
            ecs_add_id(world, result[e], ids[i]);
        }
    }
    
    return result;
}
