#include <meta.h>

typedef struct {
    ecs_i32_t x;
} Base1;

typedef struct {
    ecs_i32_t x;
    ecs_i32_t y;
} Base2;

typedef struct {
    Base1 base;
    ecs_i32_t y;
} Derived1_1;

typedef struct {
    Base2 base;
    ecs_i32_t z;
} Derived2_1;

typedef struct {
    Base1 base;
    ecs_i32_t y;
    ecs_i32_t z;
} Derived1_2;

typedef struct {
    ecs_i32_t x;
    ecs_i8_t y;
} PaddedBase;

typedef struct {
    PaddedBase base;
    ecs_i8_t z;
} PaddedDerived;

typedef struct {
    ecs_i8_t x;
} SmallBase;

typedef struct {
    SmallBase base;
    ecs_i64_t y;
} LargeDerived;

typedef struct {
    Derived1_1 base;
    ecs_i32_t z;
} Chain;

typedef struct {
    ecs_i32_t arr[3];
} ArrayBase;

typedef struct {
    ArrayBase base;
    ecs_i32_t y;
} ArrayDerived;

typedef struct {
    Derived1_1 d;
    ecs_i32_t w;
} Outer;

static ecs_entity_t create_base1(ecs_world_t *world) {
    return ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });
}

static ecs_entity_t create_base2(ecs_world_t *world) {
    return ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
}

static ecs_entity_t create_derived(
    ecs_world_t *world,
    ecs_entity_t base,
    const char *name,
    const char *member)
{
    ecs_entity_t e = ecs_entity(world, {.name = name});
    ecs_add_pair(world, e, EcsIsA, base);
    if (member) {
        return ecs_struct(world, {
            .entity = e,
            .members = {
                {member, ecs_id(ecs_i32_t)}
            }
        });
    } else {
        return ecs_struct(world, { .entity = e });
    }
}

void StructInheritance_base_1_derived_1(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    test_assert(base != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    test_assert(derived != 0);
    test_str(ecs_get_name(world, derived), "Derived");
    test_assert(ecs_has_pair(world, derived, EcsIsA, base));

    meta_test_struct(world, derived, Derived1_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_1, base.x));
    meta_test_member(world, derived, Derived1_1, y, ecs_id(ecs_i32_t), 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "x");
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "y");

    meta_test_struct(world, base, Base1);
    meta_test_member(world, base, Base1, x, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_base_2_derived_1(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    test_assert(base != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", "z");
    test_assert(derived != 0);

    meta_test_struct(world, derived, Derived2_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.x));
    _meta_test_member(world, derived, "y", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.y));
    meta_test_member(world, derived, Derived2_1, z, ecs_id(ecs_i32_t), 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_int(ecs_vec_count(&st->members), 3);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "x");
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "y");
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 2)->name, "z");

    ecs_fini(world);
}

