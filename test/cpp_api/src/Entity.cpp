#include <cpp_api.h>

void Entity_new() {
    flecs::world world;

    auto entity = world.entity();
    test_assert(entity);
}

void Entity_new_named() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity);
    test_str(entity.name().c_str(), "Foo");
}

void Entity_new_named_from_scope() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity);
    test_str(entity.name().c_str(), "Foo");

    auto prev = world.set_scope(entity);

    auto child = world.entity("Bar");
    test_assert(child != 0);

    world.set_scope(prev);

    test_str(child.name().c_str(), "Bar");
    test_str(child.path().c_str(), "::Foo::Bar");
}

void Entity_new_nested_named_from_scope() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity);
    test_str(entity.name().c_str(), "Foo");

    auto prev = world.set_scope(entity);

    auto child = world.entity("Bar::Hello");
    test_assert(child != 0);

    world.set_scope(prev);

    test_str(child.name().c_str(), "Hello");
    test_str(child.path().c_str(), "::Foo::Bar::Hello");
}

void Entity_new_nested_named_from_nested_scope() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo::Bar");
    test_assert(entity);
    test_str(entity.name().c_str(), "Bar");
    test_str(entity.path().c_str(), "::Foo::Bar");

    auto prev = world.set_scope(entity);

    auto child = world.entity("Hello::World");
    test_assert(child != 0);

    world.set_scope(prev);

    test_str(child.name().c_str(), "World");
    test_str(child.path().c_str(), "::Foo::Bar::Hello::World");
}

void Entity_new_add() {
    flecs::world world;

    world.component<Position>();

    auto entity = world.entity()
        .add<Position>();

    test_assert(entity);
    test_assert(entity.has<Position>());
}

void Entity_new_add_2() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto entity = world.entity()
        .add<Position>()
        .add<Velocity>();

    test_assert(entity);
    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_new_set() {
    flecs::world world;

    world.component<Position>();

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity);
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

    test_assert(entity);
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
    test_assert(entity);

    entity.add<Position>();
    test_assert(entity.has<Position>());
}

void Entity_remove() {
    flecs::world world;

    world.component<Position>();

    auto entity = world.entity();
    test_assert(entity);

    entity.add<Position>();
    test_assert(entity.has<Position>());

    entity.remove<Position>();
    test_assert(!entity.has<Position>());
}

void Entity_set() {
    flecs::world world;

    world.component<Position>();

    auto entity = world.entity();
    test_assert(entity);

    entity.set<Position>({10, 20});
    test_assert(entity.has<Position>());

    const Position *p = entity.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_emplace_2() {
    flecs::world ecs;

    auto e = ecs.entity()
        .emplace<Position>(10.0f, 20.0f)
        .emplace<Velocity>(30.0f, 40.0f);

    test_assert(e.has<Position>());
    test_assert(e.has<Velocity>());

    const Position *p = e.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = e.get<Velocity>();
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);
}

void Entity_emplace_after_add() {
    flecs::world ecs;

    auto e = ecs.entity()
        .add<Position>()
        .emplace<Velocity>(30.0f, 40.0f);

    test_assert(e.has<Position>());
    test_assert(e.has<Velocity>());

    const Velocity *v = e.get<Velocity>();
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);
}

void Entity_emplace_after_add_pair() {
    flecs::world ecs;

    auto dummy = ecs.entity();

    auto e = ecs.entity()
        .add(flecs::ChildOf, dummy)
        .emplace<Velocity>(30.0f, 40.0f);

    test_assert(e.has(flecs::ChildOf, dummy));
    test_assert(e.has<Velocity>());

    const Velocity *v = e.get<Velocity>();
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);
}

class SelfCtor {
public:
    SelfCtor(flecs::entity e, int x, int y) : e_(e), x_(x), y_(y) { }

    flecs::entity e_;
    int x_;
    int y_;
};

void Entity_emplace_w_self_ctor() {
    flecs::world ecs;

    auto e = ecs.entity()
        .emplace<SelfCtor>(10, 20);

    const SelfCtor *ptr = e.get<SelfCtor>();
    test_assert(ptr != NULL);
    test_int(ptr->x_, 10);
    test_int(ptr->y_, 20);
    test_assert(ptr->e_ == e);
}

void Entity_add_2() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto entity = world.entity();
    test_assert(entity);

    entity.add<Position>()
          .add<Velocity>();

    test_assert(entity.has<Position>());
    test_assert(entity.has<Velocity>());
}

void Entity_add_entity() {
    flecs::world world;

    auto tag = world.entity();
    test_assert(tag != 0);

    auto entity = world.entity();
    test_assert(entity);

    entity.add(tag);
    test_assert(entity.has(tag));
}

void Entity_add_childof() {
    flecs::world world;

    auto parent = world.entity();
    test_assert(parent != 0);

    auto entity = world.entity();
    test_assert(entity);

    entity.add(flecs::ChildOf, parent);
    test_assert(entity.has(flecs::ChildOf, parent));
}

void Entity_add_instanceof() {
    flecs::world world;

    auto base = world.entity();
    test_assert(base != 0);

    auto entity = world.entity();
    test_assert(entity);

    entity.add(flecs::IsA, base);
    test_assert(entity.has(flecs::IsA, base));
}

void Entity_remove_2() {
    flecs::world world;

    world.component<Position>();
    world.component<Velocity>();

    auto entity = world.entity();
    test_assert(entity);

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
    test_assert(entity);

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
    test_assert(tag != 0);

    auto entity = world.entity();
    test_assert(entity);

    entity.add(tag);
    test_assert(entity.has(tag));

    entity.remove(tag);
    test_assert(!entity.has(tag));
}

void Entity_remove_childof() {
    flecs::world world;

    auto parent = world.entity();
    test_assert(parent != 0);

    auto entity = world.entity();
    test_assert(entity);

    entity.add(flecs::ChildOf, parent);
    test_assert(entity.has(flecs::ChildOf, parent));

    entity.remove(flecs::ChildOf, parent);
    test_assert(!entity.has(flecs::ChildOf, parent));
}

void Entity_remove_instanceof() {
    flecs::world world;

    auto base = world.entity();
    test_assert(base != 0);

    auto entity = world.entity();
    test_assert(entity);

    entity.add(flecs::IsA, base);
    test_assert(entity.has(flecs::IsA, base));
    
    entity.remove(flecs::IsA, base);
    test_assert(!entity.has(flecs::IsA, base));
}

