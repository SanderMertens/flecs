#include <cpp_api.h>

void Doc_set_brief() {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_brief("A brief description");

    test_assert(e.has<flecs::doc::Description>(flecs::doc::Brief));

    test_str(e.doc_brief(), "A brief description");
}

void Doc_set_name() {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_name("A name");

    test_assert(e.has<flecs::doc::Description>(flecs::Name));

    test_str(e.doc_name(), "A name");
}

void Doc_set_link() {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_link("A link");

    test_assert(e.has<flecs::doc::Description>(flecs::doc::Link));

    test_str(e.doc_link(), "A link");
}

void Doc_set_color() {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    e.set_doc_color("A color");

    test_assert(e.has<flecs::doc::Description>(flecs::doc::Color));

    test_str(e.doc_color(), "A color");
}

void Doc_get_name_no_doc_name() {
    flecs::world ecs;

    auto e = ecs.entity("Foo");

    test_assert(!e.has<flecs::doc::Description>(flecs::Name));

    test_str(e.doc_name(), "Foo");
}
