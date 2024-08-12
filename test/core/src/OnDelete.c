#include <core.h>

void OnDelete_flags(void) {
    ecs_flags32_t f_remove = EcsIdOnDeleteRemove;
    ecs_flags32_t f_delete = EcsIdOnDeleteDelete;
    ecs_flags32_t f_throw = EcsIdOnDeletePanic;

    ecs_flags32_t f_obj_remove = EcsIdOnDeleteObjectRemove;
    ecs_flags32_t f_obj_delete = EcsIdOnDeleteObjectDelete;
    ecs_flags32_t f_obj_throw = EcsIdOnDeleteObjectPanic;

    test_int(ECS_ID_ON_DELETE(f_remove), EcsRemove);
    test_int(ECS_ID_ON_DELETE(f_delete), EcsDelete);
    test_int(ECS_ID_ON_DELETE(f_throw), EcsPanic);

    test_int(ECS_ID_ON_DELETE_TARGET(f_obj_remove), EcsRemove);
    test_int(ECS_ID_ON_DELETE_TARGET(f_obj_delete), EcsDelete);
    test_int(ECS_ID_ON_DELETE_TARGET(f_obj_throw), EcsPanic);

    test_int(ECS_ID_ON_DELETE(f_obj_remove), 0);
    test_int(ECS_ID_ON_DELETE(f_obj_delete), 0);
    test_int(ECS_ID_ON_DELETE(f_obj_throw), 0);

    test_int(ECS_ID_ON_DELETE_TARGET(f_remove), 0);
    test_int(ECS_ID_ON_DELETE_TARGET(f_delete), 0);
    test_int(ECS_ID_ON_DELETE_TARGET(f_throw), 0);

    test_int(ECS_ID_ON_DELETE_FLAG(EcsRemove), f_remove);
    test_int(ECS_ID_ON_DELETE_FLAG(EcsDelete), f_delete);
    test_int(ECS_ID_ON_DELETE_FLAG(EcsPanic), f_throw);

    test_int(ECS_ID_ON_DELETE_TARGET_FLAG(EcsRemove), f_obj_remove);
    test_int(ECS_ID_ON_DELETE_TARGET_FLAG(EcsDelete), f_obj_delete);
    test_int(ECS_ID_ON_DELETE_TARGET_FLAG(EcsPanic), f_obj_throw);
}

void OnDelete_id_default(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);
    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has_id(world, e, c));

    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_id_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsRemove);

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_id_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsDelete);

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_relation_default(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_relation_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDelete, EcsRemove);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_relation_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDelete, EcsDelete);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, o));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_object_default(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, o);

    test_assert(ecs_is_alive(world, r));
    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_object_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDeleteTarget, EcsRemove);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, o);

    test_assert(ecs_is_alive(world, r));
    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_object_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDeleteTarget, EcsDelete);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    ecs_delete(world, o);

    test_assert(ecs_is_alive(world, r));
    test_assert(!ecs_is_alive(world, o));
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_object_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t t_1 = ecs_new(world);
    ecs_entity_t t_2 = ecs_new(world);

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);

    ecs_entity_t o = ecs_new(world);
    ecs_add_pair(world, r_1, EcsOnDeleteTarget, EcsDelete);

    ecs_entity_t e_1 = ecs_new_w_pair(world, r_1, o);
    ecs_add_id(world, e_1, t_1);

    ecs_entity_t e_2 = ecs_new_w_pair(world, r_2, o);
    ecs_add_id(world, e_2, t_1);

    ecs_entity_t e_3 = ecs_new_w_pair(world, r_1, o);
    ecs_add_id(world, e_3, t_2);

    ecs_entity_t e_4 = ecs_new_w_pair(world, r_2, o);
    ecs_add_id(world, e_4, t_2);

    ecs_delete(world, o);

    test_assert(ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(!ecs_is_alive(world, o));

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));
    test_assert(ecs_is_alive(world, e_4));

    ecs_fini(world);
}

void OnDelete_id_throw(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsPanic);

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, c);
    test_assert(ecs_has_id(world, e, c));

    test_expect_abort();
    ecs_delete(world, c);
}

void OnDelete_relation_throw(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDelete, EcsPanic);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    test_expect_abort();
    ecs_delete(world, r);
}

void OnDelete_object_throw(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDeleteTarget, EcsPanic);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, o);
    test_assert(ecs_has_pair(world, e, r, o));

    test_expect_abort();
    ecs_delete(world, o);
}

void OnDelete_id_remove_no_instances(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsRemove);

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void OnDelete_id_delete_no_instances(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsDelete);

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void OnDelete_id_throw_no_instances(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);
    ecs_add_pair(world, c, EcsOnDelete, EcsPanic);

    test_expect_abort();
    ecs_delete(world, c);
}

void OnDelete_cyclic_self(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_entity_t r = ecs_new(world);

    ecs_add_pair(world, e, r, e);
    test_assert(ecs_has_pair(world, e, r, e));

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_nonempty_cyclic_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_entity_t r = ecs_new(world);

    ecs_add_pair(world, e, r, e);
    test_assert(ecs_has_pair(world, e, r, e));

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_cyclic_id_default(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);

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

void OnDelete_cyclic_id_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);

    ecs_add_pair(world, a, EcsOnDelete, EcsRemove);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_record_t *r_a = ecs_record_find(world, a);
    test_assert(r_a != NULL);
    test_assert((r_a->row & ECS_ROW_FLAGS_MASK) != 0);

    ecs_record_t *r_b = ecs_record_find(world, b);
    test_assert(r_b != NULL);
    test_assert((r_b->row & ECS_ROW_FLAGS_MASK) != 0);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_get_type(world, b) == NULL);
    test_assert(ecs_get_table(world, b) == NULL);

    test_assert(r_b == ecs_record_find(world, b));
    test_assert((r_b->row & ECS_ROW_FLAGS_MASK) != 0);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_id_remove_both(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsRemove);
    ecs_add_pair(world, b, EcsOnDelete, EcsRemove);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_int(ecs_get_type(world, b)->count, 1);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_id_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_id_delete_both(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);
    ecs_add_pair(world, b, EcsOnDelete, EcsDelete);

    ecs_add_id(world, a, b);
    ecs_add_id(world, b, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_relation_default(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);

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

void OnDelete_cyclic_relation_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t o_a = ecs_new(world);
    ecs_entity_t o_b = ecs_new(world);
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

void OnDelete_cyclic_relation_remove_both(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t o_a = ecs_new(world);
    ecs_entity_t o_b = ecs_new(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsRemove);
    ecs_add_pair(world, b, EcsOnDelete, EcsRemove);

    ecs_add_pair(world, a, b, o_b);
    ecs_add_pair(world, b, a, o_a);

    ecs_delete(world, a);

    test_assert(!ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, b));
    test_int(ecs_get_type(world, b)->count, 1);

    ecs_delete(world, b);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_relation_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t o_a = ecs_new(world);
    ecs_entity_t o_b = ecs_new(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);

    ecs_add_pair(world, a, b, o_b);
    ecs_add_pair(world, b, a, o_a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_relation_delete_both(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t o_a = ecs_new(world);
    ecs_entity_t o_b = ecs_new(world);
    ecs_add_pair(world, a, EcsOnDelete, EcsDelete);
    ecs_add_pair(world, b, EcsOnDelete, EcsDelete);

    ecs_add_pair(world, a, b, o_b);
    ecs_add_pair(world, b, a, o_a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_object_default(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t r = ecs_new(world);

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

void OnDelete_cyclic_object_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t r = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDeleteTarget, EcsRemove);

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

void OnDelete_cyclic_object_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t r = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDeleteTarget, EcsDelete);

    ecs_add_pair(world, a, r, b);
    ecs_add_pair(world, b, r, a);

    ecs_delete(world, a);
    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));

    ecs_fini(world);
}

