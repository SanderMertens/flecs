
#include "json.h"

#ifdef FLECS_JSON

static
int json_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *ser, 
    const void *base, 
    ecs_strbuf_t *str);

static
int json_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str);

static
int json_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base,
    ecs_strbuf_t *str);

/* Serialize enumeration */
static
int json_ser_enum(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const EcsEnum *enum_type = ecs_get(world, op->type, EcsEnum);
    ecs_check(enum_type != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t value = *(int32_t*)base;
    
    /* Enumeration constants are stored in a map that is keyed on the
     * enumeration value. */
    ecs_enum_constant_t *constant = ecs_map_get(
        enum_type->constants, ecs_enum_constant_t, value);
    if (!constant) {
        goto error;
    }

    ecs_strbuf_appendch(str, '"');
    ecs_strbuf_appendstr(str, ecs_get_name(world, constant->constant));
    ecs_strbuf_appendch(str, '"');

    return 0;
error:
    return -1;
}

/* Serialize bitmask */
static
int json_ser_bitmask(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsBitmask *bitmask_type = ecs_get(world, op->type, EcsBitmask);
    ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);

    uint32_t value = *(uint32_t*)ptr;
    ecs_map_key_t key;
    ecs_bitmask_constant_t *constant;

    if (!value) {
        ecs_strbuf_appendch(str, '0');
        return 0;
    }

    ecs_strbuf_list_push(str, "\"", "|");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(bitmask_type->constants);
    while ((constant = ecs_map_next(&it, ecs_bitmask_constant_t, &key))) {
        if ((value & key) == key) {
            ecs_strbuf_list_appendstr(str, 
                ecs_get_name(world, constant->constant));
            value -= (uint32_t)key;
        }
    }

    if (value != 0) {
        /* All bits must have been matched by a constant */
        goto error;
    }

    ecs_strbuf_list_pop(str, "\"");

    return 0;
error:
    return -1;
}

/* Serialize elements of a contiguous array */
static
int json_ser_elements(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops, 
    int32_t op_count,
    const void *base, 
    int32_t elem_count, 
    int32_t elem_size,
    ecs_strbuf_t *str)
{
    json_array_push(str);

    const void *ptr = base;

    int i;
    for (i = 0; i < elem_count; i ++) {
        ecs_strbuf_list_next(str);
        if (json_ser_type_ops(world, ops, op_count, ptr, str)) {
            return -1;
        }
        ptr = ECS_OFFSET(ptr, elem_size);
    }

    json_array_pop(str);

    return 0;
}

static
int json_ser_type_elements(
    const ecs_world_t *world,
    ecs_entity_t type, 
    const void *base, 
    int32_t elem_count, 
    ecs_strbuf_t *str)
{
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_meta_type_op_t *ops = ecs_vector_first(ser->ops, ecs_meta_type_op_t);
    int32_t op_count = ecs_vector_count(ser->ops);

    return json_ser_elements(
        world, ops, op_count, base, elem_count, comp->size, str);
}

/* Serialize array */
static
int json_ser_array(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr, 
    ecs_strbuf_t *str) 
{
    const EcsArray *a = ecs_get(world, op->type, EcsArray);
    ecs_assert(a != NULL, ECS_INTERNAL_ERROR, NULL);

    return json_ser_type_elements(
        world, a->type, ptr, a->count, str);
}

