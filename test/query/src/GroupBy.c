#include <query.h>

static
int order_by_entity(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2)
{
    return (e1 > e2) - (e1 < e2);
}

static
uint64_t group_by_first_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    void *ctx)
{
    const ecs_type_t *type = ecs_table_get_type(table);
    ecs_id_t *first = type->array;
    if (!first) {
        return 0;
    }

    return first[0];
}

static
uint64_t group_by_rel(ecs_world_t *world, ecs_table_t *table, ecs_id_t id, void *ctx) {
    ecs_id_t match;
    if (ecs_search(world, table, ecs_pair(id, EcsWildcard), &match) != -1) {
        return ecs_pair_second(world, match);
    }
    return 0;
}

void GroupBy_group_by(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagX);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagX}},
        .group_by_callback = group_by_first_id
    });

    ecs_entity_t e1 = ecs_new_w(world, TagX);
    ecs_entity_t e2 = ecs_new_w(world, TagX);
    ecs_entity_t e3 = ecs_new_w(world, TagX);

    ecs_add_id(world, e1, TagC);
    ecs_add_id(world, e2, TagB);
    ecs_add_id(world, e3, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);    

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

static int ctx_value;
static int ctx_value_free_invoked = 0;
static
void ctx_value_free(void *ctx) {
    test_assert(ctx == &ctx_value);
    ctx_value_free_invoked ++;
}

void GroupBy_group_by_w_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagX);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagX}},
        .group_by_callback = group_by_first_id,
        .group_by_ctx = &ctx_value,
        .group_by_ctx_free = ctx_value_free
    });

    ecs_entity_t e1 = ecs_new_w(world, TagX);
    ecs_entity_t e2 = ecs_new_w(world, TagX);
    ecs_entity_t e3 = ecs_new_w(world, TagX);

    ecs_add_id(world, e1, TagC);
    ecs_add_id(world, e2, TagB);
    ecs_add_id(world, e3, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);    

    test_bool(ecs_query_next(&it), false);

    ecs_query_fini(q);

    test_int(ctx_value_free_invoked, 1);

    ecs_fini(world);
}

void GroupBy_group_by_w_sort_reverse_group_creation(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t TagA = ecs_new(world);
    ecs_entity_t TagB = ecs_new(world);
    ecs_entity_t TagC = ecs_new(world);

    ecs_entity_t TagX = ecs_new(world);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{TagX}},
        .order_by_callback = order_by_entity,
        .group_by_callback = group_by_first_id
    });

    ecs_entity_t e1 = ecs_new_w_id(world, TagX);
    ecs_entity_t e2 = ecs_new_w_id(world, TagX);
    ecs_entity_t e3 = ecs_new_w_id(world, TagX);

    ecs_add_id(world, e1, TagC);
    ecs_add_id(world, e2, TagB);
    ecs_add_id(world, e3, TagA);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);    

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void GroupBy_group_by_iter_one(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Tag);

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e5 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e6 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);
    ecs_add(world, e6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by_callback = group_by_rel,
        .group_by = Rel
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, TgtB);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, it.group_id);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, it.group_id);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void GroupBy_group_by_iter_one_all_groups(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e5 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e6 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);
    ecs_add(world, e6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by_callback = group_by_rel,
        .group_by = Rel
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, TgtB);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, it.group_id);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e5, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, it.group_id);
    test_bool(false, ecs_query_next(&it));

    it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, TgtA);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, it.group_id);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e4, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, it.group_id);
    test_bool(false, ecs_query_next(&it));

    it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, TgtC);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(TgtC, it.group_id);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e6, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(TgtC, it.group_id);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void GroupBy_group_by_iter_one_empty(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, TgtD);
    ECS_TAG(world, Tag);

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);
    ecs_new_w_pair(world, Rel, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e5 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e6 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);
    ecs_add(world, e6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by_callback = group_by_rel,
        .group_by = Rel
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, TgtD);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void GroupBy_group_by_iter_one_empty_query(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by_callback = group_by_rel,
        .group_by = Rel
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, TgtA);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void GroupBy_group_by_iter_one_empty_table(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);
    ECS_TAG(world, Tag);

    ecs_new_w_pair(world, Rel, TgtA);
    ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtC);

    ecs_entity_t e4 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_entity_t e5 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e6 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_add(world, e4, Tag);
    ecs_add(world, e5, Tag);
    ecs_add(world, e6, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by_callback = group_by_rel,
        .group_by = Rel
    });

    ecs_delete(world, e3);
    ecs_delete(world, e6);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, TgtC);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void GroupBy_group_by_w_deleted_group_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by_callback = group_by_rel,
        .group_by = Rel
    });

    ecs_entity_t tgt = ecs_new(world);
    ecs_entity_t e = ecs_new_w_pair(world, Rel, tgt);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, tgt);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_delete(world, tgt);
    ecs_delete(world, e);

    it = ecs_query_iter(world, q);
    test_bool(false, ecs_query_next(&it));

    tgt = ecs_new(world);
    e = ecs_new_w_pair(world, Rel, tgt);

    it = ecs_query_iter(world, q);
    ecs_iter_set_group(&it, tgt);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