void Entity_get_generic() {
    flecs::world world;

    auto position = world.component<Position>();

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity);
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

    test_assert(entity);
    test_assert(entity.has<Position>());

    bool invoked;
    world.observer<Position>()
        .event(flecs::OnSet)
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
    flecs::id id = position;

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity);
    test_assert(entity.has<Position>());

    const void *void_p = entity.get(id);
    test_assert(void_p != nullptr);

    const Position *p = static_cast<const Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_get_generic_w_id_t() {
    flecs::world world;

    auto position = world.component<Position>();
    flecs::id_t id = position;

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity);
    test_assert(entity.has<Position>());

    const void *void_p = entity.get(id);
    test_assert(void_p != nullptr);

    const Position *p = static_cast<const Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_get_mut_generic_w_id() {
    flecs::world world;

    auto position = world.component<Position>();
    flecs::id id = position;

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity);
    test_assert(entity.has<Position>());

    bool invoked;
    world.observer<Position>()
        .event(flecs::OnSet)
        .each([&invoked](flecs::entity e, Position& p) {
            invoked = true;
        });

    void *void_p = entity.get_mut(id);
    test_assert(void_p != nullptr);

    Position *p = static_cast<Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);

    entity.modified(id);
    test_bool(invoked, true);
}

void Entity_get_mut_generic_w_id_t() {
    flecs::world world;

    auto position = world.component<Position>();
    flecs::id_t id = position;

    auto entity = world.entity()
        .set<Position>({10, 20});

    test_assert(entity);
    test_assert(entity.has<Position>());

    bool invoked;
    world.observer<Position>()
        .event(flecs::OnSet)
        .each([&invoked](flecs::entity e, Position& p) {
            invoked = true;
        });

    void *void_p = entity.get_mut(id);
    test_assert(void_p != nullptr);

    Position *p = static_cast<Position*>(void_p);
    test_int(p->x, 10);
    test_int(p->y, 20);

    entity.modified(id);
    test_bool(invoked, true);
}

