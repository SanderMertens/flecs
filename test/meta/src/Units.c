#include <meta.h>

void Units_member_w_unit() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "percentage",
        .symbol = "%"
    });
    test_assert(u != 0);
    test_str("percentage", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsUnit));

    const EcsUnit *uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "%");

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .members = {
            { .name = "value", .type = ecs_id(ecs_f32_t), .unit = u }
        }
    });
    test_assert(s != 0);

    const EcsStruct *sptr = ecs_get(world, s, EcsStruct);
    test_assert(sptr != NULL);
    test_int(1, ecs_vector_count(sptr->members));
    ecs_member_t *members = ecs_vector_first(sptr->members, ecs_member_t);
    test_str(members[0].name, "value");
    test_uint(members[0].type, ecs_id(ecs_f32_t));
    test_uint(members[0].unit, u);
    test_int(members[0].count, 1);

    ecs_entity_t m = ecs_lookup_child(world, s, "value");
    test_assert(m != 0);

    const EcsMember *mptr = ecs_get(world, m, EcsMember);
    test_assert(mptr != NULL);
    test_assert(mptr->type == ecs_id(ecs_f32_t));
    test_assert(mptr->unit == u);

    ecs_fini(world);
}

void Units_member_w_unit_type() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_primitive_init(world, &(ecs_primitive_desc_t) {
        .entity.name = "percentage",
        .kind = EcsF32
    });
    test_assert(u != 0);
    test_str("percentage", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsMetaType));

    ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.entity = u,
        .symbol = "%"
    });
    test_assert(u != 0);
    test_assert(ecs_has(world, u, EcsUnit));

    const EcsUnit *uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "%");

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .members = {
            { .name = "value", .type = u }
        }
    });
    test_assert(s != 0);

    const EcsStruct *sptr = ecs_get(world, s, EcsStruct);
    test_assert(sptr != NULL);
    test_int(1, ecs_vector_count(sptr->members));
    ecs_member_t *members = ecs_vector_first(sptr->members, ecs_member_t);
    test_str(members[0].name, "value");
    test_uint(members[0].type, u);
    test_uint(members[0].unit, u);
    test_int(members[0].count, 1);

    ecs_entity_t m = ecs_lookup_child(world, s, "value");
    test_assert(m != 0);

    const EcsMember *mptr = ecs_get(world, m, EcsMember);
    test_assert(mptr != NULL);
    test_assert(mptr->type == u);
    test_assert(mptr->unit == u);

    ecs_fini(world);
}

void Units_cursor_get_unit() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "percentage",
        .symbol = "%"
    });

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .entity.name = "s",
        .members = {
            { .name = "value", .type = ecs_id(ecs_f32_t), .unit = u }
        }
    });

    ecs_entity_t e = ecs_new_id(world);
    void *ptr = ecs_get_mut_id(world, e, s, 0);
    test_assert(ptr != NULL);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, s, ptr);
    test_int(0, ecs_meta_push(&cur));
    test_str("value", ecs_meta_get_member(&cur));
    test_uint(ecs_id(ecs_f32_t), ecs_meta_get_type(&cur));
    test_uint(u, ecs_meta_get_unit(&cur));
    test_int(0, ecs_meta_pop(&cur));

    ecs_fini(world);
}

void Units_cursor_get_unit_type() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_primitive_init(world, &(ecs_primitive_desc_t) {
        .entity.name = "percentage",
        .kind = EcsF32
    });

    ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.entity = u,
        .symbol = "%"
    });

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .members = {
            { .name = "value", .type = u }
        }
    });

    ecs_entity_t e = ecs_new_id(world);
    void *ptr = ecs_get_mut_id(world, e, s, 0);
    test_assert(ptr != NULL);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, s, ptr);
    test_int(0, ecs_meta_push(&cur));
    test_str("value", ecs_meta_get_member(&cur));
    test_uint(u, ecs_meta_get_type(&cur));
    test_uint(u, ecs_meta_get_unit(&cur));
    test_int(0, ecs_meta_pop(&cur));

    ecs_fini(world);
}

void Units_unit_w_quantity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t q = ecs_quantity_init(world, &(ecs_entity_desc_t) {
        .name = "duration"
    });
    test_assert(q != 0);
    test_str("duration", ecs_get_name(world, q));
    test_assert(ecs_has_id(world, q, EcsQuantity));

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "seconds",
        .symbol = "s",
        .quantity = q
    });
    test_assert(u != 0);
    test_assert(ecs_has(world, u, EcsUnit));

    const EcsUnit *uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");
    test_assert(ecs_has_pair(world, u, EcsQuantity, q));

    ecs_fini(world);
}

void Units_unit_w_self_quantity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t q = ecs_quantity_init(world, &(ecs_entity_desc_t) {
        .name = "percentage"
    });
    test_assert(q != 0);
    test_str("percentage", ecs_get_name(world, q));
    test_assert(ecs_has_id(world, q, EcsQuantity));

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.entity = q,
        .symbol = "%"
    });
    test_assert(u != 0);
    test_assert(u == q);
    test_assert(ecs_has(world, u, EcsUnit));

    const EcsUnit *uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "%");
    test_assert(ecs_has_pair(world, u, EcsQuantity, u));

    ecs_fini(world);
}

void Units_unit_w_sub() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u_1 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "seconds",
        .symbol = "s"
    });
    test_assert(u_1 != 0);
    test_assert(ecs_has(world, u_1, EcsUnit));

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "minutes",
        .symbol = "m",
        .unit = u_1
    });
    test_assert(u_2 != 0);
    test_assert(ecs_has(world, u_2, EcsUnit));

    const EcsUnit *uptr = ecs_get(world, u_1, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    uptr = ecs_get(world, u_2, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "m");
    test_uint(uptr->unit, u_1);

    ecs_fini(world);
}

