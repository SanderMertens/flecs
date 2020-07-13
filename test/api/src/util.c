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
    ctx->offset = 0;
    ctx->column_count = it->column_count;

    int i;
    for (i = 0; i < ctx->column_count; i ++) {
        ctx->c[ctx->invoked][i] = it->components[i];
        ctx->s[ctx->invoked][i] = ecs_column_source(it, i + 1);

        /* Make sure ecs_column functions work */
        ecs_type_t t = ecs_column_type(it, i + 1);
        test_assert(t != 0);

        ecs_entity_t e = ecs_column_entity(it, i + 1);
        test_assert(e != 0);
    }

    if (it->entities) {
        ecs_entity_t *e = ecs_column(it, ecs_entity_t, 0);
        if (e) {
            test_assert(e != NULL);
            test_assert(it->entities != NULL);
            test_assert(it->entities == e);
            
            for (i = 0; i < it->count; i ++) {
                ctx->e[i + ctx->count] = e[i];

                /* Make sure ecs_element works for all columns */
                int c;
                for (c = 0; c < ctx->column_count; c ++) {
                    ecs_element_w_size(it, 0, c, i);
                }
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
