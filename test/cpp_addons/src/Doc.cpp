#include <cpp_addons.h>

void Doc_set_brief(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_brief("A brief description");

    test_assert(e.has<flecs::doc::Description>(flecs::doc::Brief));

    test_str(e.doc_brief(), "A brief description");
}

void Doc_set_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_name("A name");

    test_assert(e.has<flecs::doc::Description>(flecs::Name));

    test_str(e.doc_name(), "A name");
}

void Doc_set_link(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_link("A link");

    test_assert(e.has<flecs::doc::Description>(flecs::doc::Link));

    test_str(e.doc_link(), "A link");
}

void Doc_set_color(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_color("A color");

    test_assert(e.has<flecs::doc::Description>(flecs::doc::Color));

    test_str(e.doc_color(), "A color");
}

void Doc_set_uuid(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_uuid("81f50b40-09ff-4ce0-a388-4a52a14052c7");

    test_assert(e.has<flecs::doc::Description>(flecs::doc::Uuid));

    test_str(e.doc_uuid(), "81f50b40-09ff-4ce0-a388-4a52a14052c7");
}

void Doc_get_name_no_doc_name(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    test_assert(!e.has<flecs::doc::Description>(flecs::Name));

    test_str(e.doc_name(), "Foo");
}

void Doc_set_source_location(void) {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    test_assert(e.doc_source_location() == nullptr);

    e.set_doc_source_location("foo.cpp", 10);

    test_assert(e.has<flecs::doc::SourceLocation>());

    const flecs::doc::SourceLocation *loc = e.doc_source_location();
    test_assert(loc != nullptr);
    test_str(loc->file, "foo.cpp");
    test_int(loc->line, 10);

    test_assert(flecs::doc::get_source_location(e) == loc);

    e.set_doc_source_location(nullptr, 0);
    test_assert(!e.has<flecs::doc::SourceLocation>());
}

void Doc_set_source_location_system(void) {
    flecs::world ecs;

    int32_t line = __LINE__; auto s = ecs.system<Position>()
        .each([](Position&) { });
    s.set_doc_source_location(__FILE__, line);

    const flecs::doc::SourceLocation *loc = s.doc_source_location();
    test_assert(loc != nullptr);
    test_str(loc->file, __FILE__);
    test_int(loc->line, line);
}

void Doc_set_source_location_observer(void) {
    flecs::world ecs;

    auto o = ecs.observer<Position>()
        .event(flecs::OnAdd)
        .each([](Position&) { });
    flecs::doc::set_source_location(o, "observers.cpp", 7);

    const flecs::doc::SourceLocation *loc = 
        flecs::doc::get_source_location(o);
    test_assert(loc != nullptr);
    test_str(loc->file, "observers.cpp");
    test_int(loc->line, 7);
}