void StructInheritance_base_1_derived_2(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    test_assert(base != 0);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    derived = ecs_struct(world, {
        .entity = derived,
        .members = {
            {"y", ecs_id(ecs_i32_t)},
            {"z", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(derived != 0);

    meta_test_struct(world, derived, Derived1_2);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_2, base.x));
    meta_test_member(world, derived, Derived1_2, y, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, derived, Derived1_2, z, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_derived_no_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    test_assert(base != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", NULL);
    test_assert(derived != 0);

    meta_test_struct(world, derived, Base2);
    meta_test_member(world, derived, Base2, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, derived, Base2, y, ecs_id(ecs_i32_t), 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_int(ecs_vec_count(&st->members), 2);

    ecs_fini(world);
}

void StructInheritance_isa_after_empty_struct_init(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    test_assert(base != 0);

    ecs_entity_t derived = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Derived"})
    });
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 0);

    ecs_add_pair(world, derived, EcsIsA, base);

    st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 2);

    meta_test_struct(world, derived, Base2);
    meta_test_member(world, derived, Base2, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, derived, Base2, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_isa_after_empty_struct_init_add_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    test_assert(base != 0);

    ecs_entity_t derived = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Derived"})
    });
    test_assert(derived != 0);

    ecs_add_pair(world, derived, EcsIsA, base);

    test_int(ecs_struct_add_member(world, derived, &(ecs_member_t){
        .name = "z", .type = ecs_id(ecs_i32_t)
    }), 0);

    meta_test_struct(world, derived, Derived2_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.x));
    _meta_test_member(world, derived, "y", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.y));
    meta_test_member(world, derived, Derived2_1, z, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_add_member_to_derived(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    test_assert(derived != 0);

    test_int(ecs_struct_add_member(world, derived, &(ecs_member_t){
        .name = "z", .type = ecs_id(ecs_i32_t)
    }), 0);

    meta_test_struct(world, derived, Derived1_2);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_2, base.x));
    meta_test_member(world, derived, Derived1_2, y, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, derived, Derived1_2, z, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_base_w_padding(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i8_t)}
        }
    });
    test_assert(base != 0);
    meta_test_struct(world, base, PaddedBase);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    derived = ecs_struct(world, {
        .entity = derived,
        .members = {
            {"z", ecs_id(ecs_i8_t)}
        }
    });
    test_assert(derived != 0);

    meta_test_struct(world, derived, PaddedDerived);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(PaddedDerived, base.x));
    _meta_test_member(world, derived, "y", ecs_id(ecs_i8_t), 0, offsetof(PaddedDerived, base.y));
    meta_test_member(world, derived, PaddedDerived, z, ecs_id(ecs_i8_t), 0);

    ecs_fini(world);
}

void StructInheritance_derived_alignment_larger_than_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i8_t)}
        }
    });
    test_assert(base != 0);
    meta_test_struct(world, base, SmallBase);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    derived = ecs_struct(world, {
        .entity = derived,
        .members = {
            {"y", ecs_id(ecs_i64_t)}
        }
    });
    test_assert(derived != 0);

    meta_test_struct(world, derived, LargeDerived);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i8_t), 0, offsetof(LargeDerived, base.x));
    meta_test_member(world, derived, LargeDerived, y, ecs_id(ecs_i64_t), 0);

    ecs_fini(world);
}

void StructInheritance_chain(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    ecs_entity_t chain = create_derived(world, derived, "Chain", "z");
    test_assert(chain != 0);

    meta_test_struct(world, chain, Chain);
    _meta_test_member(world, chain, "x", ecs_id(ecs_i32_t), 0, offsetof(Chain, base.base.x));
    _meta_test_member(world, chain, "y", ecs_id(ecs_i32_t), 0, offsetof(Chain, base.y));
    meta_test_member(world, chain, Chain, z, ecs_id(ecs_i32_t), 0);

    const EcsStruct *st = ecs_get(world, chain, EcsStruct);
    test_int(ecs_vec_count(&st->members), 3);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "x");
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "y");
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 2)->name, "z");

    ecs_fini(world);
}

void StructInheritance_chain_no_own_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", NULL);
    ecs_entity_t chain = create_derived(world, derived, "Chain", NULL);
    test_assert(chain != 0);

    meta_test_struct(world, chain, Base1);
    meta_test_member(world, chain, Base1, x, ecs_id(ecs_i32_t), 0);

    const EcsStruct *st = ecs_get(world, chain, EcsStruct);
    test_int(ecs_vec_count(&st->members), 1);

    ecs_fini(world);
}

