/**
 * @file json/serialize.c
 * @brief Serialize (component) values to JSON strings.
 */

#include "json.h"

#ifdef FLECS_JSON

/* Cached id records during serialization */
typedef struct ecs_json_ser_idr_t {
    ecs_id_record_t *idr_doc_name;
    ecs_id_record_t *idr_doc_color;
} ecs_json_ser_idr_t;

static
int json_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *ser, 
    const void *base, 
    ecs_strbuf_t *str);

static
int json_ser_type_ops(
    const ecs_world_t *world,
    ecs_meta_type_op_t *ops,
    int32_t op_count,
    const void *base, 
    ecs_strbuf_t *str,
    int32_t in_array);

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
    ecs_enum_constant_t *constant = ecs_map_get_deref(&enum_type->constants,
        ecs_enum_constant_t, (ecs_map_key_t)value);
    if (!constant) {
        /* If the value is not found, it is not a valid enumeration constant */
        char *name = ecs_get_fullpath(world, op->type);
        ecs_err("enumeration value '%d' of type '%s' is not a valid constant", 
            value, name);
        ecs_os_free(name);
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
    if (!value) {
        ecs_strbuf_appendch(str, '0');
        return 0;
    }

    ecs_strbuf_list_push(str, "\"", "|");

    /* Multiple flags can be set at a given time. Iterate through all the flags
     * and append the ones that are set. */
    ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
    while (ecs_map_next(&it)) {
        ecs_bitmask_constant_t *constant = ecs_map_ptr(&it);
        ecs_map_key_t key = ecs_map_key(&it);
        if ((value & key) == key) {
            ecs_strbuf_list_appendstr(str, 
                ecs_get_name(world, constant->constant));
            value -= (uint32_t)key;
        }
    }

    if (value != 0) {
        /* All bits must have been matched by a constant */
        char *name = ecs_get_fullpath(world, op->type);
        ecs_err("bitmask value '%u' of type '%s' contains invalid/unknown bits", 
            value, name);
        ecs_os_free(name);
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
    ecs_strbuf_t *str,
    bool is_array)
{
    flecs_json_array_push(str);

    const void *ptr = base;

    int i;
    for (i = 0; i < elem_count; i ++) {
        ecs_strbuf_list_next(str);
        if (json_ser_type_ops(world, ops, op_count, ptr, str, is_array)) {
            return -1;
        }
        ptr = ECS_OFFSET(ptr, elem_size);
    }

    flecs_json_array_pop(str);

    return 0;
}

static
int json_ser_type_elements(
    const ecs_world_t *world,
    ecs_entity_t type, 
    const void *base, 
    int32_t elem_count, 
    ecs_strbuf_t *str,
    bool is_array)
{
    const EcsMetaTypeSerialized *ser = ecs_get(
        world, type, EcsMetaTypeSerialized);
    ecs_assert(ser != NULL, ECS_INTERNAL_ERROR, NULL);

    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_meta_type_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_type_op_t);
    int32_t op_count = ecs_vec_count(&ser->ops);

    return json_ser_elements(
        world, ops, op_count, base, elem_count, comp->size, str, is_array);
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
        world, a->type, ptr, a->count, str, true);
}

