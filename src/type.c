#include "private_api.h"

static
bool match_id(
    const ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t match_with)
{
    (void)world;
    
    if (ECS_HAS_ROLE(match_with, CASE)) {
        ecs_entity_t sw = ECS_PAIR_RELATION(match_with);
        if (id == (ECS_SWITCH | sw)) {
#ifdef FLECS_SANITIZE
            ecs_entity_t sw_case = ECS_PAIR_OBJECT(match_with);
            const EcsType *sw_type = ecs_get(world, sw, EcsType);
            ecs_assert(sw_type != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ecs_type_has_id(world, sw_type->normalized, 
                ecs_get_alive(world, sw_case), false) == true,
                        ECS_INVALID_PARAMETER, NULL);
            (void)sw_case;
            (void)sw_type;
#endif
            return true;
        } else {
            return false;
        }
    } else {
        return ecs_id_match(id, match_with);
    }
}

static
int32_t search_type(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    int32_t depth,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    int32_t *count_out)
{
    ecs_assert(offset >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(depth >= 0, ECS_INTERNAL_ERROR, NULL);

    if (!id) {
        return -1;
    }

    if (!type) {
        return -1;
    }

    if (max_depth && depth > max_depth) {
        return -1;
    }

    int32_t i, count = ecs_vector_count(type);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t), tid;

    if (depth >= min_depth) {
        if (table && !offset && !(ECS_HAS_ROLE(id, CASE))) {
            ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
            if (tr) {
                int32_t column = tr->column;
                if (count_out) {
                    *count_out = tr->count;
                }
                if (id_out) {
                    *id_out = ids[column];
                }
                return column;
            }
        } else {
            for (i = offset; i < count; i ++) {
                tid = ids[i];
                if (match_id(world, tid, id)) {
                    if (id_out) {
                        *id_out = tid;
                    }
                    return i;
                }
            }
        }
    }

    if (rel && id != EcsPrefab && id != EcsDisabled && 
        ECS_PAIR_RELATION(id) != ecs_id(EcsIdentifier) &&
        ECS_PAIR_RELATION(id) != EcsChildOf)
    {
        int32_t ret;

        for (i = 0; i < count; i ++) {
            tid = ids[i];
            if (!ECS_HAS_RELATION(tid, rel)) {
                continue;
            }

            ecs_entity_t obj = ecs_pair_object(world, tid);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_table_t *obj_table = ecs_get_table(world, obj);
            if (!obj_table) {
                continue;
            }

            if ((ret = search_type(world, obj_table, obj_table->type, 0, id, 
                rel, min_depth, max_depth, depth + 1, subject_out, id_out, NULL)) != -1)
            {
                if (subject_out && !*subject_out) {
                    *subject_out = obj;
                }
                if (id_out && !*id_out) {
                    *id_out = tid;
                }
                return ret;

            /* If the id could not be found on the object and the relationship
             * is not IsA, try substituting the object type with IsA */
            } else if (rel != EcsIsA) {
                if ((ret = search_type(world, obj_table, obj_table->type, 0, 
                    id, EcsIsA, 1, 0, 0, subject_out, id_out, NULL)) != -1) 
                {
                    if (subject_out && !*subject_out) {
                        *subject_out = obj;
                    }
                    if (id_out && !*id_out) {
                        *id_out = tid;
                    }
                    return ret;
                }
            }
        }
    }

    return -1;
}

bool ecs_type_has_id(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_id_t id,
    bool owned)
{
    ecs_poly_assert(world, ecs_world_t);
    
    return search_type(world, NULL, type, 0, id, owned ? 0 : EcsIsA, 0, 0, 0, 
        NULL, NULL, NULL) != -1;
}

int32_t ecs_type_index_of(
    ecs_type_t type, 
    int32_t offset, 
    ecs_id_t id)
{
    return search_type(NULL, NULL, type, offset, id, 0, 0, 0, 0, 
        NULL, NULL, NULL);
}

int32_t ecs_type_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    int32_t *count_out)
{
    ecs_poly_assert(world, ecs_world_t);
    
    if (subject_out) {
        *subject_out = 0;
    }

    return search_type(world, table, type, offset, id, rel, min_depth, 
        max_depth, 0, subject_out, id_out, count_out);
}

char* ecs_type_str(
    const ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t id = ids[i];

        if (i) {
            ecs_strbuf_appendch(&buf, ',');
            ecs_strbuf_appendch(&buf, ' ');
        }

        if (id == 1) {
            ecs_strbuf_appendstr(&buf, "Component");
        } else {
            ecs_id_str_buf(world, id, &buf);
        }
    }

    return ecs_strbuf_get(&buf);
}
