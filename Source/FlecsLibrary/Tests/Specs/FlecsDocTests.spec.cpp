#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "Misc/AutomationTest.h"
#include "flecs.h"
#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"

BEGIN_DEFINE_SPEC(FFlecsDocTestsSpec,
                  "FlecsLibrary.Doc",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

void Doc_set_brief()
{
	flecs::world ecs;

	auto e = ecs.entity("Foo");

	e.set_doc_brief("A brief description");

	test_assert(e.has<flecs::doc::Description>(flecs::doc::Brief));

	test_str(e.doc_brief(), "A brief description");
}

void Doc_set_name()
{
	flecs::world ecs;

	auto e = ecs.entity("Foo");

	e.set_doc_name("A name");

	test_assert(e.has<flecs::doc::Description>(flecs::Name));

	test_str(e.doc_name(), "A name");
}

void Doc_set_link()
{
	flecs::world ecs;

	auto e = ecs.entity("Foo");

	e.set_doc_link("A link");

	test_assert(e.has<flecs::doc::Description>(flecs::doc::Link));

	test_str(e.doc_link(), "A link");
}

void Doc_set_color()
{
	flecs::world ecs;

	auto e = ecs.entity("Foo");

	e.set_doc_color("A color");

	test_assert(e.has<flecs::doc::Description>(flecs::doc::Color));

	test_str(e.doc_color(), "A color");
}

void Doc_set_uuid()
{
	flecs::world ecs;

	auto e = ecs.entity("Foo");

	e.set_doc_uuid("81f50b40-09ff-4ce0-a388-4a52a14052c7");

	test_assert(e.has<flecs::doc::Description>(flecs::doc::Uuid));

	test_str(e.doc_uuid(), "81f50b40-09ff-4ce0-a388-4a52a14052c7");
}

void Doc_get_name_no_doc_name()
{
	flecs::world ecs;

	auto e = ecs.entity("Foo");

	test_assert(!e.has<flecs::doc::Description>(flecs::Name));

	test_str(e.doc_name(), "Foo");
}

END_DEFINE_SPEC(FFlecsDocTestsSpec);

/*"id": "Doc",
"testcases": [
"set_brief",
"set_name",
"set_link",
"set_color",
"set_uuid",
"get_name_no_doc_name"
]
}]*/

void FFlecsDocTestsSpec::Define()
{
	It("Doc_set_brief", [this]() { Doc_set_brief(); });
	It("Doc_set_name", [this]() { Doc_set_name(); });
	It("Doc_set_link", [this]() { Doc_set_link(); });
	It("Doc_set_color", [this]() { Doc_set_color(); });
	It("Doc_set_uuid", [this]() { Doc_set_uuid(); });
	It("Doc_get_name_no_doc_name", [this]() { Doc_get_name_no_doc_name(); });
}

#endif // WITH_AUTOMATION_TESTS
