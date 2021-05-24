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

    world.component<Position>();

    auto entity = world.entity()
        .add<Position>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
}

void Entity_new_add_2() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto entity = world.entity()
        .add<Position>()
        .add<Velocity>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_new_set() {
    flecs::world world;

    world.component<Position>();

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

    world.component<Position>();
    world.component<Velocity>();

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

    world.component<Position>();

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());
}

void Entity_remove() {
    flecs::world world;

    world.component<Position>();

    auto entity = world.entity();
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());

    entity.remove<Position>();
    test_assert(!entity.has<Position>());
}

void Entity_set() {
    flecs::world world;

    world.component<Position>();

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

    world.component<Position>();
    world.component<Velocity>();

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

    world.component<Position>();
    world.component<Velocity>();

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

    world.component<Position>();
    world.component<Velocity>();

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

    world.component<Position>();
    world.component<Velocity>();

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

    auto position = world.component<Position>();

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

    auto position = world.component<Position>();

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    bool invoked;
    world.system<Position>()
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

    auto position = world.component<Position>();

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

    auto position = world.component<Position>();

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());

    bool invoked;
    world.system<Position>()
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
    test_int(p.id(), parent2.id());
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
        .set<Pod>({10});
    test_int(Pod::copy_invoked, 0);

    test_assert(e.has<Pod>());
    const Pod *p = e.get<Pod>();
    test_assert(p != NULL);
    test_int(p->value, 10);
}

void Entity_set_copy() {
    flecs::world world;

    Pod val(10);

    auto e = world.entity()
        .set<Pod>(val);
    test_int(Pod::copy_invoked, 1);

    test_assert(e.has<Pod>());
    const Pod *p = e.get<Pod>();
    test_assert(p != NULL);
    test_int(p->value, 10);
}

void Entity_set_deduced() {
    flecs::world world;

    auto e = world.entity()
        .set(Position{10, 20});

    test_assert(e.has<Position>());

    const Position *p = e.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
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

void Entity_set_template() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Template<int>>({10, 20});

    const Template<int> *ptr = e.get<Template<int>>();
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_get_1_component_w_callback() {
    flecs::world ecs;

    auto e_1 = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto e_2 = ecs.entity()
        .set<Position>({11, 22});

    auto e_3 = ecs.entity()
        .set<Velocity>({1, 2});

    test_bool(e_1.get([](const Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
    }), true);

    test_bool(e_2.get([](const Position& p) {
        test_int(p.x, 11);
        test_int(p.y, 22);
    }), true);

    test_bool(e_3.get([](const Position& p) {}), false);
}

void Entity_get_2_components_w_callback() {
    flecs::world ecs;

    auto e_1 = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto e_2 = ecs.entity()
        .set<Position>({11, 22});

    auto e_3 = ecs.entity()
        .set<Velocity>({1, 2});

    test_bool(e_1.get([](const Position& p, const Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
    }), true);

    test_bool(e_2.get([](const Position& p, const Velocity& v) {}), false);

    test_bool(e_3.get([](const Position& p, const Velocity& v) {}), false);
}

void Entity_set_1_component_w_callback() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](Position& p){
            p.x = 10;
            p.y = 20;
        });

    test_assert(e.has<Position>());

    const Position *p = e.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_set_2_components_w_callback() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](Position& p, Velocity& v){
            p = {10, 20};
            v = {1, 2};
        });

    test_assert(e.has<Position>());

    const Position *p = e.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = e.get<Velocity>();
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void Entity_set_3_components_w_callback() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](Position& p, Velocity& v, Mass& m){
            p = {10, 20};
            v = {1, 2};
            m = {50};
        });

    test_assert(e.has<Position>());

    const Position *p = e.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = e.get<Velocity>();
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    const Mass *m = e.get<Mass>();
    test_assert(m != NULL);
    test_int(m->value, 50);
}

void Entity_defer_set_1_component() {
    flecs::world ecs;

    ecs.defer_begin();

    auto e = ecs.entity()
        .set([](Position& p){
            p.x = 10;
            p.y = 20;
        });

    test_assert(!e.has<Position>());

    ecs.defer_end();

    test_assert(e.has<Position>());

    e.get([](const Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
    });
}

