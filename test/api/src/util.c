#include <include/api.h>

void ProbeSystem(
    EcsRows *rows) 
{
    SysTestData *ctx = ecs_get_context(rows->world);
    if (!ctx) {
        return;
    }

    ctx->count = rows->limit;
    ctx->offset = rows->offset;
    ctx->column_count = rows->column_count;
    ctx->param = rows->param;

    int i;
    for (i = 0; i < ctx->column_count; i ++) {
        ctx->c[i] = rows->components[i];
        ctx->s[i] = ecs_column_source(rows, i);
    }

    EcsEntity *e = ecs_column(rows, EcsEntity, 0);
    for (i = rows->offset; i < (rows->offset + rows->limit); i ++) {
        ctx->e[i - rows->offset] = e[i];
    }
}