static int group_by_ctx;
static int on_group_create_invoked;
static int on_group_delete_invoked;

static
void* on_group_create(ecs_world_t *world, uint64_t group_id, void *group_by_arg) {
    test_assert(world != NULL);
    test_assert(group_id != 0);
    test_assert(group_by_arg != NULL);
    test_assert(group_by_arg == &group_by_ctx);
    uint64_t *group_ctx = ecs_os_malloc_t(uint64_t);
    *group_ctx = group_id;
    on_group_create_invoked ++;
    return group_ctx;
}

static
void on_group_delete(ecs_world_t *world, uint64_t group_id, void *group_ctx, void *group_by_arg) {
    test_assert(world != NULL);
    test_assert(group_id != 0);
    test_assert(group_ctx != NULL);
    test_assert(group_by_arg != NULL);
    test_assert(group_by_arg == &group_by_ctx);
    test_assert(*(uint64_t*)group_ctx == group_id);
    ecs_os_free(group_ctx);
    on_group_delete_invoked ++;
}

void GroupBy_group_by_callbacks(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by_callback = group_by_rel,
        .group_by = Rel,
        .on_group_create = on_group_create,
        .on_group_delete = on_group_delete,
        .group_by_ctx = &group_by_ctx
    });

    ecs_entity_t tgt_a = ecs_new(world);
    ecs_entity_t tgt_b = ecs_new(world);

    test_int(on_group_create_invoked, 0);
    test_int(on_group_delete_invoked, 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, tgt_a);
    ecs_run_aperiodic(world, 0);
    test_int(on_group_create_invoked, 1);
    test_int(on_group_delete_invoked, 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, tgt_a);
    ecs_run_aperiodic(world, 0);
    test_int(on_group_create_invoked, 1);
    test_int(on_group_delete_invoked, 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, tgt_b);
    ecs_run_aperiodic(world, 0);
    test_int(on_group_create_invoked, 2);
    test_int(on_group_delete_invoked, 0);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(2, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(tgt_a, it.group_id);
    const ecs_query_group_info_t *gi = ecs_query_get_group_info(q, it.group_id);
    test_assert(gi != NULL);
    test_assert(gi->ctx != NULL);
    test_uint(tgt_a, *(uint64_t*)gi->ctx);

    test_assert(ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(tgt_b, it.group_id);
    gi = ecs_query_get_group_info(q, it.group_id);
    test_assert(gi != NULL);
    test_assert(gi->ctx != NULL);
    test_uint(tgt_b, *(uint64_t*)gi->ctx);
    test_assert(!ecs_query_next(&it));

    ecs_delete_with(world, ecs_pair(Rel, tgt_a));
    ecs_run_aperiodic(world, 0);
    test_int(on_group_create_invoked, 2);
    test_int(on_group_delete_invoked, 1);

    ecs_query_fini(q);

    test_int(on_group_create_invoked, 2);
    test_int(on_group_delete_invoked, 2);

    ecs_fini(world);
}

void GroupBy_group_by_default_action(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, TgtC);

    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, TgtC);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, TgtB);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        // .group_by_callback = group_by_rel, default action groups by relationship
        .group_by = Rel
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e3, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtA), ecs_field_id(&it, 0));
    test_uint(TgtA, it.group_id);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtB), ecs_field_id(&it, 0));
    test_uint(TgtB, it.group_id);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(ecs_pair(Rel, TgtC), ecs_field_id(&it, 0));
    test_uint(TgtC, it.group_id);
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void GroupBy_group_table_count(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, TgtA);
    ECS_TAG(world, TgtB);
    ECS_TAG(world, Foo);

    ecs_new_w_pair(world, Rel, TgtA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_pair(Rel, EcsWildcard) }
        },
        .group_by = Rel
    });

    ecs_run_aperiodic(world, 0);

    const ecs_query_group_info_t *gi_a;
    const ecs_query_group_info_t *gi_b;
    gi_a = ecs_query_get_group_info(q, TgtA);
    gi_b = ecs_query_get_group_info(q, TgtB);
    test_assert(gi_a != NULL);
    test_assert(gi_b == NULL);
    test_int(gi_a->table_count, 1);
    test_int(gi_a->match_count, 1);

    ecs_new_w_pair(world, Rel, TgtB);
    ecs_run_aperiodic(world, 0);

    gi_a = ecs_query_get_group_info(q, TgtA);
    gi_b = ecs_query_get_group_info(q, TgtB);
    test_assert(gi_a != NULL);
    test_assert(gi_b != NULL);
    test_int(gi_a->table_count, 1);
    test_int(gi_a->match_count, 1);
    test_int(gi_b->table_count, 1);
    test_int(gi_b->match_count, 1);

    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, TgtA);
    ecs_run_aperiodic(world, 0);

    gi_a = ecs_query_get_group_info(q, TgtA);
    gi_b = ecs_query_get_group_info(q, TgtB);
    test_assert(gi_a != NULL);
    test_assert(gi_b != NULL);
    test_int(gi_a->table_count, 1);
    test_int(gi_a->match_count, 1);
    test_int(gi_b->table_count, 1);
    test_int(gi_b->match_count, 1);

    ecs_add(world, e3, Foo);
    ecs_run_aperiodic(world, 0);

    gi_a = ecs_query_get_group_info(q, TgtA);
    gi_b = ecs_query_get_group_info(q, TgtB);
    test_assert(gi_a != NULL);
    test_assert(gi_b != NULL);
    test_int(gi_a->table_count, 2);
    test_int(gi_a->match_count, 2);
    test_int(gi_b->table_count, 1);
    test_int(gi_b->match_count, 1);

    ecs_delete_with(world, Foo);
    ecs_run_aperiodic(world, 0);

    gi_a = ecs_query_get_group_info(q, TgtA);
    gi_b = ecs_query_get_group_info(q, TgtB);
    test_assert(gi_a != NULL);
    test_assert(gi_b != NULL);
    test_int(gi_a->table_count, 1);
    test_int(gi_a->match_count, 3);
    test_int(gi_b->table_count, 1);
    test_int(gi_b->match_count, 1);

    ecs_delete_with(world, ecs_pair(Rel, TgtA));
    ecs_run_aperiodic(world, 0);

    gi_a = ecs_query_get_group_info(q, TgtA);
    gi_b = ecs_query_get_group_info(q, TgtB);
    test_assert(gi_a == NULL);
    test_assert(gi_b != NULL);
    test_int(gi_b->table_count, 1);
    test_int(gi_b->match_count, 1);

    ecs_delete_with(world, ecs_pair(Rel, TgtB));
    ecs_run_aperiodic(world, 0);

    gi_a = ecs_query_get_group_info(q, TgtA);
    gi_b = ecs_query_get_group_info(q, TgtB);
    test_assert(gi_a == NULL);
    test_assert(gi_b == NULL);

    ecs_fini(world);
}