void StructInheritance_two_derived_from_same_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t d1 = create_derived(world, base, "D1", "y");
    ecs_entity_t d2 = create_derived(world, base, "D2", "z");
    test_assert(d1 != 0);
    test_assert(d2 != 0);

    meta_test_struct(world, d1, Derived1_1);
    _meta_test_member(world, d1, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_1, base.x));
    meta_test_member(world, d1, Derived1_1, y, ecs_id(ecs_i32_t), 0);

    meta_test_struct(world, d2, Derived1_1);
    _meta_test_member(world, d2, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_1, base.x));
    test_assert(ecs_struct_get_member(world, d2, "z") != NULL);
    test_assert(ecs_struct_get_member(world, d2, "y") == NULL);
    test_assert(ecs_struct_get_member(world, d1, "z") == NULL);

    const EcsStruct *st = ecs_get(world, base, EcsStruct);
    test_int(ecs_vec_count(&st->members), 1);

    ecs_fini(world);
}

void StructInheritance_duplicate_member_add_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    test_assert(derived != 0);

    ecs_log_set_level(-4);
    test_int(ecs_struct_add_member(world, derived, &(ecs_member_t){
        .name = "x", .type = ecs_id(ecs_i32_t)
    }), -1);
    ecs_log_set_level(-1);

    meta_test_struct(world, derived, Derived1_1);
    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_int(ecs_vec_count(&st->members), 2);

    ecs_fini(world);
}

void StructInheritance_duplicate_member_struct_init(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);

    ecs_log_set_level(-4);
    test_uint(ecs_struct(world, {
        .entity = derived,
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    }), 0);
    ecs_log_set_level(-1);

    meta_test_struct(world, base, Base1);

    ecs_fini(world);
}

void StructInheritance_isa_after_members_error(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);

    ecs_entity_t derived = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Derived"}),
        .members = {
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(derived != 0);

    ecs_log_set_level(-4);
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_log_set_level(-1);

    meta_test_struct(world, derived, Base1);
    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_int(ecs_vec_count(&st->members), 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "y");

    ecs_fini(world);
}

void StructInheritance_non_struct_base(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_entity(world, {.name = "Base"});

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    derived = ecs_struct(world, {
        .entity = derived,
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(derived != 0);

    meta_test_struct(world, derived, Base1);
    meta_test_member(world, derived, Base1, x, ecs_id(ecs_i32_t), 0);
    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_int(ecs_vec_count(&st->members), 1);

    ecs_fini(world);
}

void StructInheritance_non_struct_base_no_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_entity(world, {.name = "Base"});

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    derived = ecs_struct(world, { .entity = derived });
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_assert(st != NULL);
    test_int(ecs_vec_count(&st->members), 0);
    const EcsComponent *c = ecs_get(world, derived, EcsComponent);
    test_assert(c == NULL || c->size == 0);

    ecs_fini(world);
}

void StructInheritance_isa_non_struct_does_not_add_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);

    ecs_entity_t e = ecs_entity(world, {.name = "e"});
    ecs_add_pair(world, e, EcsIsA, base);

    test_assert(!ecs_has(world, e, EcsStruct));
    test_assert(!ecs_has(world, e, EcsComponent));
    test_assert(!ecs_has(world, e, EcsType));
    test_assert(!ecs_owns(world, e, EcsTypeSerializer));

    ecs_fini(world);
}

