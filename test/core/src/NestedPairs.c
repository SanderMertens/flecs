#include <core.h>

void NestedPairs_make_nested_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    test_assert(id != 0);
    test_assert(ECS_IS_PAIR(id));
    test_assert(ECS_IS_NESTED_PAIR(id));
    test_assert(ecs_id_is_pair(id));
    test_assert(!ecs_id_is_wildcard(id));
    test_assert(ecs_id_is_valid(world, id));
    test_uint(ECS_PAIR_FIRST(id), Likes);
    test_uint(ecs_pair_second_id(world, id), ecs_pair(Eats, Pizza));

    test_assert(ECS_PAIR_SECOND(id) != 0);
    test_uint(ecs_pair_second(world, id), 0);

    ecs_fini(world);
}

void NestedPairs_make_nested_pair_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Hates);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Salad);

    ecs_id_t id_1 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    ecs_id_t id_2 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    test_uint(id_1, id_2);

    ecs_id_t id_3 = ecs_nested_pair(world, Hates, ecs_pair(Eats, Pizza));
    test_assert(id_3 != id_1);
    test_uint(ECS_PAIR_SECOND(id_3), ECS_PAIR_SECOND(id_1));

    ecs_id_t id_4 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Salad));
    test_assert(id_4 != id_1);
    test_assert(ECS_PAIR_SECOND(id_4) != ECS_PAIR_SECOND(id_1));
    test_uint(ecs_pair_second_id(world, id_4), ecs_pair(Eats, Salad));

    ecs_fini(world);
}

void NestedPairs_make_nested_pair_w_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, Pizza);
    test_uint(id, ecs_pair(Likes, Pizza));
    test_assert(!ECS_IS_NESTED_PAIR(id));
    test_uint(ecs_pair_second_id(world, id), Pizza);

    ecs_fini(world);
}

void NestedPairs_nested_pair_str(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    char *str = ecs_id_str(world, id);
    test_str(str, "(Likes,(Eats,Pizza))");
    ecs_os_free(str);

    test_str(ecs_id_flag_str(id), "NESTED_PAIR");

    ecs_fini(world);
}

void NestedPairs_nested_pair_of_nested_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Wants);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t inner = ecs_nested_pair(world, Wants, ecs_pair(Eats, Pizza));
    ecs_id_t id = ecs_nested_pair(world, Likes, inner);
    test_assert(ECS_IS_NESTED_PAIR(id));
    test_uint(ecs_pair_second_id(world, id), inner);

    char *str = ecs_id_str(world, id);
    test_str(str, "(Likes,(Wants,(Eats,Pizza)))");
    ecs_os_free(str);

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, id);
    test_assert(ecs_has_id(world, e, id));

    ecs_delete(world, Pizza);
    test_assert(!ecs_has_id(world, e, id));
    test_int(ecs_get_type(world, e)->count, 0);
    test_uint(ecs_pair_second_id(world, inner), 0);
    test_uint(ecs_pair_second_id(world, id), 0);

    ecs_fini(world);
}

void NestedPairs_add_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, id);
    test_assert(ecs_has_id(world, e, id));
    test_assert(!ecs_has_pair(world, e, Likes, ECS_PAIR_SECOND(id)));
    test_assert(!ecs_has_pair(world, e, Eats, Pizza));

    ecs_remove_id(world, e, id);
    test_assert(!ecs_has_id(world, e, id));
    test_int(ecs_get_type(world, e)->count, 0);

    ecs_fini(world);
}

void NestedPairs_add_nested_and_regular_pairs(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Apples);

    ecs_entity_t p = ecs_new(world);
    ecs_id_t id_1 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    ecs_id_t id_2 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Apples));

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, id_1);
    ecs_add_pair(world, e, Likes, Apples);
    ecs_add_pair(world, e, EcsChildOf, p);
    ecs_add_id(world, e, id_2);
    ecs_add_pair(world, e, Likes, Pizza);

    test_assert(ecs_has_id(world, e, id_1));
    test_assert(ecs_has_id(world, e, id_2));
    test_assert(ecs_has_pair(world, e, Likes, Apples));
    test_assert(ecs_has_pair(world, e, Likes, Pizza));
    test_assert(ecs_has_pair(world, e, EcsChildOf, p));
    test_int(ecs_get_type(world, e)->count, 5);

    /* Regular pair targets are not affected by nested pairs */
    test_uint(ecs_get_target(world, e, Likes, 0), Pizza);
    test_uint(ecs_get_target(world, e, Likes, 1), Apples);
    test_uint(ecs_get_target(world, e, Likes, 2), 0);

    ecs_remove_id(world, e, id_1);
    test_assert(!ecs_has_id(world, e, id_1));
    test_assert(ecs_has_id(world, e, id_2));
    test_assert(ecs_has_pair(world, e, Likes, Apples));
    test_assert(ecs_has_pair(world, e, Likes, Pizza));

    /* Removing all Likes pairs also removes nested pairs */
    ecs_remove_pair(world, e, Likes, EcsWildcard);
    test_assert(!ecs_has_id(world, e, id_2));
    test_assert(!ecs_has_pair(world, e, Likes, Apples));
    test_assert(!ecs_has_pair(world, e, Likes, Pizza));
    test_assert(ecs_has_pair(world, e, EcsChildOf, p));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_fini(world);
}