/* Serialize vector */
static
int json_ser_vector(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str) 
{
    const ecs_vec_t *value = base;
    const EcsVector *v = ecs_get(world, op->type, EcsVector);
    ecs_assert(v != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vec_count(value);
    void *array = ecs_vec_first(value);

    /* Serialize contiguous buffer of vector */
    return json_ser_type_elements(world, v->type, array, count, str, false);
}

typedef struct json_serializer_ctx_t {
    ecs_strbuf_t *str;
    bool is_collection;
    bool is_struct;
} json_serializer_ctx_t;

static
int json_ser_custom_value(
    const ecs_serializer_t *ser,
    ecs_entity_t type,
    const void *value)
{
    json_serializer_ctx_t *json_ser = ser->ctx;
    if (json_ser->is_collection) {
        ecs_strbuf_list_next(json_ser->str);
    }
    return ecs_ptr_to_json_buf(ser->world, type, value, json_ser->str);
}

static
int json_ser_custom_member(
    const ecs_serializer_t *ser,
    const char *name)
{
    json_serializer_ctx_t *json_ser = ser->ctx;
    if (!json_ser->is_struct) {
        ecs_err("serializer::member can only be called for structs");
        return -1;
    }
    flecs_json_member(json_ser->str, name);
    return 0;
}

static
int json_ser_custom_type(
    const ecs_world_t *world,
    ecs_meta_type_op_t *op, 
    const void *base, 
    ecs_strbuf_t *str)
{
    const EcsOpaque *ct = ecs_get(world, op->type, EcsOpaque);
    ecs_assert(ct != NULL, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ct->as_type != 0, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ct->serialize != NULL, ECS_INVALID_OPERATION, 
        ecs_get_name(world, op->type));

    const EcsMetaType *pt = ecs_get(world, ct->as_type, EcsMetaType);
    ecs_assert(pt != NULL, ECS_INVALID_OPERATION, NULL);

    ecs_type_kind_t kind = pt->kind;
    bool is_collection = false;
    bool is_struct = false;

    if (kind == EcsStructType) {
        flecs_json_object_push(str);
        is_struct = true;
    } else if (kind == EcsArrayType || kind == EcsVectorType) {
        flecs_json_array_push(str);
        is_collection = true;
    }

    json_serializer_ctx_t json_ser = {
        .str = str,
        .is_struct = is_struct,
        .is_collection = is_collection
    };

    ecs_serializer_t ser = {
        .world = world,
        .value = json_ser_custom_value,
        .member = json_ser_custom_member,
        .ctx = &json_ser
    };

    if (ct->serialize(&ser, base)) {
        return -1;
    }

    if (kind == EcsStructType) {
        flecs_json_object_pop(str);
    } else if (kind == EcsArrayType || kind == EcsVectorType) {
        flecs_json_array_pop(str);
    }

    return 0;
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
    case EcsOpOpaque:
        if (json_ser_custom_type(world, op, ECS_OFFSET(ptr, op->offset), str)) {
            goto error;
        }
        break;
    case EcsOpEntity: {
        ecs_entity_t e = *(ecs_entity_t*)ECS_OFFSET(ptr, op->offset);
        if (!e) {
            ecs_strbuf_appendch(str, '0');
        } else {
            flecs_json_path(str, world, e);
        }
        break;
    }

    default:
        if (ecs_primitive_to_expr_buf(world, 
            flecs_json_op_to_primitive_kind(op->kind), 
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
    ecs_strbuf_t *str,
    int32_t in_array)
{
    for (int i = 0; i < op_count; i ++) {
        ecs_meta_type_op_t *op = &ops[i];

        if (in_array <= 0) {
            if (op->name) {
                flecs_json_member(str, op->name);
            }

            int32_t elem_count = op->count;
            if (elem_count > 1) {
                /* Serialize inline array */
                if (json_ser_elements(world, op, op->op_count, base,
                    elem_count, op->size, str, true))
                {
                    return -1;
                }

                i += op->op_count - 1;
                continue;
            }
        }
        
        switch(op->kind) {
        case EcsOpPush:
            flecs_json_object_push(str);
            in_array --;
            break;
        case EcsOpPop:
            flecs_json_object_pop(str);
            in_array ++;
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
    const ecs_vec_t *v_ops,
    const void *base, 
    ecs_strbuf_t *str) 
{
    ecs_meta_type_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_type_op_t);
    int32_t count = ecs_vec_count(v_ops);
    return json_ser_type_ops(world, ops, count, base, str, 0);
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

        flecs_json_array_push(buf);

        do {
            ecs_strbuf_list_next(buf);
            if (json_ser_type(world, &ser->ops, ptr, buf)) {
                return -1;
            }

            ptr = ECS_OFFSET(ptr, size);
        } while (-- count);

        flecs_json_array_pop(buf);
    } else {
        if (json_ser_type(world, &ser->ops, ptr, buf)) {
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
bool flecs_json_skip_id(
    const ecs_world_t *world,
    ecs_id_t id,
    const ecs_entity_to_json_desc_t *desc,
    ecs_entity_t ent,
    ecs_entity_t inst,
    ecs_entity_t *pred_out,
    ecs_entity_t *obj_out,
    ecs_entity_t *role_out,
    bool *hidden_out)
{
    bool is_base = ent != inst;
    ecs_entity_t pred = 0, obj = 0, role = 0;
    bool hidden = false;

    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        pred = ecs_pair_first(world, id);
        obj = ecs_pair_second(world, id);
    } else {
        pred = id & ECS_COMPONENT_MASK;
        if (id & ECS_ID_FLAGS_MASK) {
            role = id & ECS_ID_FLAGS_MASK;
        }
    }

    if (!desc || !desc->serialize_meta_ids) {
        if (pred == EcsIsA || pred == EcsChildOf ||
            pred == ecs_id(EcsIdentifier)) 
        {
            return true;
        }
#ifdef FLECS_DOC
        if (pred == ecs_id(EcsDocDescription)) {
            return true;
        }
#endif
    }

    if (is_base) {
        if (ecs_has_id(world, pred, EcsDontInherit)) {
            return true;
        }
    }
    if (!desc || !desc->serialize_private) {
        if (ecs_has_id(world, pred, EcsPrivate)) {
            return true;
        }
    }
    if (is_base) {
        if (ecs_get_target_for_id(world, inst, EcsIsA, id) != ent) {
            hidden = true;
        }
    }
    if (hidden && (!desc || !desc->serialize_hidden)) {
        return true;
    }

    *pred_out = pred;
    *obj_out = obj;
    *role_out = role;
    if (hidden_out) *hidden_out = hidden;

    return false;
}

static
int flecs_json_append_type_labels(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf,
    const ecs_id_t *ids,
    int32_t count,
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    (void)world; (void)buf; (void)ids; (void)count; (void)ent; (void)inst; 
    (void)desc;
    
#ifdef FLECS_DOC
    if (!desc || !desc->serialize_id_labels) {
        return 0;
    }

    flecs_json_memberl(buf, "id_labels");
    flecs_json_array_push(buf);

    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t pred = 0, obj = 0, role = 0;
        if (flecs_json_skip_id(world, ids[i], desc, ent, inst, &pred, &obj, &role, 0)) {
            continue;
        }

        if (obj && (pred == EcsUnion)) {
            pred = obj;
            obj = ecs_get_target(world, ent, pred, 0);
            if (!ecs_is_alive(world, obj)) {
                /* Union relationships aren't automatically cleaned up, so they
                 * can contain invalid entity ids. Don't serialize value until
                 * relationship is valid again. */
                continue;
            }
        }

        if (desc && desc->serialize_id_labels) {
            flecs_json_next(buf);

            flecs_json_array_push(buf);
            flecs_json_next(buf);
            flecs_json_label(buf, world, pred);
            if (obj) {
                flecs_json_next(buf);
                flecs_json_label(buf, world, obj);
            }

            flecs_json_array_pop(buf);
        }
    }

    flecs_json_array_pop(buf);
#endif
    return 0;
}

static
int flecs_json_append_type_values(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf, 
    const ecs_id_t *ids,
    int32_t count,
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    if (!desc || !desc->serialize_values) {
        return 0;
    }

    flecs_json_memberl(buf, "values");
    flecs_json_array_push(buf);

    int32_t i;
    for (i = 0; i < count; i ++) {
        bool hidden;
        ecs_entity_t pred = 0, obj = 0, role = 0;
        ecs_id_t id = ids[i];
        if (flecs_json_skip_id(world, id, desc, ent, inst, &pred, &obj, &role, 
            &hidden)) 
        {
            continue;
        }

        if (!hidden) {
            bool serialized = false;
            ecs_entity_t typeid = ecs_get_typeid(world, id);
            if (typeid) {
                const EcsMetaTypeSerialized *ser = ecs_get(
                    world, typeid, EcsMetaTypeSerialized);
                if (ser) {
                    const void *ptr = ecs_get_id(world, ent, id);
                    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

                    flecs_json_next(buf);
                    if (json_ser_type(world, &ser->ops, ptr, buf) != 0) {
                        /* Entity contains invalid value */
                        return -1;
                    }
                    serialized = true;
                }
            }
            if (!serialized) {
                flecs_json_next(buf);
                flecs_json_number(buf, 0);
            }
        } else {
            if (!desc || desc->serialize_hidden) {
                flecs_json_next(buf);
                flecs_json_number(buf, 0);
            }
        }
    }

    flecs_json_array_pop(buf);
    
    return 0;
}

static
int flecs_json_append_type_info(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf, 
    const ecs_id_t *ids,
    int32_t count,
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    if (!desc || !desc->serialize_type_info) {
        return 0;
    }

    flecs_json_memberl(buf, "type_info");
    flecs_json_array_push(buf);

    int32_t i;
    for (i = 0; i < count; i ++) {
        bool hidden;
        ecs_entity_t pred = 0, obj = 0, role = 0;
        ecs_id_t id = ids[i];
        if (flecs_json_skip_id(world, id, desc, ent, inst, &pred, &obj, &role, 
            &hidden)) 
        {
            continue;
        }

        if (!hidden) {
            ecs_entity_t typeid = ecs_get_typeid(world, id);
            if (typeid) {
                flecs_json_next(buf);
                if (ecs_type_info_to_json_buf(world, typeid, buf) != 0) {
                    return -1;
                }
            } else {
                flecs_json_next(buf);
                flecs_json_number(buf, 0);
            }
        } else {
            if (!desc || desc->serialize_hidden) {
                flecs_json_next(buf);
                flecs_json_number(buf, 0);
            }
        }
    }

    flecs_json_array_pop(buf);
    
    return 0;
}

static
int flecs_json_append_type_hidden(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf, 
    const ecs_id_t *ids,
    int32_t count,
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    if (!desc || !desc->serialize_hidden) {
        return 0;
    }

    if (ent == inst) {
        return 0; /* if this is not a base, components are never hidden */
    }

    flecs_json_memberl(buf, "hidden");
    flecs_json_array_push(buf);

    int32_t i;
    for (i = 0; i < count; i ++) {
        bool hidden;
        ecs_entity_t pred = 0, obj = 0, role = 0;
        ecs_id_t id = ids[i];
        if (flecs_json_skip_id(world, id, desc, ent, inst, &pred, &obj, &role, 
            &hidden)) 
        {
            continue;
        }

        flecs_json_next(buf);
        flecs_json_bool(buf, hidden);
    }

    flecs_json_array_pop(buf);
    
    return 0;
}

static
int flecs_json_append_type(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf, 
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    const ecs_id_t *ids = NULL;
    int32_t i, count = 0;

    const ecs_type_t *type = ecs_get_type(world, ent);
    if (type) {
        ids = type->array;
        count = type->count;
    }

    flecs_json_memberl(buf, "ids");
    flecs_json_array_push(buf);

    for (i = 0; i < count; i ++) {
        ecs_entity_t pred = 0, obj = 0, role = 0;
        if (flecs_json_skip_id(world, ids[i], desc, ent, inst, &pred, &obj, &role, 0)) {
            continue;
        }

        if (obj && (pred == EcsUnion)) {
            pred = obj;
            obj = ecs_get_target(world, ent, pred, 0);
            if (!ecs_is_alive(world, obj)) {
                /* Union relationships aren't automatically cleaned up, so they
                 * can contain invalid entity ids. Don't serialize value until
                 * relationship is valid again. */
                continue;
            }
        }

        flecs_json_next(buf);
        flecs_json_array_push(buf);
        flecs_json_next(buf);
        flecs_json_path(buf, world, pred);
        if (obj || role) {
            flecs_json_next(buf);
            if (obj) {
                flecs_json_path(buf, world, obj);
            } else {
                flecs_json_number(buf, 0);
            }
            if (role) {
                flecs_json_next(buf);
                flecs_json_string(buf, ecs_id_flag_str(role));
            }
        }
        flecs_json_array_pop(buf);
    }

    flecs_json_array_pop(buf);

    if (flecs_json_append_type_labels(world, buf, ids, count, ent, inst, desc)) {
        return -1;
    }
    
    if (flecs_json_append_type_values(world, buf, ids, count, ent, inst, desc)) {
        return -1;
    }

    if (flecs_json_append_type_info(world, buf, ids, count, ent, inst, desc)) {
        return -1;
    }

    if (flecs_json_append_type_hidden(world, buf, ids, count, ent, inst, desc)) {
        return -1;
    }

    return 0;
}

static
int flecs_json_append_base(
    const ecs_world_t *world, 
    ecs_strbuf_t *buf, 
    ecs_entity_t ent, 
    ecs_entity_t inst,
    const ecs_entity_to_json_desc_t *desc) 
{
    const ecs_type_t *type = ecs_get_type(world, ent);
    ecs_id_t *ids = NULL;
    int32_t i, count = 0;
    if (type) {
        ids = type->array;
        count = type->count;
    }

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            if (flecs_json_append_base(world, buf, ecs_pair_second(world, id), inst, desc)) 
            {
                return -1;
            }
        }
    }

    ecs_strbuf_list_next(buf);
    flecs_json_object_push(buf);
    flecs_json_memberl(buf, "path");
    flecs_json_path(buf, world, ent);

    if (flecs_json_append_type(world, buf, ent, inst, desc)) {
        return -1;
    }

    flecs_json_object_pop(buf);

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

    flecs_json_object_push(buf);

    if (!desc || desc->serialize_path) {
        flecs_json_memberl(buf, "path");
        flecs_json_path(buf, world, entity);
    }

#ifdef FLECS_DOC
    if (desc && desc->serialize_label) {
        flecs_json_memberl(buf, "label");
        const char *doc_name = ecs_doc_get_name(world, entity);
        if (doc_name) {
            flecs_json_string_escape(buf, doc_name);
        } else {
            char num_buf[20];
            ecs_os_sprintf(num_buf, "%u", (uint32_t)entity);
            flecs_json_string(buf, num_buf);
        }
    }

    if (desc && desc->serialize_brief) {
        const char *doc_brief = ecs_doc_get_brief(world, entity);
        if (doc_brief) {
            flecs_json_memberl(buf, "brief");
            flecs_json_string_escape(buf, doc_brief);
        }
    }

    if (desc && desc->serialize_link) {
        const char *doc_link = ecs_doc_get_link(world, entity);
        if (doc_link) {
            flecs_json_memberl(buf, "link");
            flecs_json_string_escape(buf, doc_link);
        }
    }

    if (desc && desc->serialize_color) {
        const char *doc_color = ecs_doc_get_color(world, entity);
        if (doc_color) {
            flecs_json_memberl(buf, "color");
            flecs_json_string_escape(buf, doc_color);
        }
    }
#endif

    const ecs_type_t *type = ecs_get_type(world, entity);
    ecs_id_t *ids = NULL;
    int32_t i, count = 0;
    if (type) {
        ids = type->array;
        count = type->count;
    }

    if (!desc || desc->serialize_base) {
        if (ecs_has_pair(world, entity, EcsIsA, EcsWildcard)) {
            flecs_json_memberl(buf, "is_a");
            flecs_json_array_push(buf);

            for (i = 0; i < count; i ++) {
                ecs_id_t id = ids[i];
                if (ECS_HAS_RELATION(id, EcsIsA)) {
                    if (flecs_json_append_base(
                        world, buf, ecs_pair_second(world, id), entity, desc)) 
                    {
                        return -1;
                    }
                }
            }

            flecs_json_array_pop(buf);
        }
    }

    if (flecs_json_append_type(world, buf, entity, entity, desc)) {
        goto error;
    }

    if (desc && desc->serialize_alerts) {
#ifdef FLECS_ALERTS
        const EcsAlertsActive *alerts = ecs_get(world, entity, EcsAlertsActive);
        if (alerts) {
            flecs_json_memberl(buf, "alerts");
            flecs_json_array_push(buf);
            ecs_map_iter_t it = ecs_map_iter(&alerts->alerts);
            while (ecs_map_next(&it)) {
                flecs_json_next(buf);
                flecs_json_object_push(buf);
                ecs_entity_t a = ecs_map_key(&it);
                ecs_entity_t ai = ecs_map_value(&it);
                char *alert_name = ecs_get_fullpath(world, ai);
                flecs_json_memberl(buf, "alert");
                flecs_json_string(buf, alert_name);
                ecs_os_free(alert_name);

                ecs_entity_t severity_id = ecs_get_target(
                    world, a, ecs_id(EcsAlert), 0);
                const char *severity = ecs_get_name(world, severity_id);

                const EcsAlertInstance *alert = ecs_get(
                    world, ai, EcsAlertInstance);
                if (alert) {
                    flecs_json_memberl(buf, "message");
                    flecs_json_string(buf, alert->message);
                    flecs_json_memberl(buf, "severity");
                    flecs_json_string(buf, severity);
                }
                flecs_json_object_pop(buf);
            }
            flecs_json_array_pop(buf);
        }
#endif
    }

    flecs_json_object_pop(buf);

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
bool flecs_json_skip_variable(
    const char *name)
{
    if (!name || name[0] == '_' || !ecs_os_strcmp(name, "this")) {
        return true;
    } else {
        return false;
    }
}

static
void flecs_json_serialize_id(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_strbuf_t *buf) 
{
    flecs_json_id(buf, world, id);
}

static
void flecs_json_serialize_iter_ids(
    const ecs_world_t *world,
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf) 
{
    int32_t field_count = it->field_count;
    if (!field_count) {
        return;
    }

    flecs_json_memberl(buf, "ids");
    flecs_json_array_push(buf);

    for (int i = 0; i < field_count; i ++) {
        flecs_json_next(buf);
        flecs_json_serialize_id(world, it->terms[i].id, buf);
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_id_str(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_strbuf_t *buf)
{
    ecs_strbuf_appendch(buf, '"');
    if (ECS_IS_PAIR(id)) {
        ecs_entity_t first = ecs_pair_first(world, id);
        ecs_entity_t second = ecs_pair_first(world, id);
        ecs_strbuf_appendch(buf, '(');
        ecs_get_path_w_sep_buf(world, 0, first, ".", "", buf);
        ecs_strbuf_appendch(buf, ',');
        ecs_get_path_w_sep_buf(world, 0, second, ".", "", buf);
        ecs_strbuf_appendch(buf, ')');
    } else {
        ecs_get_path_w_sep_buf(
            world, 0, id & ECS_COMPONENT_MASK, ".", "", buf);
    }
    ecs_strbuf_appendch(buf, '"');
}

static
void flecs_json_serialize_type_info(
    const ecs_world_t *world,
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf) 
{
    int32_t field_count = it->field_count;
    if (!field_count) {
        return;
    }

    if (it->flags & EcsIterNoData) {
        return;
    }

    flecs_json_memberl(buf, "type_info");
    flecs_json_object_push(buf);

    for (int i = 0; i < field_count; i ++) {
        flecs_json_next(buf);
        ecs_entity_t typeid = 0;
        if (it->terms[i].inout != EcsInOutNone) {
            typeid = ecs_get_typeid(world, it->terms[i].id);
        }
        if (typeid) {
            flecs_json_serialize_id_str(world, typeid, buf);
            ecs_strbuf_appendch(buf, ':');
            ecs_type_info_to_json_buf(world, typeid, buf);
        } else {
            flecs_json_serialize_id_str(world, it->terms[i].id, buf);
            ecs_strbuf_appendlit(buf, ":0");
        }
    }

    flecs_json_object_pop(buf);
}

static
void flecs_json_serialize_iter_variables(ecs_iter_t *it, ecs_strbuf_t *buf) {
    char **variable_names = it->variable_names;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 0; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (flecs_json_skip_variable(var_name)) continue;

        if (!actual_count) {
            flecs_json_memberl(buf, "vars");
            flecs_json_array_push(buf);
            actual_count ++;
        }

        ecs_strbuf_list_next(buf);
        flecs_json_string(buf, var_name);
    }

    if (actual_count) {
        flecs_json_array_pop(buf);
    }
}

static
void flecs_json_serialize_iter_result_ids(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    flecs_json_memberl(buf, "ids");
    flecs_json_array_push(buf);

    for (int i = 0; i < it->field_count; i ++) {
        flecs_json_next(buf);
        flecs_json_serialize_id(world, ecs_field_id(it, i + 1), buf);
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_iter_result_table_type(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    if (!it->table) {
        return;
    }

    flecs_json_memberl(buf, "ids");
    flecs_json_array_push(buf);

    ecs_type_t *type = &it->table->type;
    for (int i = 0; i < type->count; i ++) {
        flecs_json_next(buf);
        flecs_json_serialize_id(world, type->array[i], buf);
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_iter_result_sources(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    flecs_json_memberl(buf, "sources");
    flecs_json_array_push(buf);

    for (int i = 0; i < it->field_count; i ++) {
        flecs_json_next(buf);
        ecs_entity_t subj = it->sources[i];
        if (subj) {            
            flecs_json_path(buf, world, subj);
        } else {
            ecs_strbuf_appendch(buf, '0');
        }
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_iter_result_is_set(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    if (!(it->flags & EcsIterHasCondSet)) {
        return;
    }

    flecs_json_memberl(buf, "is_set");
    flecs_json_array_push(buf);

    for (int i = 0; i < it->field_count; i ++) {
        ecs_strbuf_list_next(buf);
        if (ecs_field_is_set(it, i + 1)) {
            flecs_json_true(buf);
        } else {
            flecs_json_false(buf);
        }
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_iter_result_variables(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    char **variable_names = it->variable_names;
    ecs_var_t *variables = it->variables;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 0; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (flecs_json_skip_variable(var_name)) continue;

        if (!actual_count) {
            flecs_json_memberl(buf, "vars");
            flecs_json_array_push(buf);
            actual_count ++;
        }

        ecs_strbuf_list_next(buf);
        flecs_json_path(buf, world, variables[i].entity);
    }

    if (actual_count) {
        flecs_json_array_pop(buf);
    }
}

static
void flecs_json_serialize_iter_result_variable_labels(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    char **variable_names = it->variable_names;
    ecs_var_t *variables = it->variables;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 0; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (flecs_json_skip_variable(var_name)) continue;

        if (!actual_count) {
            flecs_json_memberl(buf, "var_labels");
            flecs_json_array_push(buf);
            actual_count ++;
        }

        ecs_strbuf_list_next(buf);
        flecs_json_label(buf, world, variables[i].entity);
    }

    if (actual_count) {
        flecs_json_array_pop(buf);
    }
}

static
void flecs_json_serialize_iter_result_variable_ids(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    char **variable_names = it->variable_names;
    ecs_var_t *variables = it->variables;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 0; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (flecs_json_skip_variable(var_name)) continue;

        if (!actual_count) {
            flecs_json_memberl(buf, "var_ids");
            flecs_json_array_push(buf);
            actual_count ++;
        }

        ecs_strbuf_list_next(buf);
        flecs_json_number(buf, (double)variables[i].entity);
    }

    if (actual_count) {
        flecs_json_array_pop(buf);
    }
}

static
bool flecs_json_serialize_iter_result_entity_names(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    ecs_assert(it->count != 0, ECS_INTERNAL_ERROR, NULL);

    EcsIdentifier *names = ecs_table_get_id(it->world, it->table, 
        ecs_pair(ecs_id(EcsIdentifier), EcsName), it->offset);
    if (!names) {
        return false;
    }

    int i;
    for (i = 0; i < it->count; i ++) {
        flecs_json_next(buf);
        flecs_json_string(buf, names[i].value);
    }

    return true;
}

static
void flecs_json_serialize_iter_result_entity_ids(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    if (!it->count) {
        return;
    }

    flecs_json_memberl(buf, "entity_ids");
    flecs_json_array_push(buf);

    ecs_entity_t *entities = it->entities;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        flecs_json_next(buf);
        flecs_json_number(buf, (double)(uint32_t)entities[i]);
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_iter_result_parent(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    ecs_table_t *table = it->table;
    if (!(table->flags & EcsTableHasChildOf)) {
        return;
    }

    const ecs_table_record_t *tr = flecs_id_record_get_table(
        world->idr_childof_wildcard, it->table);
    if (tr == NULL) {
        return;
    }

    ecs_id_t id = table->type.array[tr->column];
    ecs_entity_t parent = ecs_pair_second(world, id);
    char *path = ecs_get_fullpath(world, parent);
    flecs_json_memberl(buf, "parent");
    flecs_json_string(buf, path);
    ecs_os_free(path);
}

static
void flecs_json_serialize_iter_result_entities(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    if (!it->count) {
        return;
    }

    flecs_json_serialize_iter_result_parent(world, it, buf);

    flecs_json_memberl(buf, "entities");
    flecs_json_array_push(buf);

    if (!flecs_json_serialize_iter_result_entity_names(it, buf)) {
        ecs_entity_t *entities = it->entities;

        int i, count = it->count;
        for (i = 0; i < count; i ++) {
            flecs_json_next(buf);
            flecs_json_number(buf, (double)(uint32_t)entities[i]);
        }
    }

    flecs_json_array_pop(buf);
}

static
void flecs_json_serialize_iter_result_entity_labels(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_json_ser_idr_t *ser_idr)
{
    (void)buf;
    (void)ser_idr;
    if (!it->count) {
        return;
    }

    if (!ser_idr->idr_doc_name) {
        return;
    }

#ifdef FLECS_DOC
    const ecs_table_record_t *tr = flecs_id_record_get_table(
        ser_idr->idr_doc_name, it->table);
    if (tr == NULL) {
        return;
    }

    EcsDocDescription *labels = ecs_table_get_column(
        it->table, tr->column, it->offset);
    ecs_assert(labels != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_json_memberl(buf, "entity_labels");
    flecs_json_array_push(buf);

    int i;
    for (i = 0; i < it->count; i ++) {
        flecs_json_next(buf);
        flecs_json_string(buf, labels[i].value);
    }

    flecs_json_array_pop(buf);
#endif
}

static
void flecs_json_serialize_iter_result_colors(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_json_ser_idr_t *ser_idr) 
{
    (void)buf;
    (void)ser_idr;

    if (!it->count) {
        return;
    }

#ifdef FLECS_DOC
    if (!ser_idr->idr_doc_color) {
        return;
    }

    const ecs_table_record_t *tr = flecs_id_record_get_table(
        ser_idr->idr_doc_color, it->table);
    if (tr == NULL) {
        return;
    }

    EcsDocDescription *colors = ecs_table_get_column(
        it->table, tr->column, it->offset);
    ecs_assert(colors != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_json_memberl(buf, "colors");
    flecs_json_array_push(buf);

    int i;
    for (i = 0; i < it->count; i ++) {
        flecs_json_next(buf);
        flecs_json_string(buf, colors[i].value);
    }

    flecs_json_array_pop(buf);
#endif
}

static
int flecs_json_serialize_iter_result_values(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf) 
{
    if (!it->ptrs || (it->flags & EcsIterNoData)) {
        return 0;
    }

    flecs_json_memberl(buf, "values");
    flecs_json_array_push(buf);

    int32_t i, term_count = it->field_count;
    for (i = 0; i < term_count; i ++) {
        ecs_strbuf_list_next(buf);

        const void *ptr = NULL;
        if (it->ptrs) {
            ptr = it->ptrs[i];
        }

        if (!ptr) {
            /* No data in column. Append 0 if this is not an optional term */
            if (ecs_field_is_set(it, i + 1)) {
                ecs_strbuf_appendch(buf, '0');
                continue;
            }
        }

        if (ecs_field_is_writeonly(it, i + 1)) {
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        /* Get component id (can be different in case of pairs) */
        ecs_entity_t type = ecs_get_typeid(world, it->ids[i]);
        if (!type) {
            /* Odd, we have a ptr but no Component? Not the place of the
             * serializer to complain about that. */
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        const EcsComponent *comp = ecs_get(world, type, EcsComponent);
        if (!comp) {
            /* Also odd, typeid but not a component? */
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        const EcsMetaTypeSerialized *ser = ecs_get(
            world, type, EcsMetaTypeSerialized);
        if (!ser) {
            /* Not odd, component just has no reflection data */
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        /* If term is not set, append empty array. This indicates that the term
         * could have had data but doesn't */
        if (!ecs_field_is_set(it, i + 1)) {
            ecs_assert(ptr == NULL, ECS_INTERNAL_ERROR, NULL);
            flecs_json_array_push(buf);
            flecs_json_array_pop(buf);
            continue;
        }

        if (ecs_field_is_self(it, i + 1)) {
            int32_t count = it->count;
            if (array_to_json_buf_w_type_data(world, ptr, count, buf, comp, ser)) {
                return -1;
            }
        } else {
            if (array_to_json_buf_w_type_data(world, ptr, 0, buf, comp, ser)) {
                return -1;
            }
        }
    }

    flecs_json_array_pop(buf);

    return 0;
}

static
int flecs_json_serialize_iter_result_columns(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    ecs_table_t *table = it->table;
    if (!table || !table->storage_table) {
        return 0;
    }

    flecs_json_memberl(buf, "values");
    flecs_json_array_push(buf);

    ecs_type_t *type = &table->type;
    int32_t *storage_map = table->storage_map;
    ecs_assert(storage_map != NULL, ECS_INTERNAL_ERROR, NULL);

    for (int i = 0; i < type->count; i ++) {
        int32_t storage_column = -1;
        if (storage_map) {
            storage_column = storage_map[i];
        }

        ecs_strbuf_list_next(buf);

        if (storage_column == -1) {
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        ecs_entity_t typeid = table->type_info[storage_column]->component;
        if (!typeid) {
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        const EcsComponent *comp = ecs_get(world, typeid, EcsComponent);
        if (!comp) {
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        const EcsMetaTypeSerialized *ser = ecs_get(
            world, typeid, EcsMetaTypeSerialized);
        if (!ser) {
            ecs_strbuf_appendch(buf, '0');
            continue;
        }

        void *ptr = ecs_vec_first(&table->data.columns[storage_column]);
        if (array_to_json_buf_w_type_data(world, ptr, it->count, buf, comp, ser)) {
            return -1;
        }
    }

    flecs_json_array_pop(buf);

    return 0;
}

static
int flecs_json_serialize_iter_result(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    const ecs_json_ser_idr_t *ser_idr) 
{
    flecs_json_next(buf);
    flecs_json_object_push(buf);

    /* Each result can be matched with different component ids. Add them to
     * the result so clients know with which component an entity was matched */
    if (desc && desc->serialize_table) {
        flecs_json_serialize_iter_result_table_type(world, it, buf);
    } else {
        if (!desc || desc->serialize_ids) {
            flecs_json_serialize_iter_result_ids(world, it, buf);
        }
    }

    /* Include information on which entity the term is matched with */
    if (!desc || (desc->serialize_sources && !desc->serialize_table)) {
        flecs_json_serialize_iter_result_sources(world, it, buf);
    }

    /* Write variable values for current result */
    if (!desc || desc->serialize_variables) {
        flecs_json_serialize_iter_result_variables(world, it, buf);
    }

    /* Write labels for variables */
    if (desc && desc->serialize_variable_labels) {
        flecs_json_serialize_iter_result_variable_labels(world, it, buf);
    }

    /* Write ids for variables */
    if (desc && desc->serialize_variable_ids) {
        flecs_json_serialize_iter_result_variable_ids(it, buf);
    }

    /* Include information on which terms are set, to support optional terms */
    if (!desc || (desc->serialize_is_set && !desc->serialize_table)) {
        flecs_json_serialize_iter_result_is_set(it, buf);
    }

    /* Write entity ids for current result (for queries with This terms) */
    if (!desc || desc->serialize_entities) {
        flecs_json_serialize_iter_result_entities(world, it, buf);
    }

    /* Write ids for entities */
    if (desc && desc->serialize_entity_ids) {
        flecs_json_serialize_iter_result_entity_ids(it, buf);
    }

    /* Write labels for entities */
    if (desc && desc->serialize_entity_labels) {
        flecs_json_serialize_iter_result_entity_labels(it, buf, ser_idr);
    }

    /* Write colors for entities */
    if (desc && desc->serialize_colors) {
        flecs_json_serialize_iter_result_colors(it, buf, ser_idr);
    }

    /* Serialize component values */
    if (desc && desc->serialize_table) {
        if (flecs_json_serialize_iter_result_columns(world, it, buf)) {
            return -1;
        }
    } else {
        if (!desc || desc->serialize_values) {
            if (flecs_json_serialize_iter_result_values(world, it, buf)) {
                return -1;
            }
        }
    }

    /* Add "alerts": true member if table has entities with active alerts */
#ifdef FLECS_ALERTS
    if (it->table && (ecs_id(EcsAlertsActive) != 0)) {
        /* Only add field if alerts addon is imported */
        if (ecs_table_has_id(world, it->table, ecs_id(EcsAlertsActive))) {
            flecs_json_memberl(buf, "alerts");
            flecs_json_true(buf);
        }
    }
#endif

    flecs_json_object_pop(buf);

    return 0;
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

    flecs_json_object_push(buf);

    /* Serialize component ids of the terms (usually provided by query) */
    if (!desc || desc->serialize_term_ids) {
        flecs_json_serialize_iter_ids(world, it, buf);
    }

    /* Serialize type info if enabled */
    if (desc && desc->serialize_type_info) {
        flecs_json_serialize_type_info(world, it, buf);
    }

    /* Serialize variable names, if iterator has any */
    flecs_json_serialize_iter_variables(it, buf);

    /* Serialize results */
    flecs_json_memberl(buf, "results");
    flecs_json_array_push(buf);

    /* Use instancing for improved performance */
    ECS_BIT_SET(it->flags, EcsIterIsInstanced);

    /* If serializing entire table, don't bother letting the iterator populate
     * data fields as we'll be iterating all columns. */
    if (desc && desc->serialize_table) {
        ECS_BIT_SET(it->flags, EcsIterNoData);
    }

    /* Cache id record for flecs.doc ids */
    ecs_json_ser_idr_t ser_idr = {NULL, NULL};
#ifdef FLECS_DOC
    ser_idr.idr_doc_name = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsName));
    ser_idr.idr_doc_color = flecs_id_record_get(world, 
        ecs_pair_t(EcsDocDescription, EcsDocColor));
#endif

    ecs_iter_next_action_t next = it->next;
    while (next(it)) {
        if (flecs_json_serialize_iter_result(world, it, buf, desc, &ser_idr)) {
            ecs_strbuf_reset(buf);
            ecs_iter_fini(it);
            return -1;
        }
    }

    flecs_json_array_pop(buf);

    if (desc && desc->measure_eval_duration) {
        double dt = ecs_time_measure(&duration);
        flecs_json_memberl(buf, "eval_duration");
        flecs_json_number(buf, dt);
    }

    flecs_json_object_pop(buf);

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

int ecs_world_to_json_buf(
    ecs_world_t *world,
    ecs_strbuf_t *buf_out,
    const ecs_world_to_json_desc_t *desc)
{
    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_desc_t filter_desc = {0};
    filter_desc.storage = &f;

    if (desc && desc->serialize_builtin && desc->serialize_modules) {
        filter_desc.terms[0].id = EcsAny;
    } else {
        bool serialize_builtin = desc && desc->serialize_builtin;
        bool serialize_modules = desc && desc->serialize_modules;
        int32_t term_id = 0;

        if (!serialize_builtin) {
            filter_desc.terms[term_id].id = ecs_pair(EcsChildOf, EcsFlecs);
            filter_desc.terms[term_id].oper = EcsNot;
            filter_desc.terms[term_id].src.flags = EcsSelf | EcsParent;
            term_id ++;
        }
        if (!serialize_modules) {
            filter_desc.terms[term_id].id = EcsModule;
            filter_desc.terms[term_id].oper = EcsNot;
            filter_desc.terms[term_id].src.flags = EcsSelf | EcsParent;
        }
    }

    if (ecs_filter_init(world, &filter_desc) == NULL) {
        return -1;
    }

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_to_json_desc_t json_desc = { 
        .serialize_table = true,
        .serialize_entities = true
    };

    int ret = ecs_iter_to_json_buf(world, &it, buf_out, &json_desc);
    ecs_filter_fini(&f);
    return ret;
}

char* ecs_world_to_json(
    ecs_world_t *world,
    const ecs_world_to_json_desc_t *desc)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ecs_world_to_json_buf(world, &buf, desc)) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

#endif