void StructInheritance_multiple_isa_one_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t other = ecs_entity(world, {.name = "Other"});
    ecs_entity_t base = create_base1(world);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, other);
    ecs_add_pair(world, derived, EcsIsA, base);
    derived = ecs_struct(world, {
        .entity = derived,
        .members = {
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(derived != 0);

    meta_test_struct(world, derived, Derived1_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_1, base.x));
    meta_test_member(world, derived, Derived1_1, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_get_member_inherited(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    ecs_member_t *m = ecs_struct_get_member(world, derived, "x");
    test_assert(m != NULL);
    test_str(m->name, "x");
    test_uint(m->type, ecs_id(ecs_i32_t));
    test_int(m->offset, 0);

    m = ecs_struct_get_member(world, derived, "y");
    test_assert(m != NULL);
    test_str(m->name, "y");
    test_int(m->offset, 4);

    test_assert(ecs_struct_get_member(world, derived, "z") == NULL);

    ecs_fini(world);
}

void StructInheritance_get_nth_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    ecs_member_t *m = ecs_struct_get_nth_member(world, derived, 0);
    test_assert(m != NULL);
    test_str(m->name, "x");

    m = ecs_struct_get_nth_member(world, derived, 1);
    test_assert(m != NULL);
    test_str(m->name, "y");

    test_assert(ecs_struct_get_nth_member(world, derived, 2) == NULL);

    ecs_fini(world);
}

void StructInheritance_inherited_member_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        },
        .create_member_entities = true
    });
    test_assert(base != 0);

    ecs_entity_t base_x = ecs_lookup(world, "Base.x");
    test_assert(base_x != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    test_assert(derived != 0);

    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_int(ecs_vec_count(&st->members), 2);
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->member, base_x);
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->member, 0);

    test_uint(ecs_lookup(world, "Derived.x"), 0);

    meta_test_struct(world, derived, Derived1_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_1, base.x));
    meta_test_member(world, derived, Derived1_1, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_derived_member_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        },
        .create_member_entities = true
    });
    test_assert(base != 0);

    ecs_entity_t derived = ecs_entity(world, {.name = "Derived"});
    ecs_add_pair(world, derived, EcsIsA, base);
    derived = ecs_struct(world, {
        .entity = derived,
        .members = {
            {"y", ecs_id(ecs_i32_t)}
        },
        .create_member_entities = true
    });
    test_assert(derived != 0);

    ecs_entity_t derived_y = ecs_lookup(world, "Derived.y");
    test_assert(derived_y != 0);
    const EcsMember *m = ecs_get(world, derived_y, EcsMember);
    test_assert(m != NULL);
    test_uint(m->type, ecs_id(ecs_i32_t));
    test_int(m->offset, 4);

    ecs_entity_t base_x = ecs_lookup(world, "Base.x");
    m = ecs_get(world, base_x, EcsMember);
    test_int(m->offset, 0);

    meta_test_struct(world, derived, Derived1_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_1, base.x));
    meta_test_member(world, derived, Derived1_1, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_derived_member_entities_duplicate(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)}
        },
        .create_member_entities = true
    });
    test_assert(base != 0);

    ecs_entity_t derived = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Derived"})
    });
    ecs_add_pair(world, derived, EcsIsA, base);

    ecs_log_set_level(-4);
    ecs_entity_t m = ecs_entity(world, { .name = "x", .parent = derived });
    ecs_set(world, m, EcsMember, { .type = ecs_id(ecs_i32_t) });
    ecs_log_set_level(-1);

    meta_test_struct(world, derived, Base1);
    const EcsStruct *st = ecs_get(world, derived, EcsStruct);
    test_int(ecs_vec_count(&st->members), 1);

    ecs_fini(world);
}

void StructInheritance_ptr_to_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    Derived1_1 value = { .base.x = 10, .y = 20 };
    char *str = ecs_ptr_to_expr(world, derived, &value);
    test_assert(str != NULL);
    test_str(str, "{x: 10, y: 20}");
    ecs_os_free(str);

    ecs_fini(world);
}

void StructInheritance_ptr_from_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "z");

    Derived2_1 value = {0};
    const char *ptr = ecs_expr_run(world, "{x: 10, y: 20, z: 30}",
        &(ecs_value_t){ .type = derived, .ptr = &value }, NULL);
    test_assert(ptr != NULL);
    test_int(value.base.x, 10);
    test_int(value.base.y, 20);
    test_int(value.z, 30);

    ecs_fini(world);
}

void StructInheritance_ptr_from_expr_member_names(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "z");

    Derived2_1 value = {0};
    const char *ptr = ecs_expr_run(world, "{z: 30, x: 10}",
        &(ecs_value_t){ .type = derived, .ptr = &value }, NULL);
    test_assert(ptr != NULL);
    test_int(value.base.x, 10);
    test_int(value.base.y, 0);
    test_int(value.z, 30);

    ecs_fini(world);
}

