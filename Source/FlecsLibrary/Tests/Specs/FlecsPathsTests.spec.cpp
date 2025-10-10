
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "flecs.h"

#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"

BEGIN_DEFINE_SPEC(FFlecsPathsTestsSpec,
                  "FlecsLibrary.Paths",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

void Paths_name(void) {
    flecs::world world;

    auto e = flecs::entity(world, "foo");
    test_str(e.name().c_str(), "foo");

    auto e_world = world.lookup("foo");
    test_assert(e.id() == e_world.id());

    e_world = world.lookup("::foo");
    test_assert(e.id() == e_world.id());
}

void Paths_path_depth_1(void) {
	flecs::world world;

	auto e = flecs::entity(world, "foo::bar");
	test_str(e.name().c_str(), "bar");
	test_str(e.path().c_str(), "::foo::bar");

	auto e_world = world.lookup("bar");
	test_assert(0 == e_world.id());

	e_world = world.lookup("foo::bar");
	test_assert(e.id() == e_world.id());  

	e_world = world.lookup("::foo::bar");
	test_assert(e.id() == e_world.id());        
}

void Paths_path_depth_2(void) {
	flecs::world world;

	auto e = flecs::entity(world, "foo::bar::hello");
	test_str(e.name().c_str(), "hello");
	test_str(e.path().c_str(), "::foo::bar::hello");

	auto e_world = world.lookup("hello");
	test_assert(0 == e_world.id());

	e_world = world.lookup("foo::bar::hello");
	test_assert(e.id() == e_world.id());  

	e_world = world.lookup("::foo::bar::hello");
	test_assert(e.id() == e_world.id());  
}

void Paths_entity_lookup_name(void) {
	flecs::world world;

	auto parent = flecs::entity(world, "foo");
	test_str(parent.name().c_str(), "foo");
	test_str(parent.path().c_str(), "::foo");    

	auto e = flecs::entity(world, "foo::bar");
	test_str(e.name().c_str(), "bar");
	test_str(e.path().c_str(), "::foo::bar");

	auto parent_e = parent.lookup("bar");
	test_assert(e.id() == parent_e.id());

	parent_e = parent.lookup("::foo::bar");
	test_assert(e.id() == parent_e.id());
}

void Paths_entity_lookup_depth_1(void) {
	flecs::world world;

	auto parent = flecs::entity(world, "foo");
	test_str(parent.name().c_str(), "foo");
	test_str(parent.path().c_str(), "::foo");    

	auto e = flecs::entity(world, "foo::bar::hello");
	test_str(e.name().c_str(), "hello");
	test_str(e.path().c_str(), "::foo::bar::hello");

	auto parent_e = parent.lookup("bar::hello");
	test_assert(e.id() == parent_e.id());

	parent_e = parent.lookup("::foo::bar::hello");
	test_assert(e.id() == parent_e.id());
}

void Paths_entity_lookup_depth_2(void) {
    flecs::world world;

    auto parent = flecs::entity(world, "foo");
    test_str(parent.name().c_str(), "foo");
    test_str(parent.path().c_str(), "::foo");    

    auto e = flecs::entity(world, "foo::bar::hello::world");
    test_str(e.name().c_str(), "world");
    test_str(e.path().c_str(), "::foo::bar::hello::world");

    auto parent_e = parent.lookup("bar::hello::world");
    test_assert(e.id() == parent_e.id());

    parent_e = parent.lookup("::foo::bar::hello::world");
    test_assert(e.id() == parent_e.id());
}

void Paths_alias_component(void) {
    flecs::world ecs;
	RegisterTestTypeComponents(ecs);

    auto e = ecs.use<Position>("MyPosition");
    auto a = ecs.lookup("MyPosition");
    auto c = ecs.lookup("Position");

    test_assert(e.id() == a.id());
    test_assert(e.id() == c.id());
}

void Paths_alias_scoped_component(void) {
    flecs::world ecs;
	RegisterTestTypeComponents(ecs);

    auto e = ecs.use<test::Foo>();
    auto a = ecs.lookup("Foo");
    auto c = ecs.lookup("test::Foo");
    
    test_assert(e.id() == a.id());
    test_assert(e.id() == c.id());
}

void Paths_alias_scoped_component_w_name(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    auto e = ecs.use<test::Foo>("FooAlias");
    auto a = ecs.lookup("FooAlias");
    auto f = ecs.lookup("Foo");
    auto c = ecs.lookup("test::Foo");
    
    test_assert(e.id() == a.id());
    test_assert(e.id() == c.id());
    test_assert(f.id() == 0);
}

void Paths_alias_entity(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    ecs.use(e, "FooAlias");

    auto a = ecs.lookup("FooAlias");

    test_assert(e.id() == a.id());
}

void Paths_alias_entity_by_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    ecs.use(e, "FooAlias");

    auto l = ecs.lookup("FooAlias");

    test_assert(e.id() == l.id());
}

