#include <instancing.h>
#include <iostream>

/* Instancing is the ability of queries to iterate results with fields that have
 * different numbers of elements. The term "instancing" is borrowed from 
 * graphics APIs, where it means reusing the same data for multiple "instances".
 * 
 * Query instancing works in much the same way. By default queries match all
 * components on the same entity. It is however possible to request data from
 * other entities, like getting the Position from the entity's parent.
 * 
 * Instancing refers to the ability of queries to iterate components for 
 * multiple entities while at the same time providing "instanced" components,
 * which are always provided one element at a time.
 * 
 * Instancing is often used in combination with parent-child relationships and
 * prefabs, but is applicable to any kind of query where some of the terms are
 * matched on N entities, and some on a single entity.
 * 
 * By default queries are not instanced, which means that if a result contains
 * mixed fields, entities will be iterated one by one instead of in batches.
 * This is safer, as code doesn't have to do anything different for owned and
 * shared fields, but does come at a performance penalty.
 * 
 * The each() iterator function always uses an instanced iterator under the
 * hood. This is transparent to the application, but improves performance. For
 * this reason using each() can be faster than using uninstanced iter().
 */

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create a query for Position, Velocity. We'll create a few entities that
    // have Velocity as owned and shared component.
    auto q = ecs.query_builder<Position, const Velocity>()
        .term_at(1).self()       // Position must always be owned by the entity
        .instanced()             // create instanced query
        .build();

    // Create a prefab with Velocity. Prefabs are not matched with queries.
    auto prefab = ecs.prefab("p")
        .set<Velocity>({1, 2});

    // Create a few entities that own Position & share Velocity from the prefab.
    ecs.entity("e1").is_a(prefab)
        .set<Position>({10, 20});

    ecs.entity("e2").is_a(prefab)
        .set<Position>({10, 20});

    // Create a few entities that own all components
    ecs.entity("e3")
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    ecs.entity("e4")
        .set<Position>({10, 20})
        .set<Velocity>({4, 5});


    // Iterate the instanced query. Note how when a query is instanced, it needs
    // to check whether a field is owned or not in order to know how to access
    // it. In the case of an owned field it is iterated as an array, whereas
    // in the case of a shared field, it is accessed as a pointer.
    q.iter([](flecs::iter& it, Position *p, const Velocity *v) {

        // Check if Velocity is owned, in which case it's accessed as array.
        // Position will always be owned, since we set the term to Self.
        if (it.is_self(2)) { // Velocity is term 2
            std::cout << "Velocity is owned\n";
            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
                std::cout << it.entity(i).name() << 
                    ": {" << p[i].x << ", " << p[i].y << "}\n";
            }

        // If Velocity is shared, access the field as a pointer.
        } else {
            std::cout << "Velocity is shared\n";
            for (auto i : it) {
                p[i].x += v->x;
                p[i].y += v->y;
                std::cout << it.entity(i).name() << 
                    ": {" << p[i].x << ", " << p[i].y << "}\n";
            }
        }
    });
}