void StructInheritance_cursor(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "z");

    Derived2_1 value = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, derived, &value);
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "x"), 0);
    test_int(ecs_meta_set_int(&cur, 10), 0);
    test_int(ecs_meta_member(&cur, "z"), 0);
    test_int(ecs_meta_set_int(&cur, 30), 0);
    test_int(ecs_meta_member(&cur, "y"), 0);
    test_int(ecs_meta_set_int(&cur, 20), 0);
    test_int(ecs_meta_pop(&cur), 0);

    test_int(value.base.x, 10);
    test_int(value.base.y, 20);
    test_int(value.z, 30);

    ecs_fini(world);
}

void StructInheritance_cursor_next(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "z");

    Derived2_1 value = {0};
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, derived, &value);
    test_int(ecs_meta_push(&cur), 0);
    test_str(ecs_meta_get_member(&cur), "x");
    test_int(ecs_meta_set_int(&cur, 10), 0);
    test_int(ecs_meta_next(&cur), 0);
    test_str(ecs_meta_get_member(&cur), "y");
    test_int(ecs_meta_set_int(&cur, 20), 0);
    test_int(ecs_meta_next(&cur), 0);
    test_str(ecs_meta_get_member(&cur), "z");
    test_int(ecs_meta_set_int(&cur, 30), 0);
    test_int(ecs_meta_pop(&cur), 0);

    test_int(value.base.x, 10);
    test_int(value.base.y, 20);
    test_int(value.z, 30);

    ecs_fini(world);
}

void StructInheritance_entity_to_json(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    Derived1_1 *ptr = ecs_ensure_id(world, e, derived, sizeof(Derived1_1));
    ptr->base.x = 10;
    ptr->y = 20;

    char *json = ecs_entity_to_json(world, e, &(ecs_entity_to_json_desc_t){
        .serialize_values = true
    });
    test_assert(json != NULL);
    test_str(json,
        "{\"name\":\"e\", \"components\":{\"Derived\":{\"x\":10, \"y\":20}}}");
    ecs_os_free(json);

    ecs_fini(world);
}

void StructInheritance_entity_from_json(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    const char *r = ecs_entity_from_json(world, e,
        "{\"components\":{\"Derived\":{\"x\":10, \"y\":20}}}", NULL);
    test_assert(r != NULL);

    const Derived1_1 *ptr = ecs_get_id(world, e, derived);
    test_assert(ptr != NULL);
    test_int(ptr->base.x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void StructInheritance_type_info(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    const ecs_type_info_t *ti = ecs_get_type_info(world, derived);
    test_assert(ti != NULL);
    test_int(ti->size, sizeof(Derived1_1));
    test_int(ti->alignment, ECS_ALIGNOF(Derived1_1));

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_assert(t != NULL);
    test_assert(t->kind == EcsStructType);
    test_bool(t->existing, false);
    test_bool(t->partial, false);

    ecs_fini(world);
}

void StructInheritance_use_as_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "z");

    ecs_entity_t e = ecs_new(world);
    Derived2_1 *ptr = ecs_ensure_id(world, e, derived, sizeof(Derived2_1));
    test_assert(ptr != NULL);
    ptr->base.x = 10;
    ptr->base.y = 20;
    ptr->z = 30;
    ecs_modified_id(world, e, derived);

    const Derived2_1 *cptr = ecs_get_id(world, e, derived);
    test_assert(cptr != NULL);
    test_int(cptr->base.x, 10);
    test_int(cptr->base.y, 20);
    test_int(cptr->z, 30);

    char *str = ecs_ptr_to_expr(world, derived, cptr);
    test_str(str, "{x: 10, y: 20, z: 30}");
    ecs_os_free(str);

    ecs_fini(world);
}