void Paths_alias_entity_by_scoped_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo::Bar");

    auto a = ecs.use("Foo::Bar", "FooAlias");

    auto l = ecs.lookup("FooAlias");

    test_assert(e.id() == a.id());
    test_assert(e.id() == l.id());
}

void Paths_alias_entity_empty(void) {
    flecs::world ecs;

    auto parent = ecs.entity("parent");
    auto child = ecs.entity("child").child_of(parent);

    auto e = ecs.lookup("child");

    test_assert(e.id() == 0);

    ecs.use(child); // alias being nullptr

    e = ecs.lookup("child");

    test_assert(e.id() != 0);

    ecs.use(child, "FooAlias");

    e = ecs.lookup("child");

    test_assert(e.id() == 0);

    e = ecs.lookup("FooAlias");

    test_assert(e.id() != 0);
}

void Paths_id_from_str_0_entity(void) {
    flecs::world ecs;

    flecs::id id = ecs.id("#0");
    test_assert(id == 0);
}

void Paths_id_from_str_entity_from_str(void) {
    flecs::world ecs;

    flecs::entity foo = ecs.entity("foo");

    flecs::id id = ecs.id("foo");
    test_assert(id != 0);
    test_assert(id == foo);
}

void Paths_id_from_str_unresolved_entity_from_str(void) {
	flecs::world ecs;

	flecs::id id = ecs.id("foo");
	test_assert(id == 0);
}

void Paths_id_from_str_scoped_entity_from_str(void) {
    flecs::world ecs;

    flecs::entity foo = ecs.entity("foo::bar");

    flecs::id id = ecs.id("foo.bar");
    test_assert(id != 0);
    test_assert(id == foo);
}

void Paths_id_from_str_template_entity_from_str(void) {
    flecs::world ecs;

    flecs::entity foo = ecs.entity("foo<bar>");

    flecs::id id = ecs.id("foo<bar>");
    test_assert(id != 0);
    test_assert(id == foo);
}

void Paths_id_from_str_pair_from_str(void) {
	flecs::world ecs;

	flecs::entity rel = ecs.entity("Rel");
	flecs::entity tgt = ecs.entity("Tgt");

	flecs::id id = ecs.id("(Rel, Tgt)");
	test_assert(id != 0);
	test_assert(id == ecs.pair(rel, tgt));
}

void Paths_id_from_str_unresolved_pair_from_str(void) {
    flecs::world ecs;

    ecs.entity("Rel");

    flecs::id id = ecs.id("(Rel, Tgt)");
    test_assert(id == 0);
}

void Paths_id_from_str_wildcard_pair_from_str(void) {
    flecs::world ecs;

    flecs::entity rel = ecs.entity("Rel");

    flecs::id id = ecs.id("(Rel, *)");
    test_assert(id != 0);
    test_assert(id == ecs.pair(rel, flecs::Wildcard));
}

void Paths_id_from_str_any_pair_from_str(void) {
    flecs::world ecs;

    flecs::entity rel = ecs.entity("Rel");

    flecs::id id = ecs.id("(Rel, _)");
    test_assert(id != 0);
    test_assert(id == ecs.pair(rel, flecs::Any));
}

