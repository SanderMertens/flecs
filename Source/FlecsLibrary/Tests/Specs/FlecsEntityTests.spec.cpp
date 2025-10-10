

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "flecs.h"

#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"


struct Parent {
	struct EntityType { };
};

struct MyTag { };

struct PositionInitialized { 
	PositionInitialized() { }
	PositionInitialized(float x_, float y_) : x(x_), y(y_) { }
	float x = -1.0;
	float y = -1.0;
};

struct EntityType { };

struct FlecsTestTurret {
	struct FlecsTestBase { };
};

struct FlecsTestRailgun {
	struct FlecsTestBase { };
	struct FlecsTestHead { };
	struct FlecsTestBeam { };
};

struct FlecsTestBase {};


enum Color {
	Red, Green, Blue
};

BEGIN_DEFINE_SPEC(FFlecsEntityTestsSpec,
                  "FlecsLibrary.Entity",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

void Entity_new(void) {
	flecs::world world;

	auto entity = world.entity();
	test_assert(entity.is_valid());
}

void Entity_new_named(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo");
	test_assert(entity.is_valid());
	test_str(entity.name().c_str(), "Foo");
}

void Entity_new_named_from_scope(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo");
	test_assert(entity.is_valid());
	test_str(entity.name().c_str(), "Foo");

	auto prev = world.set_scope(entity);

	auto child = world.entity("Bar");
	test_assert(child != 0);

	world.set_scope(prev);

	test_str(child.name().c_str(), "Bar");
	test_str(child.path().c_str(), "::Foo::Bar");
}

void Entity_new_nested_named_from_scope(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo");
	test_assert(entity.is_valid());
	test_str(entity.name().c_str(), "Foo");

	auto prev = world.set_scope(entity);

	auto child = world.entity("Bar::Hello");
	test_assert(child != 0);

	world.set_scope(prev);

	test_str(child.name().c_str(), "Hello");
	test_str(child.path().c_str(), "::Foo::Bar::Hello");
}

void Entity_new_nested_named_from_nested_scope(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo::Bar");
	test_assert(entity.is_valid());
	test_str(entity.name().c_str(), "Bar");
	test_str(entity.path().c_str(), "::Foo::Bar");

	auto prev = world.set_scope(entity);

	auto child = world.entity("Hello::World");
	test_assert(child != 0);

	world.set_scope(prev);

	test_str(child.name().c_str(), "World");
	test_str(child.path().c_str(), "::Foo::Bar::Hello::World");
}

void Entity_new_named_from_scope_with_custom_separator(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo.Bar", ".", ".");
	test_assert(entity.is_valid());
	test_str(entity.name().c_str(), "Bar");
	test_str(entity.path(".", ".").c_str(), ".Foo.Bar");

	auto parent = world.lookup("Foo");
	test_assert(parent != 0);
	test_str(parent.name().c_str(), "Foo");

	auto child = world.lookup("Foo.Bar", ".", ".");
	test_assert(child != 0);
	test_str(child.name().c_str(), "Bar");
	test_str(child.path(".", ".").c_str(), ".Foo.Bar");
}


void Entity_new_add(void) {
	flecs::world world;

	world.component<Position>();

	auto entity = world.entity()
		.add<Position>();

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());
}

void Entity_new_add_2(void) {
	flecs::world world;

	world.component<Position>();
	world.component<Velocity>();

	auto entity = world.entity()
		.add<Position>()
		.add<Velocity>();

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());
	test_assert(entity.has<Velocity>());
}


void Entity_new_set(void) {
	flecs::world world;

	world.component<Position>();

	auto entity = world.entity()
		.set<Position>({10, 20});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());

	const Position *p = entity.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_new_set_2(void) {
	flecs::world world;

	world.component<Position>();
	world.component<Velocity>();

	auto entity = world.entity()
		.set<Position>({10, 20})
		.set<Velocity>({1, 2});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());
	test_assert(entity.has<Velocity>());

	const Position *p = entity.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Velocity *v = entity.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}


void Entity_add(void) {
	flecs::world world;

	world.component<Position>();

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add<Position>();
	test_assert(entity.has<Position>());
}

void Entity_remove(void) {
	flecs::world world;

	world.component<Position>();

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add<Position>();
	test_assert(entity.has<Position>());

	entity.remove<Position>();
	test_assert(!entity.has<Position>());
}