void StructInheritance_base_string_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"s", ecs_id(ecs_string_t)}
        }
    });
    test_assert(base != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", "x");
    test_assert(derived != 0);

    const ecs_type_info_t *ti = ecs_get_type_info(world, derived);
    test_assert(ti != NULL);
    test_assert(ti->hooks.dtor != NULL);
    test_assert(ti->hooks.copy != NULL);
    test_assert(ti->hooks.move != NULL);

    ecs_entity_t e = ecs_new(world);
    void *ptr = ecs_ensure_id(world, e, derived, (size_t)ti->size);
    const char *r = ecs_expr_run(world, "{s: \"hello\", x: 10}",
        &(ecs_value_t){ .type = derived, .ptr = ptr }, NULL);
    test_assert(r != NULL);
    ecs_modified_id(world, e, derived);

    char *str = ecs_ptr_to_expr(world, derived, ptr);
    test_str(str, "{s: \"hello\", x: 10}");
    ecs_os_free(str);

    ecs_entity_t e2 = ecs_clone(world, 0, e, true);
    str = ecs_ptr_to_expr(world, derived, ecs_get_id(world, e2, derived));
    test_str(str, "{s: \"hello\", x: 10}");
    ecs_os_free(str);

    ecs_delete(world, e);
    ecs_delete(world, e2);

    ecs_fini(world);
}

void StructInheritance_base_array_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"arr", ecs_id(ecs_i32_t), .count = 3}
        }
    });
    test_assert(base != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    test_assert(derived != 0);

    meta_test_struct(world, derived, ArrayDerived);
    _meta_test_member(world, derived, "arr", ecs_id(ecs_i32_t), 3, offsetof(ArrayDerived, base.arr));
    meta_test_member(world, derived, ArrayDerived, y, ecs_id(ecs_i32_t), 0);

    ArrayDerived value = { .base.arr = {1, 2, 3}, .y = 4 };
    char *str = ecs_ptr_to_expr(world, derived, &value);
    test_str(str, "{arr: [1, 2, 3], y: 4}");
    ecs_os_free(str);

    ecs_fini(world);
}

void StructInheritance_base_unit_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t u = ecs_unit(world, {
        .entity = ecs_entity(world, {.name = "unit"}),
        .symbol = "u"
    });
    test_assert(u != 0);

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), .unit = u}
        }
    });
    test_assert(base != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    test_assert(derived != 0);

    ecs_member_t *m = ecs_struct_get_member(world, derived, "x");
    test_assert(m != NULL);
    test_uint(m->unit, u);

    m = ecs_struct_get_member(world, derived, "y");
    test_assert(m != NULL);
    test_uint(m->unit, 0);

    ecs_fini(world);
}

void StructInheritance_base_member_ranges(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Base"}),
        .members = {
            {"x", ecs_id(ecs_i32_t), .range = {0, 20}, .error_range = {1, 10}}
        }
    });
    test_assert(base != 0);

    ecs_entity_t derived = create_derived(world, base, "Derived", "y");
    test_assert(derived != 0);

    ecs_member_t *m = ecs_struct_get_member(world, derived, "x");
    test_assert(m != NULL);
    test_int(m->range.min, 0);
    test_int(m->range.max, 20);
    test_int(m->error_range.min, 1);
    test_int(m->error_range.max, 10);

    ecs_fini(world);
}

void StructInheritance_base_existing_c_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Base2);

    ecs_struct(world, {
        .entity = ecs_id(Base2),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t derived = create_derived(world, ecs_id(Base2), "Derived", "z");
    test_assert(derived != 0);

    meta_test_struct(world, derived, Derived2_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.x));
    _meta_test_member(world, derived, "y", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.y));
    meta_test_member(world, derived, Derived2_1, z, ecs_id(ecs_i32_t), 0);

    const EcsType *t = ecs_get(world, derived, EcsType);
    test_bool(t->existing, false);

    ecs_fini(world);
}