void OnDelete_cyclic_overlapping_table(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ecs_entity_t obj_a = ecs_new(world);
    ecs_entity_t obj_b = ecs_new(world);
    ecs_entity_t obj_c = ecs_new(world);

    ecs_add_pair(world, obj_a, rel, obj_b);
    ecs_add_pair(world, obj_b, rel, obj_a);
    ecs_add_pair(world, obj_c, rel, obj_a);
    ecs_add_pair(world, obj_c, rel, obj_b);

    ecs_delete(world, obj_a);

    test_bool(false, ecs_is_alive(world, obj_a));
    test_bool(false, ecs_is_alive(world, obj_b));
    test_bool(false, ecs_is_alive(world, obj_c));

    ecs_fini(world);
}

void OnDelete_cyclic_overlapping_new_tables(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);

    ecs_entity_t rel_1 = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ecs_entity_t rel_2 = ecs_new(world);
    ecs_entity_t obj_a = ecs_new(world);
    ecs_entity_t obj_b = ecs_new(world);

    ecs_add_pair(world, obj_a, rel_1, obj_b);
    ecs_add_pair(world, obj_b, rel_1, obj_a);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, rel_2, obj_a);
    ecs_add_pair(world, e, rel_2, obj_b);
    ecs_add_id(world, e, tag);

    ecs_delete(world, obj_a);

    test_bool(false, ecs_is_alive(world, obj_a));
    test_bool(false, ecs_is_alive(world, obj_b));
    test_bool(true, ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_cyclic_object_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel_1 = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ecs_entity_t rel_2 = ecs_new(world);
    ecs_entity_t obj_a = ecs_new(world);
    ecs_entity_t obj_b = ecs_new(world);

    ecs_add_pair(world, obj_a, rel_1, obj_b);
    ecs_add_pair(world, obj_b, rel_1, obj_a);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, rel_2, obj_a);
    ecs_add_pair(world, e, rel_2, obj_b);

    ecs_delete(world, obj_a);

    test_bool(false, ecs_is_alive(world, obj_a));
    test_bool(false, ecs_is_alive(world, obj_b));
    test_bool(true, ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_cyclic_storage_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t rel_1 = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ecs_set(world, rel_1, EcsComponent, {1, 1});

    ecs_entity_t obj_a = ecs_new(world);
    ecs_entity_t obj_b = ecs_new(world);

    ecs_add_pair(world, obj_a, rel_1, obj_b);

    ecs_add(world, obj_b, Tag);
    ecs_add_pair(world, obj_b, rel_1, obj_a);
    ecs_add_pair(world, obj_b, rel_1, obj_b);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, rel_1, obj_a);
    ecs_add_pair(world, e, rel_1, obj_b);

    ecs_delete(world, obj_a);

    test_bool(false, ecs_is_alive(world, obj_a));
    test_bool(false, ecs_is_alive(world, obj_b));
    test_bool(false, ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_cyclic_storage_table_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t rel_1 = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ecs_set(world, rel_1, EcsComponent, {1, 1});

    ecs_entity_t obj_a = ecs_new(world);
    ecs_entity_t obj_b = ecs_new(world);

    ecs_add_pair(world, obj_a, rel_1, obj_b);

    ecs_add(world, obj_b, Tag);
    ecs_add_pair(world, obj_b, rel_1, obj_a);
    ecs_add_pair(world, obj_b, rel_1, obj_b);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, rel_1, obj_a);
    ecs_add_pair(world, e, rel_1, obj_b);

    ecs_remove_pair(world, e, rel_1, obj_b);

    ecs_delete(world, obj_a);

    test_bool(false, ecs_is_alive(world, obj_a));
    test_bool(false, ecs_is_alive(world, obj_b));
    test_bool(false, ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_cyclic_storage_table_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t rel_1 = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ecs_set(world, rel_1, EcsComponent, {1, 1});

    ecs_entity_t obj_a = ecs_new(world);
    ecs_entity_t obj_b = ecs_new(world);

    ecs_add_pair(world, obj_a, rel_1, obj_b);

    ecs_add(world, obj_b, Tag);
    ecs_add_pair(world, obj_b, rel_1, obj_a);
    ecs_add_pair(world, obj_b, rel_1, obj_b);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, rel_1, obj_a);
    ecs_add_pair(world, e, rel_1, obj_b);

    ecs_delete(world, obj_a);

    test_bool(false, ecs_is_alive(world, obj_a));
    test_bool(false, ecs_is_alive(world, obj_b));
    test_bool(false, ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_cyclic_set_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t rel_1 = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ecs_entity_t rel_2 = ecs_new(world);
    ecs_entity_t obj_a = ecs_new(world);
    ecs_entity_t obj_b = ecs_new(world);

    ecs_add_pair(world, obj_a, rel_1, obj_b);
    ecs_add_pair(world, obj_b, rel_1, obj_a);
    ecs_add_pair(world, obj_b, rel_2, obj_b);

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_add_pair(world, e, rel_2, obj_a);

    ecs_delete(world, obj_a);

    test_bool(false, ecs_is_alive(world, obj_a));
    test_bool(false, ecs_is_alive(world, obj_b));
    test_bool(true, ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_2_acyclic_relations_w_cycle(void) {
    test_quarantine("May 4th 2022");
    
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r1 = ecs_new_w_id(world, EcsTraversable);
    ecs_entity_t r2 = ecs_new_w_id(world, EcsTraversable);

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new(world);

    ecs_add_pair(world, a, r1, b);
    ecs_add_pair(world, b, r2, a);

    ecs_fini(world);
}

void OnDelete_remove_2_comps(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t c = ecs_new(world);
    ecs_entity_t e = ecs_new(world);
    
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

void OnDelete_remove_2_comps_to_existing_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t c = ecs_new(world);
    ecs_entity_t e = ecs_new(world);
    
    ecs_add_id(world, e, c);
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {20, 30});
    ecs_entity_t e3 = ecs_new(world);
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

void OnDelete_delete_recursive(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_add_pair(world, r, EcsOnDeleteTarget, EcsDelete);

    ecs_entity_t p = ecs_new(world);

    ecs_entity_t c = ecs_new(world);
    ecs_add_pair(world, c, r, p);

    ecs_entity_t gc = ecs_new(world);
    ecs_add_pair(world, gc, r, c);

    ecs_delete(world, p);

    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c));
    test_assert(!ecs_is_alive(world, gc));

    ecs_fini(world);
}

void OnDelete_component_throw(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_add_pair(world, ecs_id(Position), EcsOnDelete, EcsPanic);

    ecs_insert(world, ecs_value(Position, {10, 20}));

    test_expect_abort();
    ecs_delete(world, ecs_id(Position));
}