void Entity_set_generic() {
    flecs::world world;

    auto position = world.component<Position>();

    Position p = {10, 20};

    auto e = world.entity()
        .set_ptr(position, sizeof(Position), &p);

    test_assert(e.has<Position>());
    test_assert(e.has(position));

    const Position *ptr = e.get<Position>();
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_set_generic_w_id() {
    flecs::world world;

    auto position = world.component<Position>();
    flecs::id id = position;

    Position p = {10, 20};

    auto e = world.entity()
        .set_ptr(id, sizeof(Position), &p);

    test_assert(e.has<Position>());
    test_assert(e.has(id));

    const Position *ptr = e.get<Position>();
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_set_generic_w_id_t() {
    flecs::world world;

    auto position = world.component<Position>();
    flecs::id_t id = position;

    Position p = {10, 20};

    auto e = world.entity()
        .set_ptr(id, sizeof(Position), &p);

    test_assert(e.has<Position>());
    test_assert(e.has(id));

    const Position *ptr = e.get<Position>();
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_set_generic_no_size() {
    flecs::world world;

    auto position = world.component<Position>();

    Position p = {10, 20};

    auto e = world.entity()
        .set_ptr(position, &p);

    test_assert(e.has<Position>());
    test_assert(e.has(position));

    const Position *ptr = e.get<Position>();
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_set_generic_no_size_w_id() {
    flecs::world world;

    auto position = world.component<Position>();
    flecs::id id = position;

    Position p = {10, 20};

    auto e = world.entity()
        .set_ptr(id, &p);

    test_assert(e.has<Position>());
    test_assert(e.has(id));

    const Position *ptr = e.get<Position>();
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_set_generic_no_size_w_id_t() {
    flecs::world world;

    auto position = world.component<Position>();
    flecs::id_t id = position;

    Position p = {10, 20};

    auto e = world.entity()
        .set_ptr(id, &p);

    test_assert(e.has<Position>());
    test_assert(e.has(id));

    const Position *ptr = e.get<Position>();
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_add_role() {
    flecs::world world;

    auto entity = world.entity();

    entity = entity.add_role(flecs::Pair);

    test_assert(entity & ECS_PAIR);
}

void Entity_remove_role() {
    flecs::world world;

    auto entity = world.entity();

    flecs::entity_t id = entity;

    entity = entity.add_role(flecs::Pair);

    test_assert(entity & ECS_PAIR);

    entity = entity.remove_role();

    test_assert(entity == id);
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

    auto pair = flecs::id(a, b);
    pair = pair.add_role(flecs::Pair);
    
    test_assert(pair.has_role(flecs::Pair));

    auto rel = pair.relation();
    auto obj = pair.object();

    test_assert(rel == a);
    test_assert(obj == b);
}

void Entity_equals() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();

    auto e1_2 = world.entity(e1);
    auto e2_2 = world.entity(e2);

    test_assert(e1 == e1_2);
    test_assert(e2 == e2_2);
    test_assert(e1 >= e1_2);
    test_assert(e1 <= e1_2);
    test_assert(e2 >= e2_2);
    test_assert(e2 <= e2_2);
    test_assert(e1 != e2);

    test_assert(!(e2 == e1_2));
    test_assert(!(e1 == e2_2));
    test_assert(!(e2 <= e1_2));
    test_assert(!(e1 >= e2_2));
    test_assert(!(e2 != e2));
}

void Entity_compare_0() {
    flecs::world world;

    auto e = world.entity();
    auto e0 = world.entity(0);
    auto e0_2 = world.entity(0);

    test_assert(e != e0);
    test_assert(e > e0);
    test_assert(e >= e0);
    test_assert(e0 < e);
    test_assert(e0 <= e);

    test_assert(e0 == e0_2);
    test_assert(e0 >= e0_2);
    test_assert(e0 <= e0_2);
}

void Entity_compare_id_t() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();

    flecs::id_t id1 = e1;
    flecs::id_t id2 = e2;

    test_assert(e1 == id1);
    test_assert(e2 == id2);

    test_assert(e1 != id2);
    test_assert(e2 != id1);

    test_assert(e1 >= id1);
    test_assert(e2 >= id2);

    test_assert(e1 <= id1);
    test_assert(e2 <= id2);

    test_assert(e1 <= id2);
    test_assert(e2 >= id1);

    test_assert(e1 < id2);
    test_assert(e2 > id1); 
    

    test_assert(!(e2 == id1));
    test_assert(!(e1 == id2));

    test_assert(!(e2 != id2));
    test_assert(!(e1 != id1));

    test_assert(!(e1 >= id2));
    test_assert(!(e2 <= id1));

    test_assert(!(e2 < id2));
    test_assert(!(e1 > id1)); 
}

void Entity_compare_id() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();

    flecs::id id1 = e1;
    flecs::id id2 = e2;

    test_assert(e1 == id1);
    test_assert(e2 == id2);

    test_assert(e1 != id2);
    test_assert(e2 != id1);

    test_assert(e1 >= id1);
    test_assert(e2 >= id2);

    test_assert(e1 <= id1);
    test_assert(e2 <= id2);

    test_assert(e1 <= id2);
    test_assert(e2 >= id1);

    test_assert(e1 < id2);
    test_assert(e2 > id1); 
    

    test_assert(!(e2 == id1));
    test_assert(!(e1 == id2));

    test_assert(!(e2 != id2));
    test_assert(!(e1 != id1));

    test_assert(!(e1 >= id2));
    test_assert(!(e2 <= id1));

    test_assert(!(e2 < id2));
    test_assert(!(e1 > id1)); 
}

void Entity_compare_literal() {
    flecs::world world;

    auto e1 = world.entity(500);
    auto e2 = world.entity(600);

    test_assert(e1 == 500);
    test_assert(e2 == 600);

    test_assert(e1 != 600);
    test_assert(e2 != 500);

    test_assert(e1 >= 500);
    test_assert(e2 >= 600);

    test_assert(e1 <= 500);
    test_assert(e2 <= 600);

    test_assert(e1 <= 600);
    test_assert(e2 >= 500);

    test_assert(e1 < 600);
    test_assert(e2 > 500); 
    

    test_assert(!(e2 == 500));
    test_assert(!(e1 == 600));

    test_assert(!(e2 != 600));
    test_assert(!(e1 != 500));

    test_assert(!(e1 >= 600));
    test_assert(!(e2 <= 500));

    test_assert(!(e2 < 600));
    test_assert(!(e1 > 500));
}

void Entity_greater_than() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();

    test_assert(e2 > e1);
    test_assert(e2 >= e1);
}

void Entity_less_than() {
    flecs::world world;

    auto e1 = world.entity();
    auto e2 = world.entity();

    test_assert(e1 < e2);
    test_assert(e1 <= e2);
}

void Entity_not_0_or_1() {
    flecs::world world;

    auto e = world.entity();

    flecs::id_t id = e;

    test_assert(id != 0);
    test_assert(id != 1);
}

void Entity_not_true_or_false() {
    flecs::world world;

    auto e = world.entity();

    flecs::id_t id = e;

    test_assert(id != true);
    test_assert(id != false);
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

    e.set_name("Foo");
    test_str(e.name().c_str(), "Foo");
}

void Entity_change_name() {
    flecs::world world;

    auto e = world.entity("Bar");
    test_str(e.name().c_str(), "Bar");

    e.set_name("Foo");
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
    test_assert((uint32_t)e2 == (uint32_t)e);
    test_assert(e2 != e);
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
    test_assert(e2 > e);
}

void Entity_foce_owned() {
    flecs::world world;

    auto prefab = world.prefab()
        .add<Position>()
        .add<Velocity>()
        .override<Position>();

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
        .override<Position>()
        .override<Velocity>();

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
        .override<Position>();

    auto prefab_2 = world.prefab()
        .add(flecs::IsA, prefab);

    auto e = world.entity()
        .add(flecs::IsA, prefab_2);
    
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

void Entity_get_null_name() {
    flecs::world world;

    auto e = world.entity().set_name(nullptr);

    auto n = e.name();
    test_assert(n.size() == 0);
}

void Entity_get_parent() {
    flecs::world world;

    auto Rel = world.entity();

    auto obj1 = world.entity()
        .add<Position>();

    auto obj2 = world.entity()
        .add<Velocity>();

    auto obj3 = world.entity()
        .add<Mass>();

    auto child = world.entity()
        .add(Rel, obj1)
        .add(Rel, obj2)
        .add(Rel, obj3);

    auto p = child.get_object(Rel);
    test_assert(p != 0);
    test_assert(p == obj1);

    p = child.get_object(Rel, 0);
    test_assert(p != 0);
    test_assert(p == obj1);

    p = child.get_object(Rel, 1);
    test_assert(p != 0);
    test_assert(p == obj2);

    p = child.get_object(Rel, 2);
    test_assert(p != 0);
    test_assert(p == obj3);

    p = child.get_object(Rel, 3);
    test_assert(p == 0);
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
    test_assert(entity);

    auto type_1 = entity.type();
    test_int(type_1.count(), 0);

    auto type_2 = entity.type();
    test_int(type_2.count(), 0);
}

#include <stdio.h>

void Entity_get_nonempty_type() {
    flecs::world world;

    auto entity = world.entity()
        .add<Position>();
    test_assert(entity);

    auto type_1 = entity.type();
    test_int(type_1.count(), 1);
    test_int(type_1.get(0), world.id<Position>());

    auto type_2 = entity.type();
    test_int(type_2.count(), 1);
    test_int(type_2.get(0), world.id<Position>());
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

void Entity_override() {
    flecs::world world;

    auto base = world.entity()
        .override<Position>();

    auto e = world.entity()
        .add(flecs::IsA, base);

    test_assert(e.has<Position>());
    test_assert(e.owns<Position>());
}

void Entity_set_override() {
    flecs::world world;

    auto base = world.entity()
        .set_override<Position>({10, 20});

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

void Entity_set_override_lvalue() {
    flecs::world world;

    Position plvalue = {10, 20};

    auto base = world.entity()
        .set_override<Position>(plvalue);

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

void Entity_emplace_override() {
    flecs::world world;

    auto e = world.entity().emplace_override<NoDefaultCtor>(10);
    test_assert(e.has<NoDefaultCtor>());

    const NoDefaultCtor *ptr = e.get<NoDefaultCtor>();
    test_assert(ptr != nullptr);
    test_int(ptr->x_, 10);
}

void Entity_implicit_name_to_char() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity);
    test_str(entity.name().c_str(), "Foo");

    test_str(entity.name(), "Foo");
}

void Entity_implicit_path_to_char() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo::Bar");
    test_assert(entity);
    test_str(entity.name().c_str(), "Bar");

    test_str(entity.path(), "::Foo::Bar");
}

void Entity_implicit_type_str_to_char() {
    flecs::world world;

    auto entity = flecs::entity(world, "Foo");
    test_assert(entity);

    test_str(entity.type().str(), "(Identifier,Name)");
}

void Entity_entity_to_entity_view() {
    flecs::world world;

    flecs::entity e = world.entity()
        .set<Position>({10, 20});
    test_assert(e != 0);

    flecs::entity_view ev = e;
    test_assert(ev != 0);
    test_assert(e == ev);

    const Position *p = ev.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_entity_view_to_entity_world() {
    flecs::world world;

    flecs::entity e = world.entity()
        .set<Position>({10, 20});
    test_assert(e != 0);

    flecs::entity_view ev = e;
    test_assert(ev != 0);
    test_assert(e == ev);

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

    world.readonly_begin();

    flecs::entity ew = ev.mut(stage);

    ew.set<Position>({10, 20});
    test_assert(!ew.has<Position>());

    world.readonly_end();

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

    world.readonly_begin();

    flecs::entity_view ev = stage.entity();
    test_assert(ev != 0);

    world.readonly_end();

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

void Entity_get_mut_1_component_w_callback() {
    flecs::world ecs;

    auto e_1 = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto e_2 = ecs.entity()
        .set<Position>({11, 22});

    auto e_3 = ecs.entity()
        .set<Velocity>({1, 2});

    test_bool(e_1.get([](Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);
        p.x ++;
        p.y += 2;
    }), true);

    test_bool(e_2.get([](Position& p) {
        test_int(p.x, 11);
        test_int(p.y, 22);
        p.x ++;
        p.y += 2;
    }), true);

    const Position* 
    p = e_1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = e_2.get<Position>();
    test_int(p->x, 12);
    test_int(p->y, 24);

    test_bool(e_3.get([](const Position& p) {}), false);
}

void Entity_get_mut_2_components_w_callback() {
    flecs::world ecs;

    auto e_1 = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto e_2 = ecs.entity()
        .set<Position>({11, 22});

    auto e_3 = ecs.entity()
        .set<Velocity>({1, 2});

    test_bool(e_1.get([](Position& p, Velocity& v) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_int(v.x, 1);
        test_int(v.y, 2);

        p.x ++;
        p.y += 2;
        v.x += 3;
        v.y += 4;
    }), true);

    test_bool(e_2.get([](const Position& p, const Velocity& v) {}), false);

    test_bool(e_3.get([](const Position& p, const Velocity& v) {}), false);

    const Position* p = e_1.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);

    const Velocity* v = e_1.get<Velocity>();
    test_int(v->x, 4);
    test_int(v->y, 6);
}

void Entity_get_component_w_callback_nested() {
    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    test_bool(e.get([&](const Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_bool(e.get([](const Velocity& v) {
            test_int(v.x, 1);
            test_int(v.y, 2);
        }), true);
    }), true);
}

void Entity_get_mut_component_w_callback_nested() {
    install_test_abort();

    flecs::world ecs;

    auto e = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    test_bool(e.get([&](Position& p) {
        test_int(p.x, 10);
        test_int(p.y, 20);

        test_expect_abort();
        test_bool(e.get([](Velocity& v) {
        }), false);
    }), true);
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

    ecs.observer<Position>()
        .event(flecs::OnSet)
        .each([&](flecs::entity e, Position& p) {
            position_set ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
        });

    ecs.observer<Velocity>()
        .event(flecs::OnSet)
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

    ecs.observer<Position>()
        .event(flecs::OnSet)
        .each([&](flecs::entity e, Position& p) {
            position_set ++;
            test_int(p.x, 10);
            test_int(p.y, 20);
        });

    ecs.observer<Velocity>()
        .event(flecs::OnSet)
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
    });

    test_assert(e.has<flecs::Identifier>(flecs::Name));
    test_str(e.name(), "Foo");
}