void StructInheritance_derived_existing_c_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Base2);
    ECS_COMPONENT(world, Derived2_1);

    ecs_struct(world, {
        .entity = ecs_id(Base2),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_add_pair(world, ecs_id(Derived2_1), EcsIsA, ecs_id(Base2));
    ecs_struct(world, {
        .entity = ecs_id(Derived2_1),
        .members = {
            {"z", ecs_id(ecs_i32_t)}
        }
    });

    meta_test_struct(world, ecs_id(Derived2_1), Derived2_1);
    _meta_test_member(world, ecs_id(Derived2_1), "x", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.x));
    _meta_test_member(world, ecs_id(Derived2_1), "y", ecs_id(ecs_i32_t), 0, offsetof(Derived2_1, base.y));
    meta_test_member(world, ecs_id(Derived2_1), Derived2_1, z, ecs_id(ecs_i32_t), 0);

    const EcsType *t = ecs_get(world, ecs_id(Derived2_1), EcsType);
    test_bool(t->existing, true);
    test_bool(t->partial, false);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, Derived2_1, { .base.x = 1, .base.y = 2, .z = 3 });
    char *str = ecs_ptr_to_expr(world, ecs_id(Derived2_1),
        ecs_get(world, e, Derived2_1));
    test_str(str, "{x: 1, y: 2, z: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void StructInheritance_derived_existing_c_type_w_padding(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, PaddedBase);
    ECS_COMPONENT(world, PaddedDerived);

    ecs_struct(world, {
        .entity = ecs_id(PaddedBase),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i8_t)}
        }
    });

    ecs_add_pair(world, ecs_id(PaddedDerived), EcsIsA, ecs_id(PaddedBase));
    ecs_struct(world, {
        .entity = ecs_id(PaddedDerived),
        .members = {
            {"z", ecs_id(ecs_i8_t)}
        }
    });

    meta_test_struct(world, ecs_id(PaddedDerived), PaddedDerived);
    _meta_test_member(world, ecs_id(PaddedDerived), "x", ecs_id(ecs_i32_t), 0, offsetof(PaddedDerived, base.x));
    _meta_test_member(world, ecs_id(PaddedDerived), "y", ecs_id(ecs_i8_t), 0, offsetof(PaddedDerived, base.y));
    meta_test_member(world, ecs_id(PaddedDerived), PaddedDerived, z, ecs_id(ecs_i8_t), 0);

    const EcsType *t = ecs_get(world, ecs_id(PaddedDerived), EcsType);
    test_bool(t->existing, true);
    test_bool(t->partial, false);

    ecs_fini(world);
}

