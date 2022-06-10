#include <hooks.h>
#include <iostream>

// Component hooks are callbacks that can be registered for a type that are
// invoked during different parts of the component lifecycle.
//
// In the C++ API the ctor, dtor, copy and move methods of a type are 
// automatically registered as component hooks.

struct String {
    String(const char *v = nullptr) {
        flecs::log::trace("Ctor(const char*)");
        value = ecs_os_strdup(v);
    }

    ~String() {
        flecs::log::trace("Dtor");
        ecs_os_free(value);
    }

    String(const String& obj) {
        flecs::log::trace("Copy");
        ecs_os_free(value);
        value = ecs_os_strdup(obj.value);
    }

    String(String&& obj) {
        flecs::log::trace("Move");
        ecs_os_free(value);
        value = obj.value;
        obj.value = nullptr;
    }

    String& operator=(const String& obj) {
        flecs::log::trace("Copy assign");
        ecs_os_free(value);
        value = ecs_os_strdup(obj.value);
        return *this;
    }

    String& operator=(String&& obj) {
        flecs::log::trace("Move assign");
        ecs_os_free(value);
        value = obj.value;
        obj.value = nullptr;
        return *this;
    }

    char *value;
};

struct Tag { };

int main(int, char *[]) {
    flecs::world ecs;

    // Register additional add, remove and set hooks for type
    ecs.component<String>()
        .on_add([](flecs::entity e, String&) {
            flecs::log::trace("OnAdd: %s", e.name().c_str());
        })
        .on_remove([](flecs::entity e, String&) {
            flecs::log::trace("OnRemove: %s", e.name().c_str());
        })
        .on_set([](flecs::entity e, String&) {
            flecs::log::trace("OnSet: %s", e.name().c_str());
        });

    // Register in advance to keep output trace clean
    ecs.component<Tag>();

    flecs::log::set_level(0);

    auto e = ecs.entity("Entity");

    flecs::log::push("e.add<String>()");
    e.add<String>();
    flecs::log::pop();

    flecs::log::push("e.set<String>({\"Hello World\"})");
    e.set<String>({"Hello World"});
    flecs::log::pop();

    // This operation changes the entity's archetype, which invokes a move
    flecs::log::push("e.add<Tag>()");
    e.add<Tag>();
    flecs::log::pop();

    flecs::log::push("e.destruct()");
    e.destruct();
    flecs::log::pop();

    flecs::log::set_level(-1);

    // Output:
    //  info: e.add<String>()
    //  info: | Ctor
    //  info: | OnAdd: Entity
    //  info: e.set<String>({"Hello World"})
    //  info: | Ctor
    //  info: | Move assign
    //  info: | OnSet: Entity
    //  info: | Dtor
    //  info: e.add<Tag>()
    //  info: | Move
    //  info: | Dtor
    //  info: e.destruct()
    //  info: | OnRemove: Entity
    //  info: | Dtor
}