void NestedPairs_query_nested_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Apples);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));

    ecs_entity_t e1 = ecs_new_w_id(world, id);
    ecs_entity_t e2 = ecs_new_w_id(world, id);
    ecs_add_pair(world, e2, Likes, Apples);
    ecs_new_w_pair(world, Likes, Apples);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = id }}
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_field_id(&it, 0), id);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 0), id);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void NestedPairs_delete_first_of_nested(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Tag);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, id);
    ecs_add(world, e, Tag);
    test_assert(ecs_has_id(world, e, id));

    ecs_delete(world, Eats);

    test_uint(ecs_pair_second_id(world, id), 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, Pizza));
    test_assert(ecs_is_alive(world, Likes));
    test_assert(!ecs_has_id(world, e, id));
    test_assert(ecs_has(world, e, Tag));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_fini(world);
}

void NestedPairs_delete_second_of_nested(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Tag);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, id);
    ecs_add(world, e, Tag);
    test_assert(ecs_has_id(world, e, id));

    ecs_delete(world, Pizza);

    test_uint(ecs_pair_second_id(world, id), 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, Eats));
    test_assert(!ecs_has_id(world, e, id));
    test_assert(ecs_has(world, e, Tag));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_fini(world);
}

void NestedPairs_delete_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Tag);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    uint32_t handle = ECS_PAIR_SECOND(id);

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, id);
    ecs_add(world, e, Tag);
    test_assert(ecs_has_id(world, e, id));

    ecs_delete(world, Likes);

    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has_id(world, e, id));
    test_assert(ecs_has(world, e, Tag));
    test_int(ecs_get_type(world, e)->count, 1);

    /* Handle is still valid, as it's only tied to the nested pair */
    test_uint(ecs_pair_second_id(world, id), ecs_pair(Eats, Pizza));
    ecs_entity_t Hates = ecs_new(world);
    ecs_id_t id_2 = ecs_nested_pair(world, Hates, ecs_pair(Eats, Pizza));
    test_uint(ECS_PAIR_SECOND(id_2), handle);

    ecs_fini(world);
}

void NestedPairs_delete_first_w_delete_target(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ecs_add_pair(world, Likes, EcsOnDeleteTarget, EcsDelete);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, id);

    /* Deleting the handle deletes entities with (Likes, handle) */
    ecs_delete(world, Pizza);
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void NestedPairs_recreate_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Apples);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    ecs_id_t id_2 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Apples));

    ecs_delete(world, Pizza);
    test_uint(ecs_pair_second_id(world, id), 0);
    test_assert(!ecs_id_is_valid(world, id));

    /* Handles for other pairs are not affected */
    test_uint(ecs_pair_second_id(world, id_2), ecs_pair(Eats, Apples));

    ecs_entity_t pizza = ecs_new(world);
    ecs_id_t id_3 = ecs_nested_pair(world, Likes, ecs_pair(Eats, pizza));
    test_assert(ecs_id_is_valid(world, id_3));
    test_uint(ecs_pair_second_id(world, id_3), ecs_pair(Eats, pizza));

    ecs_entity_t e = ecs_new_w_id(world, id_3);
    test_assert(ecs_has_id(world, e, id_3));

    ecs_fini(world);
}