/* Serialize vector */
static
int json_ser_vector(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_vector_t *value = *(ecs_vector_t**)base;
    if (!value) {
        ecs_strbuf_appendstr(str, "null");
        return 0;
    }

    const EcsVector *v = ecs_get(world, op->type, EcsVector);
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsComponent *comp = ecs_get(world, v->type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vector_count(value);
    void *array = ecs_vector_first_t(value, comp->size, comp->alignment);

    /* Serialize contiguous buffer of vector */
    return json_ser_type_elements(world, v->type, array, count, str);
}

/* Forward serialization to the different type kinds */
static
int json_ser_type_op(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *ptr,
    ecs_strbuf_t *str) 
{
    switch(op->kind) {
    case EcsOpPush:
    case EcsOpPop:
        /* Should not be parsed as single op */
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
        break;
    case EcsOpF32:
        ecs_strbuf_appendflt(str, 
            (ecs_f64_t)*(ecs_f32_t*)ECS_OFFSET(ptr, op->offset), '"');
        break;
    case EcsOpF64:
        ecs_strbuf_appendflt(str, 
            *(ecs_f64_t*)ECS_OFFSET(ptr, op->offset), '"');
        break;
    case EcsOpEnum:
        if (json_ser_enum(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpBitmask:
        if (json_ser_bitmask(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpArray:
        if (json_ser_array(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpVector:
        if (json_ser_vector(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpEntity: {
        ecs_entity_t e = *(ecs_entity_t*)ECS_OFFSET(ptr, op->offset);
        if (!e) {
            ecs_strbuf_appendch(str, '0');
        } else {
            char *path = ecs_get_fullpath(world, e);
            ecs_assert(path != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_strbuf_append(str, "\"%s\"", path);
            ecs_os_free(path);
        }
        break;
    }

    default:
        if (ecs_primitive_to_expr_buf(world, 
            json_op_to_primitive_kind(op->kind), 
            ECS_OFFSET(ptr, op->offset), str)) 
        {
            /* Unknown operation */
            ecs_throw(ECS_INTERNAL_ERROR, NULL);
            return -1;
        }
        break;
    }

    return 0;
error:
    return -1;
}

/* Iterate over a slice of the type ops array */
static
int json_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base,
    ecs_strbuf_t *str) 
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];

        if (op != ops) {
            if (op->name) {
                json_member(str, op->name);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1 && op != ops) {
                /* Serialize inline array */
                if (json_ser_elements(world, op, op->op_count, base,
                    elem_count, op->size, str))
                {
                    return -1;
                }

                i += op->op_count - 1;
                continue;
            }
        }
        
        switch(op->kind) {
        case EcsOpPush:
            json_object_push(str);
            break;
        case EcsOpPop:
            json_object_pop(str);
            break;
        default:
            if (json_ser_type_op(world, op, base, str)) {
                goto error;
            }
            break;
        }
    }

    return 0;
error:
    return -1;
}

/* Iterate over the type ops of a type */
static
int json_ser_type(
    const ecs_world_t *world,
    ecs_vector_t *v_ops,
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_meta_type_op_t *ops = ecs_vector_first(v_ops, ecs_meta_type_op_t);
    int32_t count = ecs_vector_count(v_ops);
    return json_ser_type_ops(world, ops, count, base, str);
}

static
int array_to_json_buf_w_type_data(
    const ecs_world_t *world,
    const void *ptr,
    int32_t count,
    ecs_strbuf_t *buf,
    const EcsComponent *comp,
    const EcsMetaTypeSerialized *ser)
{
    if (count) {
        ecs_size_t size = comp->size;

        json_array_push(buf);

        do {
            ecs_strbuf_list_next(buf);
            if (json_ser_type(world, ser->ops, ptr, buf)) {
                return -1;
            }

            ptr = ECS_OFFSET(ptr, size);
        } while (-- count);

        json_array_pop(buf);
    } else {
        if (json_ser_type(world, ser->ops, ptr, buf)) {
            return -1;
        }
    }

    return 0;
}

int ecs_array_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    int32_t count,
    ecs_strbuf_t *buf)
{
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    if (!comp) {
        char *path = ecs_get_fullpath(world, type);
        ecs_err("cannot serialize to JSON, '%s' is not a component", path);
        ecs_os_free(path);
        return -1;
    }

    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    if (!ser) {
        char *path = ecs_get_fullpath(world, type);
        ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
        ecs_os_free(path);
        return -1;
    }

    return array_to_json_buf_w_type_data(world, ptr, count, buf, comp, ser);
}

char* ecs_array_to_json(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr,
    int32_t count)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_array_to_json_buf(world, type, ptr, count, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

int ecs_ptr_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf)
{
    return ecs_array_to_json_buf(world, type, ptr, 0, buf);
}

char* ecs_ptr_to_json(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    return ecs_array_to_json(world, type, ptr, 0);
}

static
int append_type(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf, 
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    ecs_type_t type = ecs_get_type(world, ent);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t i, count = ecs_vector_count(type);

    json_member(buf, "type");
    json_array_push(buf);

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        ecs_entity_t pred = 0, obj = 0, role = 0;

        if (ECS_HAS_RELATION(id, EcsIsA)) {
            /* Skip IsA as they are already added in "inherits" */
            continue;
        }

        if (ent != inst) {
            /* If not serializing the top level entity, skip components that are
             * never inherited from a base entity */
            if (id == ecs_pair(ecs_id(EcsIdentifier), EcsName) ||
                ECS_PAIR_FIRST(id) == EcsChildOf ||
                id == EcsPrefab)
            {
                continue;
            }
        }

        if (ECS_HAS_ROLE(id, PAIR)) {
            pred = ecs_pair_first(world, id);
            obj = ecs_pair_second(world, id);
        } else {
            pred = id & ECS_COMPONENT_MASK;
            if (id & ECS_ROLE_MASK) {
                role = id & ECS_ROLE_MASK;
            }
        }

        if (!desc || !desc->serialize_private) {
            if (ecs_has_id(world, pred, EcsPrivate)) {
                /* Skip private components */
                continue;
            }
        }

        ecs_strbuf_list_next(buf);
        json_object_push(buf);

        if (pred) {
            char *str = ecs_get_fullpath(world, pred);
            json_member(buf, "pred"); json_string(buf, str);
            ecs_os_free(str);
        }
        if (obj) {
            char *str = ecs_get_fullpath(world, obj);
            json_member(buf, "obj"); 
            json_string(buf, str);
            ecs_os_free(str);
        }
        if (role) {
            json_member(buf, "obj"); 
            json_string(buf, ecs_role_str(role));
        }

        bool hidden = false;

        if (ent != inst) {
            if (ecs_get_object_for_id(world, inst, EcsIsA, id) != ent) {
                hidden = true;
                ecs_strbuf_list_appendstr(buf, "\"hidden\":true");
            }
        }

        if (!hidden) {
            ecs_entity_t typeid = ecs_get_typeid(world, id);
            if (typeid) {
                if (!desc || desc->serialize_values) {
                    const EcsMetaTypeSerialized *ser = ecs_get(
                        world, typeid, EcsMetaTypeSerialized);
                    if (ser) {
                        const void *ptr = ecs_get_id(world, ent, id);
                        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                        json_member(buf, "value");
                        if (json_ser_type(world, ser->ops, ptr, buf) != 0) {
                            /* Entity contains invalid value */
                            return -1;
                        }

                        if (desc && desc->serialize_type_info) {
                            json_member(buf, "type_info");
                            if (ecs_type_info_to_json_buf(
                                world, typeid, buf) != 0) 
                            {
                                return -1;
                            }
                        }
                    }
                }
            }
        }

        json_object_pop(buf);
    }

    json_array_pop(buf);
    
    return 0;
}

static
int append_base(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf, 
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    ecs_type_t type = ecs_get_type(world, ent);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            if (append_base(world, buf, ecs_pair_second(world, id), inst, desc)) 
            {
                return -1;
            }
        }
    }

    char *path = ecs_get_fullpath(world, ent);
    json_member(buf, path);
    ecs_os_free(path);

    json_object_push(buf);

    if (append_type(world, buf, ent, inst, desc)) {
        return -1;
    }

    json_object_pop(buf);

    return 0;
}

int ecs_entity_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    const ecs_entity_to_json_desc_t *desc)
{
    if (!entity || !ecs_is_valid(world, entity)) {
        return -1;
    }

    json_object_push(buf);

    if (!desc || desc->serialize_path) {
        char *path = ecs_get_fullpath(world, entity);
        json_member(buf, "path");
        json_string(buf, path);
        ecs_os_free(path);
    }

    ecs_type_t type = ecs_get_type(world, entity);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t i, count = ecs_vector_count(type);

    if (!desc || desc->serialize_base) {
        if (ecs_has_pair(world, entity, EcsIsA, EcsWildcard)) {
            json_member(buf, "is_a");
            json_object_push(buf);

            for (i = 0; i < count; i ++) {
                ecs_id_t id = ids[i];
                if (ECS_HAS_RELATION(id, EcsIsA)) {
                    if (append_base(
                        world, buf, ecs_pair_second(world, id), entity, desc)) 
                    {
                        return -1;
                    }
                }
            }

            json_object_pop(buf);
        }
    }

    if (append_type(world, buf, entity, entity, desc)) {
        goto error;
    }

    json_object_pop(buf);

    return 0;
error:
    return -1;
}

char* ecs_entity_to_json(
    const ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_entity_to_json_desc_t *desc)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ecs_entity_to_json_buf(world, entity, &buf, desc) != 0) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

