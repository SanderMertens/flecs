#include <cpp_api.h>

void Entity_new() {
    flecs::world world;

    auto entity = world.entity();
    test_assert(entity.id() != 0);
}

void Entity_new_named() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Foo");
}

void Entity_new_named_from_scope() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Foo");

    auto prev = world.set_scope(entity);

    auto child = world.entity("Bar");
    test_assert(child.id() != 0);

    world.set_scope(prev);

    test_str(child.name().c_str(), "Bar");
    test_str(child.path().c_str(), "::Foo::Bar");
}

void Entity_new_nested_named_from_scope() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Foo");

    auto prev = world.set_scope(entity);

    auto child = world.entity("Bar::Hello");
    test_assert(child.id() != 0);

    world.set_scope(prev);

    test_str(child.name().c_str(), "Hello");
    test_str(child.path().c_str(), "::Foo::Bar::Hello");
}

void Entity_new_nested_named_from_nested_scope() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo::Bar");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Bar");
    test_str(entity.path().c_str(), "::Foo::Bar");

    auto prev = world.set_scope(entity);

    auto child = world.entity("Hello::World");
    test_assert(child.id() != 0);

    world.set_scope(prev);

    test_str(child.name().c_str(), "World");
    test_str(child.path().c_str(), "::Foo::Bar::Hello::World");
}

void Entity_new_add() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = world.entity()
        .add<Position>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
}

void Entity_new_add_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = world.entity()
        .add<Position>()
        .add<Velocity>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_new_set() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_new_set_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void Entity_add() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());
}

void Entity_remove() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());

    entity.remove<Position>();
    test_assert(!entity.has<Position>());
}

void Entity_set() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.set<Position>({10, 20});
    test_assert(entity.has<Position>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_replace() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.patch<Position>([](Position& p) {
        p.x = 10;
        p.y = 20;
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    entity.patch<Position>([](Position& p_arg) {
        p_arg.x = 30;
    });

    test_int(p->x, 30); 
}

void Entity_add_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add<Position>()
          .add<Velocity>();

    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_add_entity() {
    flecs::world world;

    auto tag = world.entity();
    test_assert(tag.id() != 0);

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add(tag);
    test_assert(entity.has(tag));
}

void Entity_add_childof() {
    flecs::world world;

    auto parent = world.entity();
    test_assert(parent.id() != 0);

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add(flecs::ChildOf, parent);
    test_assert(entity.has(flecs::ChildOf, parent.id()));
}

void Entity_add_instanceof() {
    flecs::world world;

    auto base = world.entity();
    test_assert(base.id() != 0);

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add(flecs::IsA, base);
    test_assert(entity.has(flecs::IsA, base.id()));
}

void Entity_remove_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add<Position>()
          .add<Velocity>();

    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());

    entity.remove<Position>()
          .remove<Velocity>();

    test_assert(!entity.has<Position>());
    test_assert(!entity.has<Velocity>());          
}

void Entity_set_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.set<Position>({10, 20})
        .set<Velocity>({1, 2});    
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = entity.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);    
}

void Entity_remove_entity() {
    flecs::world world;

    auto tag = world.entity();
    test_assert(tag.id() != 0);

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add(tag);
    test_assert(entity.has(tag));

    entity.remove(tag);
    test_assert(!entity.has(tag));
}

void Entity_remove_childof() {
    flecs::world world;

    auto parent = world.entity();
    test_assert(parent.id() != 0);

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add(flecs::ChildOf, parent);
    test_assert(entity.has(flecs::ChildOf, parent.id()));

    entity.remove(flecs::ChildOf, parent);
    test_assert(!entity.has(flecs::ChildOf, parent.id()));
}

void Entity_remove_instanceof() {
    flecs::world world;

    auto base = world.entity();
    test_assert(base.id() != 0);

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add(flecs::IsA, base);
    test_assert(entity.has(flecs::IsA, base.id()));
    
    entity.remove(flecs::IsA, base);
    test_assert(!entity.has(flecs::IsA, base.id()));
}