void NestedPairs_delete_entity_w_handle_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    uint32_t handle = ECS_PAIR_SECOND(id);

    ecs_entity_t tgt = handle;
    test_assert(tgt != Likes && tgt != Eats && tgt != Pizza);
    ecs_make_alive(world, tgt);

    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_add_pair(world, e, Likes, tgt);
    test_assert(ecs_has_id(world, e, id));
    test_assert(ecs_has_pair(world, e, Likes, tgt));
    test_int(ecs_get_type(world, e)->count, 2);

    ecs_delete(world, tgt);
    test_assert(!ecs_has_pair(world, e, Likes, tgt));
    test_assert(ecs_has_id(world, e, id));
    test_uint(ecs_pair_second_id(world, id), ecs_pair(Eats, Pizza));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_fini(world);
}

void NestedPairs_no_entity_created(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Apples);

    ecs_entity_t e1 = ecs_new(world);
    ecs_id_t id_1 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    ecs_id_t id_2 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Apples));
    ecs_entity_t e2 = ecs_new(world);
    test_uint(e2, e1 + 1);

    ecs_add_id(world, e1, id_1);
    ecs_add_id(world, e1, id_2);
    ecs_entity_t e3 = ecs_new(world);
    test_uint(e3, e2 + 1);

    ecs_delete(world, Pizza);
    test_assert(!ecs_has_id(world, e1, id_1));
    test_assert(ecs_has_id(world, e1, id_2));
    ecs_entity_t e4 = ecs_new(world);
    test_uint((uint32_t)e4, (uint32_t)Pizza);

    ecs_fini(world);
}

void NestedPairs_reuse_handle_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Apples);

    ecs_id_t id_1 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    uint32_t handle = ECS_PAIR_SECOND(id_1);

    ecs_delete(world, Pizza);
    test_uint(ecs_pair_second_id(world, id_1), 0);

    ecs_id_t id_2 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Apples));
    test_uint(ECS_PAIR_SECOND(id_2), handle);
    test_uint(ecs_pair_second_id(world, id_2), ecs_pair(Eats, Apples));

    ecs_entity_t e = ecs_new_w_id(world, id_2);
    test_assert(ecs_has_id(world, e, id_2));

    char *str = ecs_id_str(world, id_2);
    test_str(str, "(Likes,(Eats,Apples))");
    ecs_os_free(str);

    ecs_fini(world);
}

void NestedPairs_wildcard_target_query_w_handle_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    ecs_entity_t tgt = ECS_PAIR_SECOND(id);
    test_assert(tgt != Likes && tgt != Eats && tgt != Pizza);
    ecs_make_alive(world, tgt);

    ecs_new_w_id(world, id);
    ecs_entity_t e2 = ecs_new_w_pair(world, Likes, tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(EcsWildcard, tgt) }}
    });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e2);
    test_uint(ecs_field_id(&it, 0), ecs_pair(Likes, tgt));
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

static int nested_pairs_observer_invoked = 0;

static void NestedPairs_observer(ecs_iter_t *it) {
    nested_pairs_observer_invoked += it->count;
}

void NestedPairs_wildcard_target_observer_w_handle_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    ecs_entity_t tgt = ECS_PAIR_SECOND(id);
    test_assert(tgt != Likes && tgt != Eats && tgt != Pizza);
    ecs_make_alive(world, tgt);

    ecs_observer(world, {
        .query.terms = {{ .id = ecs_pair(EcsWildcard, tgt) }},
        .events = { EcsOnAdd },
        .callback = NestedPairs_observer
    });

    ecs_new_w_id(world, id);
    test_int(nested_pairs_observer_invoked, 0);

    ecs_new_w_pair(world, Likes, tgt);
    test_int(nested_pairs_observer_invoked, 1);

    ecs_fini(world);
}

void NestedPairs_delete_element_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t pizza = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, pizza));

    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_add(world, e, Tag);
    test_assert(ecs_has_id(world, e, id));

    ecs_delete(world, parent);
    test_assert(!ecs_is_alive(world, pizza));
    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has_id(world, e, id));
    test_assert(ecs_has(world, e, Tag));
    test_uint(ecs_pair_second_id(world, id), 0);

    ecs_fini(world);
}

void NestedPairs_delete_element_w_parent_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t pizza = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, pizza));

    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_add(world, e, Tag);
    test_assert(ecs_has_id(world, e, id));

    ecs_delete(world, parent);
    test_assert(!ecs_is_alive(world, pizza));
    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has_id(world, e, id));
    test_assert(ecs_has(world, e, Tag));
    test_uint(ecs_pair_second_id(world, id), 0);

    ecs_fini(world);
}

