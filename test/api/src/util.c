#include <api.h>

void probe_system_w_ctx(
    ecs_iter_t *it,
    Probe *ctx) 
{
    if (!ctx) {
        return;
    }

    ctx->param = it->param;
    ctx->system = it->system;
    ctx->self = it->self;
    ctx->event = it->event;
    ctx->event_id = it->event_id;
    ctx->offset = 0;
    ctx->term_count = it->term_count;
    ctx->term_index = it->term_index;

    int i;
    for (i = 0; i < ctx->term_count; i ++) {
        ctx->c[ctx->invoked][i] = it->ids[i];
        ctx->s[ctx->invoked][i] = ecs_term_source(it, i + 1);

        ecs_id_t e = ecs_term_id(it, i + 1);
        test_assert(e != 0);
    }

    if (it->entities) {
        ecs_entity_t *e = ecs_term(it, ecs_entity_t, 0);
        if (e) {
            test_assert(e != NULL);
            test_assert(it->entities != NULL);
            test_assert(it->entities == e);
            
            for (i = 0; i < it->count; i ++) {
                ctx->e[i + ctx->count] = e[i];
            }
            ctx->count += it->count;
        }
    }

    ctx->invoked ++;
}

void probe_system(
    ecs_iter_t *it) 
{
    Probe *ctx = ecs_get_context(it->world);
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

void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_tracing_enable(-5);
}

const ecs_entity_t* bulk_new_w_type(
    ecs_world_t *world, ecs_entity_t type_ent, int32_t count) 
{
    const EcsType *type_comp = ecs_get(world, type_ent, EcsType);
    test_assert(type_comp != NULL);
    ecs_type_t type = type_comp->normalized;

    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    const ecs_entity_t *result = ecs_bulk_new_w_id(world, ids[0], count);
    for (int i = 1; i < ecs_vector_count(type); i ++) {
        for (int e = 0; e < count; e ++) {
            ecs_add_id(world, result[e], ids[i]);
        }
    }
    
    return result;
}