void Entity_get_generic() {
    flecs::world world;

    auto position = flecs::component<Position>(world, "Position");

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    const void *void_p = entity.get(position);
    test_assert(void_p != nullptr);

    const Position *p = static_cast<const Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_get_mut_generic() {
    flecs::world world;

    auto position = flecs::component<Position>(world, "Position");

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    bool invoked;
    flecs::system<Position>(world)
        .kind(flecs::OnSet)
        .each([&invoked](flecs::entity e, Position& p) {
            invoked = true;
        });

    void *void_p = entity.get_mut(position);
    test_assert(void_p != nullptr);

    Position *p = static_cast<Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);

    entity.modified(position);
    test_bool(invoked, true);
}

void Entity_get_generic_w_id() {
    flecs::world world;

    auto position = flecs::component<Position>(world, "Position");

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    const void *void_p = entity.get(position.id());
    test_assert(void_p != nullptr);

    const Position *p = static_cast<const Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_get_mut_generic_w_id() {
    flecs::world world;

    auto position = flecs::component<Position>(world, "Position");

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    bool invoked;
    flecs::system<Position>(world)
        .kind(flecs::OnSet)
        .each([&invoked](flecs::entity e, Position& p) {
            invoked = true;
        });

    void *void_p = entity.get_mut(position.id());
    test_assert(void_p != nullptr);

    Position *p = static_cast<Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);

    entity.modified(position.id());
    test_bool(invoked, true);
}

void Entity_add_role() {
    flecs::world world;

    auto entity = world.entity();

    entity = entity.add_role(flecs::Pair);

    test_assert(entity.id() & ECS_PAIR);
}

void Entity_remove_role() {
    flecs::world world;

    auto entity = world.entity();

    flecs::entity_t id = entity.id();

    entity = entity.add_role(flecs::Pair);

    test_assert(entity.id() & ECS_PAIR);

    entity = entity.remove_role();

    test_assert(entity.id() == id);
}

void Entity_has_role() {
    flecs::world world;

    auto entity = world.entity();

    entity = entity.add_role(flecs::Pair);

    test_assert(entity.has_role(flecs::Pair));

    entity = entity.remove_role();

    test_assert(!entity.has_role(flecs::Pair));
}

void Entity_pair_role() {
    flecs::world world;

    auto a = world.entity();
    auto b = world.entity();

    auto comb = flecs::entity::comb(a, b);
    comb = comb.add_role(flecs::Pair);
    
    test_assert(comb.has_role(flecs::Pair));

    auto lo = comb.lo();
    auto hi = comb.remove_role().hi();

    test_assert(lo == a);
    test_assert(hi == b);
}

void Entity_equals() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();

    auto id1 = flecs::entity(world, e1.id());
    auto id2 = flecs::entity(world, e2.id());

    test_assert(e1 == id1);
    test_assert(e2 == id2);
    test_assert(e1 != e2);
}

void Entity_has_childof() {
    flecs::world world;

    auto parent = world.entity();

    auto e = world.entity()
        .add(flecs::ChildOf, parent);

    test_assert(e.has(flecs::ChildOf, parent));
}

void Entity_has_instanceof() {
    flecs::world world;

    auto base = world.entity();

    auto e = world.entity()
        .add(flecs::IsA, base);

    test_assert(e.has(flecs::IsA, base));
}

void Entity_has_instanceof_indirect() {
    flecs::world world;

    auto base_of_base = world.entity();

    auto base = world.entity()
        .add(flecs::IsA, base_of_base);

    auto e = world.entity()
        .add(flecs::IsA, base);

    test_assert(e.has(flecs::IsA, base_of_base));
}

void Entity_null_string() {
    flecs::world world;

    auto e = world.entity();

    test_str(e.name().c_str(), "");
}

void Entity_set_name() {
    flecs::world world;

    auto e = world.entity();
    test_str(e.name().c_str(), "");

    e.set<flecs::Name>({"Foo"});
    test_str(e.name().c_str(), "Foo");
}

void Entity_change_name() {
    flecs::world world;

    auto e = world.entity("Bar");
    test_str(e.name().c_str(), "Bar");

    e.set<flecs::Name>({"Foo"});
    test_str(e.name().c_str(), "Foo");
}

void Entity_delete() {
    flecs::world world;

    auto e = world.entity()
        .add<Position>()
        .add<Velocity>();

    e.destruct();

    test_assert(!e.is_alive());

    auto e2 = world.entity();

    // Entity ids should be equal without the generation
    test_assert((int32_t)e2.id() == (int32_t)e.id());
    test_assert(e2.id() != e.id());
}