void OnDelete_remove_2_relations(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o_1 = ecs_new(world);
    ecs_entity_t o_2 = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
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

void OnDelete_remove_object_w_2_relations(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
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

void OnDelete_remove_object_w_5_relations(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t r_3 = ecs_new(world);
    ecs_entity_t r_4 = ecs_new(world);
    ecs_entity_t r_5 = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r_1, o);
    ecs_add_pair(world, e, r_2, o);
    ecs_add_pair(world, e, r_3, o);
    ecs_add_pair(world, e, r_4, o);
    ecs_add_pair(world, e, r_5, o);

    ecs_delete(world, o);

    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_is_alive(world, r_3));
    test_assert(ecs_is_alive(world, r_4));
    test_assert(ecs_is_alive(world, r_5));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_remove_object_w_50_relations(void) {
    ecs_world_t *world = ecs_mini();

    #define NUM 50

    ecs_entity_t r[NUM];
    int i;
    for (i = 0; i < NUM; i ++) {
        r[i] = ecs_new(world);
    }

    ecs_entity_t o = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    for (i = 0; i < NUM; i ++) {
        ecs_add_pair(world, e, r[i], o);
    }

    ecs_delete(world, o);

    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    #undef NUM

    ecs_fini(world);
}

void OnDelete_remove_object_w_50_relations_3_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    #define NUM 50

    ecs_entity_t r[NUM];
    int i;
    for (i = 0; i < NUM; i ++) {
        r[i] = ecs_new(world);
    }

    ecs_entity_t o = ecs_new(world);
    ecs_entity_t e_1 = ecs_new_w_id(world, TagA);
    ecs_entity_t e_2 = ecs_new_w_id(world, TagB);
    ecs_entity_t e_3 = ecs_new_w_id(world, TagC);

    for (i = 0; i < NUM; i ++) {
        ecs_add_pair(world, e_1, r[i], o);
        ecs_add_pair(world, e_2, r[i], o);
        ecs_add_pair(world, e_3, r[i], o);
    }

    ecs_delete(world, o);

    test_assert(!ecs_is_alive(world, o));

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    test_assert(ecs_has_id(world, e_1, TagA));
    test_assert(ecs_has_id(world, e_2, TagB));
    test_assert(ecs_has_id(world, e_3, TagC));

    #undef NUM

    ecs_fini(world);
}

void OnDelete_remove_object_w_3_relations_interleaved(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t r_3 = ecs_new(world);
    ecs_entity_t t_1 = ecs_new(world);
    ecs_entity_t t_2 = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_pair(world, e, r_1, t_1);
    ecs_add_pair(world, e, r_2, t_1);
    ecs_add_pair(world, e, r_3, t_1);

    ecs_add_pair(world, e, r_1, t_2);
    ecs_add_pair(world, e, r_2, t_2);
    ecs_add_pair(world, e, r_3, t_2);

    test_assert(ecs_has_pair(world, e, r_1, t_1));
    test_assert(ecs_has_pair(world, e, r_2, t_1));
    test_assert(ecs_has_pair(world, e, r_3, t_1));

    test_assert(ecs_has_pair(world, e, r_1, t_2));
    test_assert(ecs_has_pair(world, e, r_2, t_2));
    test_assert(ecs_has_pair(world, e, r_3, t_2));

    ecs_delete(world, t_1);

    test_assert(ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_is_alive(world, r_3));
    test_assert(!ecs_is_alive(world, t_1));
    test_assert(ecs_is_alive(world, t_2));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_pair(world, e, r_1, t_1));
    test_assert(!ecs_has_pair(world, e, r_2, t_1));
    test_assert(!ecs_has_pair(world, e, r_3, t_1));

    test_assert(ecs_has_pair(world, e, r_1, t_2));
    test_assert(ecs_has_pair(world, e, r_2, t_2));
    test_assert(ecs_has_pair(world, e, r_3, t_2));

    ecs_fini(world);
}

void OnDelete_remove_id_from_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t c = ecs_new(world);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_id(world, e1, c);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_id(world, e2, c);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_get_type(world, e2)->count, 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_remove_relation_from_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, r, o);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_get_type(world, e2)->count, 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_remove_object_from_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, r, o);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, o);

    test_assert(!ecs_is_alive(world, o));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_get_type(world, e2)->count, 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_remove_id_and_relation(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, r);
    ecs_add_pair(world, e, r, o);

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void OnDelete_remove_id_and_relation_from_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e1, r);

    ecs_entity_t e2 = ecs_new(world);
    ecs_add_pair(world, e1, r, o);
    ecs_add_id(world, e2, r);
    ecs_add_id(world, e2, Tag);

    ecs_delete(world, r);

    test_assert(!ecs_is_alive(world, r));
    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_get_type(world, e1) == NULL);
    test_int(ecs_get_type(world, e2)->count, 1);
    test_assert(ecs_has_id(world, e2, Tag));

    ecs_fini(world);
}

void OnDelete_stresstest_many_objects(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    int i, ri;

    ecs_world_stats_t s = {0};
    ecs_world_stats_get(world, &s); 

    float table_count = s.tables.count.gauge.avg[s.t];

    /* Precreate relationships so we get different relationship ids */
    ecs_entity_t relationships[100];
    for (i = 0; i < 100; i ++) {
        relationships[i] = ecs_new(world);
    }

    for (ri = 0; ri < 100; ri ++) {    
        for (i = 0; i < 100; i ++) {
            ecs_entity_t o = ecs_new(world);
            ecs_entity_t e = ecs_new(world);
            ecs_add_id(world, e, Tag);
            ecs_add_pair(world, e, relationships[ri], o);
        }

        ecs_delete(world, relationships[ri]);
    }
    
    ecs_world_stats_get(world, &s);

    test_int(s.tables.count.gauge.avg[s.t] - table_count, 1);

    ecs_fini(world);
}

void OnDelete_stresstest_many_relations(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    int i, oi;

    ecs_world_stats_t s = {0};
    ecs_world_stats_get(world, &s); 

    float table_count = s.tables.count.gauge.avg[s.t];

    /* Precreate objects so we get different relationship ids */
    ecs_entity_t objects[100];
    for (i = 0; i < 100; i ++) {
        objects[i] = ecs_new(world);
    }

    for (oi = 0; oi < 100; oi ++) {    
        for (i = 0; i < 100; i ++) {
            ecs_entity_t r = ecs_new(world);
            ecs_entity_t e = ecs_new(world);
            ecs_add_id(world, e, Tag);
            ecs_add_pair(world, e, r, objects[oi]);
        }

        ecs_delete(world, objects[oi]);
    }
    
    ecs_world_stats_get(world, &s);

    test_int(s.tables.count.gauge.avg[s.t] - table_count, 1);

    ecs_fini(world);
}

void OnDelete_stresstest_many_objects_on_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    int i, ri;

    /* Precreate relationships so we get different relationship ids */
    ecs_entity_t relationships[100];
    for (i = 0; i < 100; i ++) {
        relationships[i] = ecs_new(world);
        ecs_add_pair(world, relationships[i], EcsOnDelete, EcsDelete);
    }

    ecs_world_stats_t s = {0};
    ecs_world_stats_get(world, &s); 
    float table_count = s.tables.count.gauge.avg[s.t];

    for (ri = 0; ri < 100; ri ++) {    
        for (i = 0; i < 100; i ++) {
            ecs_entity_t o = ecs_new(world);
            ecs_entity_t e = ecs_new(world);
            ecs_add_pair(world, e, relationships[ri], o);
            ecs_add_id(world, e, Tag);
        }

        ecs_delete(world, relationships[ri]);
    }
    
    ecs_world_stats_get(world, &s);
    test_int(s.tables.count.gauge.avg[s.t] - table_count, 0);

    ecs_fini(world);
}

void OnDelete_stresstest_many_relations_on_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    int i, oi;

    #define COUNT 50

    /* Precreate objects so we get different relationship ids */
    ecs_entity_t objects[COUNT];
    for (i = 0; i < COUNT; i ++) {
        objects[i] = ecs_new(world);
    }

    /* Add (OnDeleteTarget, Delete) to a dummy object so that it won't mess up
     * with the table count */
    ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);

    ecs_world_stats_t s = {0};
    ecs_world_stats_get(world, &s); 
    float table_count = s.tables.count.gauge.avg[s.t];    

    for (oi = 0; oi < COUNT; oi ++) {    
        for (i = 0; i < COUNT; i ++) {
            ecs_entity_t r = ecs_new(world);
            ecs_add_pair(world, r, EcsOnDeleteTarget, EcsDelete);

            ecs_entity_t e = ecs_new(world);
            ecs_add_pair(world, e, r, objects[oi]);
            ecs_add_id(world, e, Tag);
        }

        ecs_delete(world, objects[oi]);
    }
    
    ecs_world_stats_get(world, &s);

    test_int(s.tables.count.gauge.avg[s.t] - table_count, 0);

    #undef COUNT

    ecs_fini(world);
}