void NestedPairs_delete_element_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Tag);

    ecs_id_t id = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));

    ecs_entity_t e = ecs_new_w_id(world, id);
    ecs_add(world, e, Tag);

    ecs_frame_begin(world, 1);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_delete(stage, Pizza);
    test_assert(ecs_has_id(stage, e, id));
    test_uint(ecs_pair_second_id(stage, id), ecs_pair(Eats, Pizza));
    ecs_merge(stage);
    ecs_frame_end(world);

    test_assert(!ecs_has_id(world, e, id));
    test_assert(ecs_has(world, e, Tag));
    test_uint(ecs_pair_second_id(world, id), 0);

    ecs_fini(world);
}

void NestedPairs_create_in_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);

    ecs_entity_t e = ecs_new(world);

    ecs_frame_begin(world, 1);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_id_t id = ecs_nested_pair(stage, Likes, ecs_pair(Eats, Pizza));
    test_uint(ecs_pair_second_id(stage, id), ecs_pair(Eats, Pizza));
    ecs_add_id(stage, e, id);
    test_assert(!ecs_has_id(stage, e, id));
    ecs_merge(stage);
    ecs_frame_end(world);

    test_assert(ecs_has_id(world, e, id));

    ecs_delete(world, Eats);
    test_assert(!ecs_has_id(world, e, id));

    ecs_fini(world);
}

void NestedPairs_exclusive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Likes);
    ecs_add_id(world, Likes, EcsExclusive);
    ECS_TAG(world, Eats);
    ECS_TAG(world, Pizza);
    ECS_TAG(world, Apples);

    ecs_id_t id_1 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Pizza));
    ecs_id_t id_2 = ecs_nested_pair(world, Likes, ecs_pair(Eats, Apples));

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Likes, Pizza);
    test_assert(ecs_has_pair(world, e, Likes, Pizza));

    ecs_add_id(world, e, id_1);
    test_assert(ecs_has_id(world, e, id_1));
    test_assert(!ecs_has_pair(world, e, Likes, Pizza));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_add_id(world, e, id_2);
    test_assert(!ecs_has_id(world, e, id_1));
    test_assert(ecs_has_id(world, e, id_2));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_add_pair(world, e, Likes, Apples);
    test_assert(!ecs_has_id(world, e, id_2));
    test_assert(ecs_has_pair(world, e, Likes, Apples));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Socket);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, ParentRelative);

    ecs_add_id(world, Socket, ecs_nested_pair(world, EcsWith,
        ecs_pair(ecs_id(Position), ParentRelative)));

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Socket);
    test_assert(ecs_has(world, e, Socket));
    test_assert(ecs_has_pair(world, e, ecs_id(Position), ParentRelative));
    test_assert(!ecs_has(world, e, Position));
    test_int(ecs_get_type(world, e)->count, 2);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair_and_regular(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Socket);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, ParentRelative);

    ecs_add_pair(world, Socket, EcsWith, ecs_id(Position));
    ecs_add_id(world, Socket, ecs_nested_pair(world, EcsWith,
        ecs_pair(ecs_id(Position), ParentRelative)));
    ecs_set_name(world, Socket, "Socket");

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Socket);
    test_assert(ecs_has(world, e, Socket));
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_pair(world, e, ecs_id(Position), ParentRelative));
    test_int(ecs_get_type(world, e)->count, 3);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, ParentRelative);

    ecs_add_id(world, ecs_id(Velocity), ecs_nested_pair(world, EcsWith,
        ecs_pair(ecs_id(Position), ParentRelative)));

    ecs_entity_t e = ecs_insert(world, ecs_value(Velocity, {1, 2}));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has_pair(world, e, ecs_id(Position), ParentRelative));

    const Velocity *v = ecs_get(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    const Position *p = ecs_get_pair(world, e, Position, ParentRelative);
    test_assert(p != NULL);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair_on_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Socket);
    ECS_TAG(world, Tgt);
    ECS_TAG(world, Rel);
    ECS_TAG(world, RelTgt);

    ecs_add_id(world, Socket, ecs_nested_pair(world, EcsWith,
        ecs_pair(Rel, RelTgt)));

    /* The nested pair is added as is, the target of the added pair isn't
     * substituted like it is for regular With targets. */
    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, Socket, Tgt);
    test_assert(ecs_has_pair(world, e, Socket, Tgt));
    test_assert(ecs_has_pair(world, e, Rel, RelTgt));
    test_int(ecs_get_type(world, e)->count, 2);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair_recursive(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Socket);
    ECS_TAG(world, Rel);
    ECS_TAG(world, RelTgt);
    ECS_TAG(world, Other);

    ecs_add_id(world, Socket, ecs_nested_pair(world, EcsWith,
        ecs_pair(Rel, RelTgt)));
    ecs_add_pair(world, Rel, EcsWith, Other);

    /* Adding (Rel, RelTgt) also adds (Other, RelTgt) */
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Socket);
    test_assert(ecs_has(world, e, Socket));
    test_assert(ecs_has_pair(world, e, Rel, RelTgt));
    test_assert(ecs_has_pair(world, e, Other, RelTgt));
    test_int(ecs_get_type(world, e)->count, 3);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair_on_instance(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Socket);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, ParentRelative);

    ecs_add_id(world, Socket, ecs_nested_pair(world, EcsWith,
        ecs_pair(ecs_id(Position), ParentRelative)));

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_add(world, base, Socket);
    test_assert(ecs_has_pair(world, base, ecs_id(Position), ParentRelative));
    ecs_set_pair(world, base, Position, ParentRelative, {10, 20});

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has(world, inst, Socket));
    test_assert(ecs_has_pair(world, inst, ecs_id(Position), ParentRelative));
    test_assert(ecs_owns_pair(world, inst, ecs_id(Position), ParentRelative));

    const Position *p = ecs_get_pair(world, inst, Position, ParentRelative);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair_dont_fragment(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Socket);
    ECS_TAG(world, Rel);
    ecs_add_id(world, Socket, EcsDontFragment);
    ecs_add_id(world, Rel, EcsDontFragment);
    ECS_TAG(world, RelTgt);

    ecs_add_id(world, Socket, ecs_nested_pair(world, EcsWith,
        ecs_pair(Rel, RelTgt)));

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Socket);
    test_assert(ecs_has(world, e, Socket));
    test_assert(ecs_has_pair(world, e, Rel, RelTgt));
    test_int(ecs_get_type(world, e)->count, 0);

    ecs_fini(world);
}