void Entity_set(void) {
	flecs::world world;

	world.component<Position>();

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.set<Position>({10, 20});
	test_assert(entity.has<Position>());

	const Position *p = entity.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_emplace_2(void) {
	flecs::world ecs;
	ecs.component<Position>();
	ecs.component<Velocity>();
	
	auto e = ecs.entity()
		.emplace<Position>(10.0f, 20.0f)
		.emplace<Velocity>(30.0f, 40.0f);

	test_assert(e.has<Position>());
	test_assert(e.has<Velocity>());

	const Position *p = e.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Velocity *v = e.try_get<Velocity>();
	test_assert(v != NULL);
	test_int(v->x, 30);
	test_int(v->y, 40);
}

void Entity_emplace_after_add(void) {
	flecs::world ecs;
	ecs.component<Position>();
	ecs.component<Velocity>();

	auto e = ecs.entity()
		.add<Position>()
		.emplace<Velocity>(30.0f, 40.0f);

	test_assert(e.has<Position>());
	test_assert(e.has<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_assert(v != NULL);
	test_int(v->x, 30);
	test_int(v->y, 40);
}

void Entity_emplace_after_add_pair(void) {
	flecs::world ecs;
	ecs.component<Position>();
	ecs.component<Velocity>();

	auto dummy = ecs.entity();

	auto e = ecs.entity()
		.add(flecs::ChildOf, dummy)
		.emplace<Velocity>(30.0f, 40.0f);

	test_assert(e.has(flecs::ChildOf, dummy));
	test_assert(e.has<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_assert(v != NULL);
	test_int(v->x, 30);
	test_int(v->y, 40);
}

void Entity_emplace_pair(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.emplace<Position, Tag>(10.0f, 20.0f);

	test_assert((e.has<Position, Tag>()));

	const Position *p = e.try_get<Position, Tag>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_emplace_pair_w_entity(void) {
	flecs::world ecs;
	ecs.component<Position>();

	auto tag = ecs.entity();

	auto e = ecs.entity()
		.emplace_first<Position>(tag, 10.0f, 20.0f);

	test_assert((e.has<Position>(tag)));

	const Position *p = e.try_get<Position>(tag);
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_emplace_pair_type(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.emplace<flecs::pair<Position, Tag>>(10.0f, 20.0f);

	test_assert((e.has<Position, Tag>()));

	const Position *p = e.try_get<Position, Tag>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_emplace_pair_second(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto tag = ecs.entity();

	auto e = ecs.entity()
		.emplace_second<Position>(tag, 10.0f, 20.0f);

	test_assert((e.has_second<Position>(tag)));

	const Position *p = e.try_get_second<Position>(tag);
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_add_2(void) {
	flecs::world world;

	world.component<Position>();
	world.component<Velocity>();

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add<Position>()
		  .add<Velocity>();

	test_assert(entity.has<Position>());
	test_assert(entity.has<Velocity>());
}

void Entity_add_entity(void) {
	flecs::world world;

	auto tag = world.entity();
	test_assert(tag != 0);

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add(tag);
	test_assert(entity.has(tag));
}

void Entity_add_childof(void) {
	flecs::world world;

	auto parent = world.entity();
	test_assert(parent != 0);

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add(flecs::ChildOf, parent);
	test_assert(entity.has(flecs::ChildOf, parent));
}

void Entity_add_instanceof(void) {
	flecs::world world;

	auto base = world.entity();
	test_assert(base != 0);

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add(flecs::IsA, base);
	test_assert(entity.has(flecs::IsA, base));
}

void Entity_remove_2(void) {
	flecs::world world;

	world.component<Position>();
	world.component<Velocity>();

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add<Position>()
		  .add<Velocity>();

	test_assert(entity.has<Position>());
	test_assert(entity.has<Velocity>());

	entity.remove<Position>()
		  .remove<Velocity>();

	test_assert(!entity.has<Position>());
	test_assert(!entity.has<Velocity>());          
}

void Entity_set_2(void) {
	flecs::world world;

	world.component<Position>();
	world.component<Velocity>();

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.set<Position>({10, 20})
		.set<Velocity>({1, 2});    
	test_assert(entity.has<Position>());
	test_assert(entity.has<Velocity>());

	const Position *p = entity.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Velocity *v = entity.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);    
}

void Entity_remove_entity(void) {
	flecs::world world;

	auto tag = world.entity();
	test_assert(tag != 0);

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add(tag);
	test_assert(entity.has(tag));

	entity.remove(tag);
	test_assert(!entity.has(tag));
}

void Entity_remove_childof(void) {
	flecs::world world;

	auto parent = world.entity();
	test_assert(parent != 0);

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add(flecs::ChildOf, parent);
	test_assert(entity.has(flecs::ChildOf, parent));

	entity.remove(flecs::ChildOf, parent);
	test_assert(!entity.has(flecs::ChildOf, parent));
}

void Entity_remove_instanceof(void) {
	flecs::world world;

	auto base = world.entity();
	test_assert(base != 0);

	auto entity = world.entity();
	test_assert(entity.is_valid());

	entity.add(flecs::IsA, base);
	test_assert(entity.has(flecs::IsA, base));
    
	entity.remove(flecs::IsA, base);
	test_assert(!entity.has(flecs::IsA, base));
}

void Entity_get_generic(void) {
	flecs::world world;

	auto position = world.component<Position>();

	auto entity = world.entity()
		.set<Position>({10, 20});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());

	const void *void_p = entity.get(position);
	test_assert(void_p != nullptr);

	const Position *p = static_cast<const Position*>(void_p);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_ensure_generic(void) {
	flecs::world world;

	auto position = world.component<Position>();

	auto entity = world.entity()
		.set<Position>({10, 20});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());

	bool invoked;
	world.observer<Position>()
		.event(flecs::OnSet)
		.each([&invoked](flecs::entity e, Position& p) {
			invoked = true;
		});

	void *void_p = entity.obtain(position);
	test_assert(void_p != nullptr);

	Position *p = static_cast<Position*>(void_p);
	test_int(p->x, 10);
	test_int(p->y, 20);

	entity.modified(position);
	test_bool(invoked, true);
}

void Entity_get_generic_w_id(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id id = position;

	auto entity = world.entity()
		.set<Position>({10, 20});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());

	const void *void_p = entity.get(id);
	test_assert(void_p != nullptr);

	const Position *p = static_cast<const Position*>(void_p);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_get_generic_w_id_t(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id_t id = position;

	auto entity = world.entity()
		.set<Position>({10, 20});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());

	const void *void_p = entity.get(id);
	test_assert(void_p != nullptr);

	const Position *p = static_cast<const Position*>(void_p);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_ensure_generic_w_id(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id id = position;

	auto entity = world.entity()
		.set<Position>({10, 20});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());

	bool invoked;
	world.observer<Position>()
		.event(flecs::OnSet)
		.each([&invoked](flecs::entity e, Position& p) {
			invoked = true;
		});

	void *void_p = entity.obtain(id);
	test_assert(void_p != nullptr);

	Position *p = static_cast<Position*>(void_p);
	test_int(p->x, 10);
	test_int(p->y, 20);

	entity.modified(id);
	test_bool(invoked, true);
}

void Entity_ensure_generic_w_id_t(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id_t id = position;

	auto entity = world.entity()
		.set<Position>({10, 20});

	test_assert(entity.is_valid());
	test_assert(entity.has<Position>());

	bool invoked;
	world.observer<Position>()
		.event(flecs::OnSet)
		.each([&invoked](flecs::entity e, Position& p) {
			invoked = true;
		});

	void *void_p = entity.obtain(id);
	test_assert(void_p != nullptr);

	Position *p = static_cast<Position*>(void_p);
	test_int(p->x, 10);
	test_int(p->y, 20);

	entity.modified(id);
	test_bool(invoked, true);
}

void Entity_get_mut_w_id(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity e = world.entity();

	Position *p = static_cast<Position*>(e.try_get_mut(world.id<Position>()));
	test_assert(p == nullptr);

	e.set<Position>({10, 20});
    
	p = static_cast<Position*>(e.get_mut(world.id<Position>()));
	test_assert(p != nullptr);

	test_int(p->x, 10);
	test_int(p->y, 20);
}


void Entity_get_mut_T(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	flecs::entity e = world.entity();

	Position *p = e.try_get_mut<Position>();
	test_assert(p == nullptr);

	e.set<Position>({10, 20});
    
	p = e.try_get_mut<Position>();
	test_assert(p != nullptr);

	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_get_mut_r_t(void) {
	//@TODO
	flecs::world world;
	world.component<Position>();

	flecs::entity tgt = world.entity();
	flecs::entity e = world.entity();

	Position *p = static_cast<Position*>(e.try_get_mut(world.id<Position>(), tgt));
	test_assert(p == nullptr);

	e.set<Position>(tgt, {10, 20});
    
	p = static_cast<Position*>(e.get_mut(world.id<Position>(), tgt));
	test_assert(p != nullptr);

	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_get_mut_R_t(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity tgt = world.entity();
	flecs::entity e = world.entity();

	Position *p = e.try_get_mut<Position>(tgt);
	test_assert(p == nullptr);

	e.set<Position>(tgt, {10, 20});
    
	p = e.try_get_mut<Position>(tgt);
	test_assert(p != nullptr);

	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_get_mut_R_T(void) {
	flecs::world world;
	world.component<Position>();

	struct Tgt { };
	world.component<Tgt>();

	flecs::entity e = world.entity();

	Position *p = e.try_get_mut<Position, Tgt>();
	test_assert(p == nullptr);

	e.set<Position, Tgt>({10, 20});
    
	p = e.try_get_mut<Position, Tgt>();
	test_assert(p != nullptr);

	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_get_mut_r_T(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity rel = world.entity();
	flecs::entity e = world.entity();

	Position *p = e.try_get_mut_second<Position>(rel);
	test_assert(p == nullptr);

	e.set_second<Position>(rel, {10, 20});
    
	p = e.try_get_mut_second<Position>(rel);
	test_assert(p != nullptr);

	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_try_get_mut_w_id(void) {
    flecs::world world;
	world.component<Position>();

    flecs::entity e = world.entity();

    Position *p = static_cast<Position*>(e.try_get_mut(world.id<Position>()));
    test_assert(p == nullptr);

    e.set<Position>({10, 20});
    
    p = static_cast<Position*>(e.try_get_mut(world.id<Position>()));
    test_assert(p != nullptr);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_try_get_mut_T(void) {
    flecs::world world;
	world.component<Position>();

    flecs::entity e = world.entity();

    Position *p = e.try_get_mut<Position>();
    test_assert(p == nullptr);

    e.set<Position>({10, 20});
    
    p = e.try_get_mut<Position>();
    test_assert(p != nullptr);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_try_get_mut_r_t(void) {
    flecs::world world;
    world.component<Position>();

    flecs::entity tgt = world.entity();
    flecs::entity e = world.entity();

    Position *p = static_cast<Position*>(e.try_get_mut(world.id<Position>(), tgt));
    test_assert(p == nullptr);

    e.set<Position>(tgt, {10, 20});
    
    p = static_cast<Position*>(e.get_mut(world.id<Position>(), tgt));
    test_assert(p != nullptr);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_try_get_mut_R_t(void) {
    flecs::world world;
	world.component<Position>();

    flecs::entity tgt = world.entity();
    flecs::entity e = world.entity();

    Position *p = e.try_get_mut<Position>(tgt);
    test_assert(p == nullptr);

    e.set<Position>(tgt, {10, 20});
    
    p = e.try_get_mut<Position>(tgt);
    test_assert(p != nullptr);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_try_get_mut_R_T(void) {
    flecs::world world;
	world.component<Position>();

    struct Tgt { };
	world.component<Tgt>();

    flecs::entity e = world.entity();

    Position *p = e.try_get_mut<Position, Tgt>();
    test_assert(p == nullptr);

    e.set<Position, Tgt>({10, 20});
    
    p = e.try_get_mut<Position, Tgt>();
    test_assert(p != nullptr);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_try_get_mut_r_T(void) {
    flecs::world world;
	world.component<Position>();

    flecs::entity rel = world.entity();
    flecs::entity e = world.entity();

    Position *p = e.try_get_mut_second<Position>(rel);
    test_assert(p == nullptr);

    e.set_second<Position>(rel, {10, 20});
    
    p = e.try_get_mut_second<Position>(rel);
    test_assert(p != nullptr);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Entity_set_generic(void) {
	flecs::world world;

	auto position = world.component<Position>();

	Position p = {10, 20};

	auto e = world.entity()
		.set_ptr(position, sizeof(Position), &p);

	test_assert(e.has<Position>());
	test_assert(e.has(position));

	const Position *ptr = e.try_get<Position>();
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_set_generic_w_id(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id id = position;

	Position p = {10, 20};

	auto e = world.entity()
		.set_ptr(id, sizeof(Position), &p);

	test_assert(e.has<Position>());
	test_assert(e.has(id));

	const Position *ptr = e.try_get<Position>();
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_set_generic_w_id_t(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id_t id = position;

	Position p = {10, 20};

	auto e = world.entity()
		.set_ptr(id, sizeof(Position), &p);

	test_assert(e.has<Position>());
	test_assert(e.has(id));

	const Position *ptr = e.try_get<Position>();
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_set_generic_no_size(void) {
	flecs::world world;

	auto position = world.component<Position>();

	Position p = {10, 20};

	auto e = world.entity()
		.set_ptr(position, &p);

	test_assert(e.has<Position>());
	test_assert(e.has(position));

	const Position *ptr = e.try_get<Position>();
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_set_generic_no_size_w_id(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id id = position;

	Position p = {10, 20};

	auto e = world.entity()
		.set_ptr(id, &p);

	test_assert(e.has<Position>());
	test_assert(e.has(id));

	const Position *ptr = e.try_get<Position>();
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_set_generic_no_size_w_id_t(void) {
	flecs::world world;

	auto position = world.component<Position>();
	flecs::id_t id = position;

	Position p = {10, 20};

	auto e = world.entity()
		.set_ptr(id, &p);

	test_assert(e.has<Position>());
	test_assert(e.has(id));

	const Position *ptr = e.try_get<Position>();
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_set_T(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity e = world.entity()
		.set(Position{10, 20});

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_set_R_t(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity tgt = world.entity();
	flecs::entity e = world.entity()
		.set<Position>(tgt, {10, 20});

	const Position& p = e.get<Position>(tgt);
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_set_R_T(void) {
	flecs::world world;
	world.component<Position>();

	struct Tgt { };
	world.component<Tgt>();

	flecs::entity e = world.entity()
		.set<Position, Tgt>({10, 20});

	const Position& p = e.get<Position, Tgt>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_set_r_T(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity rel = world.entity();

	flecs::entity e = world.entity()
		.set_second<Position>(rel, {10, 20});

	const Position& p = e.get_second<Position>(rel);
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_assign_T(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity e = world.entity().add<Position>();

	e.assign(Position{10, 20});

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_assign_R_t(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity tgt = world.entity();
	flecs::entity e = world.entity().add<Position>(tgt);
	e.assign<Position>(tgt, {10, 20});

	const Position& p = e.get<Position>(tgt);
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_assign_R_T(void) {
	flecs::world world;
	world.component<Position>();

	struct Tgt { };
	world.component<Tgt>();

	flecs::entity e = world.entity().add<Position, Tgt>();
	e.assign<Position, Tgt>({10, 20});

	const Position& p = e.get<Position, Tgt>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_assign_r_T(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity rel = world.entity();

	flecs::entity e = world.entity().add_second<Position>(rel);
	e.assign_second<Position>(rel, {10, 20});

	const Position& p = e.get_second<Position>(rel);
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_assign_w_on_set_hook(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_set([&](Position& p) {
			test_int(p.x, 10);
			test_int(p.y, 20);
			invoked ++;
		});

	flecs::entity e = world.entity().add<Position>();

	test_int(invoked, 0);

	e.assign(Position{10, 20});

	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_assign_w_on_set_observer(void) {
	flecs::world world;
	world.component<Position>();

	int invoked = 0;

	world.observer<Position>()
		.event(flecs::OnSet)
		.each([&](Position& p) {
			test_int(p.x, 10);
			test_int(p.y, 20);
			invoked ++;
		});

	flecs::entity e = world.entity().add<Position>();

	test_int(invoked, 0);

	e.assign(Position{10, 20});

	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_assign_w_change_detect(void) {
	flecs::world world;
	world.component<Position>();

	auto q = world.query_builder<const Position>()
		.detect_changes()
		.build();

	test_bool(true, q.changed());
	q.each([](const Position& p) {});
	test_bool(false, q.changed());

	flecs::entity e = world.entity().add<Position>();

	test_bool(true, q.changed());
	q.each([](const Position& p) {});
	test_bool(false, q.changed());

	e.assign(Position{10, 20});

	test_bool(true, q.changed());
	q.each([](const Position& p) {});
	test_bool(false, q.changed());

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_defer_assign_w_on_set_hook(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_set([&](Position& p) {
			test_int(p.x, 10);
			test_int(p.y, 20);
			invoked ++;
		});

	flecs::entity e = world.entity().add<Position>();

	test_int(invoked, 0);

	world.defer_begin();
	e.assign(Position{10, 20});
	test_int(invoked, 0);
	world.defer_end();

	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_defer_assign_w_on_set_observer(void) {
	flecs::world world;
	world.component<Position>();

	int invoked = 0;

	world.observer<Position>()
		.event(flecs::OnSet)
		.each([&](Position& p) {
			test_int(p.x, 10);
			test_int(p.y, 20);
			invoked ++;
		});

	flecs::entity e = world.entity().add<Position>();

	test_int(invoked, 0);

	world.defer_begin();
	e.assign(Position{10, 20});
	test_int(invoked, 0);
	world.defer_end();

	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_defer_assign_w_change_detect(void) {
	flecs::world world;
	world.component<Position>();

	auto q = world.query_builder<const Position>()
		.detect_changes()
		.build();

	test_bool(true, q.changed());
	q.each([](const Position& p) {});
	test_bool(false, q.changed());

	flecs::entity e = world.entity().add<Position>();

	test_bool(true, q.changed());
	q.each([](const Position& p) {});
	test_bool(false, q.changed());

	world.defer_begin();
	e.assign(Position{10, 20});
	test_bool(false, q.changed());
	world.defer_end();

	test_bool(true, q.changed());
	q.each([](const Position& p) {});
	test_bool(false, q.changed());

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_add_role(void) {
	flecs::world world;

	auto entity = world.entity();

	entity = entity.add_flags(flecs::PAIR);

	test_assert(entity & ECS_PAIR);
}

void Entity_remove_role(void) {
	flecs::world world;

	auto entity = world.entity();

	flecs::entity_t id = entity;

	entity = entity.add_flags(flecs::PAIR);

	test_assert(entity & ECS_PAIR);

	entity = entity.remove_flags();

	test_assert(entity == id);
}

void Entity_has_role(void) {
	flecs::world world;

	auto entity = world.entity();

	entity = entity.add_flags(flecs::PAIR);

	test_assert(entity.has_flags(flecs::PAIR));

	entity = entity.remove_flags();

	test_assert(!entity.has_flags(flecs::PAIR));
}

void Entity_pair_role(void) {
	flecs::world world;

	auto a = world.entity();
	auto b = world.entity();

	auto pair = flecs::id(a, b);
	pair = pair.add_flags(flecs::PAIR);
    
	test_assert(pair.has_flags(flecs::PAIR));

	auto rel = pair.first();
	auto obj = pair.second();

	test_assert(rel == a);
	test_assert(obj == b);
}

void Entity_equals(void) {
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

void Entity_compare_0(void) {
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

void Entity_compare_id_t(void) {
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

void Entity_compare_id(void) {
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

void Entity_compare_literal(void) {
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

void Entity_greater_than(void) {
	flecs::world world;

	auto e1 = world.entity();
	auto e2 = world.entity();

	test_assert(e2 > e1);
	test_assert(e2 >= e1);
}

void Entity_less_than(void) {
	flecs::world world;

	auto e1 = world.entity();
	auto e2 = world.entity();

	test_assert(e1 < e2);
	test_assert(e1 <= e2);
}

void Entity_not_0_or_1(void) {
	flecs::world world;

	auto e = world.entity();

	flecs::id_t id = e;

	test_assert(id != 0);
	test_assert(id != 1);
}

void Entity_has_childof(void) {
	flecs::world world;

	auto parent = world.entity();

	auto e = world.entity()
		.add(flecs::ChildOf, parent);

	test_assert(e.has(flecs::ChildOf, parent));
}

void Entity_has_instanceof(void) {
	flecs::world world;

	auto base = world.entity();

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has(flecs::IsA, base));
}

void Entity_has_instanceof_indirect(void) {
	flecs::world world;

	auto base_of_base = world.entity();

	auto base = world.entity()
		.add(flecs::IsA, base_of_base);

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has(flecs::IsA, base_of_base));
}

void Entity_null_string(void) {
	flecs::world world;

	auto e = world.entity();

	test_str(e.name().c_str(), "");
}

void Entity_set_name(void) {
	flecs::world world;

	auto e = world.entity();
	test_str(e.name().c_str(), "");

	e.set_name("Foo");
	test_str(e.name().c_str(), "Foo");
}

void Entity_change_name(void) {
	flecs::world world;

	auto e = world.entity("Bar");
	test_str(e.name().c_str(), "Bar");

	e.set_name("Foo");
	test_str(e.name().c_str(), "Foo");
}

void Entity_delete(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

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

void Entity_clear(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

	auto e = world.entity()
		.add<Position>()
		.add<Velocity>();

	e.clear();

	test_assert(!e.has<Position>());
	test_assert(!e.has<Velocity>());

	auto e2 = world.entity();
	test_assert(e2 > e);
}

void Entity_foce_owned(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);
	world.component<Velocity>().add(flecs::OnInstantiate, flecs::Inherit);

	auto prefab = world.prefab()
		.add<Position>()
		.add<Velocity>()
		.auto_override<Position>();

	auto e = world.entity()
		.add(flecs::IsA, prefab);
    
	test_assert(e.has<Position>());
	test_assert(e.owns<Position>());
	test_assert(e.has<Velocity>());
	test_assert(!e.owns<Velocity>());
}

void Entity_force_owned_2(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);
	world.component<Velocity>().add(flecs::OnInstantiate, flecs::Inherit);

	auto prefab = world.prefab()
		.add<Position>()
		.add<Velocity>()
		.auto_override<Position>()
		.auto_override<Velocity>();

	auto e = world.entity()
		.add(flecs::IsA, prefab);
    
	test_assert(e.has<Position>());
	test_assert(e.owns<Position>());
	test_assert(e.has<Velocity>());
	test_assert(e.owns<Velocity>());
}

void Entity_force_owned_nested(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);
	world.component<Velocity>().add(flecs::OnInstantiate, flecs::Inherit);

	auto prefab = world.prefab()
		.add<Position>()
		.add<Velocity>()
		.auto_override<Position>();

	auto prefab_2 = world.prefab()
		.add(flecs::IsA, prefab);

	auto e = world.entity()
		.add(flecs::IsA, prefab_2);
    
	test_assert(e.has<Position>());
	test_assert(e.owns<Position>());
	test_assert(e.has<Velocity>());
	test_assert(!e.owns<Velocity>());
}

void Entity_tag_has_size_zero(void) {
	flecs::world world;

	auto comp = world.component<MyTag>();

	auto ptr = comp.try_get<flecs::Component>();
	test_int(ptr->size, 0);
	test_int(ptr->alignment, 0);
}

void Entity_get_null_name(void) {
	flecs::world world;

	auto e = world.entity().set_name(nullptr);

	auto n = e.name();
	test_assert(n.size() == 0);
}

void Entity_get_target(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

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

	auto p = child.target(Rel);
	test_assert(p != 0);
	test_assert(p == obj1);

	p = child.target(Rel, 0);
	test_assert(p != 0);
	test_assert(p == obj1);

	p = child.target(Rel, 1);
	test_assert(p != 0);
	test_assert(p == obj2);

	p = child.target(Rel, 2);
	test_assert(p != 0);
	test_assert(p == obj3);

	p = child.target(Rel, 3);
	test_assert(p == 0);
}

void Entity_get_parent(void) {
	flecs::world world;

	flecs::entity parent = world.entity();
	flecs::entity child = world.entity().child_of(parent);
    
	test_assert(child.target(flecs::ChildOf) == parent);
	test_assert(child.parent() == parent);
}

void Entity_is_component_enabled(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	world.component<Position>().add(flecs::CanToggle);

	auto e = world.entity()
		.add<Position>();

	test_assert(e.enabled<Position>());
	test_assert(!e.enabled<Velocity>());
}

void Entity_is_enabled_component_enabled(void) {
	flecs::world world;
    
	world.component<Position>().add(flecs::CanToggle);

	auto e = world.entity()
		.add<Position>()
		.enable<Position>();

	test_assert(e.enabled<Position>());
}

void Entity_is_disabled_component_enabled(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);

	auto e = world.entity()
		.add<Position>()
		.disable<Position>();

	test_assert(!e.enabled<Position>());
}

void Entity_is_pair_enabled(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);

	struct TgtA { };
	struct TgtB { };
	world.component<TgtA>();
	world.component<TgtB>();

	auto e = world.entity()
		.add<Position, TgtA>();

	test_assert((e.enabled<Position, TgtA>()));
	test_assert((!e.enabled<Position, TgtB>()));
}

void Entity_is_enabled_pair_enabled(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);
	
	world.component<Tgt>();

	world.component<Position>().add(flecs::CanToggle);

	auto e = world.entity()
		.add<Position, Tgt>()
		.enable<Position, Tgt>();

	test_assert((e.enabled<Position, Tgt>()));
}

void Entity_is_disabled_pair_enabled(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);

	world.component<Tgt>();

	auto e = world.entity()
		.add<Position, Tgt>()
		.disable<Position, Tgt>();

	test_assert((!e.enabled<Position, Tgt>()));
}

void Entity_is_pair_enabled_w_ids(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);

	auto rel = world.entity();
	auto tgt_a = world.entity();
	auto tgt_b = world.entity();

	auto e = world.entity()
		.add(rel, tgt_a);

	test_assert((e.enabled(rel, tgt_a)));
	test_assert((!e.enabled(rel, tgt_b)));
}

void Entity_is_enabled_pair_enabled_w_ids(void) {
	flecs::world world;

	auto rel = world.entity().add(flecs::CanToggle);
	auto tgt = world.entity();

	auto e = world.entity()
		.add(rel, tgt)
		.enable(rel, tgt);

	test_assert((e.enabled(rel, tgt)));
}

void Entity_is_disabled_pair_enabled_w_ids(void) {
	flecs::world world;

	auto rel = world.entity().add(flecs::CanToggle);
	auto tgt = world.entity();

	auto e = world.entity()
		.add(rel, tgt)
		.disable(rel, tgt);

	test_assert((!e.enabled(rel, tgt)));
}

void Entity_is_pair_enabled_w_tgt_id(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);

	auto tgt_a = world.entity();
	auto tgt_b = world.entity();

	auto e = world.entity()
		.add<Position>(tgt_a);

	test_assert((e.enabled<Position>(tgt_a)));
	test_assert((!e.enabled<Position>(tgt_b)));
}

void Entity_is_enabled_pair_enabled_w_tgt_id(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);

	auto tgt = world.entity();

	auto e = world.entity()
		.add<Position>(tgt)
		.enable<Position>(tgt);

	test_assert((e.enabled<Position>(tgt)));
}

void Entity_is_disabled_pair_enabled_w_tgt_id(void) {
	flecs::world world;

	world.component<Position>().add(flecs::CanToggle);

	auto tgt = world.entity();

	auto e = world.entity()
		.add<Position>(tgt)
		.disable<Position>(tgt);

	test_assert((!e.enabled<Position>(tgt)));
}

void Entity_get_type(void) {
	flecs::world world;

	auto entity = world.entity();
	test_assert(entity.is_valid());

	auto type_1 = entity.type();
	test_int(type_1.count(), 0);

	auto type_2 = entity.type();
	test_int(type_2.count(), 0);
}

void Entity_get_nonempty_type(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	auto entity = world.entity()
		.add<Position>();
	test_assert(entity.is_valid());

	auto type_1 = entity.type();
	test_int(type_1.count(), 1);
	test_int(type_1.get(0), world.id<Position>());

	auto type_2 = entity.type();
	test_int(type_2.count(), 1);
	test_int(type_2.get(0), world.id<Position>());
}

void Entity_set_no_copy(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);
	Pod::copy_invoked = 0;

	auto e = world.entity()
		.set<Pod>({10});
	test_int(Pod::copy_invoked, 0);

	test_assert(e.has<Pod>());
	const Pod *p = e.try_get<Pod>();
	test_assert(p != NULL);
	test_int(p->value, 10);
}

void Entity_set_copy(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);
	Pod::copy_invoked = 0;

	Pod val(10);

	auto e = world.entity()
		.set<Pod>(val);
	test_int(Pod::copy_invoked, 1);

	test_assert(e.has<Pod>());
	const Pod *p = e.try_get<Pod>();
	test_assert(p != NULL);
	test_int(p->value, 10);
}

void Entity_set_deduced(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	auto e = world.entity()
		.set(Position{10, 20});

	test_assert(e.has<Position>());

	const Position *p = e.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);
}


void Entity_override(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);

	auto base = world.entity()
		.auto_override<Position>();

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has<Position>());
	test_assert(e.owns<Position>());
}

void Entity_override_id(void) {
	flecs::world world;
    
	auto tag_a = world.entity().add(flecs::OnInstantiate, flecs::Inherit);
	auto tag_b = world.entity().add(flecs::OnInstantiate, flecs::Inherit);

	auto base = world.entity()
		.auto_override(tag_a)
		.add(tag_b);

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has(tag_a));
	test_assert(e.owns(tag_a));

	test_assert(e.has(tag_b));
	test_assert(!e.owns(tag_b));
}

void Entity_override_pair_w_tgt_id(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);
	auto tgt_a = world.entity();
	auto tgt_b = world.entity();

	auto base = world.entity()
		.auto_override<Position>(tgt_a)
		.add<Position>(tgt_b);

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has<Position>(tgt_a));
	test_assert(e.owns<Position>(tgt_a));

	test_assert(e.has<Position>(tgt_b));
	test_assert(!e.owns<Position>(tgt_b));
}

void Entity_override_pair_w_ids(void) {
	flecs::world world;

	auto rel = world.entity().add(flecs::OnInstantiate, flecs::Inherit);
	auto tgt_a = world.entity();
	auto tgt_b = world.entity();

	auto base = world.entity()
		.auto_override(rel, tgt_a)
		.add(rel, tgt_b);

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has(rel, tgt_a));
	test_assert(e.owns(rel, tgt_a));

	test_assert(e.has(rel, tgt_b));
	test_assert(!e.owns(rel, tgt_b));
}

void Entity_override_pair(void) {
	flecs::world world;

	struct TagA { };
	struct TagB { };
	world.component<TagA>();
	world.component<TagB>();
	
	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);

	auto base = world.entity()
		.auto_override<Position, TagA>()
		.add<Position, TagB>();

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert((e.has<Position, TagA>()));
	test_assert((e.owns<Position, TagA>()));

	test_assert((e.has<Position, TagB>()));
	test_assert((!e.owns<Position, TagB>()));
}

void Entity_set_override(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);

	auto base = world.entity()
		.set_auto_override<Position>({10, 20});

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has<Position>());
	test_assert(e.owns<Position>());

	const Position* p = e.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Position* p_base = base.try_get<Position>();
	test_assert(p != p_base);
	test_int(p_base->x, 10);
	test_int(p_base->y, 20);
}

void Entity_set_override_lvalue(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);

	Position plvalue = {10, 20};

	auto base = world.entity()
		.set_auto_override<Position>(plvalue);

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert(e.has<Position>());
	test_assert(e.owns<Position>());

	const Position* p = e.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Position* p_base = base.try_get<Position>();
	test_assert(p != p_base);
	test_int(p_base->x, 10);
	test_int(p_base->y, 20);
}

void Entity_set_override_pair(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);
	world.component<Tgt>();
	
	auto base = world.entity()
		.set_auto_override<Position, Tgt>({10, 20});

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert((e.has<Position, Tgt>()));
	test_assert((e.owns<Position, Tgt>()));

	const Position* p = e.try_get<Position, Tgt>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Position* p_base = base.try_get<Position, Tgt>();
	test_assert(p != p_base);
	test_int(p_base->x, 10);
	test_int(p_base->y, 20);
}

void Entity_set_override_pair_w_tgt_id(void) {
	flecs::world world;

	world.component<Position>().add(flecs::OnInstantiate, flecs::Inherit);

	auto tgt = world.entity();

	auto base = world.entity()
		.set_auto_override<Position>(tgt, {10, 20});

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert((e.has<Position>(tgt)));
	test_assert((e.owns<Position>(tgt)));

	const Position* p = e.try_get<Position>(tgt);
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Position* p_base = base.try_get<Position>(tgt);
	test_assert(p != p_base);
	test_int(p_base->x, 10);
	test_int(p_base->y, 20);
}

void Entity_set_override_pair_w_rel_tag(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Tgt>().add(flecs::OnInstantiate, flecs::Inherit);

	auto base = world.entity()
		.set_auto_override<Tgt, Position>({10, 20});

	auto e = world.entity()
		.add(flecs::IsA, base);

	test_assert((e.has<Tgt, Position>()));
	test_assert((e.owns<Tgt, Position>()));

	const Position* p = e.try_get<Tgt, Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Position* p_base = base.try_get<Tgt, Position>();
	test_assert(p != p_base);
	test_int(p_base->x, 10);
	test_int(p_base->y, 20);
}

void Entity_emplace_override(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	auto e = world.entity().emplace_auto_override<NoDefaultCtor>(10);
	test_assert(e.has<NoDefaultCtor>());

	const NoDefaultCtor *ptr = e.try_get<NoDefaultCtor>();
	test_assert(ptr != nullptr);
	test_int(ptr->x_, 10);
}

void Entity_emplace_override_pair(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	auto e = world.entity().emplace_auto_override<NoDefaultCtor, Tag>(10);
	test_assert((e.has<NoDefaultCtor, Tag>()));

	const NoDefaultCtor *ptr = e.try_get<NoDefaultCtor, Tag>();
	test_assert(ptr != nullptr);
	test_int(ptr->x_, 10);
}

void Entity_implicit_name_to_char(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo");
	test_assert(entity.is_valid());
	test_str(entity.name().c_str(), "Foo");

	test_str(entity.name(), "Foo");
}

void Entity_path(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	test_str(child.path().c_str(), "::parent::child");
}

void Entity_path_from(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity grandchild = world.scope(child).entity("grandchild");
	test_str(grandchild.path().c_str(), "::parent::child::grandchild");
	test_str(grandchild.path_from(parent).c_str(), "child::grandchild");
}

void Entity_path_from_type(void) {
	flecs::world world;

	flecs::entity parent = world.entity<Parent>();
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity grandchild = world.scope(child).entity("grandchild");
	test_str(grandchild.path().c_str(), "::Parent::child::grandchild");
	test_str(grandchild.path_from<Parent>().c_str(), "child::grandchild");
}

void Entity_path_custom_sep(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	test_str(child.path("_", "").c_str(), "parent_child");
}

void Entity_path_from_custom_sep(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity grandchild = world.scope(child).entity("grandchild");
	test_str(grandchild.path().c_str(), "::parent::child::grandchild");
	test_str(grandchild.path_from(parent, "_").c_str(), "child_grandchild");
}

void Entity_path_from_type_custom_sep(void) {
	flecs::world world;

	flecs::entity parent = world.entity<Parent>();
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity grandchild = world.scope(child).entity("grandchild");
	test_str(grandchild.path().c_str(), "::Parent::child::grandchild");
	test_str(grandchild.path_from<Parent>("_").c_str(), "child_grandchild");
}

void Entity_implicit_path_to_char(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo::Bar");
	test_assert(entity.is_valid());
	test_str(entity.name().c_str(), "Bar");

	test_str(entity.path(), "::Foo::Bar");
}

void Entity_implicit_type_str_to_char(void) {
	flecs::world world;

	auto entity = flecs::entity(world, "Foo");
	test_assert(entity.is_valid());

	test_str(entity.type().str(), "(Identifier,Name)");
}

void Entity_entity_to_entity_view(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity e = world.entity()
		.set<Position>({10, 20});
	test_assert(e != 0);

	flecs::entity_view ev = e;
	test_assert(ev != 0);
	test_assert(e == ev);

	const Position *p = ev.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_entity_view_to_entity_world(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity e = world.entity()
		.set<Position>({10, 20});
	test_assert(e != 0);

	flecs::entity_view ev = e;
	test_assert(ev != 0);
	test_assert(e == ev);

	flecs::entity ew = ev.mut(world);
	ew.set<Position>({10, 20});

	test_assert(ev.has<Position>());
	const Position *p = ev.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_entity_view_to_entity_stage(void) {
	flecs::world world;
	world.component<Position>();

	flecs::entity_view ev = world.entity();

	auto stage = world.get_stage(0);

	world.readonly_begin();

	flecs::entity ew = ev.mut(stage);

	ew.set<Position>({10, 20});
	test_assert(!ew.has<Position>());

	world.readonly_end();

	test_assert(ew.has<Position>());
	test_assert(ev.has<Position>());

	const Position *p = ev.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_create_entity_view_from_stage(void) {
	flecs::world world;
	world.component<Position>();

	auto stage = world.get_stage(0);

	world.readonly_begin();

	flecs::entity_view ev = stage.entity();
	test_assert(ev != 0);

	world.readonly_end();

	// Ensure we can use created ev out of stage
	auto ew = ev.mut(world);
	ew.set<Position>({10, 20});
	test_assert(ev.has<Position>());

	const Position *p = ev.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_set_template(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.set<Template<int>>({10, 20});

	const Template<int> *ptr = e.try_get<Template<int>>();
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_get_1_component_w_callback(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_get_2_components_w_callback(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_ensure_1_component_w_callback(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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
	p = e_1.try_get<Position>();
	test_int(p->x, 11);
	test_int(p->y, 22);

	p = e_2.try_get<Position>();
	test_int(p->x, 12);
	test_int(p->y, 24);

	test_bool(e_3.get([](const Position& p) {}), false);
}

void Entity_ensure_2_components_w_callback(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

	const Position* p = e_1.try_get<Position>();
	test_int(p->x, 11);
	test_int(p->y, 22);

	const Velocity* v = e_1.try_get<Velocity>();
	test_int(v->x, 4);
	test_int(v->y, 6);
}

void Entity_get_component_w_callback_nested(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_set_1_component_w_callback(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.insert([](Position& p){
			p.x = 10;
			p.y = 20;
		});

	test_assert(e.has<Position>());

	const Position *p = e.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_set_2_components_w_callback(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.insert([](Position& p, Velocity& v){
			p = {10, 20};
			v = {1, 2};
		});

	test_assert(e.has<Position>());

	const Position *p = e.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Velocity *v = e.try_get<Velocity>();
	test_assert(v != NULL);
	test_int(v->x, 1);
	test_int(v->y, 2);
}


void Entity_set_3_components_w_callback(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.insert([](Position& p, Velocity& v, Mass& m){
			p = {10, 20};
			v = {1, 2};
			m = {50};
		});

	test_assert(e.has<Position>());

	const Position *p = e.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Velocity *v = e.try_get<Velocity>();
	test_assert(v != NULL);
	test_int(v->x, 1);
	test_int(v->y, 2);

	const Mass *m = e.try_get<Mass>();
	test_assert(m != NULL);
	test_int(m->value, 50);
}

void Entity_defer_set_1_component(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	ecs.defer_begin();

	auto e = ecs.entity()
		.insert([](Position& p){
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

void Entity_defer_set_2_components(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	ecs.defer_begin();

	auto e = ecs.entity()
		.insert([](Position& p, Velocity& v){
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

void Entity_defer_set_3_components(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	ecs.defer_begin();

	auto e = ecs.entity()
		.insert([](Position& p, Velocity& v, Mass& m){
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

void Entity_set_2_w_on_set(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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
		.insert([](Position& p, Velocity& v){
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


void Entity_defer_set_2_w_on_set(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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
		.insert([](Position& p, Velocity& v){
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

void Entity_set_2_after_fluent(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.set<Mass>({50})
		.insert([](Position& p, Velocity& v){
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

void Entity_set_2_before_fluent(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.insert([](Position& p, Velocity& v){
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

void Entity_set_2_after_set_1(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	int called = 0;

	auto e = ecs.entity().set<Position>({5, 10});
	test_assert(e.has<Position>());

	test_bool(e.get([](const Position& p) {
		test_int(p.x, 5);
		test_int(p.y, 10);
	}), true);

	// Set both Position and Velocity
	e.insert([](Position& p, Velocity& v) {
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

void Entity_set_2_after_set_2(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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
	e.insert([](Position& p, Velocity& v) {
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

void Entity_with_self(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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
	auto q = ecs.query_builder<>()
		.with(Tag)
		.build();

	q.each([&](flecs::entity e) {
		test_assert(e.has(Tag));

		test_bool(e.get([&](const Self& s){
			test_assert(s.value == e);
		}), true);

		count ++;
	});

	test_int(count, 3);
}

void Entity_with_relation_type_self(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	struct Likes { };
	ecs.component<Likes>();

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
	auto q = ecs.query_builder<>()
		.with<Likes>(Bob)
		.build();

	q.each([&](flecs::entity e) {
		test_assert(e.has<Likes>(Bob));

		test_bool(e.get([&](const Self& s){
			test_assert(s.value == e);
		}), true);

		count ++;
	});

	test_int(count, 3);
}

void Entity_with_relation_self(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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
	auto q = ecs.query_builder<>()
		.with(Likes, Bob)
		.build();

	q.each([&](flecs::entity e) {
		test_assert(e.has(Likes, Bob));

		test_bool(e.get([&](const Self& s){
			test_assert(s.value == e);
		}), true);

		count ++;
	});

	test_int(count, 3);
}

void Entity_with_self_w_name(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto Tier1 = ecs.entity("Tier1").with([&]{
		auto Tier2 = ecs.entity("Tier2");
		Tier2.set<Self>({Tier2});
	});

	auto Tier2 = ecs.lookup("Tier2");
	test_assert(Tier2 != 0);

	test_assert(Tier2.has(Tier1));
}

void Entity_with_self_nested(void) {
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

void Entity_with_scope(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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
	auto q = ecs.query_builder<>()
		.with(flecs::ChildOf, parent)
		.build();

	q.each([&](flecs::entity e) {
		test_assert(e.has(flecs::ChildOf, parent));

		test_bool(e.get([&](const Self& s){
			test_assert(s.value == e);
		}), true);

		count ++;
	});

	test_int(count, 3);
}

void Entity_with_scope_nested(void) {
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

void Entity_with_scope_nested_same_name_as_parent(void) {
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

void Entity_no_recursive_lookup(void) {
	flecs::world ecs;

	auto p = ecs.entity("P");
	auto c = ecs.entity("C").child_of(p);
	auto gc = ecs.entity("GC").child_of(c);

	test_assert(c.lookup("GC") == gc);
	test_assert(c.lookup("C") == 0);
	test_assert(c.lookup("P") == 0);
}

void Entity_defer_new_w_name(void) {
	flecs::world ecs;

	flecs::entity e;

	ecs.defer([&]{
		e = ecs.entity("Foo");
		test_assert(e != 0);
	});

	test_assert(e.has<flecs::Identifier>(flecs::Name));
	test_str(e.name(), "Foo");
}

void Entity_defer_new_w_nested_name(void) {
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

void Entity_defer_new_w_scope_name(void) {
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

void Entity_defer_new_w_scope_nested_name(void) {
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

void Entity_defer_new_w_deferred_scope_nested_name(void) {
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

void Entity_defer_new_w_scope(void) {
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

void Entity_defer_new_w_with(void) {
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

void Entity_defer_new_w_name_scope_with(void) {
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

void Entity_defer_w_with_implicit_component(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_defer_suspend_resume(void) {
	flecs::world ecs;

	struct TagA { };
	struct TagB { };
	ecs.component<TagA>();
	ecs.component<TagB>();

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

void Entity_defer_ensure(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	flecs::entity e = world.entity();

	{
		world.defer_begin();
		Position& p = e.obtain<Position>();
		p.x = 10;
		p.y = 20;
		world.defer_end();
	}

	Position* p = e.try_get_mut<Position>();
	test_assert(p != nullptr);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_with_after_builder_method(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	struct Likes { };
	ecs.component<Likes>();

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

void Entity_with_before_builder_method(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	struct Likes { };
	ecs.component<Likes>();

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

void Entity_scope_after_builder_method(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	ecs.entity("P")
		.set<Position>({10, 20})
		.scope([&]{
			ecs.entity("C");
		});

	auto C = ecs.lookup("P::C");
	test_assert(C != 0);
}

void Entity_scope_before_builder_method(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	ecs.entity("P")
		.scope([&]{
			ecs.entity("C");
		})
		.set<Position>({10, 20});

	auto C = ecs.lookup("P::C");
	test_assert(C != 0);
}

void Entity_emplace(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity()
		.emplace<Position>(10.0f, 20.0f);

	test_assert(e.has<Position>());

	const Position *p = e.try_get<Position>();
	test_assert(p != NULL);
	test_int(p->x, 10);
	test_int(p->y, 20);
}

void Entity_entity_id_str(void) {
	flecs::world ecs;

	flecs::id id = ecs.entity("Foo");

	test_str("Foo", id.str());
}

void Entity_pair_id_str(void) {
	flecs::world ecs;

	flecs::id id = ecs.pair( ecs.entity("Rel"), ecs.entity("Obj") );

	test_str("(Rel,Obj)", id.str());
}

void Entity_role_id_str(void) {
	flecs::world ecs;

	flecs::id id = flecs::id(ecs, ECS_AUTO_OVERRIDE | ecs.entity("Foo"));

	test_str("AUTO_OVERRIDE|Foo", id.str());
}

void Entity_id_str_from_entity_view(void) {
	flecs::world ecs;

	flecs::entity_view id = ecs.entity("Foo");

	test_str("Foo", id.str());
}

void Entity_id_str_from_entity(void) {
	flecs::world ecs;

	flecs::entity id = ecs.entity("Foo");

	test_str("Foo", id.str());
}

void Entity_null_entity(void) {
	flecs::entity e = flecs::entity::null();
	test_assert(e.id() == 0);
}

void Entity_null_entity_w_world(void) {
	flecs::world ecs;

	flecs::entity e = flecs::entity::null(ecs);
	test_assert(e.id() == 0);
	test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_null_entity_w_0(void) {
	flecs::entity e = flecs::entity(static_cast<flecs::id_t>(0));
	test_assert(e.id() == 0);
	test_assert(e.world().c_ptr() == nullptr);
}

void Entity_null_entity_w_world_w_0(void) {
	flecs::world ecs;

	flecs::entity e = flecs::entity::null(ecs);
	test_assert(e.id() == 0);
	test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_entity_view_null_entity(void) {
	flecs::entity_view e = flecs::entity::null();
	test_assert(e.id() == 0);
}


void Entity_entity_view_null_entity_w_world(void) {
	flecs::world ecs;

	flecs::entity_view e = flecs::entity::null(ecs);
	test_assert(e.id() == 0);
	test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_entity_view_null_entity_w_0(void) {
	flecs::entity_view e = flecs::entity(static_cast<flecs::id_t>(0));
	test_assert(e.id() == 0);
	test_assert(e.world().c_ptr() == nullptr);
}

void Entity_entity_view_null_entity_w_world_w_0(void) {
	flecs::world ecs;

	flecs::entity_view e = flecs::entity::null(ecs);
	test_assert(e.id() == 0);
	test_assert(e.world().c_ptr() == ecs.c_ptr());
}

void Entity_is_wildcard(void) {
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

void Entity_has_id_t(void) {
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

void Entity_has_pair_id_t(void) {
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

void Entity_has_pair_id_t_w_type(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_has_id(void) {
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

void Entity_has_pair_id(void) {
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

void Entity_has_pair_id_w_type(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_has_wildcard_id(void) {
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

void Entity_has_wildcard_pair_id(void) {
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

void Entity_owns_id_t(void) {
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

void Entity_owns_pair_id_t(void) {
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

void Entity_owns_pair_id_t_w_type(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_owns_id(void) {
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

void Entity_owns_pair_id(void) {
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

void Entity_owns_wildcard_id(void) {
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

void Entity_owns_wildcard_pair(void) {
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

void Entity_owns_pair_id_w_type(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_id_from_world(void) {
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

void Entity_id_pair_from_world(void) {
	flecs::world ecs;

	auto rel = ecs.entity();
	test_assert(rel != 0);

	auto obj = ecs.entity();
	test_assert(obj != 0);    

	flecs::id id_1 = ecs.id(rel, obj);
	test_assert(id_1 != 0);
	test_assert(id_1.first() == rel);
	test_assert(id_1.second() == obj);
	test_assert(id_1.world() == ecs);
	test_bool(id_1.is_pair(), true);
	test_bool(id_1.is_wildcard(), false);

	flecs::id id_2 = ecs.id(rel, flecs::Wildcard);
	test_assert(id_2 != 0);
	test_assert(id_2.first() == rel);
	test_assert(id_2.second() == flecs::Wildcard);
	test_assert(id_2.world() == ecs);
	test_bool(id_2.is_pair(), true);
	test_bool(id_2.is_wildcard(), true);
}

void Entity_id_default_from_world(void) {
	flecs::world ecs;

	flecs::id id_default = ecs.id();
	test_assert(id_default == 0);
}

void Entity_is_a(void) {
	flecs::world world;

	auto base = world.entity();

	auto e = world.entity().is_a(base);

	test_assert(e.has(flecs::IsA, base));
}

void Entity_is_a_w_type(void) {
	flecs::world world;

	struct Prefab { };

	auto base = world.entity<Prefab>();

	auto e = world.entity().is_a<Prefab>();

	test_assert(e.has(flecs::IsA, base));
	test_assert(e.has_second<Prefab>(flecs::IsA));
}

void Entity_child_of(void) {
	flecs::world world;

	auto base = world.entity();

	auto e = world.entity().child_of(base);

	test_assert(e.has(flecs::ChildOf, base));
}

void Entity_child_of_w_type(void) {
	flecs::world world;

	auto base = world.entity<Parent>();

	auto e = world.entity().child_of<Parent>();

	test_assert(e.has(flecs::ChildOf, base));
	test_assert(e.has_second<Parent>(flecs::ChildOf));
}

void Entity_child(void) {
	flecs::world world;

	auto base = world.entity();

	auto e = base.child();

	test_assert(e.has(flecs::ChildOf, base));
}

void Entity_child_custom_rel(void) {
	flecs::world world;

	auto r = world.entity();

	auto base = world.entity();

	auto e = base.child(r);

	test_assert(e.has(r, base));
}

void Entity_child_custom_type(void) {
	flecs::world world;

	struct R { };
	world.component<R>();

	auto base = world.entity();

	auto e = base.child<R>();

	test_assert(e.has<R>(base));
}

void Entity_slot_of(void) {
	flecs::world world;

	auto base = world.prefab();
	auto base_child = world.prefab()
		.child_of(base)
		.slot_of(base);
    
	test_assert(base_child.has(flecs::SlotOf, base));

	auto inst = world.entity().is_a(base);
	test_assert(inst.has(base_child, flecs::Wildcard));
}

void Entity_slot_of_w_type(void) {
	flecs::world world;

	auto base = world.prefab<Parent>();
	auto base_child = world.prefab()
		.child_of(base)
		.slot_of<Parent>();
    
	test_assert(base_child.has(flecs::SlotOf, base));

	auto inst = world.entity().is_a(base);
	test_assert(inst.has(base_child, flecs::Wildcard));
}

void Entity_slot(void) {
	flecs::world world;

	auto base = world.prefab();
	auto base_child = world.prefab()
		.child_of(base).slot();
    
	test_assert(base_child.has(flecs::SlotOf, base));

	auto inst = world.entity().is_a(base);
	test_assert(inst.has(base_child, flecs::Wildcard));
}

void Entity_id_get_entity(void) {
	flecs::world world;

	auto e = world.entity();

	auto id = world.id(e);

	test_assert(id.entity() == e);
}

void Entity_each_in_stage(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);
	
	struct Obj { };
	world.component<Obj>();

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

void Entity_iter_recycled_parent(void) {
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

void Entity_get_lambda_from_stage(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

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

void Entity_default_ctor(void) {
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

void Entity_get_obj_by_template(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	flecs::entity e1 = ecs.entity();
	flecs::entity o1 = ecs.entity();
	flecs::entity o2 = ecs.entity();

	e1.add<Rel>(o1);
	e1.add<Rel>(o2);

	test_assert(o1 == e1.target<Rel>());
	test_assert(o1 == e1.target<Rel>(0));
	test_assert(o2 == e1.target<Rel>(1));
}

void Entity_create_named_twice_deferred(void) {
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

void Entity_clone(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);
	ecs.component<PositionInitialized>();

	PositionInitialized v(10, 20);

	auto src = ecs.entity().add<Tag>().set<PositionInitialized>(v);
	auto dst = src.clone(false);
	test_assert(dst.has<Tag>());
	test_assert(dst.has<PositionInitialized>());

	const PositionInitialized *ptr = dst.try_get<PositionInitialized>();
	test_assert(ptr != NULL);
	test_int(ptr->x, -1);
	test_int(ptr->y, -1);
}

void Entity_clone_w_value(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);
	ecs.component<PositionInitialized>();

	PositionInitialized v = {10, 20};

	auto src = ecs.entity().add<Tag>().set<PositionInitialized>(v);
	auto dst = src.clone();
	test_assert(dst.has<Tag>());
	test_assert(dst.has<PositionInitialized>());

	const PositionInitialized *ptr = dst.try_get<PositionInitialized>();
	test_assert(ptr != NULL);
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_clone_to_existing(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);
	ecs.component<PositionInitialized>();

	PositionInitialized v = {10, 20};

	auto src = ecs.entity().add<Tag>().set<PositionInitialized>(v);
	auto dst = ecs.entity();
	auto result = src.clone(true, dst);
	test_assert(result == dst);

	test_assert(dst.has<Tag>());
	test_assert(dst.has<PositionInitialized>());

	const PositionInitialized *ptr = dst.try_get<PositionInitialized>();
	test_assert(ptr != NULL);
	test_int(ptr->x, 10);
	test_int(ptr->y, 20);
}

void Entity_set_doc_name(void) {
	flecs::world ecs;

	auto e = ecs.entity("foo_bar")
		.set_doc_name("Foo Bar");

	test_str(e.name(), "foo_bar");
	test_str(e.doc_name(), "Foo Bar");
}

void Entity_set_doc_brief(void) {
	flecs::world ecs;

	auto e = ecs.entity("foo_bar")
		.set_doc_brief("Foo Bar");

	test_str(e.name(), "foo_bar");
	test_str(e.doc_brief(), "Foo Bar");
}

void Entity_set_doc_detail(void) {
	flecs::world ecs;

	auto e = ecs.entity("foo_bar")
		.set_doc_detail("Foo Bar");

	test_str(e.name(), "foo_bar");
	test_str(e.doc_detail(), "Foo Bar");
}

void Entity_set_doc_link(void) {
	flecs::world ecs;

	auto e = ecs.entity("foo_bar")
		.set_doc_link("Foo Bar");

	test_str(e.name(), "foo_bar");
	test_str(e.doc_link(), "Foo Bar");
}

void Entity_entity_w_root_name(void) {
	flecs::world ecs;

	auto e = ecs.entity("::foo");
	test_str(e.name(), "foo");
	test_str(e.path(), "::foo");
}

void Entity_entity_w_root_name_from_scope(void) {
	flecs::world ecs;

	auto p = ecs.entity("parent");
	ecs.set_scope(p);
	auto e = ecs.entity("::foo");
	ecs.set_scope(0);
    
	test_str(e.name(), "foo");
	test_str(e.path(), "::foo");
}

void Entity_entity_w_type(void) {
	flecs::world ecs;

	auto e = ecs.entity<EntityType>();

	test_str(e.name().c_str(), "EntityType");
	test_str(e.path().c_str(), "::EntityType");
	test_assert(!e.has<flecs::Component>());

	auto e_2 = ecs.entity<EntityType>();
	test_assert(e == e_2);
}

void Entity_prefab_w_type(void) {
	flecs::world ecs;

	auto e = ecs.prefab<EntityType>();

	test_str(e.name().c_str(), "EntityType");
	test_str(e.path().c_str(), "::EntityType");
	test_assert(!e.has<flecs::Component>());
	test_assert(e.has(flecs::Prefab));

	auto e_2 = ecs.entity<EntityType>();
	test_assert(e == e_2);
}

void Entity_prefab_hierarchy_w_types(void) {
	flecs::world ecs;

	auto turret = ecs.prefab<FlecsTestTurret>();
	auto turret_base = ecs.prefab<FlecsTestTurret::FlecsTestBase>();

	test_assert(turret != 0);
	test_assert(turret_base != 0);
	test_assert(turret_base.has(flecs::ChildOf, turret));
    
	test_str(turret.path().c_str(), "::FlecsTestTurret");
	test_str(turret_base.path().c_str(), "::FlecsTestTurret::FlecsTestBase");

	test_str(turret.symbol(), "FlecsTestTurret");
	test_str(turret_base.symbol(), "FlecsTestTurret.FlecsTestBase");

	auto railgun = ecs.prefab<FlecsTestRailgun>().is_a<FlecsTestTurret>();
	auto railgun_base = railgun.lookup("FlecsTestBase");
	auto railgun_head = ecs.prefab<FlecsTestRailgun::FlecsTestHead>();
	auto railgun_beam = ecs.prefab<FlecsTestRailgun::FlecsTestBeam>();

	test_assert(railgun != 0);
	test_assert(railgun_base != 0);
	test_assert(railgun_head != 0);
	test_assert(railgun_beam != 0);
	test_assert(railgun_base.has(flecs::ChildOf, railgun));
	test_assert(railgun_head.has(flecs::ChildOf, railgun));
	test_assert(railgun_beam.has(flecs::ChildOf, railgun));

	test_str(railgun.path().c_str(), "::FlecsTestRailgun");
	test_str(railgun_base.path().c_str(), "::FlecsTestRailgun::FlecsTestBase");
	test_str(railgun_head.path().c_str(), "::FlecsTestRailgun::FlecsTestHead");
	test_str(railgun_beam.path().c_str(), "::FlecsTestRailgun::FlecsTestBeam");

	test_str(railgun.symbol().c_str(), "FlecsTestRailgun");
	test_str(railgun_head.symbol().c_str(), "FlecsTestRailgun.FlecsTestHead");
	test_str(railgun_beam.symbol().c_str(), "FlecsTestRailgun.FlecsTestBeam");
}

void Entity_prefab_hierarchy_w_root_types(void) {
	flecs::world ecs;

	auto turret = ecs.prefab<FlecsTestTurret>();
	auto turret_base = ecs.prefab<FlecsTestBase>().child_of<FlecsTestTurret>();

	test_assert(turret != 0);
	test_assert(turret_base != 0);
	test_assert(turret_base.has(flecs::ChildOf, turret));

	test_str(turret.path().c_str(), "::FlecsTestTurret");
	test_str(turret_base.path().c_str(), "::FlecsTestTurret::FlecsTestBase");

	test_str(turret.symbol(), "FlecsTestTurret");
	test_str(turret_base.symbol(), "FlecsTestBase");

	auto inst = ecs.entity().is_a<FlecsTestTurret>();
	test_assert(inst != 0);

	auto inst_base = inst.lookup("FlecsTestBase");
	test_assert(inst_base != 0);
}

void Entity_prefab_hierarchy_w_child_override(void) {
	flecs::world ecs;

	struct Foo {};
	struct Bar {};
	ecs.component<Foo>();
	ecs.component<Bar>();

	auto t = ecs.prefab<FlecsTestTurret>();
	auto tb = ecs.prefab<FlecsTestTurret::FlecsTestBase>().add<Foo>();
	test_assert(t != 0);
	test_assert(tb != 0);

	auto r = ecs.prefab<FlecsTestRailgun>().is_a<FlecsTestTurret>();
	auto rb = ecs.prefab<FlecsTestRailgun::FlecsTestBase>().add<Bar>();
	test_assert(r != 0);
	test_assert(rb != 0);

	auto i = ecs.entity().is_a<FlecsTestRailgun>();
	test_assert(i != 0);
	auto ib = i.lookup("FlecsTestBase");
	test_assert(ib != 0);

	test_assert(ib.has<Foo>());
	test_assert(ib.has<Bar>());
}

void Entity_entity_w_nested_type(void) {
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

void Entity_entity_array(void) {
	struct TagA {};
	struct TagB {};

	flecs::world ecs;
	ecs.component<TagA>();
	ecs.component<TagB>();

	flecs::to_array({
		ecs.entity(),
		ecs.entity(),
		ecs.entity()
	}).each([](flecs::entity e) { e.add<TagA>().add<TagB>(); });

	test_int( ecs.count<TagA>(), 3 );
	test_int( ecs.count<TagB>(), 3 );
}

void Entity_entity_w_type_defer(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	ecs.defer_begin();
	auto e = ecs.entity<Tag>();
	ecs.defer_end();

	test_str(e.name(), "Tag");
	test_str(e.symbol(), "Tag");
	test_assert(ecs.id<Tag>() == e);
}

void Entity_add_if_true_T(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity();

	e.add_if<Tag>(true);
	test_assert( e.has<Tag>());
}

void Entity_add_if_false_T(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity();

	e.add_if<Tag>(false);
	test_assert( !e.has<Tag>());

	e.add<Tag>();
	test_assert( e.has<Tag>());
	e.add_if<Tag>(false);
	test_assert( !e.has<Tag>());
}

void Entity_add_if_true_id(void) {
	flecs::world ecs;

	auto e = ecs.entity();
	auto t = ecs.entity();

	e.add_if(true, t);
	test_assert( e.has(t));
}

void Entity_add_if_false_id(void) {
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

void Entity_add_if_true_R_O(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);
	
	struct Obj { };
	ecs.component<Obj>();

	auto e = ecs.entity();

	e.add_if<Rel, Obj>(true);
	test_assert( (e.has<Rel, Obj>()) );
}

void Entity_add_if_false_R_O(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	struct Obj { };
	ecs.component<Obj>();

	auto e = ecs.entity();

	e.add_if<Rel, Obj>(false);
	test_assert( (!e.has<Rel, Obj>()));
	e.add<Rel, Obj>();
	test_assert( (e.has<Rel, Obj>()));
	e.add_if<Rel, Obj>(false);
	test_assert( (!e.has<Rel, Obj>()));
}

void Entity_add_if_true_R_o(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity();
	auto o = ecs.entity();

	e.add_if<Rel>(true, o);
	test_assert( e.has<Rel>(o));
}

void Entity_add_if_false_R_o(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	auto e = ecs.entity();
	auto o = ecs.entity();

	e.add_if<Rel>(false, o);
	test_assert( !e.has<Rel>(o));
	e.add<Rel>(o);
	test_assert( e.has<Rel>(o));
	e.add_if<Rel>(false, o);
	test_assert( !e.has<Rel>(o));
}

void Entity_add_if_true_r_o(void) {
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

void Entity_add_if_exclusive_r_o(void) {
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

void Entity_add_if_exclusive_R_o(void) {
	flecs::world ecs;

	struct First { };

	ecs.component<First>().add(flecs::Exclusive);

	auto e = ecs.entity();
	auto o_1 = ecs.entity();
	auto o_2 = ecs.entity();

	e.add<First>(o_1);
	test_assert(e.has<First>(o_1));

	e.add_if<First>(true, o_2);
	test_assert(!e.has<First>(o_1));
	test_assert(e.has<First>(o_2));

	e.add_if<First>(false, o_1);
	test_assert(!e.has<First>(o_1));
	test_assert(!e.has<First>(o_2));
}

void Entity_add_if_exclusive_R_O(void) {
	flecs::world ecs;
	
	struct O_1 { };
	struct O_2 { };

	ecs.component<R>().add(flecs::Exclusive);
	ecs.component<O_1>();
	ecs.component<O_2>();

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

void Entity_add_if_pair_w_0_object(void) {
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


void Entity_children_w_custom_relation(void) {
	flecs::world ecs;

	flecs::entity rel = ecs.entity();

	flecs::entity parent = ecs.entity();
	flecs::entity child_1 = ecs.entity().add(rel, parent);
	flecs::entity child_2 = ecs.entity().add(rel, parent);
	ecs.entity().child_of(parent);

	bool child_1_found = false;
	bool child_2_found = false;
	int32_t count = 0;

	parent.children(rel, [&](flecs::entity child) {
		if (child == child_1) {
			child_1_found = true;
		} else if (child == child_2) {
			child_2_found = true;
		}
		count ++;
	});

	test_int(count, 2);
	test_assert(child_1_found == true);
	test_assert(child_2_found == true);
}

void Entity_children_w_custom_relation_type(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	flecs::entity parent = ecs.entity();
	flecs::entity child_1 = ecs.entity().add<Rel>(parent);
	flecs::entity child_2 = ecs.entity().add<Rel>(parent);
	ecs.entity().child_of(parent);

	bool child_1_found = false;
	bool child_2_found = false;
	int32_t count = 0;

	parent.children<Rel>([&](flecs::entity child) {
		if (child == child_1) {
			child_1_found = true;
		} else if (child == child_2) {
			child_2_found = true;
		}
		count ++;
	});

	test_int(count, 2);
	test_assert(child_1_found == true);
	test_assert(child_2_found == true);
}

void Entity_children_w_this(void) {
	flecs::world ecs;

	int32_t count = 0;
	ecs.entity(flecs::This).children([&](flecs::entity) {
		count ++;
	});
	test_assert(count == 0);
}

void Entity_children_w_wildcard(void) {
	flecs::world ecs;

	int32_t count = 0;
	ecs.entity(flecs::Wildcard).children([&](flecs::entity) {
		count ++;
	});
	test_assert(count == 0);
}

void Entity_children_w_any(void) {
	flecs::world ecs;

	int32_t count = 0;
	ecs.entity(flecs::Any).children([&](flecs::entity) {
		count ++;
	});
	test_assert(count == 0);
}

// @TODO
void Entity_children_from_root(void) {
	flecs::world ecs;

	int32_t count = 0;
	ecs.entity(0).children([&](flecs::entity e) {
		test_assert(e == ecs.entity("flecs"));
		count ++;
	});
	test_assert(count == 1);
}

// @TODO
void Entity_children_from_root_world(void) {
	flecs::world ecs;

	int32_t count = 0;
	ecs.children([&](flecs::entity e) {
		test_assert(e == ecs.entity("flecs"));
		count ++;
	});
	test_assert(count == 1);
}

void Entity_get_depth(void) {
	flecs::world world;

	flecs::entity e1 = world.entity();
	flecs::entity e2 = world.entity().child_of(e1);
	flecs::entity e3 = world.entity().child_of(e2);
	flecs::entity e4 = world.entity().child_of(e3);

	test_int(0, e1.depth(flecs::ChildOf));
	test_int(1, e2.depth(flecs::ChildOf));
	test_int(2, e3.depth(flecs::ChildOf));
	test_int(3, e4.depth(flecs::ChildOf));
}

void Entity_get_depth_w_type(void) {
	flecs::world world;

	world.component<Rel>().add(flecs::Traversable);

	flecs::entity e1 = world.entity();
	flecs::entity e2 = world.entity().add<Rel>(e1);
	flecs::entity e3 = world.entity().add<Rel>(e2);
	flecs::entity e4 = world.entity().add<Rel>(e3);

	test_int(0, e1.depth<Rel>());
	test_int(1, e2.depth<Rel>());
	test_int(2, e3.depth<Rel>());
	test_int(3, e4.depth<Rel>());
}

void Entity_to_view(void) {
	flecs::world world;

	flecs::entity e = world.entity();
	flecs::entity_view ev = e.view();
	test_assert(e == ev);
}

void Entity_to_view_from_stage(void) {
	flecs::world world;

	auto stage = world.get_stage(0);

	flecs::entity e = stage.entity();
	flecs::entity_view ev = e.view();
	test_assert(e == ev);
	test_assert(e.world() == stage);
	test_assert(ev.world() == world);
}

void Entity_set_alias(void) {
	flecs::world world;

	flecs::entity e = world.entity("parent::child");
	e.set_alias("parent_child");

	test_assert(e == world.lookup("parent::child"));
	test_assert(e == world.lookup("parent_child"));
}

void Entity_emplace_w_observer(void) {
	flecs::world ecs;
	RegisterTestTypeComponents(ecs);

	ecs.observer<Position>()
		.event(flecs::OnAdd)
		.each([](flecs::entity e, Position&) {
			e.emplace<Velocity>(1.0f, 2.0f);
		});

	auto e = ecs.entity()
		.emplace<Position>(10.0f, 20.0f);

	test_assert(e.has<Position>());
	test_assert(e.has<Velocity>());
	test_int(e.try_get<Velocity>()->x, 1);
	test_int(e.try_get<Velocity>()->y, 2);
	test_int(e.try_get<Position>()->x, 10);
	test_int(e.try_get<Position>()->y, 20);
}

void Entity_scoped_world(void) {
	flecs::world world;

	flecs::entity parent = world.entity();
	flecs::entity child = parent.scope().entity();
	test_assert(child.parent() == parent);
}

void Entity_entity_lookup_not_recursive(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity foo = world.scope(parent).entity("foo");

	test_assert(child.lookup("foo") == 0);
	test_assert(child.lookup("foo", true) == foo);
}

void Entity_world_lookup_not_recursive(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity foo = world.scope(parent).entity("foo");

	test_assert(world.scope(child).lookup("foo") == foo);
	test_assert(world.scope(child).lookup("foo", "::", "::", false) == 0);
}

void Entity_world_lookup_custom_sep(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity foo = world.scope(parent).entity("foo");

	test_assert(world.lookup("parent.child", ".") == child);
	test_assert(world.lookup("parent.foo", ".") == foo);
}

void Entity_world_lookup_custom_root_sep(void) {
	flecs::world world;

	flecs::entity parent = world.entity("parent");
	flecs::entity child = world.scope(parent).entity("child");
	flecs::entity foo = world.scope(parent).entity("foo");

	test_assert(world.lookup(".parent.child", ".", ".") == child);
	test_assert(world.lookup(".parent.foo", ".", ".") == foo);
}

void Entity_depends_on(void) {
	flecs::world world;

	flecs::entity a = world.entity();
	flecs::entity b = world.entity().depends_on(a);
	test_assert(b.has(flecs::DependsOn, a));
}

void Entity_depends_on_type(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	flecs::entity b = world.entity().depends_on<Position>();
	test_assert(b.has(flecs::DependsOn, world.id<Position>()));
}

void Entity_const_entity_add_remove(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	const flecs::entity e = world.entity();

	e.add<Tag>();
	test_assert(e.has<Tag>());

	e.remove<Tag>();
	test_assert(!e.has<Tag>());
}

void Entity_const_entity_set(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	const flecs::entity e = world.entity();

	e.set<Position>({10, 20});
	test_assert(e.try_get<Position>() != nullptr);
	test_int(e.try_get<Position>()->x, 10);
	test_int(e.try_get<Position>()->y, 20);
}

void Entity_const_entity_get_mut(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	const flecs::entity e = world.entity();

	Position *p = e.try_get_mut<Position>();
	test_assert(p == nullptr);
	test_assert(!e.has<Position>());

	e.add<Position>();
	p = e.try_get_mut<Position>();
	test_assert(p != nullptr);
	test_assert(e.has<Position>());

	e.modified<Position>();
}

void Entity_const_entity_ensure(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	const flecs::entity e = world.entity();

	e.obtain<Position>();
	test_assert(e.has<Position>());
	e.modified<Position>();
}

void Entity_const_entity_destruct(void) {
	flecs::world world;

	const flecs::entity e = world.entity();

	e.destruct();
	test_assert(!e.is_alive());
}

void Entity_const_entity_emit_after_build(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	const flecs::entity e = world.entity();

	int32_t count = 0;
	e.observe([&](Velocity& v) {
		test_int(v.x, 1);
		test_int(v.y, 2);
		count ++;
	});

	e.set<Position>({10, 20}).emit<Velocity>({1, 2});

	test_assert(e.try_get<Position>() != nullptr);
	test_int(e.try_get<Position>()->x, 10);
	test_int(e.try_get<Position>()->y, 20);

	test_int(count, 1);
}

void Entity_const_entity_set_doc(void) {
	flecs::world world;

	const flecs::entity e = world.entity();

	e.set_doc_name("name");
	e.set_doc_color("color");
	e.set_doc_detail("detail");
	e.set_doc_brief("brief");
	e.set_doc_link("link");

	test_str(e.doc_name(), "name");
	test_str(e.doc_color(), "color");
	test_str(e.doc_detail(), "detail");
	test_str(e.doc_brief(), "brief");
	test_str(e.doc_link(), "link");
}

void Entity_set_sparse(void) {
	flecs::world world;

	world.component<Velocity>().add(flecs::Sparse);

	flecs::entity e = world.entity().set(Velocity{1, 2});

	test_assert(e.has<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}

void Entity_insert_1_sparse(void) {
	flecs::world world;

	world.component<Velocity>().add(flecs::Sparse);

	flecs::entity e = world.entity().insert([](Velocity& v) {
		v.x = 1;
		v.y = 2;
	});

	test_assert(e.has<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}

void Entity_insert_2_w_1_sparse(void) {
	flecs::world world;

	world.component<Position>();
	world.component<Velocity>().add(flecs::Sparse);

	flecs::entity e = world.entity().insert([](Position& p, Velocity& v) {
		p.x = 10;
		p.y = 20;
		v.x = 1;
		v.y = 2;
	});

	test_assert(e.has<Position>());
	test_assert(e.has<Velocity>());

	const Position *p = e.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}

void Entity_insert_1_dont_fragment(void) {
	flecs::world world;

	world.component<Velocity>().add(flecs::DontFragment);

	flecs::entity e = world.entity().insert([](Velocity& v) {
		v.x = 1;
		v.y = 2;
	});

	test_assert(e.has<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}

void Entity_insert_2_w_1_dont_fragment(void) {
	flecs::world world;

	world.component<Position>();
	world.component<Velocity>().add(flecs::DontFragment);

	flecs::entity e = world.entity().insert([](Position& p, Velocity& v) {
		p.x = 10;
		p.y = 20;
		v.x = 1;
		v.y = 2;
	});

	test_assert(e.has<Position>());
	test_assert(e.has<Velocity>());

	const Position *p = e.try_get<Position>();
	test_int(p->x, 10);
	test_int(p->y, 20);

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}

void Entity_emplace_sparse(void) {
	flecs::world world;

	world.component<Velocity>().add(flecs::Sparse);

	flecs::entity e = world.entity().emplace<Velocity>(1.0f, 2.0f);

	test_assert(e.has<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}

void Entity_override_sparse(void) {
	flecs::world world;

	world.component<Velocity>().add(flecs::Sparse);

	flecs::entity base = world.entity().set(Velocity{1, 2});

	flecs::entity e = world.entity().is_a(base);

	test_assert(e.has<Velocity>());
	test_assert(e.owns<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);
}

void Entity_delete_w_override_sparse(void) {
	flecs::world world;

	world.component<Velocity>().add(flecs::Sparse);

	flecs::entity base = world.entity().set(Velocity{1, 2});

	flecs::entity e = world.entity().is_a(base);

	test_assert(e.has<Velocity>());
	test_assert(e.owns<Velocity>());

	const Velocity *v = e.try_get<Velocity>();
	test_int(v->x, 1);
	test_int(v->y, 2);

	e.destruct();
}

void Entity_iter_type(void) {
	flecs::world world;
	RegisterTestTypeComponents(world);

	flecs::entity e = world.entity().add<Position>().add<Velocity>();

	int32_t count = 0;
	bool pos_found = false;
	bool velocity_found = false;

	for (auto id : e.type()) {
		count ++;
		if (id == world.id<Position>()) {
			pos_found = true;
		}
		if (id == world.id<Velocity>()) {
			velocity_found = true;
		}
	}

	test_int(count, 2);
	test_assert(pos_found == true);
	test_assert(velocity_found == true);
}

void Entity_iter_empty_type(void) {
	flecs::world world;

	flecs::entity e = world.entity();

	int32_t count = 0;

	for (auto id : e.type()) {
		test_assert(id != 0);
		count ++;
	}

	test_int(count, 0);
}

void Entity_untyped_component_use_low_id(void) {
	flecs::world world;
    
	flecs::untyped_component c = world.component("test_low_id_comp");
	test_assert(c.is_valid());
	test_assert(c < FLECS_HI_COMPONENT_ID);
}

void Entity_add_remove_enum_component(void) {
	flecs::world world;
	world.component<Color>();

	flecs::entity e = world.entity();

	e.set<Color>(Blue);
	test_assert(e.has<Color>());

	{
		const Color *c = e.try_get<Color>();
		test_assert(c != nullptr);
		test_assert(*c == Blue);
	}

	e.set<Color>(Green);
	test_assert(e.has<Color>());

	{
		const Color *c = e.try_get<Color>();
		test_assert(c != nullptr);
		test_assert(*c == Green);
	}

	e.remove<Color>();
	test_assert(!e.has<Color>());
}

void Entity_on_replace_w_set(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	e.set(Position{10, 20});
	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_on_replace_w_set_existing(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	e.set(Position{10, 20});
	test_int(invoked, 1);

	e.set(Position{11, 21});
	test_int(invoked, 2);

	const Position& p = e.get<Position>();
	test_int(p.x, 11);
	test_int(p.y, 21);
}

void Entity_on_replace_w_assign(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	e.assign(Position{10, 20});
	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_on_replace_w_assign_existing(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	e.assign(Position{10, 20});
	test_int(invoked, 1);

	e.assign(Position{11, 21});
	test_int(invoked, 2);

	const Position& p = e.get<Position>();
	test_int(p.x, 11);
	test_int(p.y, 21);
}

void Entity_defer_on_replace_w_set(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity();
	test_int(invoked, 0);

	world.defer_begin();
	e.set(Position{10, 20});
	test_int(invoked, 0);
	world.defer_end();
	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_defer_on_replace_w_set_twice(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity();
	test_int(invoked, 0);

	world.defer_begin();
	e.set(Position{10, 20});
	test_int(invoked, 0);
	e.set(Position{11, 21});
	test_int(invoked, 0);
	world.defer_end();
	test_int(invoked, 2);

	const Position& p = e.get<Position>();
	test_int(p.x, 11);
	test_int(p.y, 21);
}
void Entity_defer_on_replace_w_set_existing(void) {
    flecs::world world;

    int invoked = 0;

    world.component<Position>()
        .on_add([&](Position& p) {
            p.x = 0; p.y = 0;
        })
        .on_replace([&](Position& prev, Position& next) {
            switch(invoked) {
            case 0:
                test_int(prev.x, 0); test_int(prev.y, 0);
                test_int(next.x, 10); test_int(next.y, 20);
                break;
            case 1:
                test_int(prev.x, 10); test_int(prev.y, 20);
                test_int(next.x, 11); test_int(next.y, 21);
                break;
            }
            invoked ++;
        });

    flecs::entity e = world.entity().add<Position>();
    test_int(invoked, 0);

    world.defer_begin();
    e.set(Position{10, 20});
    test_int(invoked, 1);
    world.defer_end();
    test_int(invoked, 1);

    const Position& p = e.get<Position>();
    test_int(p.x, 10);
    test_int(p.y, 20);
}

void Entity_defer_on_replace_w_set_existing_twice(void) {
    flecs::world world;

    int invoked = 0;

    world.component<Position>()
        .on_add([&](Position& p) {
            p.x = 0; p.y = 0;
        })
        .on_replace([&](Position& prev, Position& next) {
            switch(invoked) {
            case 0:
                test_int(prev.x, 0); test_int(prev.y, 0);
                test_int(next.x, 10); test_int(next.y, 20);
                break;
            case 1:
                test_int(prev.x, 10); test_int(prev.y, 20);
                test_int(next.x, 11); test_int(next.y, 21);
                break;
            }
            invoked ++;
        });

    flecs::entity e = world.entity().add<Position>();
    test_int(invoked, 0);

    world.defer_begin();
    e.set(Position{10, 20});
    test_int(invoked, 1);
    e.set(Position{11, 21});
    test_int(invoked, 2);
    world.defer_end();
    test_int(invoked, 2);

    const Position& p = e.get<Position>();
    test_int(p.x, 11);
    test_int(p.y, 21);
}

void Entity_defer_on_replace_w_set_batched(void) {
    flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

    int invoked = 0;

    world.component<Position>()
        .on_add([&](Position& p) {
            p.x = 0; p.y = 0;
        })
        .on_replace([&](Position& prev, Position& next) {
            switch(invoked) {
            case 0:
                test_int(prev.x, 0); test_int(prev.y, 0);
                test_int(next.x, 10); test_int(next.y, 20);
                break;
            case 1:
                test_int(prev.x, 10); test_int(prev.y, 20);
                test_int(next.x, 11); test_int(next.y, 21);
                break;
            }
            invoked ++;
        });

    flecs::entity e = world.entity();
    test_int(invoked, 0);

    world.defer_begin();
    e.set(Position{10, 20});
    e.add<Velocity>();
    test_int(invoked, 0);
    world.defer_end();
    test_int(invoked, 1);

    const Position& p = e.get<Position>();
    test_int(p.x, 10);
    test_int(p.y, 20);

    test_assert(e.has<Velocity>());
}

void Entity_defer_on_replace_w_set_batched_twice(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity();
	test_int(invoked, 0);

	world.defer_begin();
	e.set(Position{10, 20});
	e.set(Position{11, 21});
	e.add<Velocity>();
	test_int(invoked, 0);
	world.defer_end();
	test_int(invoked, 2);

	const Position& p = e.get<Position>();
	test_int(p.x, 11);
	test_int(p.y, 21);

	test_assert(e.has<Velocity>());
}

void Entity_defer_on_replace_w_set_batched_existing(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	world.defer_begin();
	e.set(Position{10, 20});
	e.add<Velocity>();
	test_int(invoked, 1);
	world.defer_end();
	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);

	test_assert(e.has<Velocity>());
}

void Entity_defer_on_replace_w_set_batched_existing_twice(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	world.defer_begin();
	e.set(Position{10, 20});
	test_int(invoked, 1);
	e.set(Position{11, 21});
	test_int(invoked, 2);
	e.add<Velocity>();
	world.defer_end();
	test_int(invoked, 2);

	const Position& p = e.get<Position>();
	test_int(p.x, 11);
	test_int(p.y, 21);

	test_assert(e.has<Velocity>());
}

void Entity_defer_on_replace_w_assign_existing(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	world.defer_begin();
	e.assign(Position{10, 20});
	test_int(invoked, 1);
	world.defer_end();
	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);
}

void Entity_defer_on_replace_w_assign_existing_twice(void) {
	flecs::world world;

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	world.defer_begin();
	e.assign(Position{10, 20});
	test_int(invoked, 1);
	e.assign(Position{11, 21});
	test_int(invoked, 2);
	world.defer_end();
	test_int(invoked, 2);

	const Position& p = e.get<Position>();
	test_int(p.x, 11);
	test_int(p.y, 21);
}

void Entity_defer_on_replace_w_assign_batched_existing(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	world.defer_begin();
	e.assign(Position{10, 20});
	e.add<Velocity>();
	test_int(invoked, 1);
	world.defer_end();
	test_int(invoked, 1);

	const Position& p = e.get<Position>();
	test_int(p.x, 10);
	test_int(p.y, 20);

	test_assert(e.has<Velocity>());
}

void Entity_defer_on_replace_w_assign_batched_existing_twice(void) {
	flecs::world world;
	world.component<Position>();
	world.component<Velocity>();

	int invoked = 0;

	world.component<Position>()
		.on_add([&](Position& p) {
			p.x = 0; p.y = 0;
		})
		.on_replace([&](Position& prev, Position& next) {
			switch(invoked) {
			case 0:
					test_int(prev.x, 0); test_int(prev.y, 0);
					test_int(next.x, 10); test_int(next.y, 20);
					break;
				case 1:
					test_int(prev.x, 10); test_int(prev.y, 20);
					test_int(next.x, 11); test_int(next.y, 21);
					break;
				}
				invoked ++;
			});

	flecs::entity e = world.entity().add<Position>();
	test_int(invoked, 0);

	world.defer_begin();
	e.assign(Position{10, 20});
	test_int(invoked, 1);
	e.assign(Position{11, 21});
	test_int(invoked, 2);
	e.add<Velocity>();
	world.defer_end();
	test_int(invoked, 2);

	const Position& p = e.get<Position>();
	test_int(p.x, 11);
	test_int(p.y, 21);

	test_assert(e.has<Velocity>());
}

void Entity_set_lvalue_to_mutable(void) {
    flecs::world world;
	world.component<FlecsTestLifecycleTracker>();

    auto e = world.entity();

    FlecsTestLifecycleTracker src;

    e.set(src);

    // Assertions are the exact same as in set_lvalue_to_const; src should have been copied, not moved
    const auto* attached = e.try_get<FlecsTestLifecycleTracker>();
    test_assert(attached != nullptr);

    test_assert(attached->constructed_via == FlecsTestLifecycleTracker::Constructor::default_);
    test_int(attached->times_copy_assigned_into, 1);
    test_int(attached->times_copy_assigned_from, 0);
    test_int(attached->times_copy_constructed_from, 0);
    test_false(attached->moved_into() || attached->moved_from());
    test_int(attached->times_destructed, 0);

    test_assert(src.constructed_via == FlecsTestLifecycleTracker::Constructor::default_);
    test_int(src.times_copy_assigned_into, 0);
    test_int(src.times_copy_assigned_from, 1);
    test_int(src.times_copy_constructed_from, 0);
    test_false(src.moved_into() || src.moved_from());
    test_int(src.times_destructed, 0);
}

void Entity_set_lvalue_to_const(void) {
    flecs::world world;
	world.component<FlecsTestLifecycleTracker>();

    auto e = world.entity();

    FlecsTestLifecycleTracker const src;

    e.set(src);

    const auto* attached = e.try_get<FlecsTestLifecycleTracker>();
    test_assert(attached != nullptr);

    test_assert(attached->constructed_via == FlecsTestLifecycleTracker::Constructor::default_);
    test_int(attached->times_copy_assigned_into, 1);
    test_int(attached->times_copy_assigned_from, 0);
    test_int(attached->times_copy_constructed_from, 0);
    test_false(attached->moved_into() || attached->moved_from());
    test_int(attached->times_destructed, 0);

    test_assert(src.constructed_via == FlecsTestLifecycleTracker::Constructor::default_);
    test_int(src.times_copy_assigned_into, 0);
    test_int(src.times_copy_assigned_from, 1);
    test_int(src.times_copy_constructed_from, 0);
    test_false(src.moved_into() || src.moved_from());
    test_int(src.times_destructed, 0);
}

void Entity_set_rvalue(void) {
    flecs::world world;
	world.component<FlecsTestLifecycleTracker>();

    auto e = world.entity();

    FlecsTestLifecycleTracker src;

    e.set(std::move(src));

    const auto* attached = e.try_get<FlecsTestLifecycleTracker>();
    test_assert(attached != nullptr);

    test_assert(attached->constructed_via == FlecsTestLifecycleTracker::Constructor::default_);
    test_int(attached->times_move_assigned_into, 1);
    test_int(attached->times_move_assigned_from, 0);
    test_int(attached->times_move_constructed_from, 0);
    test_false(attached->copied_into() || attached->copied_from());
    test_int(attached->times_destructed, 0);

    test_assert(src.constructed_via == FlecsTestLifecycleTracker::Constructor::default_);
    test_int(src.times_move_assigned_into, 0);
    test_int(src.times_move_assigned_from, 1);
    test_int(src.times_move_constructed_from, 0);
    test_false(src.copied_into() || src.copied_from());
    test_int(src.times_destructed, 0);
}

void Entity_set_non_copy_assignable(void) {
	flecs::world world;
	world.component<NonCopyAssignable>();

	NonCopyAssignable v;
	v.x = 10;

	flecs::entity e = world.entity().set(v);

	const NonCopyAssignable* comp = e.try_get<NonCopyAssignable>();
	test_assert(comp != nullptr);
	test_int(comp->x, 10);
}

void Entity_set_non_copy_assignable_w_move_assign(void) {
	flecs::world world;
	world.component<NonCopyAssignableWMoveAssign>();

	NonCopyAssignableWMoveAssign v;
	v.x = 10;
	test_int(v.moved, 0);

	flecs::entity e = world.entity().set(v);

	const NonCopyAssignableWMoveAssign* comp = e.try_get<NonCopyAssignableWMoveAssign>();
	test_assert(comp != nullptr);
	test_int(comp->x, 10);
	test_int(comp->moved, 1);
}

void Entity_assign_non_copy_assignable(void) {
	flecs::world world;
	world.component<NonCopyAssignable>();

	NonCopyAssignable v;
	v.x = 10;

	flecs::entity e = world.entity().add<NonCopyAssignable>();
	e.assign(v);

	const NonCopyAssignable* comp = e.try_get<NonCopyAssignable>();
	test_assert(comp != nullptr);
	test_int(comp->x, 10);
}

void Entity_assign_non_copy_assignable_w_move_assign(void) {
	flecs::world world;
	world.component<NonCopyAssignableWMoveAssign>();

	NonCopyAssignableWMoveAssign v;
	v.x = 10;
	test_int(v.moved, 0);

	flecs::entity e = world.entity().add<NonCopyAssignableWMoveAssign>();
	e.assign(v);

	const NonCopyAssignableWMoveAssign* comp = e.try_get<NonCopyAssignableWMoveAssign>();
	test_assert(comp != nullptr);
	test_int(comp->x, 10);
	test_int(comp->moved, 1);
}

END_DEFINE_SPEC(FFlecsEntityTestsSpec);

/*""id": "Entity",
            "testcases": [
                "new",
                "new_named",
                "new_named_from_scope",
                "new_nested_named_from_scope",
                "new_nested_named_from_nested_scope",
                "new_named_from_scope_with_custom_separator",
                "new_add",
                "new_add_2",
                "new_set",
                "new_set_2",
                "add",
                "add_2",
                "add_entity",
                "add_childof",
                "add_instanceof",
                "remove",
                "remove_2",
                "remove_entity",
                "remove_childof",
                "remove_instanceof",
                "set",
                "set_2",
                "emplace",
                "emplace_2",
                "emplace_after_add",
                "emplace_after_add_pair",
                "emplace_pair",
                "emplace_pair_w_entity",
                "emplace_pair_type",
                "emplace_pair_second",
                "get_generic",
                "ensure_generic",
                "get_generic_w_id",
                "get_generic_w_id_t",
                "ensure_generic_w_id",
                "ensure_generic_w_id_t",
                "get_w_id",
                "get_T",
                "get_r_t",
                "get_R_t",
                "get_R_T",
                "get_r_T",
                "get_w_id_not_found",
                "get_T_not_found",
                "get_r_t_not_found",
                "get_R_t_not_found",
                "get_R_T_not_found",
                "get_r_T_not_found",
                "try_get_w_id",
                "try_get_T",
                "try_get_r_t",
                "try_get_R_t",
                "try_get_R_T",
                "try_get_r_T",
                "get_mut_w_id",
                "get_mut_T",
                "get_mut_r_t",
                "get_mut_R_t",
                "get_mut_R_T",
                "get_mut_r_T",
                "get_mut_w_id_not_found",
                "get_mut_T_not_found",
                "get_mut_r_t_not_found",
                "get_mut_R_t_not_found",
                "get_mut_R_T_not_found",
                "get_mut_r_T_not_found",
                "try_get_mut_w_id",
                "try_get_mut_T",
                "try_get_mut_r_t",
                "try_get_mut_R_t",
                "try_get_mut_R_T",
                "try_get_mut_r_T",
                "set_generic",
                "set_generic_w_id",
                "set_generic_w_id_t",
                "set_generic_no_size",
                "set_generic_no_size_w_id",
                "set_generic_no_size_w_id_t",
                "set_T",
                "set_R_t",
                "set_R_T",
                "set_r_T",
                "assign_T",
                "assign_R_t",
                "assign_R_T",
                "assign_r_T",
                "assign_T_not_found",
                "assign_R_t_not_found",
                "assign_R_T_not_found",
                "assign_r_T_not_found",
                "assign_w_on_set_hook",
                "assign_w_on_set_observer",
                "assign_w_change_detect",
                "defer_assign_w_on_set_hook",
                "defer_assign_w_on_set_observer",
                "defer_assign_w_change_detect",
                "add_role",
                "remove_role",
                "has_role",
                "pair_role",
                "equals",
                "compare_0",
                "compare_id_t",
                "compare_id",
                "compare_literal",
                "greater_than",
                "less_than",
                "not_0_or_1",
                "not_true_or_false",
                "has_childof",
                "has_instanceof",
                "has_instanceof_indirect",
                "null_string",
                "set_name",
                "change_name",
                "delete",
                "clear",
                "foce_owned",
                "force_owned_2",
                "force_owned_nested",
                "tag_has_size_zero",
                "get_null_name",
                "get_target",
                "get_parent",
                "is_component_enabled",
                "is_enabled_component_enabled",
                "is_disabled_component_enabled",
                "is_pair_enabled",
                "is_enabled_pair_enabled",
                "is_disabled_pair_enabled",
                "is_pair_enabled_w_ids",
                "is_enabled_pair_enabled_w_ids",
                "is_disabled_pair_enabled_w_ids",
                "is_pair_enabled_w_tgt_id",
                "is_enabled_pair_enabled_w_tgt_id",
                "is_disabled_pair_enabled_w_tgt_id",
                "get_type",
                "get_nonempty_type",
                "set_no_copy",
                "set_copy",
                "set_deduced",
                "override",
                "override_id",
                "override_pair",
                "override_pair_w_tgt_id",
                "override_pair_w_ids",
                "set_override",
                "set_override_lvalue",
                "set_override_pair",
                "set_override_pair_w_tgt_id",
                "set_override_pair_w_rel_tag",
                "emplace_override",
                "emplace_override_pair",
                "path",
                "path_from",
                "path_from_type",
                "path_custom_sep",
                "path_from_custom_sep",
                "path_from_type_custom_sep",
                "implicit_name_to_char",
                "implicit_path_to_char",
                "implicit_type_str_to_char",
                "entity_to_entity_view",
                "entity_view_to_entity_world",
                "entity_view_to_entity_stage",
                "create_entity_view_from_stage",
                "set_template",
                "get_1_component_w_callback",
                "get_2_components_w_callback",
                "set_1_component_w_callback",
                "set_2_components_w_callback",
                "set_3_components_w_callback",
                "ensure_1_component_w_callback",
                "ensure_2_components_w_callback",
                "get_component_w_callback_nested",
                "ensure_component_w_callback_nested",
                "defer_set_1_component",
                "defer_set_2_components",
                "defer_set_3_components",
                "set_2_w_on_set",
                "defer_set_2_w_on_set",
                "set_2_after_fluent",
                "set_2_before_fluent",
                "set_2_after_set_1",
                "set_2_after_set_2",
                "with_self",
                "with_relation_type_self",
                "with_relation_self",
                "with_self_w_name",
                "with_self_nested",
                "with_after_builder_method",
                "with_before_builder_method",
                "with_scope",
                "with_scope_nested",
                "with_scope_nested_same_name_as_parent",
                "scope_after_builder_method",
                "scope_before_builder_method",
                "no_recursive_lookup",
                "defer_new_w_name",
                "defer_new_w_nested_name",
                "defer_new_w_scope_name",
                "defer_new_w_scope_nested_name",
                "defer_new_w_deferred_scope_nested_name",
                "defer_new_w_scope",
                "defer_new_w_with",
                "defer_new_w_name_scope_with",
                "defer_w_with_implicit_component",
                "defer_suspend_resume",
                "defer_ensure",
                "entity_id_str",
                "pair_id_str",
                "role_id_str",
                "id_str_from_entity_view",
                "id_str_from_entity",
                "null_entity",
                "null_entity_w_world",
                "null_entity_w_0",
                "null_entity_w_world_w_0",
                "entity_view_null_entity",
                "entity_view_null_entity_w_world",
                "entity_view_null_entity_w_0",
                "entity_view_null_entity_w_world_w_0",
                "is_wildcard",
                "has_id_t",
                "has_pair_id_t",
                "has_pair_id_t_w_type",
                "has_id",
                "has_pair_id",
                "has_pair_id_w_type",
                "has_wildcard_id",
                "has_wildcard_pair_id",
                "owns_id_t",
                "owns_pair_id_t",
                "owns_pair_id_t_w_type",
                "owns_id",
                "owns_pair_id",
                "owns_pair_id_w_type",
                "owns_wildcard_id",
                "owns_wildcard_pair",
                "id_from_world",
                "id_pair_from_world",
                "id_default_from_world",
                "is_a",
                "is_a_w_type",
                "child_of",
                "child_of_w_type",
                "child",
                "child_custom_rel",
                "child_custom_type",
                "slot_of",
                "slot_of_w_type",
                "slot",
                "id_get_entity",
                "id_get_invalid_entity",
                "each_in_stage",
                "iter_recycled_parent",
                "get_lambda_from_stage",
                "default_ctor",
                "get_obj_by_template",
                "create_named_twice_deferred",
                "clone",
                "clone_w_value",
                "clone_to_existing",
                "clone_to_existing_overlap",
                "set_doc_name",
                "set_doc_brief",
                "set_doc_detail",
                "set_doc_link",
                "entity_w_root_name",
                "entity_w_root_name_from_scope",
                "entity_w_type",
                "entity_w_nested_type",
                "entity_w_type_defer",
                "prefab_w_type",
                "prefab_hierarchy_w_types",
                "prefab_hierarchy_w_root_types",
                "prefab_hierarchy_w_child_override",
                "entity_array",
                "add_if_true_T",
                "add_if_false_T",
                "add_if_true_id",
                "add_if_false_id",
                "add_if_true_R_O",
                "add_if_false_R_O",
                "add_if_true_R_o",
                "add_if_false_R_o",
                "add_if_true_r_o",
                "add_if_exclusive_r_o",
                "add_if_exclusive_R_o",
                "add_if_exclusive_R_O",
                "add_if_pair_w_0_object",
                "children_w_custom_relation",
                "children_w_custom_relation_type",
                "children_w_this",
                "children_w_wildcard",
                "children_w_any",
                "children_from_root",
                "children_from_root_world",
                "get_depth",
                "get_depth_w_type",
                "to_view",
                "to_view_from_stage",
                "set_alias",
                "emplace_w_observer",
                "scoped_world",
                "entity_lookup_not_recursive",
                "world_lookup_not_recursive",
                "world_lookup_custom_sep",
                "world_lookup_custom_root_sep",
                "depends_on",
                "depends_on_type",
                "const_entity_add_remove",
                "const_entity_set",
                "const_entity_get_mut",
                "const_entity_ensure",
                "const_entity_destruct",
                "const_entity_emit_after_build",
                "const_entity_set_doc",
                "set_sparse",
                "insert_1_sparse",
                "insert_2_w_1_sparse",
                "insert_1_dont_fragment",
                "insert_2_w_1_dont_fragment",
                "emplace_sparse",
                "override_sparse",
                "delete_w_override_sparse",
                "get_pair_second_invalid_type",
                "get_mut_pair_second_invalid_type",
                "ensure_pair_second_invalid_type",
                "set_pair_second_invalid_type",
                "get_ref_pair_second_invalid_type",
                "iter_type",
                "iter_empty_type",
                "untyped_component_use_low_id",
                "add_remove_enum_component",
                "on_replace_w_set",
                "on_replace_w_set_existing",
                "on_replace_w_assign",
                "on_replace_w_assign_existing",
                "defer_on_replace_w_set",
                "defer_on_replace_w_set_twice",
                "defer_on_replace_w_set_existing",
                "defer_on_replace_w_set_existing_twice",
                "defer_on_replace_w_set_batched",
                "defer_on_replace_w_set_batched_twice",
                "defer_on_replace_w_assign_existing",
                "defer_on_replace_w_assign_existing_twice",
                "defer_on_replace_w_assign_batched_existing",
                "defer_on_replace_w_assign_batched_existing_twice",
                "set_lvalue_to_mutable",
                "set_lvalue_to_const",
                "set_rvalue",
                "set_non_copy_assignable",
                "set_non_copy_assignable_w_move_assign",
                "assign_non_copy_assignable",
                "assign_non_copy_assignable_w_move_assign"
                
            ]*/

void FFlecsEntityTestsSpec::Define()
{
	It("Entity_new", [&]() { Entity_new(); });
	It("Entity_new_named", [&]() { Entity_new_named(); });
	It("Entity_new_named_from_scope", [&]() { Entity_new_named_from_scope(); });
	It("Entity_new_nested_named_from_scope", [&]() { Entity_new_nested_named_from_scope(); });
	It("Entity_new_nested_named_from_nested_scope", [&]() { Entity_new_nested_named_from_nested_scope(); });
	It("Entity_new_named_from_scope_with_custom_separator", [&]() { Entity_new_named_from_scope_with_custom_separator(); });
	It("Entity_new_add", [&]() { Entity_new_add(); });
	It("Entity_new_add_2", [&]() { Entity_new_add_2(); });
	It("Entity_new_set", [&]() { Entity_new_set(); });
	It("Entity_new_set_2", [&]() { Entity_new_set_2(); });
	It("Entity_add", [&]() { Entity_add(); });
	It("Entity_add_2", [&]() { Entity_add_2(); });
	It("Entity_add_entity", [&]() { Entity_add_entity(); });
	It("Entity_add_childof", [&]() { Entity_add_childof(); });
	It("Entity_add_instanceof", [&]() { Entity_add_instanceof(); });
	It("Entity_remove", [&]() { Entity_remove(); });
	It("Entity_remove_2", [&]() { Entity_remove_2(); });
	It("Entity_remove_entity", [&]() { Entity_remove_entity(); });
	It("Entity_remove_childof", [&]() { Entity_remove_childof(); });
	It("Entity_remove_instanceof", [&]() { Entity_remove_instanceof(); });
	It("Entity_set", [&]() { Entity_set(); });
	It("Entity_set_2", [&]() { Entity_set_2(); });
	It("Entity_emplace", [&]() { Entity_emplace(); });
	It("Entity_emplace_2", [&]() { Entity_emplace_2(); });
	It("Entity_emplace_after_add", [&]() { Entity_emplace_after_add(); });
	It("Entity_emplace_after_add_pair", [&]() { Entity_emplace_after_add_pair(); });
	It("Entity_emplace_pair", [&]() { Entity_emplace_pair(); });
	It("Entity_emplace_pair_w_entity", [&]() { Entity_emplace_pair_w_entity(); });
	It("Entity_emplace_pair_type", [&]() { Entity_emplace_pair_type(); });
	It("Entity_emplace_pair_second", [&]() { Entity_emplace_pair_second(); });
	It("Entity_get_generic", [&]() { Entity_get_generic(); });
	It("Entity_ensure_generic", [&]() { Entity_ensure_generic(); });
	It("Entity_get_generic_w_id", [&]() { Entity_get_generic_w_id(); });
	It("Entity_get_generic_w_id_t", [&]() { Entity_get_generic_w_id_t(); });
	It("Entity_ensure_generic_w_id", [&]() { Entity_ensure_generic_w_id(); });
	It("Entity_ensure_generic_w_id_t", [&]() { Entity_ensure_generic_w_id_t(); });
	It("Entity_get_mut_w_id", [&]() { Entity_get_mut_w_id(); });
	It("Entity_get_mut_T_type", [&]() { Entity_get_mut_T(); });
	It("Entity_get_mut_r_entity_t_entity", [&]() { Entity_get_mut_r_t(); });
	It("Entity_get_mut_R_type_t_entity", [&]() { Entity_get_mut_R_t(); });
	It("Entity_get_mut_R_type_T_type", [&]() { Entity_get_mut_R_T(); });
	It("Entity_get_mut_r_entity_T_type", [&]() { Entity_get_mut_r_T(); });
	It("Entity_set_generic", [&]() { Entity_set_generic(); });
	It("Entity_set_generic_w_id", [&]() { Entity_set_generic_w_id(); });
	It("Entity_set_generic_w_id_t", [&]() { Entity_set_generic_w_id_t(); });
	It("Entity_set_generic_no_size", [&]() { Entity_set_generic_no_size(); });
	It("Entity_set_generic_no_size_w_id", [&]() { Entity_set_generic_no_size_w_id(); });
	It("Entity_set_generic_no_size_w_id_t", [&]() { Entity_set_generic_no_size_w_id_t(); });
	It("Entity_set_T", [&]() { Entity_set_T(); });
	It("Entity_set_R_type_t_entity", [&]() { Entity_set_R_t(); });
	It("Entity_set_R_type_T_type", [&]() { Entity_set_R_T(); });
	It("Entity_set_r_entity_T_type", [&]() { Entity_set_r_T(); });
	It("Entity_assign_T", [&]() { Entity_assign_T(); });
	It("Entity_assign_R_type_t_entity", [&]() { Entity_assign_R_t(); });
	It("Entity_assign_R_type_T_type", [&]() { Entity_assign_R_T(); });
	It("Entity_assign_r_entity_T_type", [&]() { Entity_assign_r_T(); });
	It("Entity_assign_w_on_set_hook", [&]() { Entity_assign_w_on_set_hook(); });
	It("Entity_assign_w_on_set_observer", [&]() { Entity_assign_w_on_set_observer(); });
	It("Entity_assign_w_change_detect", [&]() { Entity_assign_w_change_detect(); });
	It("Entity_defer_assign_w_on_set_hook", [&]() { Entity_defer_assign_w_on_set_hook(); });
	It("Entity_defer_assign_w_on_set_observer", [&]() { Entity_defer_assign_w_on_set_observer(); });
	It("Entity_defer_assign_w_change_detect", [&]() { Entity_defer_assign_w_change_detect(); });
	It("Entity_add_role", [&]() { Entity_add_role(); });
	It("Entity_remove_role", [&]() { Entity_remove_role(); });
	It("Entity_has_role", [&]() { Entity_has_role(); });
	It("Entity_pair_role", [&]() { Entity_pair_role(); });
	It("Entity_equals", [&]() { Entity_equals(); });
	It("Entity_compare_0", [&]() { Entity_compare_0(); });
	It("Entity_compare_id_t", [&]() { Entity_compare_id_t(); });
	It("Entity_compare_id", [&]() { Entity_compare_id(); });
	It("Entity_compare_literal", [&]() { Entity_compare_literal(); });
	It("Entity_greater_than", [&]() { Entity_greater_than(); });
	It("Entity_less_than", [&]() { Entity_less_than(); });
	It("Entity_not_0_or_1", [&]() { Entity_not_0_or_1(); });
	It("Entity_has_childof", [&]() { Entity_has_childof(); });
	It("Entity_has_instanceof", [&]() { Entity_has_instanceof(); });
	It("Entity_has_instanceof_indirect", [&]() { Entity_has_instanceof_indirect(); });
	It("Entity_null_string", [&]() { Entity_null_string(); });
	It("Entity_set_name", [&]() { Entity_set_name(); });
	It("Entity_change_name", [&]() { Entity_change_name(); });
	It("Entity_delete", [&]() { Entity_delete(); });
	It("Entity_clear", [&]() { Entity_clear(); });
	It("Entity_foce_owned", [&]() { Entity_foce_owned(); });
	It("Entity_force_owned_2", [&]() { Entity_force_owned_2(); });
	It("Entity_force_owned_nested", [&]() { Entity_force_owned_nested(); });
	It("Entity_tag_has_size_zero", [&]() { Entity_tag_has_size_zero(); });
	It("Entity_get_null_name", [&]() { Entity_get_null_name(); });
	It("Entity_get_target", [&]() { Entity_get_target(); });
	It("Entity_get_parent", [&]() { Entity_get_parent(); });
	It("Entity_is_component_enabled", [&]() { Entity_is_component_enabled(); });
	It("Entity_is_enabled_component_enabled", [&]() { Entity_is_enabled_component_enabled(); });
	It("Entity_is_disabled_component_enabled", [&]() { Entity_is_disabled_component_enabled(); });
	It("Entity_is_pair_enabled", [&]() { Entity_is_pair_enabled(); });
	It("Entity_is_enabled_pair_enabled", [&]() { Entity_is_enabled_pair_enabled(); });
	It("Entity_is_disabled_pair_enabled", [&]() { Entity_is_disabled_pair_enabled(); });
	It("Entity_is_pair_enabled_w_ids", [&]() { Entity_is_pair_enabled_w_ids(); });
	It("Entity_is_enabled_pair_enabled_w_ids", [&]() { Entity_is_enabled_pair_enabled_w_ids(); });
	It("Entity_is_disabled_pair_enabled_w_ids", [&]() { Entity_is_disabled_pair_enabled_w_ids(); });
	It("Entity_is_pair_enabled_w_tgt_id", [&]() { Entity_is_pair_enabled_w_tgt_id(); });
	It("Entity_is_enabled_pair_enabled_w_tgt_id", [&]() { Entity_is_enabled_pair_enabled_w_tgt_id(); });
	It("Entity_is_disabled_pair_enabled_w_tgt_id", [&]() { Entity_is_disabled_pair_enabled_w_tgt_id(); });
	It("Entity_get_type", [&]() { Entity_get_type(); });
	It("Entity_get_nonempty_type", [&]() { Entity_get_nonempty_type(); });
	It("Entity_set_no_copy", [&]() { Entity_set_no_copy(); });
	It("Entity_set_copy", [&]() { Entity_set_copy(); });
	It("Entity_set_deduced", [&]() { Entity_set_deduced(); });
	It("Entity_override", [&]() { Entity_override(); });
	It("Entity_override_id", [&]() { Entity_override_id(); });
	It("Entity_override_pair_w_tgt_id", [&]() { Entity_override_pair_w_tgt_id(); });
	It("Entity_override_pair_w_ids", [&]() { Entity_override_pair_w_ids(); });
	It("Entity_override_pair", [&]() { Entity_override_pair(); });
	It("Entity_set_override", [&]() { Entity_set_override(); });
	It("Entity_set_override_lvalue", [&]() { Entity_set_override_lvalue(); });
	It("Entity_set_override_pair", [&]() { Entity_set_override_pair(); });
	It("Entity_set_override_pair_w_tgt_id", [&]() { Entity_set_override_pair_w_tgt_id(); });
	It("Entity_set_override_pair_w_rel_tag", [&]() { Entity_set_override_pair_w_rel_tag(); });
	It("Entity_emplace_override", [&]() { Entity_emplace_override(); });
	It("Entity_emplace_override_pair", [&]() { Entity_emplace_override_pair(); });
	It("Entity_path", [&]() { Entity_path(); });
	It("Entity_path_from", [&]() { Entity_path_from(); });
	It("Entity_path_from_type", [&]() { Entity_path_from_type(); });
	It("Entity_path_custom_sep", [&]() { Entity_path_custom_sep(); });
	It("Entity_path_from_custom_sep", [&]() { Entity_path_from_custom_sep(); });
	It("Entity_path_from_type_custom_sep", [&]() { Entity_path_from_type_custom_sep(); });
	It("Entity_implicit_name_to_char", [&]() { Entity_implicit_name_to_char(); });
	It("Entity_implicit_path_to_char", [&]() { Entity_implicit_path_to_char(); });
	It("Entity_implicit_type_str_to_char", [&]() { Entity_implicit_type_str_to_char(); });
	It("Entity_entity_to_entity_view", [&]() { Entity_entity_to_entity_view(); });
	It("Entity_entity_view_to_entity_world", [&]() { Entity_entity_view_to_entity_world(); });
	It("Entity_entity_view_to_entity_stage", [&]() { Entity_entity_view_to_entity_stage(); });
	It("Entity_create_entity_view_from_stage", [&]() { Entity_create_entity_view_from_stage(); });
	It("Entity_set_template", [&]() { Entity_set_template(); });
	It("Entity_get_1_component_w_callback", [&]() { Entity_get_1_component_w_callback(); });
	It("Entity_get_2_components_w_callback", [&]() { Entity_get_2_components_w_callback(); });
	It("Entity_ensure_1_component_w_callback", [&]() { Entity_ensure_1_component_w_callback(); });
	It("Entity_ensure_2_components_w_callback", [&]() { Entity_ensure_2_components_w_callback(); });
	It("Entity_get_component_w_callback_nested", [&]() { Entity_get_component_w_callback_nested(); });
	It("Entity_set_1_component_w_callback", [&]() { Entity_set_1_component_w_callback(); });
	It("Entity_set_2_components_w_callback", [&]() { Entity_set_2_components_w_callback(); });
	It("Entity_set_3_components_w_callback", [&]() { Entity_set_3_components_w_callback(); });
	It("Entity_defer_set_1_component", [&]() { Entity_defer_set_1_component(); });
	It("Entity_defer_set_2_components", [&]() { Entity_defer_set_2_components(); });
	It("Entity_defer_set_3_components", [&]() { Entity_defer_set_3_components(); });
	It("Entity_set_2_w_on_set", [&]() { Entity_set_2_w_on_set(); });
	It("Entity_defer_set_2_w_on_set", [&]() { Entity_defer_set_2_w_on_set(); });
	It("Entity_set_2_after_fluent", [&]() { Entity_set_2_after_fluent(); });
	It("Entity_set_2_before_fluent", [&]() { Entity_set_2_before_fluent(); });
	It("Entity_set_2_after_set_1", [&]() { Entity_set_2_after_set_1(); });
	It("Entity_set_2_after_set_2", [&]() { Entity_set_2_after_set_2(); });
	It("Entity_with_self", [&]() { Entity_with_self(); });
	It("Entity_with_relation_type_self", [&]() { Entity_with_relation_type_self(); });
	It("Entity_with_relation_self", [&]() { Entity_with_relation_self(); });
	It("Entity_with_self_w_name", [&]() { Entity_with_self_w_name(); });
	It("Entity_with_self_nested", [&]() { Entity_with_self_nested(); });
	It("Entity_with_scope", [&]() { Entity_with_scope(); });
	It("Entity_with_scope_nested", [&]() { Entity_with_scope_nested(); });
	It("Entity_with_scope_nested_same_name_as_parent", [&]() { Entity_with_scope_nested_same_name_as_parent(); });
	It("Entity_no_recursive_lookup", [&]() { Entity_no_recursive_lookup(); });
	It("Entity_defer_new_w_name", [&]() { Entity_defer_new_w_name(); });
	It("Entity_defer_new_w_nested_name", [&]() { Entity_defer_new_w_nested_name(); });
	It("Entity_defer_new_w_scope_name", [&]() { Entity_defer_new_w_scope_name(); });
	It("Entity_defer_new_w_scope_nested_name", [&]() { Entity_defer_new_w_scope_nested_name(); });
	It("Entity_defer_new_w_deferred_scope_nested_name", [&]() { Entity_defer_new_w_deferred_scope_nested_name(); });
	It("Entity_defer_new_w_scope", [&]() { Entity_defer_new_w_scope(); });
	It("Entity_defer_new_w_with", [&]() { Entity_defer_new_w_with(); });
	It("Entity_defer_new_w_name_scope_with", [&]() { Entity_defer_new_w_name_scope_with(); });
	It("Entity_defer_new_w_nested_name_scope_with", [&]() { Entity_defer_new_w_nested_name_scope_with(); });
	It("Entity_defer_w_with_implicit_component", [&]() { Entity_defer_w_with_implicit_component(); });
	It("Entity_defer_suspend_resume", [&]() { Entity_defer_suspend_resume(); });
	It("Entity_defer_ensure", [&]() { Entity_defer_ensure(); });
	It("Entity_entity_id_str", [&]() { Entity_entity_id_str(); });
	It("Entity_pair_id_str", [&]() { Entity_pair_id_str(); });
	It("Entity_role_id_str", [&]() { Entity_role_id_str(); });
	It("Entity_id_str_from_entity_view", [&]() { Entity_id_str_from_entity_view(); });
	It("Entity_id_str_from_entity", [&]() { Entity_id_str_from_entity(); });
	It("Entity_null_entity", [&]() { Entity_null_entity(); });
	It("Entity_null_entity_w_world", [&]() { Entity_null_entity_w_world(); });
	It("Entity_null_entity_w_0", [&]() { Entity_null_entity_w_0(); });
	It("Entity_null_entity_w_world_w_0", [&]() { Entity_null_entity_w_world_w_0(); });
	It("Entity_entity_view_null_entity", [&]() { Entity_entity_view_null_entity(); });
	It("Entity_entity_view_null_entity_w_world", [&]() { Entity_entity_view_null_entity_w_world(); });
	It("Entity_entity_view_null_entity_w_0", [&]() { Entity_entity_view_null_entity_w_0(); });
	It("Entity_entity_view_null_entity_w_world_w_0", [&]() { Entity_entity_view_null_entity_w_world_w_0(); });
	It("Entity_is_wildcard", [&]() { Entity_is_wildcard(); });
	It("Entity_has_id_t", [&]() { Entity_has_id_t(); });
	It("Entity_has_pair_id_t", [&]() { Entity_has_pair_id_t(); });
	It("Entity_has_pair_id_t_w_type", [&]() { Entity_has_pair_id_t_w_type(); });
	It("Entity_has_id", [&]() { Entity_has_id(); });
	It("Entity_has_pair_id", [&]() { Entity_has_pair_id(); });
	It("Entity_has_pair_id_w_type", [&]() { Entity_has_pair_id_w_type(); });
	It("Entity_has_wildcard_id", [&]() { Entity_has_wildcard_id(); });
	It("Entity_has_wildcard_pair_id", [&]() { Entity_has_wildcard_pair_id(); });
	It("Entity_owns_id_t", [&]() { Entity_owns_id_t(); });
	It("Entity_owns_pair_id_t", [&]() { Entity_owns_pair_id_t(); });
	It("Entity_owns_pair_id_t_w_type", [&]() { Entity_owns_pair_id_t_w_type(); });
	It("Entity_owns_id", [&]() { Entity_owns_id(); });
	It("Entity_owns_pair_id", [&]() { Entity_owns_pair_id(); });
	It("Entity_owns_pair_id_w_type", [&]() { Entity_owns_pair_id_w_type(); });
	It("Entity_owns_wildcard_id", [&]() { Entity_owns_wildcard_id(); });
	It("Entity_owns_wildcard_pair", [&]() { Entity_owns_wildcard_pair(); });
	It("Entity_id_from_world", [&]() { Entity_id_from_world(); });
	It("Entity_id_pair_from_world", [&]() { Entity_id_pair_from_world(); });
	It("Entity_id_default_from_world", [&]() { Entity_id_default_from_world(); });
	It("Entity_is_a", [&]() { Entity_is_a(); });
	It("Entity_is_a_w_type", [&]() { Entity_is_a_w_type(); });
	It("Entity_child_of", [&]() { Entity_child_of(); });
	It("Entity_child_of_w_type", [&]() { Entity_child_of_w_type(); });
	It("Entity_child", [&]() { Entity_child(); });
	It("Entity_child_custom_rel", [&]() { Entity_child_custom_rel(); });
	It("Entity_child_custom_type", [&]() { Entity_child_custom_type(); });
	It("Entity_slot_of", [&]() { Entity_slot_of(); });
	It("Entity_slot_of_w_type", [&]() { Entity_slot_of_w_type(); });
	It("Entity_slot", [&]() { Entity_slot(); });
	It("Entity_id_get_entity", [&]() { Entity_id_get_entity(); });
	It("Entity_each_in_stage", [&]() { Entity_each_in_stage(); });
	It("Entity_iter_recycled_parent", [&]() { Entity_iter_recycled_parent(); });
	It("Entity_get_lambda_from_stage", [&]() { Entity_get_lambda_from_stage(); });
	It("Entity_default_ctor", [&]() { Entity_default_ctor(); });
	It("Entity_get_obj_by_template", [&]() { Entity_get_obj_by_template(); });
	It("Entity_create_named_twice_deferred", [&]() { Entity_create_named_twice_deferred(); });
	It("Entity_clone", [&]() { Entity_clone(); });
	It("Entity_clone_w_value", [&]() { Entity_clone_w_value(); });
	It("Entity_clone_to_existing", [&]() { Entity_clone_to_existing(); });
	It("Entity_set_doc_name", [&]() { Entity_set_doc_name(); });
	It("Entity_set_doc_brief", [&]() { Entity_set_doc_brief(); });
	It("Entity_set_doc_detail", [&]() { Entity_set_doc_detail(); });
	It("Entity_set_doc_link", [&]() { Entity_set_doc_link(); });
	It("Entity_entity_w_root_name", [&]() { Entity_entity_w_root_name(); });
	It("Entity_entity_w_root_name_from_scope", [&]() { Entity_entity_w_root_name_from_scope(); });
	It("Entity_entity_w_type", [&]() { Entity_entity_w_type(); });
	It("Entity_prefab_w_type", [&]() { Entity_prefab_w_type(); });
	It("Entity_prefab_hierarchy_w_types", [&]() { Entity_prefab_hierarchy_w_types(); });
	It("Entity_prefab_hierarchy_w_root_types", [&]() { Entity_prefab_hierarchy_w_root_types(); });
	It("Entity_prefab_hierarchy_w_child_override", [&]() { Entity_prefab_hierarchy_w_child_override(); });
	It("Entity_entity_w_nested_type", [&]() { Entity_entity_w_nested_type(); });
	It("Entity_entity_array", [&]() { Entity_entity_array(); });
	It("Entity_entity_w_type_defer", [&]() { Entity_entity_w_type_defer(); });
	It("Entity_add_if_true_T", [&]() { Entity_add_if_true_T(); });
	It("Entity_add_if_false_T", [&]() { Entity_add_if_false_T(); });
	It("Entity_add_if_true_id", [&]() { Entity_add_if_true_id(); });
	It("Entity_add_if_false_id", [&]() { Entity_add_if_false_id(); });
	It("Entity_add_if_true_R_type_O_type", [&]() { Entity_add_if_true_R_O(); });
	It("Entity_add_if_false_R_type_O_type", [&]() { Entity_add_if_false_R_O(); });
	It("Entity_add_if_true_R_type_o_entity", [&]() { Entity_add_if_true_R_o(); });
	It("Entity_add_if_false_R_type_o_entity", [&]() { Entity_add_if_false_R_o(); });
	It("Entity_add_if_true_r_entity_o_entity", [&]() { Entity_add_if_true_r_o(); });
	It("Entity_add_if_false_r_entity_o_entity", [&]() { Entity_add_if_false_r_o(); });
	It("Entity_add_if_exclusive_r_entity_o_entity", [&]() { Entity_add_if_exclusive_r_o(); });
	It("Entity_add_if_exclusive_R_type_o_entity", [&]() { Entity_add_if_exclusive_R_o(); });
	It("Entity_add_if_exclusive_R_type_O_type", [&]() { Entity_add_if_exclusive_R_O(); });
	It("Entity_add_if_pair_w_0_object", [&]() { Entity_add_if_pair_w_0_object(); });
	It("Entity_children_w_custom_relation", [&]() { Entity_children_w_custom_relation(); });
	It("Entity_children_w_custom_relation_type", [&]() { Entity_children_w_custom_relation_type(); });
	It("Entity_children_w_this", [&]() { Entity_children_w_this(); });
	It("Entity_children_w_wildcard", [&]() { Entity_children_w_wildcard(); });
	It("Entity_children_w_any", [&]() { Entity_children_w_any(); });
	//xIt("Entity_children_from_root", [&]() { Entity_children_from_root(); });
	//xIt("Entity_children_from_root_world", [&]() { Entity_children_from_root_world(); });
	It("Entity_get_depth", [&]() { Entity_get_depth(); });
	It("Entity_get_depth_w_type", [&]() { Entity_get_depth_w_type(); });
	It("Entity_to_view", [&]() { Entity_to_view(); });
	It("Entity_to_view_from_stage", [&]() { Entity_to_view_from_stage(); });
	It("Entity_set_alias", [&]() { Entity_set_alias(); });
	It("Entity_emplace_w_observer", [&]() { Entity_emplace_w_observer(); });
	It("Entity_scoped_world", [&]() { Entity_scoped_world(); });
	It("Entity_entity_lookup_not_recursive", [&]() { Entity_entity_lookup_not_recursive(); });
	It("Entity_world_lookup_not_recursive", [&]() { Entity_world_lookup_not_recursive(); });
	It("Entity_world_lookup_custom_sep", [&]() { Entity_world_lookup_custom_sep(); });
	It("Entity_world_lookup_custom_root_sep", [&]() { Entity_world_lookup_custom_root_sep(); });
	It("Entity_depends_on", [&]() { Entity_depends_on(); });
	It("Entity_depends_on_type", [&]() { Entity_depends_on_type(); });
	It("Entity_const_entity_add_remove", [&]() { Entity_const_entity_add_remove(); });
	It("Entity_const_entity_set", [&]() { Entity_const_entity_set(); });
	It("Entity_const_entity_get_mut", [&]() { Entity_const_entity_get_mut(); });
	It("Entity_const_entity_ensure", [&]() { Entity_const_entity_ensure(); });
	It("Entity_const_entity_destruct", [&]() { Entity_const_entity_destruct(); });
	It("Entity_const_entity_emit_after_build", [&]() { Entity_const_entity_emit_after_build(); });
	It("Entity_const_entity_set_doc", [&]() { Entity_const_entity_set_doc(); });
	It("Entity_set_sparse", [&]() { Entity_set_sparse(); });
	It("Entity_insert_1_sparse", [&]() { Entity_insert_1_sparse(); });
	It("Entity_insert_2_w_1_sparse", [&]() { Entity_insert_2_w_1_sparse(); });
	It("Entity_insert_1_dont_fragment", [&]() { Entity_insert_1_dont_fragment(); });
	It("Entity_insert_2_w_1_dont_fragment", [&]() { Entity_insert_2_w_1_dont_fragment(); });
	It("Entity_emplace_sparse", [&]() { Entity_emplace_sparse(); });
	It("Entity_override_sparse", [&]() { Entity_override_sparse(); });
	It("Entity_delete_w_override_sparse", [&]() { Entity_delete_w_override_sparse(); });
	It("Entity_iter_type", [&]() { Entity_iter_type(); });
	It("Entity_iter_empty_type", [&]() { Entity_iter_empty_type(); });
	It("Entity_untyped_component_use_low_id", [&]() { Entity_untyped_component_use_low_id(); });
	It("Entity_add_remove_enum_component", [&]() { Entity_add_remove_enum_component(); });
	It("Entity_on_replace_w_set", [&]() { Entity_on_replace_w_set(); });
	It("Entity_on_replace_w_set_existing", [&]() { Entity_on_replace_w_set_existing(); });
	It("Entity_on_replace_w_assign", [&]() { Entity_on_replace_w_assign(); });
	It("Entity_on_replace_w_assign_existing", [&]() { Entity_on_replace_w_assign_existing(); });
	It("Entity_defer_on_replace_w_set", [&]() { Entity_defer_on_replace_w_set(); });
	It("Entity_defer_on_replace_w_set_twice", [&]() { Entity_defer_on_replace_w_set_twice(); });
	It("Entity_defer_on_replace_w_set_existing", [&]() { Entity_defer_on_replace_w_set_existing(); });
	It("Entity_defer_on_replace_w_set_existing_twice", [&]() { Entity_defer_on_replace_w_set_existing_twice(); });
	It("Entity_defer_on_replace_w_set_batched", [&]() { Entity_defer_on_replace_w_set_batched(); });
	It("Entity_defer_on_replace_w_set_batched_twice", [&]() { Entity_defer_on_replace_w_set_batched_twice(); });
	It("Entity_defer_on_replace_w_assign_existing", [&]() { Entity_defer_on_replace_w_assign_existing(); });
	It("Entity_defer_on_replace_w_assign_existing_twice", [&]() { Entity_defer_on_replace_w_assign_existing_twice(); });
	It("Entity_defer_on_replace_w_assign_batched_existing", [&]() { Entity_defer_on_replace_w_assign_batched_existing(); });
	It("Entity_defer_on_replace_w_assign_batched_existing_twice", [&]() { Entity_defer_on_replace_w_assign_batched_existing_twice(); });
	It("Entity_set_lvalue_to_mutable", [&]() { Entity_set_lvalue_to_mutable(); });
	It("Entity_set_lvalue_to_const", [&]() { Entity_set_lvalue_to_const(); });
	It("Entity_set_rvalue", [&]() { Entity_set_rvalue(); });
	It("Entity_set_non_copy_assignable", [&]() { Entity_set_non_copy_assignable(); });
	It("Entity_set_non_copy_assignable_w_move_assign", [&]() { Entity_set_non_copy_assignable_w_move_assign(); });
	It("Entity_assign_non_copy_assignable", [&]() { Entity_assign_non_copy_assignable(); });
	It("Entity_assign_non_copy_assignable_w_move_assign", [&]() { Entity_assign_non_copy_assignable_w_move_assign(); });
}

#endif // WITH_AUTOMATION_TESTS
