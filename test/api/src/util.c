#include <api.h>

void probe_system(
    ecs_rows_t *rows) 
{
    Probe *ctx = ecs_get_context(rows->world);
    if (!ctx) {
        ctx = rows->param;
        if (!ctx) {
            return;
        }
        ctx->param = NULL;
    } else {
        ctx->param = rows->param;
    }

    ctx->system = rows->system;
    ctx->offset = 0;
    ctx->column_count = rows->column_count;

    int i;
    for (i = 0; i < ctx->column_count; i ++) {
        ctx->c[ctx->invoked][i] = rows->components[i];
        ctx->s[ctx->invoked][i] = ecs_column_source(rows, i + 1);

        /* Make sure ecs_column functions work */
        ecs_type_t t = ecs_column_type(rows, i + 1);
        test_assert(t != 0);

        ecs_entity_t e = ecs_column_entity(rows, i + 1);
        test_assert(e != 0);
    }

    if (rows->table_columns) {
        ecs_entity_t *e = ecs_column(rows, ecs_entity_t, 0);
        if (e) {
            test_assert(e != NULL);
            test_assert(rows->entities != NULL);
            test_assert(rows->entities == e);
            
            for (i = 0; i < rows->count; i ++) {
                ctx->e[i + ctx->count] = e[i];

                /* Make sure ecs_field works for all columns */
                int c;
                for (c = 0; c < ctx->column_count; c ++) {
                    _ecs_field(rows, 0, c, i);
                }
            }
            ctx->count += rows->count;
        }
    }

    ctx->invoked ++;
}

void probe_trigger(
    ecs_rows_t *rows)
{
    // Probe *probe = rows->param;
    // if (!ctx) {
    //     return;
    // }

    // probe->system = 0;
    // probe->param = NULL;
    // probe->offset = 0;
    // probe->column_count = 1;
    // probe->c[probe->invoked][0] = component;
    // probe->s[probe->invoked][0] = 0;

    // int32_t i;
    // for (i = 0; i < count; i ++) {
    //     probe->e[i] = entities[i];
    // }

    // probe->count = count;
    // probe->invoked ++;
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