static ecs_entity_t trigger_entity;
static int trigger_count;

static
void dummy_on_remove(ecs_iter_t *it) {
    test_int(it->count, 1);
    trigger_entity = it->entities[0];

    trigger_count ++;
    test_int(trigger_count, 1);
}

void OnDelete_empty_table_w_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    ecs_add_id(world, e1, e2);
    ecs_add_id(world, e1, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = e2,
        .events = {EcsOnRemove},
        .callback = dummy_on_remove
    });

    ecs_delete(world, e1);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(trigger_entity != 0);
    test_assert(trigger_entity == e1);
    test_int(trigger_count, 1);

    // Will delete table of e1, which is now empty. Should not call trigger
    ecs_delete_with(world, e2);

    test_int(trigger_count, 1);

    ecs_fini(world);
}

typedef struct {
    ecs_entity_t other;
} Entity;

static 
void delete_on_remove(ecs_iter_t *it) {
    test_int(it->count, 1);
    Entity *comp = ecs_field(it, Entity, 0);
    test_assert(comp != NULL);
    test_assert(comp->other != 0);
    ecs_delete(it->world, comp->other);
    trigger_count ++;
}

static 
void delete_self_on_remove(ecs_iter_t *it) {
    ecs_entity_t e = *(ecs_entity_t*)it->ctx;
    test_int(it->count, 1);
    ecs_delete(it->world, e);
    trigger_count ++;
}

void OnDelete_delete_table_in_on_remove_during_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    /* Setup scenario so that the order in which tables are deleted doesn't
     * matter for reproducing the issue */
    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    /* Add entities to each other so that deleting one will delete the table of
     * the other. */
    ecs_add_id(world, e1, e2);
    ecs_add_id(world, e2, e1);
    
    /* Make sure entities are in different tables */
    ecs_add_id(world, e1, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .events = {EcsOnRemove},
        .query.terms[0].id = e1,
        .callback = delete_self_on_remove,
        .ctx = &e2
    });

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .events = {EcsOnRemove},
        .query.terms[0].id = e2,
        .callback = delete_self_on_remove,
        .ctx = &e1
    });    

    ecs_fini(world);

    /* Trigger must have been called for both entities */
    test_int(trigger_count, 2);
}

void OnDelete_delete_other_in_on_remove_during_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Entity);
    ECS_TAG(world, Tag);

    /* Setup scenario so that the order in which tables are deleted doesn't
     * matter for reproducing the issue */

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);

    /* Removing Entity from one will delete the other */
    ecs_set(world, e1, Entity, {e2});
    ecs_set(world, e2, Entity, {e1});

    /* Make sure entities are in different tables */
    ecs_add_id(world, e1, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .events = {EcsOnRemove},
        .query.terms[0].id = ecs_id(Entity),
        .callback = delete_on_remove
    });

    ecs_fini(world);

    /* Trigger must have been called for both entities */
    test_int(trigger_count, 2);
}

void OnDelete_remove_id_w_role(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    
    ecs_add_id(world, e, ECS_TOGGLE | c);
    test_assert(ecs_has_id(world, e, ECS_TOGGLE | c));

    ecs_delete(world, c);

    test_assert(!ecs_is_alive(world, c));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_get_type(world, e) == NULL);
    test_assert(!ecs_has_id(world, e, ECS_TOGGLE | c));

    ecs_fini(world);
}

void OnDelete_remove_rel_w_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t t = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t)));

    ecs_delete(world, r_1);

    test_assert(!ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_is_alive(world, t));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t)));

    ecs_fini(world);
}

void OnDelete_remove_obj_w_override_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t t_1 = ecs_new(world);
    ecs_entity_t t_2 = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r, t_2));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r, t_2)));

    ecs_delete(world, t_1);

    test_assert(ecs_is_alive(world, r));
    test_assert(!ecs_is_alive(world, t_1));
    test_assert(ecs_is_alive(world, t_2));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r, t_2)));

    ecs_fini(world);
}

void OnDelete_remove_rel_w_override_pair_after_on_delete_target(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t t = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t)));

    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, r_1, EcsChildOf, p);

    ecs_delete(world, p);

    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_is_alive(world, t));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t)));

    ecs_fini(world);
}

void OnDelete_remove_rel_w_override_pair_2_ids(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t t_1 = ecs_new(world);
    ecs_entity_t t_2 = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));

    ecs_delete(world, r_1);

    test_assert(!ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_is_alive(world, t_1));
    test_assert(ecs_is_alive(world, t_2));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));

    ecs_fini(world);
}

void OnDelete_remove_obj_w_override_pair_2_ids(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t t_1 = ecs_new(world);
    ecs_entity_t t_2 = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));

    ecs_delete(world, t_1);

    test_assert(ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(!ecs_is_alive(world, t_1));
    test_assert(ecs_is_alive(world, t_2));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));

    ecs_fini(world);
}

void OnDelete_remove_obj_w_override_pair_3_ids(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t r_3 = ecs_new(world);
    ecs_entity_t t_1 = ecs_new(world);
    ecs_entity_t t_2 = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_1));

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_2));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_2));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_1)));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_2)));

    ecs_delete(world, t_1);

    test_assert(ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_is_alive(world, r_3));
    test_assert(!ecs_is_alive(world, t_1));
    test_assert(ecs_is_alive(world, t_2));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_1)));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_2)));

    ecs_fini(world);
}

void OnDelete_remove_mixed_w_override_pair_3_ids(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r_1 = ecs_new(world);
    ecs_entity_t r_2 = ecs_new(world);
    ecs_entity_t r_3 = ecs_new(world);

    ecs_entity_t t_1 = ecs_new(world);
    ecs_entity_t t_2 = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_1));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_2));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_3));

    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_2));
    ecs_add_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_2));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_1)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_3)));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_2)));

    ecs_delete(world, t_1);

    test_assert(ecs_is_alive(world, r_1));
    test_assert(ecs_is_alive(world, r_2));
    test_assert(ecs_is_alive(world, r_3));
    test_assert(!ecs_is_alive(world, t_1));
    test_assert(ecs_is_alive(world, t_2));
    test_assert(ecs_is_alive(world, e));

    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_1)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_2)));
    test_assert(!ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(t_1, r_3)));

    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_1, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_2, t_2)));
    test_assert(ecs_has_id(world, e, ECS_AUTO_OVERRIDE | ecs_pair(r_3, t_2)));

    ecs_fini(world);
}

void OnDelete_merge_pair_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e_1 = ecs_new(world);
    ecs_entity_t e_2 = ecs_new(world);
    ecs_entity_t obj = ecs_new(world);

    ecs_add_id(world, e_1, e_2);
    ecs_set_pair(world, e_1, Position, obj, {10, 20});
    ecs_delete(world, e_2);

    const Position *ptr = ecs_get_pair(world, e_1, Position, obj);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void OnDelete_delete_with_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e_1 = ecs_new_w_id(world, TagA);
    ecs_entity_t e_2 = ecs_new_w_id(world, TagA);
    ecs_entity_t e_3 = ecs_new_w_id(world, TagA);

    ecs_add_id(world, e_3, TagB);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    ecs_delete_with(world, TagA);

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));

    ecs_fini(world);
}

