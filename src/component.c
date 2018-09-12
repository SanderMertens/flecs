#include "reflecs.h"

EcsEntity* ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    EcsEntity *result = ecs_new(world, id);
    EcsComponent *component_data = ecs_add(result, world->component);
    component_data->size = size;
    return result;
}

EcsArray* ecs_components_diff(
    EcsArray *to,
    EcsArray *from)
{
    EcsIter it = ecs_array_iter(to, &entityptr_arr_params);
    bool found;
    EcsArray *result = NULL;

    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e_to = *(EcsEntity**)ecs_iter_next(&it);

        EcsArrayIter iter_data;
        EcsIter it_new = _ecs_array_iter(from, &entityptr_arr_params, &iter_data);
        found = false;
        while (!found && ecs_iter_hasnext(&it_new)) {
            EcsEntity *e_from = *(EcsEntity**)ecs_iter_next(&it);
            if (e_from == e_to) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (!result) {
                result = ecs_array_new(4, &entityptr_arr_params);
            }

            EcsEntity **elem;
            ecs_array_add(result, &entityptr_arr_params, &elem);
            *elem = e_to;
        }
    }

    return result;
}

bool ecs_components_is_union_empty(
    EcsArray *to,
    EcsArray *from)
{
    EcsIter it = ecs_array_iter(to, &entityptr_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e_to = *(EcsEntity**)ecs_iter_next(&it);

        EcsArrayIter iter_data;
        EcsIter it_new = _ecs_array_iter(from, &entityptr_arr_params, &iter_data);
        while (ecs_iter_hasnext(&it_new)) {
            EcsEntity *e_from = *(EcsEntity**)ecs_iter_next(&it_new);
            if (e_from == e_to) {
                return false;
            }
        }
    }

    return true;
}
