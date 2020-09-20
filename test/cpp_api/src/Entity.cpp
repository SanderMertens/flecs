#include <cpp_api.h>

void Entity_new() {
    flecs::world world;

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);
}

void Entity_new_named() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity.id() != 0);
    test_str(entity.name().c_str(), "Foo");
}

void Entity_new_add() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world)
        .add<Position>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
}

void Entity_new_add_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world)
        .add<Position>()
        .add<Velocity>();

    test_assert(entity.id() != 0);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_new_set() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world)
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

    auto entity = flecs::entity(world)
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

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());
}

void Entity_remove() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add<Position>();
    test_assert(entity.has<Position>());

    entity.remove<Position>();
    test_assert(!entity.has<Position>());
}

void Entity_set() {
    flecs::world world;

    flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world);
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

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.patch<Position>([](Position& p, bool exists) {
        test_assert(!exists);
        p.x = 10;
        p.y = 20;
    });

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    entity.patch<Position>([](Position& p_arg, bool exists) {
        test_assert(exists);
        p_arg.x = 30;
    });

    test_int(p->x, 30); 
}

void Entity_add_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add<Position>()
          .add<Velocity>();

    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_add_entity() {
    flecs::world world;

    auto tag = flecs::entity(world);
    test_assert(tag.id() != 0);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(tag);
    test_assert(entity.has(tag));
}

void Entity_add_childof() {
    flecs::world world;

    auto parent = flecs::entity(world);
    test_assert(parent.id() != 0);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add_childof(parent);
    test_assert(entity.has(flecs::Childof | parent.id()));
}

void Entity_add_instanceof() {
    flecs::world world;

    auto base = flecs::entity(world);
    test_assert(base.id() != 0);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add_instanceof(base);
    test_assert(entity.has(flecs::Instanceof | base.id()));
}

void Entity_remove_2() {
    flecs::world world;

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto entity = flecs::entity(world);
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

    auto entity = flecs::entity(world);
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

    auto tag = flecs::entity(world);
    test_assert(tag.id() != 0);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add(tag);
    test_assert(entity.has(tag));

    entity.remove(tag);
    test_assert(!entity.has(tag));
}

void Entity_remove_childof() {
    flecs::world world;

    auto parent = flecs::entity(world);
    test_assert(parent.id() != 0);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add_childof(parent);
    test_assert(entity.has(flecs::Childof | parent.id()));

    entity.remove_childof(parent);
    test_assert(!entity.has(flecs::Childof | parent.id()));
}

void Entity_remove_instanceof() {
    flecs::world world;

    auto base = flecs::entity(world);
    test_assert(base.id() != 0);

    auto entity = flecs::entity(world);
    test_assert(entity.id() != 0);

    entity.add_instanceof(base);
    test_assert(entity.has(flecs::Instanceof | base.id()));
    
    entity.remove_instanceof(base);
    test_assert(!entity.has(flecs::Instanceof | base.id()));
}

void Entity_get_generic() {
    flecs::world world;

    auto position = flecs::component<Position>(world, "Position");

    auto entity = flecs::entity(world)
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

    auto entity = flecs::entity(world)
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

    auto entity = flecs::entity(world)
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

    auto entity = flecs::entity(world)
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

    auto entity = flecs::entity(world);

    entity = entity.add_role(flecs::Trait);

    test_assert(entity.id() & ECS_TRAIT);
}

void Entity_remove_role() {
    flecs::world world;

    auto entity = flecs::entity(world);

    flecs::entity_t id = entity.id();

    entity = entity.add_role(flecs::Trait);

    test_assert(entity.id() & ECS_TRAIT);

    entity = entity.remove_role();

    test_assert(entity.id() == id);
}

void Entity_has_role() {
    flecs::world world;

    auto entity = flecs::entity(world);

    entity = entity.add_role(flecs::Trait);

    test_assert(entity.has_role(flecs::Trait));

    entity = entity.remove_role();

    test_assert(!entity.has_role(flecs::Trait));
}

void Entity_trait_role() {
    flecs::world world;

    auto a = flecs::entity(world);
    auto b = flecs::entity(world);

    auto comb = flecs::entity::comb(a, b);
    comb = comb.add_role(flecs::Trait);
    
    test_assert(comb.has_role(flecs::Trait));

    auto lo = comb.lo();
    auto hi = comb.remove_role().hi();

    test_assert(lo == a);
    test_assert(hi == b);
}

void Entity_equals() {
    flecs::world world;

    auto e1 = flecs::entity(world);
    auto e2 = flecs::entity(world);

    auto id1 = flecs::entity(world, e1.id());
    auto id2 = flecs::entity(world, e2.id());

    test_assert(e1 == id1);
    test_assert(e2 == id2);
    test_assert(e1 != e2);
}

void Entity_has_childof() {
    flecs::world world;

    auto parent = flecs::entity(world);

    auto e = flecs::entity(world)
        .add_childof(parent);

    test_assert(e.has_childof(parent));
}

void Entity_has_instanceof() {
    flecs::world world;

    auto base = flecs::entity(world);

    auto e = flecs::entity(world)
        .add_instanceof(base);

    test_assert(e.has_instanceof(base));
}

void Entity_has_instanceof_indirect() {
    flecs::world world;

    auto base_of_base = flecs::entity(world);

    auto base = flecs::entity(world)
        .add_instanceof(base_of_base);

    auto e = flecs::entity(world)
        .add_instanceof(base);

    test_assert(e.has_instanceof(base_of_base));
}

void Entity_null_string() {
    flecs::world world;

    auto e = flecs::entity(world);

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

    test_assert(!e.has<Position>());
    test_assert(!e.has<Velocity>());

    auto e2 = world.entity();
    test_assert(e2.id() == e.id());
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
        .add_instanceof(prefab);
    
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
        .add_instanceof(prefab);
    
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
        .add_instanceof(prefab);

    auto e = world.entity()
        .add_instanceof(prefab_2);
    
    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());
    test_assert(e.has<Velocity>());
    test_assert(!e.owns<Velocity>());
}

struct MyTag { };

void Entity_tag_has_size_zero() {
    flecs::world world;

    auto comp = world.component<MyTag>();

    auto ptr = comp.get<flecs::Component>();
    test_int(ptr->size, 0);
    test_int(ptr->alignment, 0);
}