void OnDelete_delete_with_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e_1 = ecs_new_w(world, Position);
    ecs_entity_t e_2 = ecs_new_w(world, Position);
    ecs_entity_t e_3 = ecs_new_w(world, Position);

    ecs_add(world, e_3, Velocity);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    ecs_delete_with(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));

    ecs_fini(world);
}

void OnDelete_delete_with_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, Obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, Obj);

    ecs_add_id(world, e_3, Tag);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    ecs_delete_with(world, ecs_pair(Rel, Obj));

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));

    ecs_fini(world);
}

void OnDelete_delete_with_object_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, ObjB);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, ObjB);

    ecs_add_id(world, e_3, Tag);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    ecs_delete_with(world, ecs_pair(Rel, EcsWildcard));

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));

    ecs_fini(world);
}

void OnDelete_delete_with_relation_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, RelA, Obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, RelB, Obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, RelB, Obj);

    ecs_add_id(world, e_3, Tag);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    ecs_delete_with(world, ecs_pair(EcsWildcard, Obj));

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));

    ecs_fini(world);
}

void OnDelete_delete_all_with_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Tag);

    ecs_entity_t e_1 = ecs_new_w_pair(world, RelA, Obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, RelB, Obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, RelB, Obj);
    ecs_entity_t e_4 = ecs_new_w_pair(world, Obj, RelA);
    ecs_entity_t e_5 = ecs_new_w_pair(world, Obj, RelB);
    ecs_entity_t e_6 = ecs_new_w_id(world, Obj);

    ecs_add_id(world, e_3, Tag);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);
    test_assert(e_4 != 0);
    test_assert(e_5 != 0);
    test_assert(e_6 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    test_assert(ecs_is_alive(world, e_4));
    test_assert(ecs_is_alive(world, e_5));
    test_assert(ecs_is_alive(world, e_6));

    ecs_delete_with(world, Obj);
    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    test_assert(ecs_is_alive(world, e_4));
    test_assert(ecs_is_alive(world, e_5));
    test_assert(!ecs_is_alive(world, e_6));

    ecs_delete_with(world, ecs_pair(Obj, EcsWildcard));
    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    test_assert(!ecs_is_alive(world, e_4));
    test_assert(!ecs_is_alive(world, e_5));
    test_assert(!ecs_is_alive(world, e_6));

    ecs_delete_with(world, ecs_pair(EcsWildcard, Obj));
    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));
    test_assert(!ecs_is_alive(world, e_4));
    test_assert(!ecs_is_alive(world, e_5));
    test_assert(!ecs_is_alive(world, e_6));

    ecs_fini(world);
}

void OnDelete_remove_childof_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent_1 = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);

    ecs_entity_t e_1 = ecs_new_w_pair(world, EcsChildOf, parent_1);
    ecs_entity_t e_2 = ecs_new_w_pair(world, EcsChildOf, parent_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, EcsChildOf, parent_1);
    ecs_entity_t e_4 = ecs_new_w_pair(world, EcsChildOf, parent_2);

    ecs_add_id(world, e_3, Tag);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);
    test_assert(e_4 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    test_assert(ecs_is_alive(world, e_4));

    ecs_remove_all(world, ecs_pair(EcsChildOf, parent_1));

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    test_assert(ecs_is_alive(world, e_4));

    test_assert(!ecs_has_pair(world, e_1, EcsChildOf, parent_1));
    test_assert(!ecs_has_pair(world, e_2, EcsChildOf, parent_1));
    test_assert(!ecs_has_pair(world, e_3, EcsChildOf, parent_1));
    
    test_assert(!ecs_has_pair(world, e_1, EcsChildOf, parent_2));
    test_assert(!ecs_has_pair(world, e_2, EcsChildOf, parent_2));
    test_assert(!ecs_has_pair(world, e_3, EcsChildOf, parent_2));
    test_assert(ecs_has_pair(world, e_4, EcsChildOf, parent_2));

    test_assert(ecs_has_id(world, e_3, Tag));

    ecs_fini(world);
}

void OnDelete_remove_childof_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    /* Create custom childof relationship so we won't try to remove ChildOf from
     * builtin entities */
    ecs_entity_t child_of = ecs_new(world);

    /* Simulate same behavior as ChildOf */
    ecs_add_pair(world, child_of, EcsOnDeleteTarget, EcsDelete);

    ecs_entity_t parent_1 = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);

    ecs_entity_t e_1 = ecs_new_w_pair(world, child_of, parent_1);
    ecs_entity_t e_2 = ecs_new_w_pair(world, child_of, parent_1);
    ecs_entity_t e_3 = ecs_new_w_pair(world, child_of, parent_1);
    ecs_entity_t e_4 = ecs_new_w_pair(world, child_of, parent_2);

    ecs_add_id(world, e_3, Tag);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);
    test_assert(e_4 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    test_assert(ecs_is_alive(world, e_4));

    ecs_remove_all(world, ecs_pair(child_of, EcsWildcard));

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));
    test_assert(ecs_is_alive(world, e_4));

    test_assert(!ecs_has_pair(world, e_1, child_of, parent_1));
    test_assert(!ecs_has_pair(world, e_2, child_of, parent_1));
    test_assert(!ecs_has_pair(world, e_3, child_of, parent_1));
    test_assert(!ecs_has_pair(world, e_4, child_of, parent_2));

    test_assert(!ecs_has_id(world, e_1, Tag));
    test_assert(!ecs_has_id(world, e_2, Tag));
    test_assert(!ecs_has_id(world, e_4, Tag));
    test_assert(ecs_has_id(world, e_3, Tag));

    ecs_fini(world);
}

void OnDelete_delete_child_of_delete_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t obj = ecs_new(world);

    ecs_entity_t e_1 = ecs_new_w_pair(world, Rel, obj);
    ecs_entity_t e_2 = ecs_new_w_pair(world, Rel, obj);
    ecs_entity_t e_3 = ecs_new_w_pair(world, Rel, obj);

    ecs_entity_t c_1 = ecs_new_w_pair(world, EcsChildOf, e_1);
    ecs_entity_t c_2 = ecs_new_w_pair(world, EcsChildOf, e_2);
    ecs_entity_t c_3 = ecs_new_w_pair(world, EcsChildOf, e_3);

    test_assert(e_1 != 0);
    test_assert(e_2 != 0);
    test_assert(e_3 != 0);

    test_assert(c_1 != 0);
    test_assert(c_2 != 0);
    test_assert(c_3 != 0);

    test_assert(ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, e_2));
    test_assert(ecs_is_alive(world, e_3));

    test_assert(ecs_is_alive(world, c_1));
    test_assert(ecs_is_alive(world, c_2));
    test_assert(ecs_is_alive(world, c_3));

    ecs_delete_with(world, ecs_pair(Rel, obj));

    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));

    test_assert(!ecs_is_alive(world, c_1));
    test_assert(!ecs_is_alive(world, c_2));
    test_assert(!ecs_is_alive(world, c_3));

    ecs_fini(world);
}

void OnDelete_delete_with_component_after_delete_cyclic_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add_pair(world, e2, Rel, e2);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));

    ecs_delete_with(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));

    ecs_fini(world);
}

void OnDelete_delete_with_component_after_delete_cyclic(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);
    ecs_add_pair(world, e2, Rel, e3);
    ecs_add_pair(world, e3, Rel, e2);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));

    ecs_delete_with(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    ecs_fini(world);
}

