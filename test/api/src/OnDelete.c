#include <api.h>

void OnDelete_on_delete_id_default() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_id_remove() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_new_id(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsRemove);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_id_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_new_id(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsDelete);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_on_delete_relation_default() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_relation_remove() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDelete, EcsRemove);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_relation_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDelete, EcsDelete);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, o));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_on_delete_object_default() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, o);

    test_assert(ecs_is_alive(world, r));
    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_object_remove() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDeleteObject, EcsRemove);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, o);

    test_assert(ecs_is_alive(world, r));
    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_object_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDeleteObject, EcsDelete);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, o);

    test_assert(ecs_is_alive(world, r));
    test_assert(!ecs_is_alive(world, o));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_on_delete_id_throw() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_new_id(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsThrow);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    test_expect_abort();
    ecs_delete(world, c);
}

void OnDelete_on_delete_relation_throw() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDelete, EcsThrow);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    test_expect_abort();
    ecs_delete(world, r);
}

void OnDelete_on_delete_object_throw() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDeleteObject, EcsThrow);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    test_expect_abort();
    ecs_delete(world, o);
}

void OnDelete_on_delete_id_remove_no_instances() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_new_id(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsRemove);

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void OnDelete_on_delete_id_delete_no_instances() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_new_id(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsDelete);

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void OnDelete_on_delete_id_throw_no_instances() {
    install_test_abort();
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = ecs_new_id(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsThrow);

    test_expect_abort();
    ecs_delete(world, c);
}

void OnDelete_on_delete_cyclic_id_default() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_id_remove() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsRemove);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_id_remove_both() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsRemove);
    ecs_add_pair(world, b, EcsOnDelete, EcsRemove);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_int(ecs_vector_count(ecs_get_type(world, b)), 1);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_id_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_id_delete_both() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);
    ecs_add_pair(world, b, EcsOnDelete, EcsDelete);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_relation_default() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_relation_remove() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_entity_t o_a = ecs_new_id(world);
    ecs_entity_t o_b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsRemove);

    ecs_add_pair(world, a, b, o_b);
    ecs_add_pair(world, b, a, o_a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_relation_remove_both() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_entity_t o_a = ecs_new_id(world);
    ecs_entity_t o_b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsRemove);
    ecs_add_pair(world, b, EcsOnDelete, EcsRemove);

    ecs_add_pair(world, a, b, o_b);
    ecs_add_pair(world, b, a, o_a);

    ecs_delete(world, a);

    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_int(ecs_vector_count(ecs_get_type(world, b)), 1);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_relation_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_entity_t o_a = ecs_new_id(world);
    ecs_entity_t o_b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);

    ecs_add_pair(world, a, b, o_b);
    ecs_add_pair(world, b, a, o_a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_relation_delete_both() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_entity_t o_a = ecs_new_id(world);
    ecs_entity_t o_b = ecs_new_id(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);
    ecs_add_pair(world, b, EcsOnDelete, EcsDelete);

    ecs_add_pair(world, a, b, o_b);
    ecs_add_pair(world, b, a, o_a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_object_default() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_entity_t r = ecs_new_id(world);

    ecs_add_pair(world, a, r, b);
    ecs_add_pair(world, b, r, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_object_remove() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_entity_t r = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDeleteObject, EcsRemove);

    ecs_add_pair(world, a, r, b);
    ecs_add_pair(world, b, r, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_cyclic_object_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t a = ecs_new_id(world);
    ecs_entity_t b = ecs_new_id(world);
    ecs_entity_t r = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDeleteObject, EcsDelete);

    ecs_add_pair(world, a, r, b);
    ecs_add_pair(world, b, r, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_on_delete_remove_2_comps() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t c = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    
    ecs_add_id(world, e, c);
    ecs_set(world, e, Position, {10, 20});

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void OnDelete_on_delete_remove_2_comps_to_existing_table() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t c = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    
    ecs_add_id(world, e, c);
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_entity_t e3 = ecs_new_id(world);
    ecs_set(world, e3, Position, {40, 50});

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_int(p->x, 20);
    test_int(p->y, 30);

    p = ecs_get(world, e3, Position);
    test_int(p->x, 40);
    test_int(p->y, 50);

    ecs_fini(world);
}

void OnDelete_on_delete_delete_recursive() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_add_pair(world, r, EcsOnDeleteObject, EcsDelete);

    ecs_entity_t p = ecs_new_id(world);

    ecs_entity_t c = ecs_new_id(world);
    ecs_add_pair(world, c, r, p);

    ecs_entity_t gc = ecs_new_id(world);
    ecs_add_pair(world, gc, r, c);

    ecs_delete(world, p);

    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c));
    test_assert(!ecs_is_alive(world, gc));

    ecs_fini(world);
}

void OnDelete_on_delete_component_throw() {
    install_test_abort();
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set(world, 0, Position, {10, 20});

    test_expect_abort();
    ecs_delete(world, ecs_id(Position));
}