void Units_unit_w_over() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u_1 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "seconds",
        .symbol = "s"
    });
    test_assert(u_1 != 0);
    test_assert(ecs_has(world, u_1, EcsUnit));

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "meters",
        .symbol = "m"
    });
    test_assert(u_2 != 0);
    test_assert(ecs_has(world, u_2, EcsUnit));

    ecs_entity_t u_3 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "meters_per_second",
        .symbol = "mps",
        .unit = u_1,
        .over = u_2
    });
    test_assert(u_3 != 0);
    test_assert(ecs_has(world, u_3, EcsUnit));

    const EcsUnit *uptr = ecs_get(world, u_1, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    uptr = ecs_get(world, u_2, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "m");

    uptr = ecs_get(world, u_3, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "mps");
    test_uint(uptr->unit, u_1);
    test_uint(uptr->over, u_2);

    ecs_fini(world);
}

void Units_member_w_invalid_unit() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_new_id(world); /* not a unit */
    test_assert(u != 0);

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_struct_init(world, &(ecs_struct_desc_t) {
        .members = {
            { .name = "value", .type = ecs_id(ecs_f32_t), .unit = u }
        }
    });
    test_assert(s == 0);

    ecs_fini(world);
}

void Units_unit_w_invalid_quantity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t q = ecs_new_id(world); /* not a quantity */
    test_assert(q != 0);

    ecs_log_set_level(-4);

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "seconds",
        .symbol = "s",
        .quantity = q
    });
    test_assert(u == 0);
    
    ecs_fini(world);
}

void Units_unit_w_invalid_sub() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u_1 = ecs_new_id(world); /* not a unit */
    test_assert(u_1 != 0);

    ecs_log_set_level(-4);

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "meters",
        .symbol = "m",
        .unit = u_1
    });
    test_assert(u_2 == 0);

    ecs_fini(world);
}

void Units_unit_w_invalid_over() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u_1 = ecs_new_id(world); /* not a unit */
    test_assert(u_1 != 0);

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "meters",
        .symbol = "m"
    });
    test_assert(u_2 != 0);

    ecs_log_set_level(-4);

    ecs_entity_t u_3 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "meters_per_second",
        .symbol = "mps",
        .unit = u_2,
        .over = u_1
    });
    test_assert(u_3 == 0);

    ecs_fini(world);
}

void Units_unit_w_over_no_sub() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u_2 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "meters",
        .symbol = "m"
    });
    test_assert(u_2 != 0);

    ecs_log_set_level(-4);

    ecs_entity_t u_3 = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "meters_per_second",
        .symbol = "mps",
        .over = u_2
    });
    test_assert(u_3 == 0);

    ecs_fini(world);
}

void Units_define_twice() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "seconds",
        .symbol = "s"
    });
    test_assert(u != 0);

    test_assert(u != 0);
    test_str("seconds", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsUnit));

    const EcsUnit *uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    test_assert(u == ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.entity = u,
        .symbol = "s"
    }));
    
    test_assert(u != 0);
    test_str("seconds", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsUnit));

    uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    ecs_fini(world);
}

void Units_define_twice_different_quantity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t q_1 = ecs_quantity_init(world, &(ecs_entity_desc_t) {
        .name = "percentage"
    });
    test_assert(q_1 != 0);

    ecs_entity_t q_2 = ecs_quantity_init(world, &(ecs_entity_desc_t) {
        .name = "duration"
    });
    test_assert(q_2 != 0);

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "seconds",
        .symbol = "s",
        .quantity = q_1
    });
    test_assert(u != 0);

    test_assert(u != 0);
    test_str("seconds", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsUnit));
    test_assert(ecs_has_pair(world, u, EcsQuantity, q_1));

    const EcsUnit *uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    test_assert(u == ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.entity = u,
        .symbol = "s",
        .quantity = q_2
    }));
    
    test_assert(u != 0);
    test_str("seconds", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsUnit));
    test_assert(!ecs_has_pair(world, u, EcsQuantity, q_1));
    test_assert(ecs_has_pair(world, u, EcsQuantity, q_2));

    uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    ecs_fini(world);
}

void Units_define_twice_remove_quantity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t q_1 = ecs_quantity_init(world, &(ecs_entity_desc_t) {
        .name = "percentage"
    });
    test_assert(q_1 != 0);

    ecs_entity_t u = ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.name = "seconds",
        .symbol = "s",
        .quantity = q_1
    });
    test_assert(u != 0);

    test_assert(u != 0);
    test_str("seconds", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsUnit));
    test_assert(ecs_has_pair(world, u, EcsQuantity, q_1));

    const EcsUnit *uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    test_assert(u == ecs_unit_init(world, &(ecs_unit_desc_t) {
        .entity.entity = u,
        .symbol = "s"
    }));
    
    test_assert(u != 0);
    test_str("seconds", ecs_get_name(world, u));
    test_assert(ecs_has(world, u, EcsUnit));
    test_assert(!ecs_has_pair(world, u, EcsQuantity, q_1));
    test_assert(!ecs_has_pair(world, u, EcsQuantity, EcsWildcard));

    uptr = ecs_get(world, u, EcsUnit);
    test_assert(uptr != NULL);
    test_str(uptr->symbol, "s");

    ecs_fini(world);
}