void OnDelete_delete_with_component_after_delete_cyclic_w_alive_moved(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_entity_t e3 = ecs_new_w(world, Position);

    ecs_entity_t e4 = ecs_new(world);
    ecs_add_pair(world, e4, Rel, e2);
    ecs_add(world, e4, Velocity);

    ecs_add_pair(world, e2, Rel, e3);
    ecs_add_pair(world, e3, Rel, e2);

    ecs_entity_t e5 = ecs_new(world);
    ecs_add_pair(world, e5, Rel, e2);
    ecs_add_pair(world, e5, Rel, e3);
    ecs_add(world, e5, Velocity);

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));
    test_assert(ecs_is_alive(world, e4));
    test_assert(ecs_is_alive(world, e5));

    ecs_delete_with(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));
    test_assert(ecs_is_alive(world, e4));
    test_assert(ecs_is_alive(world, e5));

    test_assert(ecs_has(world, e4, Velocity));
    test_assert(ecs_has(world, e5, Velocity));

    ecs_fini(world);
}

void OnDelete_deep_clean_64(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    test_assert(r != 0);

    ecs_entity_t first = ecs_new(world);
    ecs_entity_t prev = first;
    ecs_entity_t entities[64];

    for (int i = 0; i < 64; i ++) {
        entities[i] = ecs_new(world);
        ecs_add_pair(world, entities[i], r, prev);
        prev = entities[i];
    }

    for (int i = 0; i < 64; i ++) {
        test_assert(ecs_is_alive(world, entities[i]));
    }

    ecs_delete(world, first);

    for (int i = 0; i < 64; i ++) {
        test_assert(!ecs_is_alive(world, entities[i]));
    }

    test_assert(!ecs_is_alive(world, first));

    ecs_fini(world);
}

void OnDelete_deep_clean_256(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    test_assert(r != 0);

    ecs_entity_t first = ecs_new(world);
    ecs_entity_t prev = first;
    ecs_entity_t entities[256];

    for (int i = 0; i < 256; i ++) {
        entities[i] = ecs_new(world);
        ecs_add_pair(world, entities[i], r, prev);
        prev = entities[i];
    }

    for (int i = 0; i < 256; i ++) {
        test_assert(ecs_is_alive(world, entities[i]));
    }

    ecs_delete(world, first);

    for (int i = 0; i < 256; i ++) {
        test_assert(!ecs_is_alive(world, entities[i]));
    }

    test_assert(!ecs_is_alive(world, first));

    ecs_fini(world);
}

void OnDelete_id_w_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, CanToggle);


    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_entity_t e3 = ecs_new_w(world, Tag);
    ecs_entity_t e4 = ecs_new_w(world, Tag);

    ecs_add_id(world, e1, ECS_TOGGLE | Tag);
    ecs_add_id(world, e2, ECS_TOGGLE | Tag);
    ecs_add_id(world, e3, ECS_TOGGLE | Tag);
    ecs_add_id(world, e4, ECS_TOGGLE | Tag);

    test_bool(false, ecs_is_enabled(world, e1, Tag));
    test_bool(false, ecs_is_enabled(world, e2, Tag));
    test_bool(false, ecs_is_enabled(world, e3, Tag));
    test_bool(false, ecs_is_enabled(world, e4, Tag));

    ecs_enable_component(world, e1, Tag, true);
    ecs_enable_component(world, e3, Tag, true);

    test_bool(true, ecs_is_enabled(world, e1, Tag));
    test_bool(false, ecs_is_enabled(world, e2, Tag));
    test_bool(true, ecs_is_enabled(world, e3, Tag));
    test_bool(false, ecs_is_enabled(world, e4, Tag));

    ecs_entity_t t = ecs_new(world);

    ecs_add_id(world, e3, t);
    ecs_add_id(world, e4, t);

    test_bool(true, ecs_is_enabled(world, e1, Tag));
    test_bool(false, ecs_is_enabled(world, e2, Tag));
    test_bool(true, ecs_is_enabled(world, e3, Tag));
    test_bool(false, ecs_is_enabled(world, e4, Tag));
    
    ecs_delete(world, t);

    test_bool(true, ecs_is_enabled(world, e1, Tag));
    test_bool(false, ecs_is_enabled(world, e2, Tag));
    test_bool(true, ecs_is_enabled(world, e3, Tag));
    test_bool(false, ecs_is_enabled(world, e4, Tag));

    ecs_fini(world);
}

void OnDelete_id_to_no_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_entity_t e3 = ecs_new_w(world, TagA);
    ecs_entity_t e4 = ecs_new_w(world, TagA);

    ecs_add(world, e1, TagB);
    ecs_add(world, e2, TagB);

    ecs_add_id(world, e1, ECS_TOGGLE | TagB);
    ecs_add_id(world, e2, ECS_TOGGLE | TagB);

    test_bool(false, ecs_is_enabled(world, e1, TagB));
    test_bool(false, ecs_is_enabled(world, e2, TagB));

    ecs_delete(world, TagB);

    test_assert( ecs_has_id(world, e1, TagA));
    test_assert( ecs_has_id(world, e2, TagA));
    test_assert( ecs_has_id(world, e3, TagA));
    test_assert( ecs_has_id(world, e4, TagA));

    test_assert( !ecs_has_id(world, e1, TagB));
    test_assert( !ecs_has_id(world, e2, TagB));
    test_assert( !ecs_has_id(world, e3, TagB));
    test_assert( !ecs_has_id(world, e4, TagB));

    ecs_fini(world);
}

void OnDelete_remove_on_delete_action(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    test_assert( !ecs_has_pair(world, ecs_id(Position), EcsOnDelete, EcsPanic));

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert( ecs_has(world, e, Position));

    ecs_delete(world, ecs_id(Position));

    test_assert( !ecs_has(world, e, Position));

    ecs_fini(world);
}

void OnDelete_delete_with_w_relation(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);
    ecs_add_pair(world, e2, Rel, e1);

    ecs_delete_with(world, Tag);

    test_assert( !ecs_is_alive(world, e1));
    test_assert( !ecs_is_alive(world, e2));

    ecs_fini(world);
}

static int delete_target_invoked = 0;

static
void DeleteTarget(ecs_iter_t *it) {
    ecs_id_t pair = ecs_field_id(it, 0);
    test_assert(ecs_id_is_pair(pair));
    ecs_delete(it->world, ECS_PAIR_SECOND(pair));
    delete_target_invoked ++;
}

void OnDelete_delete_self_in_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Rel = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = { EcsOnRemove },
        .callback = DeleteTarget
    });

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new_w_pair(world, Rel, a);

    test_int(0, delete_target_invoked);

    ecs_delete(world, a);

    test_int(1, delete_target_invoked);
    test_assert( !ecs_is_alive(world, a));
    test_assert( !ecs_is_alive(world, b));
    test_assert( !ecs_id_in_use(world, ecs_pair(EcsWildcard, a)));
    test_assert( !ecs_id_in_use(world, ecs_pair(Rel, a)));

    ecs_fini(world);
}

static
void DeleteOther(ecs_iter_t *it) {
    ecs_entity_t *ctx = it->ctx;
    test_assert(ctx != NULL);

    ecs_entity_t e = *ctx;
    test_assert(e != 0);

    ecs_delete(it->world, e);
    delete_target_invoked ++;
}

void OnDelete_delete_nested_in_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Rel = ecs_new_w_pair(world, EcsOnDeleteTarget, EcsDelete);
    ECS_TAG(world, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = ecs_pair(Rel, EcsWildcard),
        .events = { EcsOnRemove },
        .callback = DeleteOther,
        .ctx = &Tag
    });

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new_w_pair(world, Rel, a);
    ecs_entity_t c = ecs_new_w(world, Tag);

    test_int(0, delete_target_invoked);

    ecs_delete(world, a);

    test_int(1, delete_target_invoked);
    test_assert( !ecs_is_alive(world, a));
    test_assert( !ecs_is_alive(world, b));
    test_assert( ecs_is_alive(world, c));
    test_assert( ecs_get_type(world, c) == NULL);

    test_assert( !ecs_id_in_use(world, ecs_pair(EcsWildcard, a)));
    test_assert( !ecs_id_in_use(world, ecs_pair(Rel, a)));
    test_assert( !ecs_id_in_use(world, Tag));

    ecs_fini(world);
}

