#include <prefab_variant.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Create a prefab. Prefabs are entities that are solely intended as
     * templates for other entities. Prefabs are by default not matched with
     * systems. In that way they are similar to regular entities with the
     * EcsDisbled tag, except that they have more features which are 
     * demonstrated in the nested_prefab example. */
    auto BasePrefab = ecs.prefab("BasePrefab")
        .set<Position>({10, 20});

    auto SubPrefab1 = ecs.prefab("SubPrefab1")
        .add(flecs::IsA, BasePrefab)
        .set<Velocity>({1, 2});

    auto SubPrefab2 = ecs.prefab("SubPrefab2")
        .add(flecs::IsA, BasePrefab)
        .set<Velocity>({3, 4});

    std::cout << SubPrefab1.type().str() << std::endl;

    /* Create two types for SubPrefab1 and SubPrefab2 which automatically
     * override the component values. The Position component will be overridden
     * from the BasePrefab, while Velocity will be overridden from SubPrefab1
     * and SubPrefab2 respectively. */
    auto Sub1 = ecs.type("Sub1")
        .add(flecs::IsA, SubPrefab1)
        .add<Position>()
        .add<Velocity>();

    auto Sub2 = ecs.type("Sub2")
        .add(flecs::IsA, SubPrefab2)
        .add<Position>()
        .add<Velocity>();

    /* Create new entities from Sub1 and Sub2 */
    auto e1 = ecs.entity()
        .add(Sub1);

    auto e2 = ecs.entity()
        .add(Sub2);        

    /* Print values of e1 */
    const Position *p = e1.get<Position>();
    const Velocity *v = e1.get<Velocity>();
    std::cout << "e1 Position: {" << p->x << ", " << p->y << "} (owned = " 
        << e1.owns<Position>() << "} " << std::endl;
    std::cout << "e1 Velocity: {" << v->x << ", " << v->y << "} (owned = " 
        << e1.owns<Velocity>() << "} " << std::endl;

    /* Print values of e2 */
    p = e2.get<Position>();
    v = e2.get<Velocity>();
    std::cout << "e2 Position: {" << p->x << ", " << p->y << "} (owned = " 
        << e2.owns<Position>() << "} " << std::endl;
    std::cout << "e2 Velocity: {" << v->x << ", " << v->y << "} (owned = " 
        << e2.owns<Velocity>() << "} " << std::endl;        
}
