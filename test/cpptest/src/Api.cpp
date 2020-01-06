#include <cpptest.h>

typedef struct MyModule {
    ECS_DECLARE_COMPONENT(Position);
} MyModule;

void Api_module() {
    ecs_world_t *world = ecs_init();

    ECS_MODULE(world, MyModule);
    ECS_COMPONENT(world, Position);
    ECS_EXPORT_COMPONENT(Position);

    MyModule *module_ptr = ecs_get_ptr(world, EcsSingleton, MyModule);
    test_assert(module_ptr != NULL);
    test_assert(module_ptr->ecs_entity(Position) == ecs_entity(Position));

    MyModule *module_ptr2 = ecs_get_singleton_ptr(world, MyModule);
    test_assert(module_ptr2 != NULL);
    test_assert(module_ptr2 == module_ptr);


}
