#include <doc.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world world;

    auto e = world.entity();
    flecs::doc::set_brief(e, "brief");
    flecs::doc::set_detail(e, "detail");
    flecs::doc::set_link(e, "link");

    assert(!strcmp(flecs::doc::get_brief(e), "brief"));
    assert(!strcmp(flecs::doc::get_detail(e), "detail"));
    assert(!strcmp(flecs::doc::get_link(e), "link"));
}