static
void AddRemoved(ecs_iter_t *it) {
    ecs_id_t id = ecs_field_id(it, 0);
    for (int i = 0; i < it->count; i ++) {
        ecs_new_w_id(it->world, id); /* create entity with removed id */
    }
}

void OnDelete_add_deleted_in_on_remove(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t t = ecs_new(world);
    ecs_new_w_id(world, t);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = t,
        .events = { EcsOnRemove },
        .callback = AddRemoved
    });

    test_expect_abort();
    ecs_delete(world, t);
}

void OnDelete_delete_tree_w_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_ENTITY(world, Rel, Traversable);
    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag(up Rel)" });
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    ecs_add_pair(world, e2, Rel, e1);
    ecs_add_id(world, e1, Foo);

    ecs_query_fini(q);

    ecs_fini(world);

    // Make sure query can handle matching with tables while entities are 
    // getting deleted.
    test_assert(true);
}

static int test_alive_parent_count = 0;
static ecs_entity_t last_deleted = 0;

static void TestAlive(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(it->world, e, EcsChildOf, 0);
        if (parent) {
            test_assert(ecs_is_alive(it->world, parent));
            test_alive_parent_count ++;
        }

        test_assert(!last_deleted || e < last_deleted);
        last_deleted = e;
    }
}

void OnDelete_fini_cleanup_order(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_w(world, Tag);
        ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
            ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
                ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, e3);

    ecs_add(world, e4, Tag);
    ecs_add(world, e3, Tag);
    ecs_add(world, e2, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms[0].id = Tag,
        .events = {EcsOnRemove},
        .callback = TestAlive
    });

    ecs_fini(world);

    test_int(test_alive_parent_count, 3);
}

void OnDelete_fini_cleanup_order_root_id_w_trait(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_add_id(world, r, EcsExclusive);
    ecs_entity_t t = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, r, t);

    ecs_fini(world);

    test_assert(true); // fini should not assert
}

static ECS_COMPONENT_DECLARE(Position);

static int on_remove_velocity = 0;
static void ecs_on_remove(Velocity)(ecs_iter_t *it) {
    on_remove_velocity ++;
    test_assert(ecs_is_alive(it->world, ecs_id(Position)));
}

void OnDelete_fini_cleanup_order_entity_after_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, Velocity, {
        .on_remove = ecs_on_remove(Velocity)
    });

    ecs_singleton_set(world, Position, {10, 20});
    
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Velocity);

    ecs_fini(world);

    test_int(on_remove_velocity, 1);
}

void OnDelete_fini_cleanup_order_entity_after_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_hooks(world, Velocity, {
        .on_remove = ecs_on_remove(Velocity)
    });
    
    ecs_entity_t e = ecs_new(world);
    ecs_add(world, e, Velocity);

    ecs_fini(world);

    test_int(on_remove_velocity, 1);
}

void OnDelete_on_delete_parent_w_in_use_id_w_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_w(world, Tag);
    ecs_entity_t r = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t o = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_add_pair(world, r, EcsOnDelete, EcsRemove);

    ecs_entity_t e = ecs_new_w_pair(world, r, o);

    ecs_delete(world, p);

    test_assert( !ecs_is_alive(world, p));
    test_assert( !ecs_is_alive(world, r));
    test_assert( !ecs_is_alive(world, o));
    test_assert( ecs_is_alive(world, e));
    test_assert( !ecs_has_pair(world, e, r, o));

    ecs_fini(world);
}

void OnDelete_on_delete_parent_w_in_use_id_w_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_new_w(world, Tag);
    ecs_entity_t r = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t o = ecs_new_w_pair(world, EcsChildOf, p);

    ecs_add_pair(world, r, EcsOnDelete, EcsDelete);

    ecs_entity_t e = ecs_new_w_pair(world, r, o);

    ecs_delete(world, p);

    test_assert( !ecs_is_alive(world, p));
    test_assert( !ecs_is_alive(world, r));
    test_assert( !ecs_is_alive(world, o));
    test_assert( !ecs_is_alive(world, e));

    ecs_fini(world);
}

void OnDelete_create_after_delete_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(Position));
    test_assert(ti != NULL);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));

    ecs_delete_with(world, ecs_id(Position));
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, ecs_id(Position)));

    ti = ecs_get_type_info(world, ecs_id(Position));
    test_assert(ti != NULL);

    ecs_entity_t id = ecs_new(world);
    test_assert(ecs_get_table(world, id) == NULL);
    e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void OnDelete_delete_with_inherited_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_assert(ecs_is_alive(world, base));
    test_assert(ecs_is_alive(world, inst));

    ecs_delete_with(world, Tag);

    test_assert(!ecs_is_alive(world, base));
    test_assert(ecs_is_alive(world, inst));
    test_assert(ecs_is_alive(world, Tag));

    ecs_fini(world);
}

void OnDelete_delete_with_inherited_tag_w_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    
    ecs_query_t *query = ecs_query(world, {
        .terms = {{ Tag }}
    });
    test_assert(query != NULL);

    test_assert(ecs_is_alive(world, base));
    test_assert(ecs_is_alive(world, inst));

    ecs_iter_t it = ecs_query_iter(world, query);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(base, it.entities[0]);

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(inst, it.entities[0]);
    test_bool(false, ecs_query_next(&it));

    ecs_delete_with(world, Tag);

    test_assert(!ecs_is_alive(world, base));
    test_assert(ecs_is_alive(world, inst));
    test_assert(ecs_is_alive(world, Tag));

    it = ecs_query_iter(world, query);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(query);

    ecs_fini(world);
}

static void Observer(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

void OnDelete_delete_with_inherited_tag_w_observer(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_add_pair(world, Tag, EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new_w(world, Tag);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    
    Probe ctx;
    ecs_entity_t o = ecs_observer_init(world, &(ecs_observer_desc_t){
        .query.terms = {{ Tag }},
        .events = { EcsOnRemove },
        .callback = Observer,
        .ctx = &ctx
    });
    test_assert(o != 0);

    test_assert(ecs_is_alive(world, base));
    test_assert(ecs_is_alive(world, inst));

    ecs_delete_with(world, Tag);

    test_int(ctx.invoked, 2);
    test_int(ctx.count, 2);
    test_uint(ctx.e[0], inst);
    test_uint(ctx.e[1], base);

    test_assert(!ecs_is_alive(world, base));
    test_assert(ecs_is_alive(world, inst));
    test_assert(ecs_is_alive(world, Tag));

    ecs_fini(world);
}

void OnDelete_delete_symmetric_relation(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, EcsSymmetric);
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    // Ensures internal observers are cleaned up before relationship id

    ecs_fini(world);
}

void OnDelete_delete_observed_symmetric_relation(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, EcsPairIsTag);

    ecs_add_id(world, e, EcsSymmetric);
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    // Ensures internal observers are cleaned up before relationship id

    ecs_fini(world);
}

static void on_remove_delete_with(ecs_iter_t *it) {
    test_int(it->count, 1);
    ecs_delete_with(it->world, ecs_pair(EcsChildOf, it->entities[0]));
}