void Entity_defer_set_2_components() {
    flecs::world ecs;

    ecs.defer_begin();

    auto e = ecs.entity()
        .set([](Position& p, Velocity& v){
            p = {10, 20};
            v = {1, 2};
        });

    test_assert(!e.has<Position>());
    test_assert(!e.has<Velocity>());

    ecs.defer_end();

    test_assert(e.has<Position>());
    test_assert(e.has<Velocity>());

    e.get([](const Position& p, const Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
    });    
}

void Entity_defer_set_3_components() {
    flecs::world ecs;

    ecs.defer_begin();

    auto e = ecs.entity()
        .set([](Position& p, Velocity& v, Mass& m){
            p = {10, 20};
            v = {1, 2};
            m = {50};
        });

    test_assert(!e.has<Position>());
    test_assert(!e.has<Velocity>());
    test_assert(!e.has<Mass>());

    ecs.defer_end();

    test_assert(e.has<Position>());
    test_assert(e.has<Velocity>());
    test_assert(e.has<Mass>());

    e.get([](const Position& p, const Velocity& v, const Mass& m) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);

        test_int(m.value, 50);
    });  
}

void Entity_set_2_w_on_set() {
    flecs::world ecs;

    int32_t position_set = 0;
    int32_t velocity_set = 0;

    ecs.system<Position>()
        .kind(flecs::OnSet)
        .each([&](flecs::entity e, Position& p) {
            position_set ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
        });

    ecs.system<Velocity>()
        .kind(flecs::OnSet)
        .each([&](flecs::entity e, Velocity& v) {
            velocity_set ++;
            test_int(v.x, 1);
            test_int(v.y, 2);
        });        

    auto e = ecs.entity()
        .set([](Position& p, Velocity& v){
            p = {10, 20};
            v = {1, 2};
        });

    test_int(position_set, 1);
    test_int(velocity_set, 1);

    test_bool(e.get([](const Position& p, const Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
    }), true);
}

void Entity_defer_set_2_w_on_set() {
    flecs::world ecs;

    int32_t position_set = 0;
    int32_t velocity_set = 0;

    ecs.system<Position>()
        .kind(flecs::OnSet)
        .each([&](flecs::entity e, Position& p) {
            position_set ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
        });

    ecs.system<Velocity>()
        .kind(flecs::OnSet)
        .each([&](flecs::entity e, Velocity& v) {
            velocity_set ++;
            test_int(v.x, 1);
            test_int(v.y, 2);
        });

    ecs.defer_begin();   

    auto e = ecs.entity()
        .set([](Position& p, Velocity& v){
            p = {10, 20};
            v = {1, 2};
        });

    test_int(position_set, 0);
    test_int(velocity_set, 0);

    ecs.defer_end();

    test_int(position_set, 1);
    test_int(velocity_set, 1);

    test_bool(e.get([](const Position& p, const Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
    }), true);
}

void Entity_set_2_after_fluent() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Mass>({50})
        .set([](Position& p, Velocity& v){
            p = {10, 20};
            v = {1, 2};
        });

    test_assert(e.has<Position>());
    test_assert(e.has<Velocity>());
    test_assert(e.has<Mass>());

    test_bool(e.get([](const Position& p, const Velocity& v, const Mass& m) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
        
        test_int(m.value, 50);
    }), true);
}

void Entity_set_2_before_fluent() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set([](Position& p, Velocity& v){
            p = {10, 20};
            v = {1, 2};
        })
        .set<Mass>({50});

    test_assert(e.has<Position>());
    test_assert(e.has<Velocity>());
    test_assert(e.has<Mass>());

    test_bool(e.get([](const Position& p, const Velocity& v, const Mass& m) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
        
        test_int(m.value, 50);
    }), true);
}

void Entity_set_2_after_set_1() {
    flecs::world ecs;

    int called = 0;

    auto e = ecs.entity().set<Position>({5, 10});
    test_assert(e.has<Position>());

    test_bool(e.get([](const Position& p) {
        test_int(p.x, 5);
        test_int(p.y, 10);
    }), true);

    // Set both Position and Velocity
    e.set([](Position& p, Velocity& v) {
        p = {10, 20};
        v = {1, 2};
    });

    test_bool(e.get([&](const Position& p, const Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);
        
        called ++;
    }), true);

    test_int(called, 1);
}