void Entity_clear() {
    flecs::world world;

    auto e = world.entity()
        .add<Position>()
        .add<Velocity>();

    e.clear();

    test_assert(!e.has<Position>());
    test_assert(!e.has<Velocity>());

    auto e2 = world.entity();
    test_assert(e2.id() > e.id());
}

void Entity_foce_owned() {
    flecs::world world;

    auto prefab = world.prefab()
        .add<Position>()
        .add<Velocity>()
        .add_owned<Position>();

    auto e = world.entity()
        .add(flecs::IsA, prefab);
    
    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());
    test_assert(e.has<Velocity>());
    test_assert(!e.owns<Velocity>());
}

void Entity_force_owned_2() {
    flecs::world world;

    auto prefab = world.prefab()
        .add<Position>()
        .add<Velocity>()
        .add_owned<Position>()
        .add_owned<Velocity>();

    auto e = world.entity()
        .add(flecs::IsA, prefab);
    
    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());
    test_assert(e.has<Velocity>());
    test_assert(e.owns<Velocity>());
}

void Entity_force_owned_nested() {
    flecs::world world;

    auto prefab = world.prefab()
        .add<Position>()
        .add<Velocity>()
        .add_owned<Position>();

    auto prefab_2 = world.prefab()
        .add(flecs::IsA, prefab);

    auto e = world.entity()
        .add(flecs::IsA, prefab_2);
    
    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());
    test_assert(e.has<Velocity>());
    test_assert(!e.owns<Velocity>());
}

void Entity_force_owned_type() {
    flecs::world world;

    auto type = world.type()
        .add<Position>()
        .add<Velocity>();

    auto prefab = world.prefab()
        .add<Position>()
        .add<Velocity>()
        .add<Rotation>()
        .add_owned(type);

    auto e = world.entity()
        .add(flecs::IsA, prefab);
    
    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());
    test_assert(e.has<Velocity>());
    test_assert(e.owns<Velocity>());
    test_assert(e.has<Rotation>());
    test_assert(!e.owns<Rotation>());
}

void Entity_force_owned_type_w_pair() {
    flecs::world world;

    auto type = world.type()
        .add<Position, Velocity>()
        .add<Velocity>();

    auto prefab = world.prefab()
        .add<Position, Velocity>()
        .add<Rotation>()
        .add_owned(type);

    auto e = world.entity()
        .add(flecs::IsA, prefab);
    
    test_assert((e.has<Position, Velocity>()));
    test_assert(e.has<Rotation>());
    test_assert(!e.owns<Rotation>());

    const Position *pp = prefab.get<Position, Velocity>();
    const Position *p = e.get<Position, Velocity>();
    test_assert(pp != p);
}

struct MyTag { };

void Entity_tag_has_size_zero() {
    flecs::world world;

    auto comp = world.component<MyTag>();

    auto ptr = comp.get<flecs::Component>();
    test_int(ptr->size, 0);
    test_int(ptr->alignment, 0);
}

void Entity_get_null_name() {
    flecs::world world;

    auto e = world.entity()
        .set<flecs::Name>({nullptr});

    auto n = e.name();
    test_assert(n.size() == 0);
}

void Entity_get_parent() {
    flecs::world world;

    auto parent1 = world.entity()
        .add<Position>();

    auto parent2 = world.entity()
        .add<Velocity>();

    auto parent3 = world.entity()
        .add<Mass>();

    auto child = world.entity()
        .add(flecs::ChildOf, parent1)
        .add(flecs::ChildOf, parent2)
        .add(flecs::ChildOf, parent3);

    auto p = child.get_parent<Velocity>();
    test_assert(p.id() != 0);
    test_assert(p == parent2);
}

void Entity_get_parent_w_tag() {
    flecs::world world;

    auto TagA = world.entity();
    auto TagB = world.entity();
    auto TagC = world.entity();

    auto parent1 = world.entity()
        .add(TagA);

    auto parent2 = world.entity()
        .add(TagB);

    auto parent3 = world.entity()
        .add(TagC);

    auto child = world.entity()
        .add(flecs::ChildOf, parent1)
        .add(flecs::ChildOf, parent2)
        .add(flecs::ChildOf, parent3);

    auto p = child.get_parent(TagB);
    test_assert(p.id() != 0);
    test_assert(p == parent2);
}

void Entity_is_component_enabled() {
    flecs::world world;

    auto e = world.entity()
        .add<Position>();

    test_assert(e.is_enabled<Position>());
}

