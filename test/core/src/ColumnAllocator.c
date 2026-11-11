#include <core.h>

static ECS_COMPONENT_DECLARE(Position);

#define MAX_ALLOCS (64)

typedef struct {
    void *ptr;
    ecs_size_t size;
} col_alloc_t;

typedef struct {
    int32_t malloc_count;
    int32_t realloc_count;
    int32_t free_count;
    ecs_size_t alignment;
    col_alloc_t live[MAX_ALLOCS];
    int32_t live_count;
} col_ctx_t;

static void col_track(col_ctx_t *ctx, void *ptr, ecs_size_t size) {
    test_assert(ptr != NULL);
    test_assert(size > 0);
    test_assert(ctx->live_count < MAX_ALLOCS);
    ctx->live[ctx->live_count ++] = (col_alloc_t){ ptr, size };
}

static void col_untrack(col_ctx_t *ctx, void *ptr, ecs_size_t size) {
    for (int32_t i = 0; i < ctx->live_count; i ++) {
        if (ctx->live[i].ptr == ptr) {
            test_int(ctx->live[i].size, size);
            ctx->live[i] = ctx->live[-- ctx->live_count];
            return;
        }
    }

    test_assert(false); /* freeing memory not allocated by allocator */
}

static col_alloc_t* col_find(col_ctx_t *ctx, void *ptr) {
    for (int32_t i = 0; i < ctx->live_count; i ++) {
        if (ctx->live[i].ptr == ptr) {
            return &ctx->live[i];
        }
    }
    return NULL;
}

static void* col_malloc(ecs_size_t size, ecs_size_t alignment, void *ctx_ptr) {
    col_ctx_t *ctx = ctx_ptr;
    ctx->malloc_count ++;
    ctx->alignment = alignment;
    void *result = ecs_os_malloc(size);
    col_track(ctx, result, size);
    return result;
}

static void* col_realloc(
    void *ptr,
    ecs_size_t old_size,
    ecs_size_t new_size,
    ecs_size_t alignment,
    void *ctx_ptr)
{
    col_ctx_t *ctx = ctx_ptr;
    ctx->realloc_count ++;
    ctx->alignment = alignment;
    col_untrack(ctx, ptr, old_size);
    void *result = ecs_os_realloc(ptr, new_size);
    col_track(ctx, result, new_size);
    return result;
}

static void col_free(void *ptr, ecs_size_t size, void *ctx_ptr) {
    col_ctx_t *ctx = ctx_ptr;
    ctx->free_count ++;
    col_untrack(ctx, ptr, size);
    ecs_os_free(ptr);
}

static void set_allocator(
    ecs_world_t *world,
    ecs_entity_t component,
    col_ctx_t *ctx,
    bool with_realloc)
{
    ecs_set_hooks_id(world, component, &(ecs_type_hooks_t){
        .column_allocator = {
            .malloc_ = col_malloc,
            .realloc_ = with_realloc ? col_realloc : NULL,
            .free_ = col_free,
            .ctx = ctx
        }
    });
}