void OnDelete_nested_delete_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_remove = on_remove_delete_with
    });

    ecs_entity_t e = ecs_new_w(world, Position);
    ecs_entity_t c = ecs_new_w_pair(world, EcsChildOf, e);
    ecs_delete_with(world, ecs_id(Position));

    test_assert(ecs_is_alive(world, ecs_id(Position)));
    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void OnDelete_deferred_delete_with_after_create_named(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_add(world, e1, Tag);

    ecs_defer_begin(world);
    ecs_delete_with(world, Tag);
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_add(world, e2, Tag);
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_has(world, e2, Tag));
    test_str(ecs_get_name(world, e2), "e2");

    ecs_fini(world);
}

void OnDelete_deferred_delete_with_childof_after_create_named(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_add(world, parent, Tag);

    ecs_entity_t e1 = ecs_entity(world, { .name = "parent.e1" });
    ecs_add(world, e1, Tag);

    ecs_defer_begin(world);
    ecs_delete_with(world, ecs_childof(parent));
    ecs_entity_t e2 = ecs_entity(world, { .name = "parent.e2" });
    ecs_add(world, e2, Tag);
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_has(world, e2, Tag));
    test_str(ecs_get_name(world, e2), "e2");

    ecs_fini(world);
}

void OnDelete_match_marked_for_deletion(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ns = ecs_entity(world, { .name = "ns" });
    ecs_set_scope(world, ns);
    ECS_COMPONENT(world, Position);
    ecs_entity_t Foo = ecs_component(world, { .entity = ecs_new(world) });
    ecs_entity_t Bar = ecs_component(world, { .entity = ecs_new(world) });
    ecs_set_scope(world, 0);

    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Foo, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, Bar, EcsOnInstantiate, EcsInherit);

    /* During cleanup a new table will be created that matches the query */
    ecs_query_t *q = ecs_query(world, { .expr = "ns.Position" });
    test_assert(q != NULL);

    ecs_entity_t prefab = ecs_new(world);
    ecs_add_id(world, prefab, EcsPrefab);
    ecs_add(world, prefab, Position);
    ecs_auto_override_id(world, prefab, Foo);

    ecs_entity_t newEnemy = ecs_new_w_pair(world, EcsIsA, prefab);
    ecs_add_id(world, newEnemy, Bar);

    ecs_query_fini(q);

    ecs_fini(world);

    test_assert(true); /* Ensure cleanup was successful */
}

void OnDelete_delete_w_low_rel_mixed_cleanup(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new_low_id(world);
    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_new(world);
    ecs_add_pair(world, child, EcsChildOf, parent);
    ecs_add_pair(world, child, rel, parent);

    ecs_delete(world, parent);

    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, child));

    ecs_fini(world);
}

void OnDelete_delete_w_low_rel_mixed_cleanup_interleaved_ids(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new_low_id(world);
    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t other = ecs_new(world);
    ecs_entity_t child = ecs_new(world);
    ecs_add_pair(world, child, EcsChildOf, parent);
    ecs_add_pair(world, child, rel, other);
    ecs_add_pair(world, child, rel, parent);

    ecs_delete(world, parent);

    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, child));

    ecs_fini(world);
}

void OnDelete_fini_query_w_singleton_in_scope_no_module(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t t = ecs_entity(world, { .name = "ns.foo" });
    ecs_entity_t s = ecs_entity(world, { .name = "ns.singleton" });
    ecs_add_id(world, s, s);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, ns.singleton($)" });

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, t);
    ecs_add(world, e, Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_iter_is_true(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void OnDelete_fini_query_w_singleton_in_module(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t n = ecs_entity(world, { .name = "ns" });
    ecs_add_id(world, n, EcsModule);

    ecs_entity_t t = ecs_entity(world, { .name = "ns.foo" });
    ecs_entity_t s = ecs_entity(world, { .name = "ns.singleton" });
    ecs_add_id(world, s, s);

    ecs_query_t *q = ecs_query(world, { .expr = "Position, ns.singleton($)" });

    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, t);
    ecs_add(world, e, Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_iter_is_true(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void OnDelete_fini_observer_w_relationship_in_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t Tag = ecs_new(world);

    ecs_entity_t ns = ecs_entity(world, { .name = "ns" });
    ecs_entity_t Rel = ecs_component(world, {
        .entity = ecs_entity(world, { .name = "Rel" }),
    });

    ecs_add_pair(world, Rel, EcsChildOf, ns);

    ecs_observer(world, {
        .query.terms = {
            { ecs_pair(Rel, EcsWildcard) },
            { Tag }
        },
        .events = { EcsOnRemove },
        .callback = Observer
    });

    ecs_new_w_id(world, Tag);

    ecs_fini(world);

    // Tests edge case where above code would cause a crash
    test_assert(true);
}

void OnDelete_add_on_delete_from_prefab(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_insert(world, {EcsPrefab});
    ecs_add_pair(world, e, EcsOnDelete, EcsPanic);

    test_expect_abort();
    ecs_delete(world, e);
}

void OnDelete_add_on_delete_from_disabled(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_insert(world, {EcsDisabled});
    ecs_add_pair(world, e, EcsOnDelete, EcsPanic);

    test_expect_abort();
    ecs_delete(world, e);
}

void OnDelete_delete_on_delete_from_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsOnDelete, EcsPanic);
    ecs_add_id(world, e, EcsPrefab);
    ecs_remove_pair(world, e, EcsOnDelete, EcsPanic);
    ecs_delete(world, e);
    
    test_assert(true); // shouldn't panic

    ecs_fini(world);
}

void OnDelete_delete_on_delete_from_disabled(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsOnDelete, EcsPanic);
    ecs_add_id(world, e, EcsDisabled);
    ecs_remove_pair(world, e, EcsOnDelete, EcsPanic);
    ecs_delete(world, e);
    
    test_assert(true); // shouldn't panic

    ecs_fini(world);
}

void OnDelete_remove_all_1(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, 20});

    ecs_remove_all(world, ecs_id(Position));

    test_assert(ecs_is_alive(world, e1));

    test_assert(!ecs_has(world, e1, Position));

    test_assert(ecs_lookup(world, "e1") == e1);

    test_int(0, ecs_count(world, Position));

    ecs_fini(world);
}

void OnDelete_remove_all_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_remove_all(world, ecs_id(Position));

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));

    test_assert(!ecs_has(world, e1, Position));
    test_assert(!ecs_has(world, e2, Position));

    test_assert(ecs_lookup(world, "e1") == e1);
    test_assert(ecs_lookup(world, "e2") == e2);

    test_int(0, ecs_count(world, Position));

    ecs_fini(world);
}

void OnDelete_remove_all_3(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    ecs_remove_all(world, ecs_id(Position));

    test_assert(ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(ecs_is_alive(world, e3));

    test_assert(!ecs_has(world, e1, Position));
    test_assert(!ecs_has(world, e2, Position));
    test_assert(!ecs_has(world, e3, Position));

    test_assert(ecs_lookup(world, "e1") == e1);
    test_assert(ecs_lookup(world, "e2") == e2);
    test_assert(ecs_lookup(world, "e3") == e3);

    test_int(0, ecs_count(world, Position));

    ecs_fini(world);
}

void OnDelete_delete_with_1(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, 20});

    ecs_delete_with(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e1));

    test_int(0, ecs_count(world, Position));

    ecs_fini(world);
}

void OnDelete_delete_with_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});

    ecs_delete_with(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));

    test_int(0, ecs_count(world, Position));

    ecs_fini(world);
}

void OnDelete_delete_with_3(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_entity_t e3 = ecs_entity(world, { .name = "e3" });
    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {30, 40});
    ecs_set(world, e3, Position, {50, 60});

    ecs_delete_with(world, ecs_id(Position));

    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    test_int(0, ecs_count(world, Position));

    ecs_fini(world);
}