void Entity_is_enabled_component_enabled() {
    flecs::world world;

    auto e = world.entity()
        .add<Position>()
        .enable<Position>();

    test_assert(e.is_enabled<Position>());
}

void Entity_is_disabled_component_enabled() {
    flecs::world world;

    auto e = world.entity()
        .add<Position>()
        .disable<Position>();

    test_assert(!e.is_enabled<Position>());
}

void Entity_get_type() {
    flecs::world world;

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    auto type_1 = entity.type();
    test_assert(type_1.id() == 0);
    test_int(type_1.vector().size(), 0);

    auto type_2 = entity.type();
    test_assert(type_2.id() == 0);
    test_int(type_1.vector().size(), 0);
}

void Entity_get_nonempty_type() {
    flecs::world world;

    auto entity = world.entity()
        .add<Position>();
    test_assert(entity.id() != 0);

    auto type_1 = entity.type();
    test_assert(type_1.id() == 0);
    test_int(type_1.vector().size(), 1);

    auto type_2 = entity.type();
    test_assert(type_2.id() == 0);
    test_int(type_1.vector().size(), 1);
}

void Entity_set_no_copy() {
    flecs::world world;

    auto e = world.entity()
        .set<POD>({10});
    test_int(POD::copy_invoked, 0);

    test_assert(e.has<POD>());
    const POD *p = e.get<POD>();
    test_assert(p != NULL);
    test_int(p->value, 10);
}

void Entity_set_copy() {
    flecs::world world;

    POD val(10);

    auto e = world.entity()
        .set<POD>(val);
    test_int(POD::copy_invoked, 1);

    test_assert(e.has<POD>());
    const POD *p = e.get<POD>();
    test_assert(p != NULL);
    test_int(p->value, 10);
}

void Entity_add_owned() {
    flecs::world world;

    auto base = world.entity()
        .add_owned<Position>();

    auto e = world.entity()
        .add(flecs::IsA, base);

    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());
}

void Entity_set_owned() {
    flecs::world world;

    auto base = world.entity()
        .set_owned<Position>({10, 20});

    auto e = world.entity()
        .add(flecs::IsA, base);

    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());

    const Position* p = e.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Position* p_base = base.get<Position>();
    test_assert(p != p_base);
    test_int(p_base->x, 10);
    test_int(p_base->y, 20);
}

void Entity_implicit_name_to_char() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Foo");

    test_str(entity.name(), "Foo");
}

void Entity_implicit_path_to_char() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo::Bar");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Bar");

    test_str(entity.path(), "::Foo::Bar");
}

void Entity_implicit_type_str_to_char() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity.id() != 0);

    test_str(entity.type().str(), "Name");
}

void Entity_entity_to_entity_view() {
    flecs::world world;

    flecs::entity e = world.entity()
        .set<Position>({10, 20});
    test_assert(e.id() != 0);

    flecs::entity_view ev = e;
    test_assert(ev.id() != 0);
    test_assert(e.id() == ev.id());

    const Position *p = ev.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_entity_view_to_entity_world() {
    flecs::world world;

    flecs::entity e = world.entity()
        .set<Position>({10, 20});
    test_assert(e.id() != 0);

    flecs::entity_view ev = e;
    test_assert(ev.id() != 0);
    test_assert(e.id() == ev.id());

    flecs::entity ew = ev.mut(world);
    ew.set<Position>({10, 20});

    test_assert(ev.has<Position>());
    const Position *p = ev.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_entity_view_to_entity_stage() {
    flecs::world world;

    flecs::entity_view ev = world.entity();

    auto stage = world.get_stage(0);

    world.staging_begin();

    flecs::entity ew = ev.mut(stage);

    ew.set<Position>({10, 20});
    test_assert(!ew.has<Position>());

    world.staging_end();

    test_assert(ew.has<Position>());
    test_assert(ev.has<Position>());

    const Position *p = ev.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_create_entity_view_from_stage() {
    flecs::world world;

    auto stage = world.get_stage(0);

    world.staging_begin();

    flecs::entity_view ev = stage.entity();
    test_assert(ev != 0);

    world.staging_end();

    // Ensure we can use created ev out of stage
    auto ew = ev.mut(world);
    ew.set<Position>({10, 20});
    test_assert(ev.has<Position>());

    const Position *p = ev.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}