static void verify_positions(ecs_world_t *world, ecs_entity_t *ents, int32_t count) {
    for (int32_t i = 0; i < count; i ++) {
        const Position *p = ecs_get(world, ents[i], Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    }
}

static void test_grow_and_free(bool with_realloc) {
    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT(world, Velocity);
    set_allocator(world, ecs_id(Position), &ctx, with_realloc);

    ecs_entity_t ents[100];
    for (int32_t i = 0; i < 100; i ++) {
        ents[i] = ecs_insert(world, ecs_value(Position, {i, i * 2}),
            ecs_value(Velocity, {1, 1}));
    }

    test_assert(ctx.malloc_count >= 1);
    if (with_realloc) {
        test_assert(ctx.realloc_count >= 1);
    } else {
        test_int(ctx.realloc_count, 0);
        test_assert(ctx.malloc_count > 1);
        test_assert(ctx.free_count >= 1);
    }
    test_int(ctx.alignment, ECS_ALIGNOF(Position));

    /* Position column of [Position, Velocity] is owned by the allocator */
    ecs_table_t *table = ecs_get_table(world, ents[0]);
    test_assert(table != NULL);
    void *column = ecs_table_get_column(table,
        ecs_table_get_column_index(world, table, ecs_id(Position)), 0);
    col_alloc_t *a = col_find(&ctx, column);
    test_assert(a != NULL);
    test_assert(a->size >= ECS_SIZEOF(Position) * 100);

    /* Velocity column is not allocated with the custom allocator */
    void *vel = ecs_table_get_column(table,
        ecs_table_get_column_index(world, table, ecs_id(Velocity)), 0);
    test_assert(col_find(&ctx, vel) == NULL);

    verify_positions(world, ents, 100);

    ecs_fini(world);

    test_int(ctx.live_count, 0);
    test_int(ctx.malloc_count, ctx.free_count);
}

void ColumnAllocator_grow_and_free(void) {
    test_grow_and_free(true);
}

void ColumnAllocator_grow_and_free_no_realloc(void) {
    test_grow_and_free(false);
}

static ECS_MOVE(Position, dst, src, {
    *dst = *src;
    src->x = -1;
    src->y = -1;
})

static ECS_DTOR(Position, ptr, {
    (void)ptr;
})

void ColumnAllocator_grow_w_move_hooks(void) {
    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ecs_set_hooks(world, Position, {
        .move = ecs_move(Position),
        .dtor = ecs_dtor(Position),
        .column_allocator = {
            .malloc_ = col_malloc,
            .realloc_ = col_realloc,
            .free_ = col_free,
            .ctx = &ctx
        }
    });

    ecs_entity_t ents[100];
    for (int32_t i = 0; i < 100; i ++) {
        ents[i] = ecs_insert(world, ecs_value(Position, {i, i * 2}));
    }

    /* Types with move hooks are moved to a new allocation */
    test_assert(ctx.malloc_count > 1);
    test_assert(ctx.free_count >= 1);
    test_int(ctx.live_count, 1);

    verify_positions(world, ents, 100);

    ecs_fini(world);

    test_int(ctx.live_count, 0);
    test_int(ctx.malloc_count, ctx.free_count);
}

void ColumnAllocator_shrink(void) {
    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    set_allocator(world, ecs_id(Position), &ctx, true);

    ecs_entity_t ents[100];
    for (int32_t i = 0; i < 100; i ++) {
        ents[i] = ecs_insert(world, ecs_value(Position, {i, i * 2}));
    }

    for (int32_t i = 10; i < 100; i ++) {
        ecs_delete(world, ents[i]);
    }

    ecs_shrink(world);

    test_int(ctx.live_count, 1);
    test_int(ctx.live[0].size, ECS_SIZEOF(Position) * 10);

    ecs_table_t *table = ecs_get_table(world, ents[0]);
    test_assert(ecs_table_get_column(table, 0, 0) == ctx.live[0].ptr);

    verify_positions(world, ents, 10);

    for (int32_t i = 0; i < 10; i ++) {
        ecs_delete(world, ents[i]);
    }

    ecs_shrink(world);

    test_int(ctx.live_count, 0);

    ecs_fini(world);

    test_int(ctx.malloc_count, ctx.free_count);
}

void ColumnAllocator_merge(void) {
    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);
    set_allocator(world, ecs_id(Position), &ctx, true);

    ecs_entity_t ents[20];
    for (int32_t i = 0; i < 20; i ++) {
        ents[i] = ecs_insert(world, ecs_value(Position, {i, i * 2}));
        if (i % 2) {
            ecs_add(world, ents[i], Tag);
        }
    }

    /* [Position] and [Position, Tag] */
    test_int(ctx.live_count, 2);

    /* Merges [Position, Tag] into [Position] */
    ecs_delete(world, Tag);

    test_int(ctx.live_count, 1);

    for (int32_t i = 0; i < 20; i ++) {
        test_assert(!ecs_has(world, ents[i], Tag));
    }

    verify_positions(world, ents, 20);

    ecs_fini(world);

    test_int(ctx.live_count, 0);
    test_int(ctx.malloc_count, ctx.free_count);
}

void ColumnAllocator_merge_into_empty(void) {
    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);
    set_allocator(world, ecs_id(Position), &ctx, true);

    /* Create [Position] table with storage, then empty it */
    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {0, 0}));
    ecs_delete(world, e);

    ecs_entity_t ents[20];
    for (int32_t i = 0; i < 20; i ++) {
        ents[i] = ecs_insert(world, ecs_value(Position, {i, i * 2}));
        ecs_add(world, ents[i], Tag);
    }

    test_int(ctx.live_count, 2);

    /* Moves storage of [Position, Tag] into empty [Position] */
    ecs_delete(world, Tag);

    test_int(ctx.live_count, 1);

    verify_positions(world, ents, 20);

    ecs_fini(world);

    test_int(ctx.live_count, 0);
    test_int(ctx.malloc_count, ctx.free_count);
}

void ColumnAllocator_delete_table(void) {
    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tag);
    set_allocator(world, ecs_id(Position), &ctx, true);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {1, 2}));
    ecs_add(world, e, Tag);
    test_int(ctx.live_count, 2); /* [Position] and [Position, Tag] */

    ecs_remove(world, e, Tag);
    test_int(ctx.live_count, 2);

    /* Deleting Tag deletes the now-empty [Position, Tag] table */
    ecs_delete(world, Tag);
    test_int(ctx.live_count, 1);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);

    test_int(ctx.live_count, 0);
    test_int(ctx.malloc_count, ctx.free_count);
}

void ColumnAllocator_pair(void) {
    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_TAG(world, Tgt);
    set_allocator(world, ecs_id(Position), &ctx, true);

    ecs_entity_t e = ecs_new(world);
    ecs_set_pair(world, e, Position, Tgt, {10, 20});
    test_int(ctx.live_count, 1);

    const Position *p = ecs_get_pair(world, e, Position, Tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);

    test_int(ctx.live_count, 0);
    test_int(ctx.malloc_count, ctx.free_count);
}

void ColumnAllocator_set_in_use(void) {
    install_test_abort();

    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ecs_insert(world, ecs_value(Position, {1, 2}));

    test_expect_abort();
    set_allocator(world, ecs_id(Position), &ctx, true);
}

void ColumnAllocator_set_missing_free(void) {
    install_test_abort();

    col_ctx_t ctx = {0};

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);

    test_expect_abort();
    ecs_set_hooks(world, Position, {
        .column_allocator = {
            .malloc_ = col_malloc,
            .ctx = &ctx
        }
    });
}