static
bool skip_variable(
    const char *name)
{
    if (!name || name[0] == '_' || name[0] == '.') {
        return true;
    } else {
        return false;
    }
}

static
void serialize_id(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_strbuf_t *buf) 
{
    json_id(buf, world, id);
}

static
void serialize_iter_ids(
    const ecs_world_t *world,
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf) 
{
    int32_t term_count = it->term_count;
    if (!term_count) {
        return;
    }

    json_member(buf, "ids");
    json_array_push(buf);

    for (int i = 0; i < term_count; i ++) {
        json_next(buf);
        serialize_id(world, it->terms[i].id, buf);
    }

    json_array_pop(buf);
}

static
void serialize_type_info(
    const ecs_world_t *world,
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf) 
{
    int32_t term_count = it->term_count;
    if (!term_count) {
        return;
    }

    json_member(buf, "type_info");
    json_object_push(buf);

    for (int i = 0; i < term_count; i ++) {
        json_next(buf);
        ecs_entity_t typeid = ecs_get_typeid(world, it->terms[i].id);
        if (typeid) {
            serialize_id(world, typeid, buf);
            ecs_strbuf_appendstr(buf, ":");
            ecs_type_info_to_json_buf(world, typeid, buf);
        } else {
            serialize_id(world, it->terms[i].id, buf);
            ecs_strbuf_appendstr(buf, ":");
            ecs_strbuf_appendstr(buf, "0");
        }
    }

    json_object_pop(buf);
}

