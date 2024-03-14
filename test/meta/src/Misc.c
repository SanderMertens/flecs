#include <meta.h>

void Misc_primitive_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_primitive(stage, { .kind = EcsI32 });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsPrimitive));

    ecs_fini(world);
}

void Misc_enum_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_enum(stage, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsEnum));

    ecs_fini(world);
}

void Misc_bitmask_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_bitmask(stage, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsBitmask));

    ecs_fini(world);
}

void Misc_struct_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_struct(stage, { .members = {
        {"x", ecs_id(ecs_i32_t)},
        {"y", ecs_id(ecs_i32_t)},
    } });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsStruct));

    ecs_fini(world);
}

void Misc_opaque_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_opaque(stage, {
        .entity = ecs_id(Position),
        .type.as_type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsOpaque));

    ecs_fini(world);
}

void Misc_array_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_array(stage, {
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsArray));

    ecs_fini(world);
}

void Misc_vector_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_vector(stage, {
        .type = ecs_id(ecs_i32_t)
    });

    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsVector));

    ecs_fini(world);
}

void Misc_unit_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_unit(stage, {
        .symbol = "f"
    });

    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnit));

    ecs_fini(world);
}

void Misc_unit_prefix_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_unit_prefix(stage, {
        .symbol = "f"
    });

    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnitPrefix));

    ecs_fini(world);
}

void Misc_quantity_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entity_t t = ecs_quantity(stage, {
        .name = "q"
    });

    test_assert(t != 0);
    test_assert(ecs_has_id(world, t, EcsQuantity));

    ecs_fini(world);
}

void Misc_primitive_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_primitive(world, { .kind = EcsI32 });
    test_assert(t != 0);

    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsPrimitive));

    ecs_fini(world);
}

void Misc_enum_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_enum(world, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);

    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsEnum));

    ecs_fini(world);
}

void Misc_bitmask_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_bitmask(world, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);

    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsBitmask));

    ecs_fini(world);
}

void Misc_array_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });
    test_assert(t != 0);

    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsArray));

    ecs_fini(world);
}

void Misc_vector_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_vector(world, {
        .type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);

    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsVector));

    ecs_fini(world);
}

void Misc_struct_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_struct(world, { .members = {
        {"x", ecs_id(ecs_i32_t)},
        {"y", ecs_id(ecs_i32_t)},
    } });
    test_assert(t != 0);

    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsStruct));

    ecs_fini(world);
}

void Misc_opaque_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_opaque(world, {
        .entity = ecs_id(Position),
        .type.as_type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsOpaque));

    ecs_defer_end(world);

    ecs_fini(world);
}

void Misc_unit_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_unit(world, {
        .symbol = "f"
    });

    ecs_defer_end(world);
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnit));

    ecs_fini(world);
}

void Misc_unit_prefix_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_unit_prefix(world, {
        .symbol = "f"
    });

    ecs_defer_end(world);
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnitPrefix));

    ecs_fini(world);
}

void Misc_quantity_from_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t t = ecs_quantity(world, {
        .name = "q"
    });

    ecs_defer_end(world);
    test_assert(t != 0);
    test_assert(ecs_has_id(world, t, EcsQuantity));

    ecs_fini(world);
}

void Misc_primitive_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_primitive(world, { .kind = EcsI32 });
    test_assert(t != 0);

    ecs_readonly_end(world);
    test_assert(ecs_has(world, t, EcsPrimitive));

    ecs_fini(world);
}

void Misc_enum_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_enum(world, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);

    ecs_readonly_end(world);
    test_assert(ecs_has(world, t, EcsEnum));

    ecs_fini(world);
}

void Misc_bitmask_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_bitmask(world, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);

    ecs_readonly_end(world);
    test_assert(ecs_has(world, t, EcsBitmask));

    ecs_fini(world);
}

void Misc_array_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });
    test_assert(t != 0);

    ecs_readonly_end(world);
    test_assert(ecs_has(world, t, EcsArray));

    ecs_fini(world);
}

void Misc_vector_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_vector(world, {
        .type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);

    ecs_readonly_end(world);
    test_assert(ecs_has(world, t, EcsVector));

    ecs_fini(world);
}

void Misc_struct_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_struct(world, { .members = {
        {"x", ecs_id(ecs_i32_t)},
        {"y", ecs_id(ecs_i32_t)},
    } });
    test_assert(t != 0);

    ecs_readonly_end(world);
    test_assert(ecs_has(world, t, EcsStruct));

    ecs_fini(world);
}

void Misc_opaque_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_opaque(world, {
        .entity = ecs_id(Position),
        .type.as_type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsOpaque));

    ecs_readonly_end(world);

    ecs_fini(world);
}

void Misc_unit_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_unit(world, {
        .symbol = "f"
    });

    ecs_readonly_end(world);
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnit));

    ecs_fini(world);
}

void Misc_unit_prefix_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_unit_prefix(world, {
        .symbol = "f"
    });

    ecs_readonly_end(world);
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnitPrefix));

    ecs_fini(world);
}

void Misc_quantity_from_readonly(void) {
    ecs_world_t *world = ecs_init();

    ecs_readonly_begin(world, false);

    ecs_entity_t t = ecs_quantity(world, {
        .name = "q"
    });

    ecs_readonly_end(world);
    test_assert(t != 0);
    test_assert(ecs_has_id(world, t, EcsQuantity));

    ecs_fini(world);
}

void Misc_primitive_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_primitive(world, { .kind = EcsI32 });
    test_assert(t != 0);

    ecs_defer_resume(world);
    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsPrimitive));

    ecs_fini(world);
}

void Misc_enum_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_enum(world, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);

    ecs_defer_resume(world);
    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsEnum));

    ecs_fini(world);
}

void Misc_bitmask_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_bitmask(world, {
        .constants = {
            {"Lettuce"}, {"Bacon"}, {"Tomato"}, {"Cheese"}
        }
    });
    test_assert(t != 0);

    ecs_defer_resume(world);
    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsBitmask));

    ecs_fini(world);
}

void Misc_array_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_array(world, {
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });
    test_assert(t != 0);

    ecs_defer_resume(world);
    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsArray));

    ecs_fini(world);
}

void Misc_vector_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_vector(world, {
        .type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);

    ecs_defer_resume(world);
    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsVector));

    ecs_fini(world);
}

void Misc_struct_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_struct(world, { .members = {
        {"x", ecs_id(ecs_i32_t)},
        {"y", ecs_id(ecs_i32_t)},
    } });
    test_assert(t != 0);

    ecs_defer_resume(world);
    ecs_defer_end(world);
    test_assert(ecs_has(world, t, EcsStruct));

    ecs_fini(world);
}

void Misc_opaque_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_opaque(world, {
        .entity = ecs_id(Position),
        .type.as_type = ecs_id(ecs_i32_t)
    });
    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsOpaque));

    ecs_defer_resume(world);
    ecs_defer_end(world);

    ecs_fini(world);
}

void Misc_unit_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_unit(world, {
        .symbol = "f"
    });

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnit));

    ecs_fini(world);
}

void Misc_unit_prefix_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_unit_prefix(world, {
        .symbol = "f"
    });

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(t != 0);
    test_assert(ecs_has(world, t, EcsUnitPrefix));

    ecs_fini(world);
}

void Misc_quantity_from_suspend_defer(void) {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);
    ecs_defer_suspend(world);

    ecs_entity_t t = ecs_quantity(world, {
        .name = "q"
    });

    ecs_defer_resume(world);
    ecs_defer_end(world);

    test_assert(t != 0);
    test_assert(ecs_has_id(world, t, EcsQuantity));

    ecs_fini(world);
}