void StructInheritance_nested_derived_as_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    ecs_entity_t outer = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Outer"}),
        .members = {
            {"d", derived},
            {"w", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(outer != 0);

    meta_test_struct(world, outer, Outer);
    meta_test_member(world, outer, Outer, d, derived, 0);
    meta_test_member(world, outer, Outer, w, ecs_id(ecs_i32_t), 0);

    Outer value = { .d.base.x = 1, .d.y = 2, .w = 3 };
    char *str = ecs_ptr_to_expr(world, outer, &value);
    test_str(str, "{d: {x: 1, y: 2}, w: 3}");
    ecs_os_free(str);

    ecs_fini(world);
}

void StructInheritance_derived_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world, false);
    ecs_entity_t derived = ecs_entity(stage, {.name = "Derived"});
    ecs_add_pair(stage, derived, EcsIsA, base);
    ecs_readonly_end(world);

    ecs_readonly_begin(world, false);
    derived = ecs_struct(stage, {
        .entity = derived,
        .members = {
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    ecs_readonly_end(world);
    test_assert(derived != 0);

    meta_test_struct(world, derived, Derived1_1);
    _meta_test_member(world, derived, "x", ecs_id(ecs_i32_t), 0, offsetof(Derived1_1, base.x));
    meta_test_member(world, derived, Derived1_1, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_deferred_isa(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);

    ecs_entity_t derived = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Derived"})
    });

    ecs_defer_begin(world);
    ecs_add_pair(world, derived, EcsIsA, base);
    ecs_defer_end(world);

    meta_test_struct(world, derived, Base2);
    meta_test_member(world, derived, Base2, x, ecs_id(ecs_i32_t), 0);
    meta_test_member(world, derived, Base2, y, ecs_id(ecs_i32_t), 0);

    ecs_fini(world);
}

void StructInheritance_serializer_ops(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    const EcsTypeSerializer *ser = ecs_get(world, derived, EcsTypeSerializer);
    test_assert(ser != NULL);
    test_assert(ser->ops.count > 2);

    char *str = ecs_meta_serializer_to_str(world, derived);
    test_assert(str != NULL);
    ecs_os_free(str);

    ecs_fini(world);
}

void StructInheritance_derived_equals(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    Derived1_1 a = { .base.x = 1, .y = 2 };
    Derived1_1 b = { .base.x = 1, .y = 2 };
    Derived1_1 c = { .base.x = 1, .y = 3 };

    test_assert(ecs_value_equals(world, &(ecs_value_t){ .type = derived, .ptr = &a }, &(ecs_value_t){ .type = derived, .ptr = &b }));
    test_assert(!ecs_value_equals(world, &(ecs_value_t){ .type = derived, .ptr = &a }, &(ecs_value_t){ .type = derived, .ptr = &c }));

    ecs_fini(world);
}

void StructInheritance_cursor_set_value_derived(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "z");

    Derived2_1 dv = { .base.x = 10, .base.y = 20, .z = 30 };
    Base2 bv = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, base, &bv);
    test_int(ecs_meta_set_value(&cur, &(ecs_value_t){
        .type = derived, .ptr = &dv
    }), 0);

    test_int(bv.x, 10);
    test_int(bv.y, 20);

    ecs_fini(world);
}

void StructInheritance_cursor_set_value_derived_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base1(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "y");

    ecs_entity_t outer = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Outer"}),
        .members = {
            {"d", base},
            {"w", ecs_id(ecs_i32_t)}
        }
    });
    test_assert(outer != 0);

    Derived1_1 dv = { .base.x = 10, .y = 20 };
    struct { Base1 d; ecs_i32_t w; } ov = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, outer, &ov);
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "d"), 0);
    test_int(ecs_meta_set_value(&cur, &(ecs_value_t){
        .type = derived, .ptr = &dv
    }), 0);
    test_int(ecs_meta_member(&cur, "w"), 0);
    test_int(ecs_meta_set_int(&cur, 5), 0);
    test_int(ecs_meta_pop(&cur), 0);

    test_int(ov.d.x, 10);
    test_int(ov.w, 5);

    ecs_fini(world);
}

void StructInheritance_cursor_set_value_base_to_derived_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t derived = create_derived(world, base, "Derived", "z");

    Base2 bv = { .x = 10, .y = 20 };
    Derived2_1 dv = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, derived, &dv);
    ecs_log_set_level(-4);
    test_int(ecs_meta_set_value(&cur, &(ecs_value_t){
        .type = base, .ptr = &bv
    }), -1);
    ecs_log_set_level(-1);

    test_int(dv.base.x, 0);

    ecs_fini(world);
}

void StructInheritance_cursor_set_value_unrelated_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = create_base2(world);
    ecs_entity_t other = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Other"}),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    Base2 ov = { .x = 10, .y = 20 };
    Base2 bv = {0};

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, base, &bv);
    ecs_log_set_level(-4);
    test_int(ecs_meta_set_value(&cur, &(ecs_value_t){
        .type = other, .ptr = &ov
    }), -1);
    ecs_log_set_level(-1);

    test_int(bv.x, 0);

    ecs_fini(world);
}