void NestedPairs_with_nested_pair_delete_element(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Socket);
    ECS_TAG(world, Rel);
    ECS_TAG(world, RelTgt);

    ecs_id_t with = ecs_nested_pair(world, EcsWith, ecs_pair(Rel, RelTgt));
    ecs_add_id(world, Socket, with);

    ecs_delete(world, RelTgt);
    test_assert(!ecs_has_id(world, Socket, with));

    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Socket);
    test_assert(ecs_has(world, e, Socket));
    test_int(ecs_get_type(world, e)->count, 1);

    ecs_fini(world);
}

void NestedPairs_auto_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set_pair(world, base, Position, Tgt, {10, 20});
    ecs_auto_override_pair(world, base, ecs_id(Position), Tgt);

    ecs_id_t override =
        ecs_nested_pair(world, EcsAutoOverride, ecs_pair_t(Position, Tgt));
    test_assert(ecs_has_id(world, base, override));

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_owns_pair(world, inst, ecs_id(Position), Tgt));
    test_assert(!ecs_has_id(world, inst, override));

    const Position *p = ecs_get_pair(world, inst, Position, Tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(p != ecs_get_pair(world, base, Position, Tgt));

    ecs_fini(world);
}

void NestedPairs_auto_override_pair_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base_child = ecs_entity(world, { .name = "child", .parent = base });
    ecs_set_pair(world, base_child, Position, Tgt, {10, 20});
    ecs_auto_override_pair(world, base_child, ecs_id(Position), Tgt);
    ecs_auto_override(world, base_child, Position);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t inst_child = ecs_lookup_from(world, inst, "child");
    test_assert(inst_child != 0);

    test_assert(ecs_owns_pair(world, inst_child, ecs_id(Position), Tgt));
    test_assert(ecs_owns(world, inst_child, Position));
    test_assert(!ecs_has_id(world, inst_child,
        ecs_nested_pair(world, EcsAutoOverride, ecs_pair_t(Position, Tgt))));
    test_assert(!ecs_has_pair(world, inst_child,
        EcsAutoOverride, ecs_id(Position)));

    const Position *p = ecs_get_pair(world, inst_child, Position, Tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void NestedPairs_auto_override_str(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t e = ecs_new(world);
    ecs_auto_override_id(world, e, Tag);
    ecs_auto_override_pair(world, e, Rel, Tgt);

    char *str = ecs_type_str(world, ecs_get_type(world, e));
    test_str(str, "(AutoOverride,Tag), (AutoOverride,(Rel,Tgt))");
    ecs_os_free(str);

    ecs_fini(world);
}
