#include <api.h>

void probe_system_w_ctx(
    ecs_view_t *view,
    Probe *ctx) 
{
    if (!ctx) {
        return;
    }

    ctx->param = view->param;
    ctx->system = view->system;
    ctx->offset = 0;
    ctx->column_count = view->column_count;

    int i;
    for (i = 0; i < ctx->column_count; i ++) {
        ctx->c[ctx->invoked][i] = view->components[i];
        ctx->s[ctx->invoked][i] = ecs_column_source(view, i + 1);

        /* Make sure ecs_column functions work */
        ecs_type_t t = ecs_column_type(view, i + 1);
        test_assert(t != 0);

        ecs_entity_t e = ecs_column_entity(view, i + 1);
        test_assert(e != 0);
    }

    if (view->entities) {
        ecs_entity_t *e = ecs_column(view, ecs_entity_t, 0);
        if (e) {
            test_assert(e != NULL);
            test_assert(view->entities != NULL);
            test_assert(view->entities == e);
            
            for (i = 0; i < view->count; i ++) {
                ctx->e[i + ctx->count] = e[i];

                /* Make sure ecs_field works for all columns */
                int c;
                for (c = 0; c < ctx->column_count; c ++) {
                    _ecs_field(view, 0, c, i);
                }
            }
            ctx->count += view->count;
        }
    }

    ctx->invoked ++;
}

void probe_system(
    ecs_view_t *view) 
{
    Probe *ctx = ecs_get_context(view->world);
    probe_system_w_ctx(view, ctx);
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