static
void serialize_iter_variables(ecs_iter_t *it, ecs_strbuf_t *buf) {
    char **variable_names = it->variable_names;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 0; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (skip_variable(var_name)) continue;

        if (!actual_count) {
            json_member(buf, "vars");
            json_array_push(buf);
            actual_count ++;
        }

        ecs_strbuf_list_next(buf);
        json_string(buf, var_name);
    }

    if (actual_count) {
        json_array_pop(buf);
    }
}

static
void serialize_iter_result_ids(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    json_member(buf, "ids");
    json_array_push(buf);

    for (int i = 0; i < it->term_count; i ++) {
        json_next(buf);
        serialize_id(world,  ecs_term_id(it, i + 1), buf);
    }

    json_array_pop(buf);
}

static
void serialize_iter_result_subjects(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    json_member(buf, "subjects");
    json_array_push(buf);

    for (int i = 0; i < it->term_count; i ++) {
        json_next(buf);
        ecs_entity_t subj = it->subjects[i];
        if (subj) {            
            json_path(buf, world, subj);
        } else {
            json_literal(buf, "0");
        }
    }

    json_array_pop(buf);
}

static
void serialize_iter_result_is_set(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    json_member(buf, "is_set");
    json_array_push(buf);

    for (int i = 0; i < it->term_count; i ++) {
        ecs_strbuf_list_next(buf);
        if (ecs_term_is_set(it, i + 1)) {
            json_true(buf);
        } else {
            json_false(buf);
        }
    }

    json_array_pop(buf);
}

static
void serialize_iter_result_variables(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    char **variable_names = it->variable_names;
    ecs_entity_t *variables = it->variables;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 0; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (skip_variable(var_name)) continue;

        if (!actual_count) {
            json_member(buf, "vars");
            json_array_push(buf);
            actual_count ++;
        }

        ecs_strbuf_list_next(buf);
        json_path(buf, world, variables[i]);
    }

    if (actual_count) {
        json_array_pop(buf);
    }
}

static
void serialize_iter_result_variable_labels(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    char **variable_names = it->variable_names;
    ecs_entity_t *variables = it->variables;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 0; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (skip_variable(var_name)) continue;

        if (!actual_count) {
            json_member(buf, "var_labels");
            json_array_push(buf);
            actual_count ++;
        }

        ecs_strbuf_list_next(buf);
        json_label(buf, world, variables[i]);
    }

    if (actual_count) {
        json_array_pop(buf);
    }
}

static
void serialize_iter_result_entities(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    int32_t count = it->count;
    if (!it->count) {
        return;
    }

    json_member(buf, "entities");
    json_array_push(buf);

    ecs_entity_t *entities = it->entities;

    for (int i = 0; i < count; i ++) {
        json_next(buf);
        json_path(buf, world, entities[i]);
    }

    json_array_pop(buf);
}