void OnDelete_on_delete_remove_2_relations() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o_1 = ecs_new_id(world);
    ecs_entity_t o_2 = ecs_new_id(world);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r, o_1);
    ecs_add_pair(world, e, r, o_2);

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, o_1));
    test_assert(ecs_is_alive(world, o_2));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_on_delete_remove_object_w_2_relations() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r_1 = ecs_new_id(world);
    ecs_entity_t r_2 = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, r_1, o);
    ecs_add_pair(world, e, r_2, o);

    ecs_delete(world, o);

    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_remove_id_from_2_tables() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t c = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_id(world, e1, c);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_id(world, e2, c);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_vector_count(ecs_get_type(world, e2)), 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_remove_relation_from_2_tables() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, r, o);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_vector_count(ecs_get_type(world, e2)), 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_remove_object_from_2_tables() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, r, o);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, o);

    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_vector_count(ecs_get_type(world, e2)), 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_remove_id_and_relation() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, r);
    ecs_add_pair(world, e, r, o);

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_remove_id_and_relation_from_2_tables() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e1, r);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e2, r);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_vector_count(ecs_get_type(world, e2)), 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_stresstest_many_objects() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    int i, ri;

    ecs_world_stats_t s = {0};
    ecs_get_world_stats(world, &s); 

    float table_count = s.table_count.avg[s.t];

    /* Precreate relations so we get different relationship ids */
    ecs_entity_t relations[100];
    for (i = 0; i < 100; i ++) {
        relations[i] = ecs_new_id(world);
    }

    for (ri = 0; ri < 100; ri ++) {    
        for (i = 0; i < 100; i ++) {
            ecs_entity_t o = ecs_new_id(world);
            ecs_entity_t e = ecs_new_id(world);
            ecs_add_id(world, e, Tag);
            ecs_add_pair(world, e, relations[ri], o);
        }

        ecs_delete(world, relations[ri]);
    }
    
    ecs_get_world_stats(world, &s);

    test_int(s.table_count.avg[s.t] - table_count, 1);

    ecs_fini(world);
}

void OnDelete_stresstest_many_relations() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    int i, oi;

    ecs_world_stats_t s = {0};
    ecs_get_world_stats(world, &s); 

    float table_count = s.table_count.avg[s.t];

    /* Precreate objects so we get different relationship ids */
    ecs_entity_t objects[100];
    for (i = 0; i < 100; i ++) {
        objects[i] = ecs_new_id(world);
    }

    for (oi = 0; oi < 100; oi ++) {    
        for (i = 0; i < 100; i ++) {
            ecs_entity_t r = ecs_new_id(world);
            ecs_entity_t e = ecs_new_id(world);
            ecs_add_id(world, e, Tag);
            ecs_add_pair(world, e, r, objects[oi]);
        }

        ecs_delete(world, objects[oi]);
    }
    
    ecs_get_world_stats(world, &s);

    test_int(s.table_count.avg[s.t] - table_count, 1);

    ecs_fini(world);
}

void OnDelete_stresstest_many_objects_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    int i, ri;

    /* Precreate relations so we get different relationship ids */
    ecs_entity_t relations[100];
    for (i = 0; i < 100; i ++) {
        relations[i] = ecs_new_id(world);
        ecs_add_pair(world, relations[i], EcsOnDelete, EcsDelete);
    }

    ecs_world_stats_t s = {0};
    ecs_get_world_stats(world, &s); 
    float table_count = s.table_count.avg[s.t];

    for (ri = 0; ri < 100; ri ++) {    
        for (i = 0; i < 100; i ++) {
            ecs_entity_t o = ecs_new_id(world);
            ecs_entity_t e = ecs_new_id(world);
            ecs_add_pair(world, e, relations[ri], o);
            ecs_add_id(world, e, Tag);
        }

        ecs_delete(world, relations[ri]);
    }
    
    ecs_get_world_stats(world, &s);
    test_int(s.table_count.avg[s.t] - table_count, 0);

    ecs_fini(world);
}

void OnDelete_stresstest_many_relations_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    int i, oi;

    /* Precreate objects so we get different relationship ids */
    ecs_entity_t objects[100];
    for (i = 0; i < 100; i ++) {
        objects[i] = ecs_new_id(world);
    }

    ecs_world_stats_t s = {0};
    ecs_get_world_stats(world, &s); 
    float table_count = s.table_count.avg[s.t];    

    for (oi = 0; oi < 100; oi ++) {    
        for (i = 0; i < 100; i ++) {
            ecs_entity_t r = ecs_new_id(world);
            ecs_add_pair(world, r, EcsOnDeleteObject, EcsDelete);

            ecs_entity_t e = ecs_new_id(world);
            ecs_add_pair(world, e, r, objects[oi]);
            ecs_add_id(world, e, Tag);
        }

        ecs_delete(world, objects[oi]);
    }
    
    ecs_get_world_stats(world, &s);

    /* 1 table for the relations */
    test_int(s.table_count.avg[s.t] - table_count, 1);

    ecs_fini(world);
}