void Paths_id_from_str_invalid_pair(void) {
    flecs::world ecs;

    ecs.entity("Rel");
    ecs.entity("Tgt");

    flecs::id id = ecs.id("(Rel, Tgt");
    test_assert(id == 0);
}

END_DEFINE_SPEC(FFlecsPathsTestsSpec);

/*"id": "Paths",
	"testcases": [
	"name",
	"path_depth_1",
	"path_depth_2",
	"entity_lookup_name",
	"entity_lookup_depth_1",
	"entity_lookup_depth_2",
	"entity_lookup_from_0",
	"entity_lookup_from_0_w_world",
	"alias_component",
	"alias_scoped_component",
	"alias_scoped_component_w_name",
	"alias_entity",
	"alias_entity_by_name",
	"alias_entity_by_scoped_name",
	"alias_entity_empty",
	"id_from_str_0_entity",
	"id_from_str_entity_from_str",
	"id_from_str_unresolved_entity_from_str",
	"id_from_str_scoped_entity_from_str",
	"id_from_str_template_entity_from_str",
	"id_from_str_pair_from_str",
	"id_from_str_unresolved_pair_from_str",
	"id_from_str_wildcard_pair_from_str",
	"id_from_str_any_pair_from_str",
	"id_from_str_invalid_pair"
]*/

void FFlecsPathsTestsSpec::Define()
{
	It("Paths_name", [this]() { Paths_name(); });
	It("Paths_path_depth_1", [this]() { Paths_path_depth_1(); });
	It("Paths_path_depth_2", [this]() { Paths_path_depth_2(); });
	It("Paths_entity_lookup_name", [this]() { Paths_entity_lookup_name(); });
	It("Paths_entity_lookup_depth_1", [this]() { Paths_entity_lookup_depth_1(); });
	It("Paths_entity_lookup_depth_2", [this]() { Paths_entity_lookup_depth_2(); });
	It("Paths_alias_component", [this]() { Paths_alias_component(); });
	It("Paths_alias_scoped_component", [this]() { Paths_alias_scoped_component(); });
	It("Paths_alias_scoped_component_w_name", [this]() { Paths_alias_scoped_component_w_name(); });
	It("Paths_alias_entity", [this]() { Paths_alias_entity(); });
	It("Paths_alias_entity_by_name", [this]() { Paths_alias_entity_by_name(); });
	It("Paths_alias_entity_by_scoped_name", [this]() { Paths_alias_entity_by_scoped_name(); });
	It("Paths_alias_entity_empty", [this]() { Paths_alias_entity_empty(); });
	It("Paths_id_from_str_0_entity", [this]() { Paths_id_from_str_0_entity(); });
	It("Paths_id_from_str_entity_from_str", [this]() { Paths_id_from_str_entity_from_str(); });
	It("Paths_id_from_str_unresolved_entity_from_str", [this]() { Paths_id_from_str_unresolved_entity_from_str(); });
	It("Paths_id_from_str_scoped_entity_from_str", [this]() { Paths_id_from_str_scoped_entity_from_str(); });
	It("Paths_id_from_str_template_entity_from_str", [this]() { Paths_id_from_str_template_entity_from_str(); });
	It("Paths_id_from_str_pair_from_str", [this]() { Paths_id_from_str_pair_from_str(); });
	It("Paths_id_from_str_unresolved_pair_from_str", [this]() { Paths_id_from_str_unresolved_pair_from_str(); });
	It("Paths_id_from_str_wildcard_pair_from_str", [this]() { Paths_id_from_str_wildcard_pair_from_str(); });
	It("Paths_id_from_str_any_pair_from_str", [this]() { Paths_id_from_str_any_pair_from_str(); });
	It("Paths_id_from_str_invalid_pair", [this]() { Paths_id_from_str_invalid_pair(); });
}

#endif // WITH_AUTOMATION_TESTS
