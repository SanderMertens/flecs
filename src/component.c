#include "include/private/reflecs.h"

EcsArray* ecs_components_diff(
    EcsArray *to,
    EcsArray *from)
{
    EcsIter it = ecs_array_iter(to, &handle_arr_params);
    bool found;
    EcsArray *result = NULL;

    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e_to = *(EcsEntity**)ecs_iter_next(&it);

        EcsArrayIter iter_data;
        EcsIter it_new = _ecs_array_iter(from, &handle_arr_params, &iter_data);
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
                result = ecs_array_new(4, &handle_arr_params);
            }

            EcsEntity **elem;
            ecs_array_add(result, &handle_arr_params, &elem);
            *elem = e_to;
        }
    }

    return result;
}

bool ecs_components_is_union_empty(
    EcsArray *to,
    EcsArray *from)
{
    EcsIter it = ecs_array_iter(to, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e_to = *(EcsEntity**)ecs_iter_next(&it);

        EcsArrayIter iter_data;
        EcsIter it_new = _ecs_array_iter(from, &handle_arr_params, &iter_data);
        while (ecs_iter_hasnext(&it_new)) {
            EcsEntity *e_from = *(EcsEntity**)ecs_iter_next(&it_new);
            if (e_from == e_to) {
                return false;
            }
        }
    }

    return true;
}

EcsHandle ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    EcsHandle result = ecs_new(world);
    ecs_stage(world, result, world->component);
    ecs_stage(world, result, world->id);
    ecs_commit(world, result);
    EcsComponent *component_data = ecs_get(world, result, world->component);
    component_data->size = size;
    EcsId *id_data = ecs_get(world, result, world->id);
    id_data->id = id;
    return result;
}
