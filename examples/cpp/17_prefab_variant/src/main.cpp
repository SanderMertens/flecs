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
        .set<Position>({10, 20})
        .add_owned<Position>(); // Ensure that component is always overridden

    auto SubPrefab1 = ecs.prefab("SubPrefab1")
        .is_a(BasePrefab)
        .set<Velocity>({1, 2})
        .add_owned<Velocity>();

    auto SubPrefab2 = ecs.prefab("SubPrefab2")
        .is_a(BasePrefab)
        .set<Velocity>({3, 4})
        .add_owned<Velocity>();

    /* Create new entities from prefabs */
    auto e1 = ecs.entity()
        .is_a(SubPrefab1);

    auto e2 = ecs.entity()
        .is_a(SubPrefab2);        

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
