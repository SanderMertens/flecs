
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "flecs.h"

#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"

struct Base {
    int x;
};

struct Derived : public Base {
    Derived() { }

    Derived(int x_, int y_) {
        x = x_;
        y = y_;
    }

    int y;
};

struct BaseEmpty { };
struct DerivedFromEmpty : public BaseEmpty {
    DerivedFromEmpty() { }

    DerivedFromEmpty(int y_) {
        y = y_;
    }

    int y;
};

BEGIN_DEFINE_SPEC(FFlecsRefsTestsSpec,
                  "FlecsLibrary.Refs",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

void Refs_get_ref_by_ptr(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_get_ref_by_method(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    test_assert(ref.get()->x == 10);
    test_assert(ref.get()->y == 20);
}

void Refs_ref_after_add(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();

    e.add<Velocity>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_after_remove(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    auto ref = e.get_ref<Position>();

    e.remove<Velocity>();
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_after_set(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    
    e.set<Velocity>({1, 1});
    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_ref_before_set(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity e = world.entity();
    auto ref = e.get_ref<Position>();

    e.set<Position>({10, 20});

    test_assert(ref->x == 10);
    test_assert(ref->y == 20);
}

void Refs_non_const_ref(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity e = world.entity().set<Position>({10, 20});
    auto ref = e.get_ref<Position>();
    ref->x ++;

    test_int(e.get<Position>().x, 11);
}

void Refs_pair_ref(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = world.entity().set<Position, Tag>({10, 20});
    auto ref = e.get_ref<Position, Tag>();
    ref->x ++;

    test_int((e.get<Position, Tag>().x), 11);
}

using PositionTag = flecs::pair<Position, Tag>;

void Refs_pair_ref_w_pair_type(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = world.entity().set<PositionTag>({10, 20});
    auto ref = e.get_ref<PositionTag>();
    ref->x ++;

    test_int((e.get<PositionTag>().x), 11);
}

using TagPosition = flecs::pair<Tag, Position>;

void Refs_pair_ref_w_pair_type_second(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = world.entity().set<TagPosition>({10, 20});
    auto ref = e.get_ref<TagPosition>();
    ref->x++;

    test_int((e.get<TagPosition>().x), 11);
}

void Refs_pair_ref_w_entity(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto tag = world.entity();
    auto e = world.entity().set<Position>(tag, {10, 20});
    auto ref = e.get_ref<Position>(tag);
    ref->x ++;

    test_int(e.get<Position>(tag).x, 11);
}

void Refs_pair_ref_second(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto tag = world.entity();
    auto e = world.entity().set_second<Position>(tag, {10, 20});
    auto ref = e.get_ref_second<Position>(tag);
    ref->x ++;

    test_int(e.get_second<Position>(tag).x, 11);
}

void Refs_from_stage(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);
    flecs::world stage = world.get_stage(0); // get default stage
    flecs::entity e = stage.entity().set<Position>({10, 20});
    auto ref = e.get_ref<Position>();
    test_int(ref->x, 10);
    test_int(ref->y, 20);
}

void Refs_default_ctor(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    // Make sure default ctor works
    flecs::ref<Position> p;

    flecs::entity e = world.entity().set<Position>({10, 20});

    p = e.get_ref<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Refs_ctor_from_entity(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity e = world.entity().set<Position>({10, 20});

    flecs::ref<Position> p(e);

    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Refs_implicit_operator_bool(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity e = world.entity().set<Position>({10, 20});

    flecs::ref<Position> p(e);

    test_assert(p);
}

void Refs_try_get(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::ref<Position> p;

    test_assert(p.try_get() == nullptr);
}

void Refs_try_get_after_delete(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity e = world.entity().set(Position{10, 20});

    flecs::ref<Position> p = e.get_ref<Position>();
    Position *ptr = p.try_get();
    test_assert(ptr != nullptr);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    e.destruct();

    ptr = p.try_get();
    test_assert(ptr == nullptr);
}

void Refs_has(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity e = world.entity();

    {
        flecs::ref<Position> p = e.get_ref<Position>();
        test_assert(!p.has());
    }

    e.set<Position>({10, 20});

    {
        flecs::ref<Position> p = e.get_ref<Position>();
        test_assert(p.has());
    }
}

void Refs_bool_operator(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity e = world.entity();

    {
        flecs::ref<Position> p = e.get_ref<Position>();
        test_assert(!p);
    }

    e.set<Position>({10, 20});

    {
        flecs::ref<Position> p = e.get_ref<Position>();
        test_assert(p);
    }
}

void Refs_base_type(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);
    world.component<Base>();
    world.component<Derived>();

    flecs::entity e = world.entity().set(Derived{10, 20});

    flecs::ref<Base> r = e.get_ref_w_id<Base>(world.id<Derived>());
    test_int(r->x, 10);
}

void Refs_empty_base_type(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);
    world.component<BaseEmpty>();
    world.component<DerivedFromEmpty>();

    flecs::entity e = world.entity().set(DerivedFromEmpty{20});

    flecs::ref<BaseEmpty> r = e.get_ref_w_id<BaseEmpty>(world.id<DerivedFromEmpty>());

    BaseEmpty *b = r.get();
    DerivedFromEmpty *d = static_cast<DerivedFromEmpty*>(b);
    test_int(d->y, 20);
}

void Refs_get_component(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    auto ref = e.get_ref<Position>();
    test_assert(ref.component() == world.id<Position>());
}

void Refs_untyped_get_ref_by_method(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    auto e = flecs::entity(world)
        .set<Position>({10, 20});

    flecs::untyped_ref ref = e.get_ref(world.id<Position>());
    Position* pos = static_cast<Position*>(ref.get());
    test_assert(pos->x == 10);
    test_assert(pos->y == 20);
}

void Refs_untyped_pair_ref(void) {
    flecs::world world;
    RegisterTestTypeComponents(world);

    flecs::entity tag = world.entity();

    auto e = world.entity().set<Position>(tag,{10, 20});
    flecs::untyped_ref ref = e.get_ref(world.id<Position>(), tag);
    Position* pos = static_cast<Position*>(ref.get());
    pos->x ++;

    test_int((e.get<Position>(tag).x), 11);
}

void Refs_untyped_runtime_component_ref(void) {
    flecs::world world;

    auto Position = world.component("Position")
        .member(flecs::I32, "x")
        .member(flecs::I32, "y");

    auto e = flecs::entity(world);

    void* pos_ptr = e.obtain(Position);

    flecs::cursor cur(world, Position, pos_ptr);
    cur.push();
    cur.member("x");
    cur.set_int(10);
    cur.member("y");
    cur.set_int(20);
    cur.pop();

    flecs::untyped_ref ref = e.get_ref(Position);

    cur = flecs::cursor(world, Position, ref.get());

    cur.push();
    cur.member("x");
    test_int(cur.get_int(), 10);
    cur.member("y");
    test_int(cur.get_int(), 20);
    cur.pop();

}

void Refs_ref_world(void) {
    flecs::world world;
    world.component<Position>();

    flecs::entity e = world.entity().set<Position>({10, 20});

    flecs::ref<Position> r = e.get_ref<Position>();
    auto w = r.world();
    test_assert(w == world);
}

END_DEFINE_SPEC(FFlecsRefsTestsSpec);

/*"id": "Refs",
    "testcases": [
    "get_ref_by_ptr",
    "get_ref_by_method",
    "ref_after_add",
    "ref_after_remove",
    "ref_after_set",
    "ref_before_set",
    "non_const_ref",
    "pair_ref",
    "pair_ref_w_pair_type",
    "pair_ref_w_pair_type_second",
    "pair_ref_w_entity",
    "pair_ref_second",
    "from_stage",
    "default_ctor",
    "ctor_from_entity",
    "implicit_operator_bool",
    "try_get",
    "try_get_after_delete",
    "has",
    "bool_operator",
    "base_type",
    "empty_base_type",
    "get_component",
    "untyped_get_ref_by_method",
    "untyped_pair_ref",
    "untyped_runtime_component_ref",
    "ref_world"
]*/

void FFlecsRefsTestsSpec::Define()
{
	It("Refs_get_ref_by_ptr", [this]() { Refs_get_ref_by_ptr(); });
    It("Refs_get_ref_by_method", [this]() { Refs_get_ref_by_method(); });
    It("Refs_ref_after_add", [this]() { Refs_ref_after_add(); });
    It("Refs_ref_after_remove", [this]() { Refs_ref_after_remove(); });
    It("Refs_ref_after_set", [this]() { Refs_ref_after_set(); });
    It("Refs_ref_before_set", [this]() { Refs_ref_before_set(); });
    It("Refs_non_const_ref", [this]() { Refs_non_const_ref(); });
    It("Refs_pair_ref", [this]() { Refs_pair_ref(); });
    It("Refs_pair_ref_w_pair_type", [this]() { Refs_pair_ref_w_pair_type(); });
    It("Refs_pair_ref_w_pair_type_second", [this]() { Refs_pair_ref_w_pair_type_second(); });
    It("Refs_pair_ref_w_entity", [this]() { Refs_pair_ref_w_entity(); });
    It("Refs_pair_ref_second", [this]() { Refs_pair_ref_second(); });
    It("Refs_from_stage", [this]() { Refs_from_stage(); });
    It("Refs_default_ctor", [this]() { Refs_default_ctor(); });
    It("Refs_ctor_from_entity", [this]() { Refs_ctor_from_entity(); });
    It("Refs_implicit_operator_bool", [this]() { Refs_implicit_operator_bool(); });
    It("Refs_try_get", [this]() { Refs_try_get(); });
    It("Refs_try_get_after_delete", [this]() { Refs_try_get_after_delete(); });
    It("Refs_has", [this]() { Refs_has(); });
    It("Refs_bool_operator", [this]() { Refs_bool_operator(); });
    It("Refs_base_type", [this]() { Refs_base_type(); });
    It("Refs_empty_base_type", [this]() { Refs_empty_base_type(); });
    It("Refs_get_component", [this]() { Refs_get_component(); });
    It("Refs_untyped_get_ref_by_method", [this]() { Refs_untyped_get_ref_by_method(); });
    It("Refs_untyped_pair_ref", [this]() { Refs_untyped_pair_ref(); });
    It("Refs_untyped_runtime_component_ref", [this]() { Refs_untyped_runtime_component_ref(); });
    It("Refs_ref_world", [this]() { Refs_ref_world(); });
}

#endif // WITH_AUTOMATION_TESTS
