#include <entity_type.h>
#include <stdio.h>

ECS_STRUCT(TypeWithEntity, {
    ecs_entity_t e;
});

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_META_COMPONENT(ecs, TypeWithEntity);

    ecs_entity_t foo = ecs_set_name(ecs, 0, "Foo");
    ecs_entity_t e = ecs_set(ecs, 0, TypeWithEntity, {foo});

    // Convert component to flecs expression string
    const TypeWithEntity *ptr = ecs_get(ecs, e, TypeWithEntity);
    char *str = ecs_ptr_to_expr(ecs, ecs_id(TypeWithEntity), ptr);
    printf("%s\n", str); // {e: Foo}
    ecs_os_free(str);

    return ecs_fini(ecs);
}
