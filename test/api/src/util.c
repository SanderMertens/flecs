#include <api.h>

void ProbeSystem(
    ecs_rows_t *rows) 
{
    SysTestData *ctx = ecs_get_context(rows->world);
    if (!ctx) {
        return;
    }

    ctx->system = rows->system;
    ctx->offset = 0;
    ctx->column_count = rows->column_count;
    ctx->param = rows->param;

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