void Entity_defer_new_w_nested_name() {
    flecs::world ecs;

    flecs::entity e;

    ecs.defer([&]{
        e = ecs.entity("Foo::Bar");
        test_assert(e != 0);
    });

    test_assert(e.has<flecs::Identifier>(flecs::Name));
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
        });
    });

    test_assert(e.has<flecs::Identifier>(flecs::Name));
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
        });
    });

    test_assert(e.has<flecs::Identifier>(flecs::Name));
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
        });
    });

    test_assert(parent.has<flecs::Identifier>(flecs::Name));
    test_str(parent.name(), "Parent");
    test_str(parent.path(), "::Parent");

    test_assert(e.has<flecs::Identifier>(flecs::Name));
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
                test_assert(!e.has(Tag));
            });
            test_assert(!e.has(Tag));
        });
        test_assert(!e.has(Tag));
    });

    test_assert(e.has(Tag));
    test_assert(e.has<flecs::Identifier>(flecs::Name));
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
                test_assert(!e.has(Tag));
            });
            test_assert(!e.has(Tag));
        });
        test_assert(!e.has(Tag));
    });

    test_assert(e.has(Tag));
    test_assert(e.has<flecs::Identifier>(flecs::Name));
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

void Entity_defer_suspend_resume() {
    flecs::world ecs;

    struct TagA { };
    struct TagB { };

    flecs::entity e = ecs.entity();

    ecs.defer([&]{
        e.add<TagA>();
        test_assert(!e.has<TagA>());

        ecs.defer_suspend();
        e.add<TagB>();
        test_assert(!e.has<TagA>());
        test_assert(e.has<TagB>());
        ecs.defer_resume();

        test_assert(!e.has<TagA>());
        test_assert(e.has<TagB>());
    });

    test_assert(e.has<TagA>());
    test_assert(e.has<TagB>());
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

void Entity_emplace() {
    flecs::world ecs;

    auto e = ecs.entity()
        .emplace<Position>(10.0f, 20.0f);

    test_assert(e.has<Position>());

    const Position *p = e.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_entity_id_str() {
    flecs::world ecs;

    flecs::id id = ecs.entity("Foo");

    test_str("Foo", id.str());
}

void Entity_pair_id_str() {
    flecs::world ecs;

    flecs::id id = ecs.pair( ecs.entity("Rel"), ecs.entity("Obj") );

    test_str("(Rel,Obj)", id.str());
}

void Entity_role_id_str() {
    flecs::world ecs;

    flecs::id id = flecs::id(ecs, ECS_OVERRIDE | ecs.entity("Foo"));

    test_str("OVERRIDE|Foo", id.str());
}

void Entity_id_str_from_entity_view() {
    flecs::world ecs;

    flecs::entity_view id = ecs.entity("Foo");

    test_str("Foo", id.str());
}

void Entity_id_str_from_entity() {
    flecs::world ecs;

    flecs::entity id = ecs.entity("Foo");

    test_str("Foo", id.str());
}

void Entity_null_entity() {
    flecs::entity e = flecs::entity::null();
    test_assert(e.id() == 0);
}

void Entity_null_entity_w_world() {
    flecs::world ecs;

    flecs::entity e = flecs::entity::null(ecs);
    test_assert(e.id() == 0);
    test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_null_entity_w_0() {
    flecs::entity e = flecs::entity(static_cast<flecs::id_t>(0));
    test_assert(e.id() == 0);
    test_assert(e.world().c_ptr() == nullptr);
}

void Entity_null_entity_w_world_w_0() {
    flecs::world ecs;

    flecs::entity e = flecs::entity::null(ecs);
    test_assert(e.id() == 0);
    test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_entity_view_null_entity() {
    flecs::entity_view e = flecs::entity::null();
    test_assert(e.id() == 0);
}

void Entity_entity_view_null_entity_w_world() {
    flecs::world ecs;

    flecs::entity_view e = flecs::entity::null(ecs);
    test_assert(e.id() == 0);
    test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_entity_view_null_entity_w_0() {
    flecs::entity_view e = flecs::entity(static_cast<flecs::id_t>(0));
    test_assert(e.id() == 0);
    test_assert(e.world().c_ptr() == nullptr);
}

void Entity_entity_view_null_entity_w_world_w_0() {
    flecs::world ecs;

    flecs::entity_view e = flecs::entity::null(ecs);
    test_assert(e.id() == 0);
    test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_is_wildcard() {
    flecs::world ecs;

    auto e1 = ecs.entity();
    auto e2 = ecs.entity();

    auto p0 = e1;
    auto p1 = ecs.pair(e1, e2);
    auto p2 = ecs.pair(e1, flecs::Wildcard);
    auto p3 = ecs.pair(flecs::Wildcard, e2);
    auto p4 = ecs.pair(flecs::Wildcard, flecs::Wildcard);

    test_bool(e1.is_wildcard(), false);
    test_bool(e2.is_wildcard(), false);
    test_bool(p0.is_wildcard(), false);
    test_bool(p1.is_wildcard(), false);
    test_bool(p2.is_wildcard(), true);
    test_bool(p3.is_wildcard(), true);
    test_bool(p4.is_wildcard(), true);   
}

void Entity_has_id_t() {
    flecs::world ecs;

    flecs::id_t id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id_t id_2 = ecs.entity();
    test_assert(id_2 != 0);    

    auto e = ecs.entity()
        .add(id_1);

    test_assert(e != 0);
    test_bool(e.has(id_1), true);
    test_bool(e.has(id_2), false);
}

void Entity_has_pair_id_t() {
    flecs::world ecs;

    flecs::id_t id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id_t id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id_t id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add(id_1, id_2);

    test_assert(e != 0);
    test_bool(e.has(id_1, id_2), true);
    test_bool(e.has(id_1, id_3), false);
}

void Entity_has_pair_id_t_w_type() {
    flecs::world ecs;

    struct Rel { };

    flecs::id_t id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id_t id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add<Rel>(id_2);

    test_assert(e != 0);
    test_bool(e.has<Rel>(id_2), true);
    test_bool(e.has<Rel>(id_3), false);
}

void Entity_has_id() {
    flecs::world ecs;

    flecs::id id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id id_2 = ecs.entity();
    test_assert(id_2 != 0);    

    auto e = ecs.entity()
        .add(id_1);

    test_assert(e != 0);
    test_bool(e.has(id_1), true);
    test_bool(e.has(id_2), false);
}

void Entity_has_pair_id() {
    flecs::world ecs;

    flecs::id id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add(id_1, id_2);

    test_assert(e != 0);
    test_bool(e.has(id_1, id_2), true);
    test_bool(e.has(id_1, id_3), false);
}

void Entity_has_pair_id_w_type() {
    flecs::world ecs;

    struct Rel { };

    flecs::id id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add<Rel>(id_2);

    test_assert(e != 0);
    test_bool(e.has<Rel>(id_2), true);
    test_bool(e.has<Rel>(id_3), false);
}

void Entity_has_wildcard_id() {
    flecs::world ecs;

    flecs::id id = ecs.entity();
    test_assert(id != 0);

    auto e1 = ecs.entity().add(id);
    auto e2 = ecs.entity();

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    test_bool(e1.has(flecs::Wildcard), true);
    test_bool(e2.has(flecs::Wildcard), false);
}

void Entity_has_wildcard_pair_id() {
    flecs::world ecs;

    flecs::id rel = ecs.entity();
    test_assert(rel != 0);

    flecs::id obj = ecs.entity();
    test_assert(obj != 0);

    flecs::id obj_2 = ecs.entity();
    test_assert(obj_2 != 0);

    flecs::id w1 = ecs.id(rel, flecs::Wildcard);
    flecs::id w2 = ecs.id(flecs::Wildcard, obj);

    auto e1 = ecs.entity().add(rel, obj);
    auto e2 = ecs.entity().add(rel, obj_2);

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    test_bool(e1.has(w1), true);
    test_bool(e1.has(w2), true);

    test_bool(e2.has(w1), true);
    test_bool(e2.has(w2), false);
}

void Entity_owns_id_t() {
    flecs::world ecs;

    flecs::id_t id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id_t id_2 = ecs.entity();
    test_assert(id_2 != 0);    

    auto e = ecs.entity()
        .add(id_1);

    test_assert(e != 0);
    test_bool(e.owns(id_1), true);
    test_bool(e.owns(id_2), false);
}

void Entity_owns_pair_id_t() {
    flecs::world ecs;

    flecs::id_t id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id_t id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id_t id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add(id_1, id_2);

    test_assert(e != 0);
    test_bool(e.owns(id_1, id_2), true);
    test_bool(e.owns(id_1, id_3), false);
}

void Entity_owns_pair_id_t_w_type() {
    flecs::world ecs;

    struct Rel { };

    flecs::id_t id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id_t id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add<Rel>(id_2);

    test_assert(e != 0);
    test_bool(e.owns<Rel>(id_2), true);
    test_bool(e.owns<Rel>(id_3), false);
}

void Entity_owns_id() {
    flecs::world ecs;

    flecs::id id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id id_2 = ecs.entity();
    test_assert(id_2 != 0);    

    auto e = ecs.entity()
        .add(id_1);

    test_assert(e != 0);
    test_bool(e.owns(id_1), true);
    test_bool(e.owns(id_2), false);
}

void Entity_owns_pair_id() {
    flecs::world ecs;

    flecs::id id_1 = ecs.entity();
    test_assert(id_1 != 0);

    flecs::id id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add(id_1, id_2);

    test_assert(e != 0);
    test_bool(e.owns(id_1, id_2), true);
    test_bool(e.owns(id_1, id_3), false);
}

void Entity_owns_wildcard_id() {
    flecs::world ecs;

    flecs::id id = ecs.entity();
    test_assert(id != 0);

    auto e1 = ecs.entity().add(id);
    auto e2 = ecs.entity();

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    test_bool(e1.owns(flecs::Wildcard), true);
    test_bool(e2.owns(flecs::Wildcard), false);
}

void Entity_owns_wildcard_pair() {
    flecs::world ecs;

    flecs::id rel = ecs.entity();
    test_assert(rel != 0);

    flecs::id obj = ecs.entity();
    test_assert(obj != 0);

    flecs::id obj_2 = ecs.entity();
    test_assert(obj_2 != 0);

    flecs::id w1 = ecs.id(rel, flecs::Wildcard);
    flecs::id w2 = ecs.id(flecs::Wildcard, obj);

    auto e1 = ecs.entity().add(rel, obj);
    auto e2 = ecs.entity().add(rel, obj_2);

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    test_bool(e1.owns(w1), true);
    test_bool(e1.owns(w2), true);

    test_bool(e2.owns(w1), true);
    test_bool(e2.owns(w2), false);
}

void Entity_owns_pair_id_w_type() {
    flecs::world ecs;

    struct Rel { };

    flecs::id id_2 = ecs.entity();
    test_assert(id_2 != 0);  

    flecs::id id_3 = ecs.entity();
    test_assert(id_3 != 0);

    auto e = ecs.entity()
        .add<Rel>(id_2);

    test_assert(e != 0);
    test_bool(e.owns<Rel>(id_2), true);
    test_bool(e.owns<Rel>(id_3), false);
}

void Entity_id_from_world() {
    flecs::world ecs;

    auto e = ecs.entity();
    test_assert(e != 0);

    flecs::id id_1 = ecs.id(e);
    test_assert(id_1 != 0);
    test_assert(id_1 == e);
    test_assert(id_1.world() == ecs);
    test_bool(id_1.is_pair(), false);
    test_bool(id_1.is_wildcard(), false);

    flecs::id id_2 = ecs.id(flecs::Wildcard);
    test_assert(id_2 != 0);
    test_assert(id_2 == flecs::Wildcard);
    test_assert(id_2.world() == ecs);
    test_bool(id_2.is_pair(), false);
    test_bool(id_2.is_wildcard(), true);
}

void Entity_id_pair_from_world() {
    flecs::world ecs;

    auto rel = ecs.entity();
    test_assert(rel != 0);

    auto obj = ecs.entity();
    test_assert(obj != 0);    

    flecs::id id_1 = ecs.id(rel, obj);
    test_assert(id_1 != 0);
    test_assert(id_1.relation() == rel);
    test_assert(id_1.object() == obj);
    test_assert(id_1.world() == ecs);
    test_bool(id_1.is_pair(), true);
    test_bool(id_1.is_wildcard(), false);

    flecs::id id_2 = ecs.id(rel, flecs::Wildcard);
    test_assert(id_2 != 0);
    test_assert(id_2.relation() == rel);
    test_assert(id_2.object() == flecs::Wildcard);
    test_assert(id_2.world() == ecs);
    test_bool(id_2.is_pair(), true);
    test_bool(id_2.is_wildcard(), true);
}

void Entity_id_default_from_world() {
    flecs::world ecs;

    flecs::id id_default = ecs.id();
    test_assert(id_default == 0);
}

void Entity_is_a() {
    flecs::world world;

    auto base = world.entity();

    auto e = world.entity().is_a(base);

    test_assert(e.has(flecs::IsA, base));
}

void Entity_is_a_w_type() {
    flecs::world world;

    struct Prefab { };

    auto base = world.entity<Prefab>();

    auto e = world.entity().is_a<Prefab>();

    test_assert(e.has(flecs::IsA, base));
    test_assert(e.has_w_object<Prefab>(flecs::IsA));
}

void Entity_child_of() {
    flecs::world world;

    auto base = world.entity();

    auto e = world.entity().child_of(base);

    test_assert(e.has(flecs::ChildOf, base));
}

void Entity_child_of_w_type() {
    flecs::world world;

    struct Parent { };

    auto base = world.entity<Parent>();

    auto e = world.entity().child_of<Parent>();

    test_assert(e.has(flecs::ChildOf, base));
    test_assert(e.has_w_object<Parent>(flecs::ChildOf));
}

void Entity_id_get_entity() {
    flecs::world world;

    auto e = world.entity();

    auto id = world.id(e);

    test_assert(id.entity() == e);
}

void Entity_id_get_invalid_entity() {
    install_test_abort();
    
    flecs::world world;

    auto r = world.entity();
    auto o = world.entity();

    auto id = world.id(r, o);

    test_expect_abort();
    
    id.entity();
}

void Entity_each_in_stage() {
    flecs::world world;

    struct Rel { };
    struct Obj { };

    auto e = world.entity().add<Rel, Obj>();
    test_assert((e.has<Rel, Obj>()));

    world.readonly_begin();

    auto s = world.get_stage(0);
    auto em = e.mut(s);
    test_assert((em.has<Rel, Obj>()));

    int count = 0;

    em.each<Rel>([&](flecs::entity obj) {
        count ++;
        test_assert(obj == world.id<Obj>());
    });

    test_int(count, 1);

    world.readonly_end();
}

void Entity_iter_recycled_parent() {
    flecs::world ecs;
    
    auto e = ecs.entity();
    e.destruct();

    auto e2 = ecs.entity();
    test_assert(e != e2);
    test_assert((uint32_t)e.id() == (uint32_t)e2.id());

    auto e_child = ecs.entity().child_of(e2);
    int32_t count = 0;

    e2.children([&](flecs::entity child){
        count ++;
        test_assert(child == e_child);
    });

    test_int(count, 1);
}

void Entity_get_lambda_from_stage() {
    flecs::world ecs;

    auto e = ecs.entity().set<Position>({10, 20});

    ecs.readonly_begin();

    flecs::world stage = ecs.get_stage(0);

    bool invoked = false;
    e.mut(stage).get([&](const Position& p) {
        invoked = true;
        test_int(p.x, 10);
        test_int(p.y, 20);
    });
    test_bool(invoked, true);
    
    ecs.readonly_end();
}

void Entity_default_ctor() {
    flecs::world ecs;

    flecs::entity e1;
    flecs::entity e2 = {};

    flecs::entity_view e3;
    flecs::entity_view e4 = {};

    flecs::id id1;
    flecs::id id2 = {};

    e1 = ecs.entity();
    e2 = ecs.entity();
    e3 = ecs.entity();
    e4 = ecs.entity();

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    test_assert(id2 == 0);
}

void Entity_get_obj_by_template() {
    flecs::world ecs;

    struct Rel {};

    flecs::entity e1 = ecs.entity();
    flecs::entity o1 = ecs.entity();
    flecs::entity o2 = ecs.entity();

    e1.add<Rel>(o1);
    e1.add<Rel>(o2);

    test_assert(o1 == e1.get_object<Rel>());
    test_assert(o1 == e1.get_object<Rel>(0));
    test_assert(o2 == e1.get_object<Rel>(1));
}

void Entity_create_named_twice_deferred() {
    flecs::world ecs;

    ecs.defer_begin();

    auto e1 = ecs.entity("e");
    auto e2 = ecs.entity("e");

    auto f1 = ecs.entity("p::f");
    auto f2 = ecs.entity("p::f");

    auto g1 = ecs.scope(ecs.entity("q")).entity("g");
    auto g2 = ecs.scope(ecs.entity("q")).entity("g");

    ecs.defer_end();

    test_str(e1.path().c_str(), "::e");
    test_str(f1.path().c_str(), "::p::f");
    test_str(g1.path().c_str(), "::q::g");

    test_assert(e1 == e2);
    test_assert(f1 == f2);
    test_assert(g1 == g2);
}

struct PositionInitialized { 
    PositionInitialized() { }
    PositionInitialized(float x_, float y_) : x(x_), y(y_) { }
    float x = -1.0;
    float y = -1.0;
};

void Entity_clone() {
    flecs::world ecs;

    PositionInitialized v(10, 20);

    auto src = ecs.entity().add<Tag>().set<PositionInitialized>(v);
    auto dst = src.clone(false);
    test_assert(dst.has<Tag>());
    test_assert(dst.has<PositionInitialized>());

    const PositionInitialized *ptr = dst.get<PositionInitialized>();
    test_assert(ptr != NULL);
    test_int(ptr->x, -1);
    test_int(ptr->y, -1);
}

void Entity_clone_w_value() {
    flecs::world ecs;

    PositionInitialized v = {10, 20};

    auto src = ecs.entity().add<Tag>().set<PositionInitialized>(v);
    auto dst = src.clone();
    test_assert(dst.has<Tag>());
    test_assert(dst.has<PositionInitialized>());

    const PositionInitialized *ptr = dst.get<PositionInitialized>();
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_clone_to_existing() {
    flecs::world ecs;

    PositionInitialized v = {10, 20};

    auto src = ecs.entity().add<Tag>().set<PositionInitialized>(v);
    auto dst = ecs.entity();
    auto result = src.clone(true, dst);
    test_assert(result == dst);

    test_assert(dst.has<Tag>());
    test_assert(dst.has<PositionInitialized>());

    const PositionInitialized *ptr = dst.get<PositionInitialized>();
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
}

void Entity_clone_to_existing_overlap() {
    install_test_abort();
    flecs::world ecs;

    PositionInitialized v = {10, 20};

    auto src = ecs.entity().add<Tag>().set<PositionInitialized>(v);
    auto dst = ecs.entity().add<PositionInitialized>();

    test_expect_abort();
    src.clone(true, dst);
}

void Entity_set_doc_name() {
    flecs::world ecs;

    auto e = ecs.entity("foo_bar")
        .set_doc_name("Foo Bar");

    test_str(e.name(), "foo_bar");
    test_str(e.doc_name(), "Foo Bar");
}

void Entity_set_doc_brief() {
    flecs::world ecs;

    auto e = ecs.entity("foo_bar")
        .set_doc_brief("Foo Bar");

    test_str(e.name(), "foo_bar");
    test_str(e.doc_brief(), "Foo Bar");
}

void Entity_set_doc_detail() {
    flecs::world ecs;

    auto e = ecs.entity("foo_bar")
        .set_doc_detail("Foo Bar");

    test_str(e.name(), "foo_bar");
    test_str(e.doc_detail(), "Foo Bar");
}

void Entity_set_doc_link() {
    flecs::world ecs;

    auto e = ecs.entity("foo_bar")
        .set_doc_link("Foo Bar");

    test_str(e.name(), "foo_bar");
    test_str(e.doc_link(), "Foo Bar");
}

void Entity_entity_w_root_name() {
    flecs::world ecs;

    auto e = ecs.entity("::foo");
    test_str(e.name(), "foo");
    test_str(e.path(), "::foo");
}

void Entity_entity_w_root_name_from_scope() {
    flecs::world ecs;

    auto p = ecs.entity("parent");
    ecs.set_scope(p);
    auto e = ecs.entity("::foo");
    ecs.set_scope(0);
    
    test_str(e.name(), "foo");
    test_str(e.path(), "::foo");
}

struct EntityType { };

void Entity_entity_w_type() {
    flecs::world ecs;

    auto e = ecs.entity<EntityType>();

    test_str(e.name().c_str(), "EntityType");
    test_str(e.path().c_str(), "::EntityType");
    test_assert(!e.has<flecs::Component>());

    auto e_2 = ecs.entity<EntityType>();
    test_assert(e == e_2);
}

struct Parent {
    struct EntityType { };
};

void Entity_entity_w_nested_type() {
    flecs::world ecs;

    auto e = ecs.entity<Parent::EntityType>();
    auto p = ecs.entity<Parent>();

    test_str(e.name().c_str(), "EntityType");
    test_str(e.path().c_str(), "::Parent::EntityType");
    test_assert(e.has(flecs::ChildOf, p));
    test_assert(!e.has<flecs::Component>());

    auto e_2 = ecs.entity<Parent::EntityType>();
    test_assert(e == e_2);
}

void Entity_entity_array() {
    struct TagA {};
    struct TagB {};

    flecs::world ecs;

    flecs::to_array({
        ecs.entity(),
        ecs.entity(),
        ecs.entity()
    }).each([](flecs::entity e) { e.add<TagA>().add<TagB>(); });

    test_int( ecs.count<TagA>(), 3 );
    test_int( ecs.count<TagB>(), 3 );
}

void Entity_entity_w_type_defer() {
    flecs::world ecs;

    ecs.defer_begin();
    auto e = ecs.entity<Tag>();
    ecs.defer_end();

    test_str(e.name(), "Tag");
    test_str(e.symbol(), "Tag");
    test_assert(ecs.id<Tag>() == e);
}

void Entity_add_if_true_T() {
    flecs::world ecs;

    auto e = ecs.entity();

    e.add_if<Tag>(true);
    test_assert( e.has<Tag>());
}

void Entity_add_if_false_T() {
    flecs::world ecs;

    auto e = ecs.entity();

    e.add_if<Tag>(false);
    test_assert( !e.has<Tag>());

    e.add<Tag>();
    test_assert( e.has<Tag>());
    e.add_if<Tag>(false);
    test_assert( !e.has<Tag>());
}

void Entity_add_if_true_id() {
    flecs::world ecs;

    auto e = ecs.entity();
    auto t = ecs.entity();

    e.add_if(true, t);
    test_assert( e.has(t));
}

void Entity_add_if_false_id() {
    flecs::world ecs;

    auto e = ecs.entity();
    auto t = ecs.entity();

    e.add_if(false, t);
    test_assert( !e.has(t));
    e.add(t);
    test_assert( e.has(t));
    e.add_if(false, t);
    test_assert( !e.has(t));
}

void Entity_add_if_true_R_O() {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };

    auto e = ecs.entity();

    e.add_if<Rel, Obj>(true);
    test_assert( (e.has<Rel, Obj>()) );
}

void Entity_add_if_false_R_O() {
    flecs::world ecs;

    struct Rel { };
    struct Obj { };

    auto e = ecs.entity();

    e.add_if<Rel, Obj>(false);
    test_assert( (!e.has<Rel, Obj>()));
    e.add<Rel, Obj>();
    test_assert( (e.has<Rel, Obj>()));
    e.add_if<Rel, Obj>(false);
    test_assert( (!e.has<Rel, Obj>()));
}

void Entity_add_if_true_R_o() {
    flecs::world ecs;

    struct Rel { };

    auto e = ecs.entity();
    auto o = ecs.entity();

    e.add_if<Rel>(true, o);
    test_assert( e.has<Rel>(o));
}

void Entity_add_if_false_R_o() {
    flecs::world ecs;

    struct Rel { };

    auto e = ecs.entity();
    auto o = ecs.entity();

    e.add_if<Rel>(false, o);
    test_assert( !e.has<Rel>(o));
    e.add<Rel>(o);
    test_assert( e.has<Rel>(o));
    e.add_if<Rel>(false, o);
    test_assert( !e.has<Rel>(o));
}

void Entity_add_if_true_r_o() {
    flecs::world ecs;

    auto e = ecs.entity();
    auto r = ecs.entity();
    auto o = ecs.entity();

    e.add_if(true, r, o);
    test_assert( e.has(r, o));
}

void Entity_add_if_false_r_o() {
    flecs::world ecs;

    auto e = ecs.entity();
    auto r = ecs.entity();
    auto o = ecs.entity();

    e.add_if(false, r, o);
    test_assert( !e.has(r, o));
    e.add(r, o);
    test_assert( e.has(r, o));
    e.add_if(false, r, o);
    test_assert( !e.has(r, o));
}

void Entity_add_if_exclusive_r_o() {
    flecs::world ecs;

    auto e = ecs.entity();
    auto r = ecs.entity().add(flecs::Exclusive);
    auto o_1 = ecs.entity();
    auto o_2 = ecs.entity();

    e.add(r, o_1);
    test_assert(e.has(r, o_1));

    e.add_if(true, r, o_2);
    test_assert(!e.has(r, o_1));
    test_assert(e.has(r, o_2));

    e.add_if(false, r, o_1);
    test_assert(!e.has(r, o_1));
    test_assert(!e.has(r, o_2));
}

void Entity_add_if_exclusive_R_o() {
    flecs::world ecs;

    struct R { };

    ecs.component<R>().add(flecs::Exclusive);

    auto e = ecs.entity();
    auto o_1 = ecs.entity();
    auto o_2 = ecs.entity();

    e.add<R>(o_1);
    test_assert(e.has<R>(o_1));

    e.add_if<R>(true, o_2);
    test_assert(!e.has<R>(o_1));
    test_assert(e.has<R>(o_2));

    e.add_if<R>(false, o_1);
    test_assert(!e.has<R>(o_1));
    test_assert(!e.has<R>(o_2));
}

void Entity_add_if_exclusive_R_O() {
    flecs::world ecs;

    struct R { };
    struct O_1 { };
    struct O_2 { };

    ecs.component<R>().add(flecs::Exclusive);

    auto e = ecs.entity();

    e.add<R, O_1>();
    test_assert((e.has<R, O_1>()));

    e.add_if<R, O_2>(true);
    test_assert((!e.has<R, O_1>()));
    test_assert((e.has<R, O_2>()));

    e.add_if<R, O_1>(false);
    test_assert((!e.has<R, O_1>()));
    test_assert((!e.has<R, O_2>()));
}

void Entity_add_if_pair_w_0_object() {
    flecs::world ecs;

    auto e = ecs.entity();
    auto r = ecs.entity();
    auto o_1 = ecs.entity();

    e.add(r, o_1);
    test_assert(e.has(r, o_1));

    e.add_if(0, r, 0);
    test_assert(!e.has(r, o_1));
    test_assert(!e.has(r, flecs::Wildcard));
}