static
void serialize_iter_result_entity_labels(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    int32_t count = it->count;
    if (!it->count) {
        return;
    }

    json_member(buf, "entity_labels");
    json_array_push(buf);

    ecs_entity_t *entities = it->entities;

    for (int i = 0; i < count; i ++) {
        json_next(buf);
        json_label(buf, world, entities[i]);
    }

    json_array_pop(buf);
}

static
void serialize_iter_result_values(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    int32_t count = it->count;
    if (!it->count) {
        return;
    }

    json_member(buf, "values");
    json_array_push(buf);

    int32_t i, term_count = it->term_count;
    for (i = 0; i < term_count; i ++) {
        ecs_strbuf_list_next(buf);
        
        const void *ptr = it->ptrs[i];
        if (!ptr) {
            /* No data in column */
            json_literal(buf, "0");
            continue;
        }

        /* Get component id (can be different in case of pairs) */
        ecs_entity_t type = ecs_get_typeid(world, it->ids[i]);
        if (!type) {
            /* Odd, we have a ptr but no Component? Not the place of the
             * serializer to complain about that. */
            json_literal(buf, "0");
            continue;
        }

        const EcsComponent *comp = ecs_get(world, type, EcsComponent);
        if (!comp) {
            /* Also odd, typeid but not a component? */
            json_literal(buf, "0");
            continue;
        }

        const EcsMetaTypeSerialized *ser = ecs_get(
            world, type, EcsMetaTypeSerialized);
        if (!ser) {
            /* Not odd, component just has no reflection data */
            json_literal(buf, "0");
            continue;
        }

        if (ecs_term_is_owned(it, i + 1)) {
            array_to_json_buf_w_type_data(world, ptr, count, buf, comp, ser);
        } else {
            array_to_json_buf_w_type_data(world, ptr, 0, buf, comp, ser);
        }
    }

    json_array_pop(buf);
}

static
void serialize_iter_result(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc) 
{
    json_next(buf);
    json_object_push(buf);

    /* Each result can be matched with different component ids. Add them to
     * the result so clients know with which component an entity was matched */
    if (!desc || desc->serialize_ids) {
        serialize_iter_result_ids(world, it, buf);
    }

    /* Include information on which entity the term is matched with */
    if (!desc || desc->serialize_ids) {
        serialize_iter_result_subjects(world, it, buf);
    }

    /* Write variable values for current result */
    if (!desc || desc->serialize_variables) {
        serialize_iter_result_variables(world, it, buf);
    }

    /* Write labels for variables */
    if (desc && desc->serialize_variable_labels) {
        serialize_iter_result_variable_labels(world, it, buf);
    }

    /* Include information on which terms are set, to support optional terms */
    if (!desc || desc->serialize_is_set) {
        serialize_iter_result_is_set(it, buf);
    }

    /* Write entity ids for current result (for queries with This terms) */
    if (!desc || desc->serialize_entities) {
        serialize_iter_result_entities(world, it, buf);
    }

    /* Write labels for entities */
    if (desc && desc->serialize_entity_labels) {
        serialize_iter_result_entity_labels(world, it, buf);
    }

    /* Serialize component values */
    if (!desc || desc->serialize_values) {
        serialize_iter_result_values(world, it, buf);
    }

    json_object_pop(buf);
}

int ecs_iter_to_json_buf(
    const ecs_world_t *world,
    ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_time_t duration = {0};
    if (desc && desc->measure_eval_duration) {
        ecs_time_measure(&duration);
    }

    json_object_push(buf);

    /* Serialize component ids of the terms (usually provided by query) */
    if (!desc || desc->serialize_term_ids) {
        serialize_iter_ids(world, it, buf);
    }

    /* Serialize type info if enabled */
    if (desc && desc->serialize_type_info) {
        serialize_type_info(world, it, buf);
    }

    /* Serialize variable names, if iterator has any */
    serialize_iter_variables(it, buf);

    /* Serialize results */
    json_member(buf, "results");
    json_array_push(buf);

    /* Use instancing for improved performance */
    it->is_instanced = true;

    ecs_iter_next_action_t next = it->next;
    while (next(it)) {
        serialize_iter_result(world, it, buf, desc);
    }

    json_array_pop(buf);

    if (desc && desc->measure_eval_duration) {
        double dt = ecs_time_measure(&duration);
        json_member(buf, "eval_duration");
        json_number(buf, dt);
    }

    json_object_pop(buf);

    return 0;
}

char* ecs_iter_to_json(
    const ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ecs_iter_to_json_buf(world, it, &buf, desc)) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

#endif
