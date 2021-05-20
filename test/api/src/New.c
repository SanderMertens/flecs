#include <api.h>

void New_setup() {
    ecs_tracing_enable(-2);
}

void New_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_get_type(world, e));

    ecs_fini(world);
}

void New_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void New_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_type_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_type_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1);

    ecs_entity_t e = ecs_new(world, Type_2);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_type_w_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Velocity);
    ECS_TYPE(world, Type_3, AND | Type_1, AND | Type_2);

    ecs_entity_t e = ecs_new(world, Type_3);

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void New_type_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1, Velocity);

    ecs_entity_t e = ecs_new(world, Type_2);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void New_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e = ecs_new_w_entity(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag));
    
    ecs_fini(world);
}

void New_type_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    ECS_TYPE(world, Type, Tag);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag));
    
    ecs_fini(world);
}

void New_type_w_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);

    ECS_TYPE(world, Type, Tag_1, Tag_2);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_1));
    
    ecs_fini(world);
}

void New_type_w_tag_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);
    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Position, Tag);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_redefine_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = 0;

    {
        ECS_COMPONENT(world, Position);
        c = ecs_typeid(Position);
    }

    {
        ECS_COMPONENT(world, Position);
        test_assert(c == ecs_typeid(Position));
    }
    
    ecs_fini(world);
}

void New_recycle_id_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_recycle_id_w_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_new(world, 0);

    ecs_entity_t e1 = ecs_new_w_entity(world, tag);
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new_w_entity(world, tag);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_recycle_id_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w_type(world, ecs_type(Position));
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new_w_type(world, ecs_type(Position));
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_recycle_empty_staged_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_defer_begin(world);
    ecs_delete(world, e1);
    ecs_defer_end(world);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_recycle_staged_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);

    ecs_defer_begin(world);
    ecs_delete(world, e1);
    ecs_defer_end(world);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_new_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    test_assert(!ecs_get_type(world, e));

    ecs_fini(world);
}

void New_new_component_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_component_id(world);
    test_assert(e != 0);
    test_assert(e < ECS_HI_COMPONENT_ID);
    test_assert(!ecs_get_type(world, e));

    ecs_fini(world);
}

void New_new_component_id_skip_used() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new_component_id(world);
    test_assert(e != 0);
    test_assert(e < ECS_HI_COMPONENT_ID);
    test_assert(!ecs_get_type(world, e));

    /* Explicitly set an id that is one above the last issued id */
    ecs_add_id(world, e + 1, Foo);

    ecs_entity_t e2 = ecs_new_component_id(world);
    test_assert(e2 != 0);
    test_assert(e2 < ECS_HI_COMPONENT_ID);
    test_assert(!ecs_get_type(world, e2));    
    test_assert(e2 != (e + 1));

    ecs_fini(world);
}

void New_new_component_id_skip_to_hi_id() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new_component_id(world);
    test_assert(e != 0);

    /* Use up all low component ids */
    int i;
    for (i = (int)e; i < ECS_HI_COMPONENT_ID; i ++) {
        ecs_add_id(world, i, Foo);
    }

    ecs_entity_t e2 = ecs_new_component_id(world);
    test_assert(e2 != 0);
    test_assert(e2 > ECS_HI_COMPONENT_ID);
    test_assert(!ecs_get_type(world, e2));

    ecs_entity_t e3 = ecs_new_id(world);
    test_assert(e3 != e2);
    test_assert(e3 > e2);
    test_assert(!ecs_get_type(world, e3));

    ecs_fini(world);
}

void New_new_hi_component_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_entity_t c;
    do {
        c = ecs_new_component_id(world);
    } while (c < ECS_HI_COMPONENT_ID);

    test_assert(c != e);
    test_assert(c > e);

    ecs_fini(world);
}

void New_new_w_entity_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_w_entity(world, 0);
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

ECS_ENTITY_DECLARE(Foo);

void New_create_w_explicit_id_2_worlds() {
    ecs_world_t *world_1 = ecs_init();
    ecs_world_t *world_2 = ecs_init();

    ecs_entity_t p1 = ecs_set(world_1, 0, EcsName, {"Parent"});
    ecs_entity_t p2 = ecs_set(world_2, 0, EcsName, {"Parent"});

    ecs_set_scope(world_1, p1);
    ecs_set_scope(world_2, p2);

    ECS_ENTITY_DEFINE(world_1, Foo, 0);
    ECS_ENTITY_DEFINE(world_2, Foo, 0);

    char *path = ecs_get_fullpath(world_1, Foo);
    test_str(path, "Parent.Foo");
    ecs_os_free(path);

    path = ecs_get_fullpath(world_2, Foo);
    test_str(path, "Parent.Foo");
    ecs_os_free(path);

    ecs_fini(world_1);
    ecs_fini(world_2);
}

void New_new_w_id_0_w_with() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_new_w_id(world, 0);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_id_w_with() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_id_w_with_w_scope() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_set_scope(world, parent);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));
    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void New_new_w_id_w_with_defer() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);
    
    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Tag2));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_id_w_with_defer_w_scope() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_set_scope(world, parent);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Tag2));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));
    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void New_new_w_type_0_w_with() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_type_w_with() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_type_w_with_w_scope() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_set_scope(world, parent);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void New_new_w_type_w_with_defer() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    
    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_type_w_with_defer_w_scope() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_set_scope(world, parent);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}
