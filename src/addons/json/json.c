#include "json.h"

#ifdef FLECS_JSON

void json_next(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_next(buf);
}

void json_literal(
    ecs_strbuf_t *buf,
    const char *value)
{
    ecs_strbuf_appendstr(buf, value);
}

void json_number(
    ecs_strbuf_t *buf,
    double value)
{
    ecs_strbuf_appendflt(buf, value, '"');
}

void json_true(
    ecs_strbuf_t *buf)
{
    json_literal(buf, "true");
}

void json_false(
    ecs_strbuf_t *buf)
{
    json_literal(buf, "false");
}

void json_array_push(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_push(buf, "[", ", ");
}

void json_array_pop(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_pop(buf, "]");
}

void json_object_push(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_push(buf, "{", ", ");
}

void json_object_pop(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_pop(buf, "}");
}

void json_string(
    ecs_strbuf_t *buf,
    const char *value)
{
    ecs_strbuf_appendstr(buf, "\"");
    ecs_strbuf_appendstr(buf, value);
    ecs_strbuf_appendstr(buf, "\"");
}

void json_member(
    ecs_strbuf_t *buf,
    const char *name)
{
    ecs_strbuf_list_appendstr(buf, "\"");
    ecs_strbuf_appendstr(buf, name);
    ecs_strbuf_appendstr(buf, "\":");
}

void json_path(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_strbuf_appendch(buf, '"');
    ecs_get_fullpath_buf(world, e, buf);
    ecs_strbuf_appendch(buf, '"');
}

void json_label(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e)
{
    const char *lbl = NULL;
#ifdef FLECS_DOC
    lbl = ecs_doc_get_name(world, e);
#else
    lbl = ecs_get_name(world, e);
#endif

    if (lbl) {
        ecs_strbuf_appendch(buf, '"');
        ecs_strbuf_appendstr(buf, lbl);
        ecs_strbuf_appendch(buf, '"');
    } else {
        ecs_strbuf_appendstr(buf, "0");
    }
}

void json_id(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_strbuf_appendch(buf, '"');
    ecs_id_str_buf(world, id, buf);
    ecs_strbuf_appendch(buf, '"');
}

ecs_primitive_kind_t json_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind) 
{
    return kind - EcsOpPrimitive;
}

#endif