void Entity_set_2_after_set_2() {
    flecs::world ecs;

    int called = 0;

    auto e = ecs.entity()
        .set<Position>({5, 10})
        .set<Velocity>({1, 2});
    test_assert(e.has<Position>());
    test_assert(e.has<Velocity>());    

    test_bool(e.get([&](const Position& p, const Velocity& v) {
        test_int(p.x, 5);
        test_int(p.y, 10);

        test_int(v.x, 1);
        test_int(v.y, 2);

        called ++;
    }), true);

    test_int(called, 1);

    // Set both Position and Velocity (doesn't add any components)
    e.set([](Position& p, Velocity& v) {
        p = {10, 20};
        v = {3, 4};
    });

    test_bool(e.get([&](const Position& p, const Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 3);
        test_int(v.y, 4);

        called ++;
    }), true);  

    test_int(called, 2);
}

void Entity_with_self() {
    flecs::world ecs;

    auto Tag = ecs.entity().with([&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get the tag.
    auto self = ecs.component<Self>();
    test_assert(!self.has(Tag));

    int count = 0;
    auto q = ecs.query_builder<>().term(Tag).build();

    q.each([&](flecs::entity e) {
        test_assert(e.has(Tag));

        test_bool(e.get([&](const Self& s){
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    test_int(count, 3);
}

void Entity_with_relation_type_self() {
    flecs::world ecs;

    struct Likes { };

    auto Bob = ecs.entity().with<Likes>([&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get the tag.
    auto self = ecs.component<Self>();
    test_assert(!self.has<Likes>(Bob));

    int count = 0;
    auto q = ecs.query_builder<>().term<Likes>(Bob).build();

    q.each([&](flecs::entity e) {
        test_assert(e.has<Likes>(Bob));

        test_bool(e.get([&](const Self& s){
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    test_int(count, 3);
}

void Entity_with_relation_self() {
    flecs::world ecs;

    auto Likes = ecs.entity();

    auto Bob = ecs.entity().with(Likes, [&]{
        auto e1 = ecs.entity(); e1.set<Self>({e1});
        auto e2 = ecs.entity(); e2.set<Self>({e2});
        auto e3 = ecs.entity(); e3.set<Self>({e3});
    });

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not get the tag.
    auto self = ecs.component<Self>();
    test_assert(!self.has(Likes, Bob));

    int count = 0;
    auto q = ecs.query_builder<>().term(Likes, Bob).build();

    q.each([&](flecs::entity e) {
        test_assert(e.has(Likes, Bob));

        test_bool(e.get([&](const Self& s){
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    test_int(count, 3);
}

void Entity_with_self_w_name() {
    flecs::world ecs;

    auto Tier1 = ecs.entity("Tier1").with([&]{
        auto Tier2 = ecs.entity("Tier2");
        Tier2.set<Self>({Tier2});
    });

    auto Tier2 = ecs.lookup("Tier2");
    test_assert(Tier2 != 0);

    test_assert(Tier2.has(Tier1));
}

void Entity_with_self_nested() {
    flecs::world ecs;

    auto Tier1 = ecs.entity("Tier1").with([&]{
        ecs.entity("Tier2").with([&]{
            ecs.entity("Tier3");
        });
    });

    auto Tier2 = ecs.lookup("Tier2");
    test_assert(Tier2 != 0);

    auto Tier3 = ecs.lookup("Tier3");
    test_assert(Tier3 != 0);

    test_assert(Tier2.has(Tier1));
    test_assert(Tier3.has(Tier2));
}

void Entity_with_scope() {
    flecs::world ecs;

    auto parent = ecs.entity("P").scope([&]{
        auto e1 = ecs.entity("C1"); e1.set<Self>({e1});
        auto e2 = ecs.entity("C2"); e2.set<Self>({e2});
        auto e3 = ecs.entity("C3"); e3.set<Self>({e3});

        // Ensure relative lookups work
        test_assert(ecs.lookup("C1") == e1);
        test_assert(ecs.lookup("C2") == e2);
        test_assert(ecs.lookup("C3") == e3);

        test_assert(ecs.lookup("::P::C1") == e1);
        test_assert(ecs.lookup("::P::C2") == e2);
        test_assert(ecs.lookup("::P::C3") == e3);        
    });

    // Ensure entities are created in correct scope
    test_assert(ecs.lookup("C1") == 0);
    test_assert(ecs.lookup("C2") == 0);
    test_assert(ecs.lookup("C3") == 0);

    test_assert(parent.lookup("C1") != 0);
    test_assert(parent.lookup("C2") != 0);
    test_assert(parent.lookup("C3") != 0);

    test_assert(ecs.lookup("P::C1") == parent.lookup("C1"));
    test_assert(ecs.lookup("P::C2") == parent.lookup("C2"));
    test_assert(ecs.lookup("P::C3") == parent.lookup("C3"));

    // Ensures that while Self is (implicitly) registered within the with, it
    // does not become a child of the parent.
    auto self = ecs.component<Self>();
    test_assert(!self.has(flecs::ChildOf, parent));

    int count = 0;
    auto q = ecs.query_builder<>().term(flecs::ChildOf, parent).build();

    q.each([&](flecs::entity e) {
        test_assert(e.has(flecs::ChildOf, parent));

        test_bool(e.get([&](const Self& s){
            test_assert(s.value == e);
        }), true);

        count ++;
    });

    test_int(count, 3);
}

void Entity_with_scope_nested() {
    flecs::world ecs;

    auto parent = ecs.entity("P").scope([&]{
        auto child = ecs.entity("C").scope([&]{
            auto gchild = ecs.entity("GC");
            test_assert(gchild == ecs.lookup("GC"));
            test_assert(gchild == ecs.lookup("::P::C::GC"));
        });

        // Ensure relative lookups work
        test_assert(ecs.lookup("C") == child);
        test_assert(ecs.lookup("::P::C") == child);
        test_assert(ecs.lookup("::P::C::GC") != 0);
    });

    test_assert(0 == ecs.lookup("C"));
    test_assert(0 == ecs.lookup("GC"));
    test_assert(0 == ecs.lookup("C::GC"));

    auto child = ecs.lookup("P::C");
    test_assert(0 != child);
    test_assert(child.has(flecs::ChildOf, parent));

    auto gchild = ecs.lookup("P::C::GC");
    test_assert(0 != gchild);
    test_assert(gchild.has(flecs::ChildOf, child));
}

void Entity_with_scope_nested_same_name_as_parent() {
    flecs::world ecs;

    auto parent = ecs.entity("P").scope([&]{
        auto child = ecs.entity("C").scope([&]{
            auto gchild = ecs.entity("C");
            test_assert(gchild == ecs.lookup("C"));
            test_assert(gchild == ecs.lookup("::P::C::C"));
        });

        // Ensure relative lookups work
        test_assert(ecs.lookup("C") == child);
        test_assert(ecs.lookup("::P::C") == child);
        test_assert(ecs.lookup("::P::C::C") != 0);
    });

    test_assert(0 == ecs.lookup("C"));
    test_assert(0 == ecs.lookup("C"));
    test_assert(0 == ecs.lookup("C::C"));

    auto child = ecs.lookup("P::C");
    test_assert(0 != child);
    test_assert(child.has(flecs::ChildOf, parent));

    auto gchild = ecs.lookup("P::C::C");
    test_assert(0 != gchild);
    test_assert(gchild.has(flecs::ChildOf, child));
}

void Entity_no_recursive_lookup() {
    flecs::world ecs;

    auto p = ecs.entity("P");
    auto c = ecs.entity("C").child_of(p);
    auto gc = ecs.entity("GC").child_of(c);

    test_assert(c.lookup("GC") == gc);
    test_assert(c.lookup("C") == 0);
    test_assert(c.lookup("P") == 0);
}

void Entity_defer_new_w_name() {
    flecs::world ecs;

    flecs::entity e;

    ecs.defer([&]{
        e = ecs.entity("Foo");
        test_assert(e != 0);
        test_assert(0 == ecs.lookup("Foo"));
        test_assert(!e.has<flecs::Name>());
    });

    test_assert(e.has<flecs::Name>());
    test_str(e.name(), "Foo");
}

void Entity_defer_new_w_nested_name() {
    flecs::world ecs;

    flecs::entity e;

    ecs.defer([&]{
        e = ecs.entity("Foo::Bar");
        test_assert(e != 0);
        test_assert(0 == ecs.lookup("Foo"));
        test_assert(0 == ecs.lookup("Bar"));
        test_assert(!e.has<flecs::Name>());
    });

    test_assert(e.has<flecs::Name>());
    test_str(e.name(), "Bar");
    test_str(e.path(), "::Foo::Bar");
}


void Entity_defer_new_w_scope_name() {
    flecs::world ecs;

    flecs::entity e, parent = ecs.entity("Parent");

    ecs.defer([&]{
        parent.scope([&]{
            e = ecs.entity("Foo");
            test_assert(e != 0);
            test_assert(0 == ecs.lookup("Foo"));
            test_assert(!e.has<flecs::Name>());
        });
    });

    test_assert(e.has<flecs::Name>());
    test_str(e.name(), "Foo");
    test_str(e.path(), "::Parent::Foo");
}

void Entity_defer_new_w_scope_nested_name() {
    flecs::world ecs;

    flecs::entity e, parent = ecs.entity("Parent");

    ecs.defer([&]{
        parent.scope([&]{
            e = ecs.entity("Foo::Bar");
            test_assert(e != 0);
            test_assert(0 == ecs.lookup("Foo"));
            test_assert(0 == ecs.lookup("Bar"));
            test_assert(!e.has<flecs::Name>());
        });
    });

    test_assert(e.has<flecs::Name>());
    test_str(e.name(), "Bar");
    test_str(e.path(), "::Parent::Foo::Bar");
}

void Entity_defer_new_w_deferred_scope_nested_name() {
    flecs::world ecs;

    flecs::entity e, parent;

    ecs.defer([&]{
        parent = ecs.entity("Parent").scope([&]{
            e = ecs.entity("Foo::Bar");
            test_assert(e != 0);
            test_assert(0 == ecs.lookup("Foo"));
            test_assert(0 == ecs.lookup("Bar"));
            test_assert(!e.has<flecs::Name>());
        });

        test_assert(0 == ecs.lookup("Parent"));
        test_assert(0 == ecs.lookup("Foo"));
        test_assert(0 == ecs.lookup("Bar"));
        test_assert(!parent.has<flecs::Name>());
    });

    test_assert(parent.has<flecs::Name>());
    test_str(parent.name(), "Parent");
    test_str(parent.path(), "::Parent");

    test_assert(e.has<flecs::Name>());
    test_str(e.name(), "Bar");
    test_str(e.path(), "::Parent::Foo::Bar");
}

void Entity_defer_new_w_scope() {
    flecs::world ecs;

    flecs::entity e, parent = ecs.entity();

    ecs.defer([&]{
        parent.scope([&]{
            e = ecs.entity();
            test_assert(e != 0);
        });
    });

    test_assert(e.has(flecs::ChildOf, parent));
}

void Entity_defer_new_w_with() {
    flecs::world ecs;

    flecs::entity e, Tag = ecs.entity();

    ecs.defer([&]{
        Tag.with([&]{
            e = ecs.entity();
            test_assert(e != 0);
            test_assert(!e.has(Tag));
        });
    });

    test_assert(e.has(Tag));
}

void Entity_defer_new_w_name_scope_with() {
    flecs::world ecs;

    flecs::entity e, Tag = ecs.entity(), parent = ecs.entity("Parent");

    ecs.defer([&]{
        Tag.with([&]{
            parent.scope([&]{
                e = ecs.entity("Foo");
                test_assert(e != 0);
                test_assert(0 == ecs.lookup("Foo"));
                test_assert(!e.has(Tag));
                test_assert(!e.has<flecs::Name>());
            });
            test_assert(0 == ecs.lookup("Foo"));
            test_assert(!e.has(Tag));
            test_assert(!e.has<flecs::Name>());
        });
        test_assert(0 == ecs.lookup("Foo"));
        test_assert(!e.has(Tag));
        test_assert(!e.has<flecs::Name>());
    });

    test_assert(e.has(Tag));
    test_assert(e.has<flecs::Name>());
    test_str(e.name(), "Foo");
    test_str(e.path(), "::Parent::Foo");
}

void Entity_defer_new_w_nested_name_scope_with() {
    flecs::world ecs;

    flecs::entity e, Tag = ecs.entity(), parent = ecs.entity("Parent");

    ecs.defer([&]{
        Tag.with([&]{
            parent.scope([&]{
                e = ecs.entity("Foo::Bar");
                test_assert(e != 0);
                test_assert(0 == ecs.lookup("Foo"));
                test_assert(0 == ecs.lookup("Bar"));
                test_assert(!e.has(Tag));
                test_assert(!e.has<flecs::Name>());
            });
            test_assert(0 == ecs.lookup("Foo"));
            test_assert(!e.has(Tag));
            test_assert(!e.has<flecs::Name>());
        });
        test_assert(0 == ecs.lookup("Foo"));
        test_assert(!e.has(Tag));
        test_assert(!e.has<flecs::Name>());
    });

    test_assert(e.has(Tag));
    test_assert(e.has<flecs::Name>());
    test_str(e.name(), "Bar");
    test_str(e.path(), "::Parent::Foo::Bar");
}

void Entity_defer_w_with_implicit_component() {
    flecs::world ecs;

    struct Tag { };

    flecs::entity e;

    ecs.defer([&]{
        ecs.with<Tag>([&]{
            e = ecs.entity();
            test_assert(!e.has<Tag>());
        });
        test_assert(!e.has<Tag>());
    });

    test_assert(e.has<Tag>());
}

void Entity_with_after_builder_method() {
    flecs::world ecs;

    struct Likes { };

    auto A = ecs.entity()
        .set<Position>({10, 20})
        .with([&]{
            ecs.entity("X");
        });

    auto B = ecs.entity().set<Position>({30, 40})
        .with<Likes>([&]{
            ecs.entity("Y");
        }); 

    auto C = ecs.entity().set<Position>({50, 60})
        .with(flecs::IsA, [&]{
            ecs.entity("Z");
        });                

    test_assert(A.get([](const Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
    }));

    test_assert(B.get([](const Position& p) {
        test_int(p.x, 30);
        test_int(p.y, 40);
    }));

    test_assert(C.get([](const Position& p) {
        test_int(p.x, 50);
        test_int(p.y, 60);
    }));

    auto X = ecs.lookup("X");
    test_assert(X != 0);
    test_assert(X.has(A));

    auto Y = ecs.lookup("Y");
    test_assert(Y != 0);
    test_assert(Y.has<Likes>(B));

    auto Z = ecs.lookup("Z");
    test_assert(Z != 0);
    test_assert(Z.has(flecs::IsA, C));
}

void Entity_with_before_builder_method() {
    flecs::world ecs;

    struct Likes { };

    auto A = ecs.entity()
        .with([&]{
            ecs.entity("X");
        })
        .set<Position>({10, 20});

    auto B = ecs.entity().with<Likes>([&]{
            ecs.entity("Y");
        })
        .set<Position>({30, 40});

    auto C = ecs.entity().with(flecs::IsA, [&]{
            ecs.entity("Z");
        })
        .set<Position>({50, 60});          

    test_assert(A.get([](const Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
    }));

    test_assert(B.get([](const Position& p) {
        test_int(p.x, 30);
        test_int(p.y, 40);
    }));

    test_assert(C.get([](const Position& p) {
        test_int(p.x, 50);
        test_int(p.y, 60);
    }));

    auto X = ecs.lookup("X");
    test_assert(X != 0);
    test_assert(X.has(A));

    auto Y = ecs.lookup("Y");
    test_assert(Y != 0);
    test_assert(Y.has<Likes>(B));

    auto Z = ecs.lookup("Z");
    test_assert(Z != 0);
    test_assert(Z.has(flecs::IsA, C));
}

void Entity_scope_after_builder_method() {
    flecs::world ecs;

    ecs.entity("P")
        .set<Position>({10, 20})
        .scope([&]{
            ecs.entity("C");
        });

    auto C = ecs.lookup("P::C");
    test_assert(C != 0);
}

void Entity_scope_before_builder_method() {
    flecs::world ecs;

    ecs.entity("P")
        .scope([&]{
            ecs.entity("C");
        })
        .set<Position>({10, 20});

    auto C = ecs.lookup("P::C");
    test_assert(C != 0);
}

