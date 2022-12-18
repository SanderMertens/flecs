/**
 * @file json/json.c
 * @brief JSON serializer utilities.
 */

#include "json.h"

#ifdef FLECS_JSON

void flecs_json_next(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_next(buf);
}

void flecs_json_number(
    ecs_strbuf_t *buf,
    double value)
{
    ecs_strbuf_appendflt(buf, value, '"');
}

void flecs_json_true(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_appendlit(buf, "true");
}

void flecs_json_false(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_appendlit(buf, "false");
}

void flecs_json_bool(
    ecs_strbuf_t *buf,
    bool value)
{
    if (value) {
        flecs_json_true(buf);
    } else {
        flecs_json_false(buf);
    }
}

void flecs_json_array_push(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_push(buf, "[", ", ");
}

void flecs_json_array_pop(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_pop(buf, "]");
}

void flecs_json_object_push(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_push(buf, "{", ", ");
}

void flecs_json_object_pop(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_list_pop(buf, "}");
}

void flecs_json_string(
    ecs_strbuf_t *buf,
    const char *value)
{
    ecs_strbuf_appendch(buf, '"');
    ecs_strbuf_appendstr(buf, value);
    ecs_strbuf_appendch(buf, '"');
}

void flecs_json_string_escape(
    ecs_strbuf_t *buf,
    const char *value)
{
    ecs_size_t length = ecs_stresc(NULL, 0, '"', value);
    if (length == ecs_os_strlen(value)) {
        ecs_strbuf_appendch(buf, '"');
        ecs_strbuf_appendstrn(buf, value, length);
        ecs_strbuf_appendch(buf, '"');
    } else {
        char *out = ecs_os_malloc(length + 3);
        ecs_stresc(out + 1, length, '"', value);
        out[0] = '"';
        out[length + 1] = '"';
        out[length + 2] = '\0';
        ecs_strbuf_appendstr_zerocpy(buf, out);
    }
}

void flecs_json_member(
    ecs_strbuf_t *buf,
    const char *name)
{
    flecs_json_membern(buf, name, ecs_os_strlen(name));
}

void flecs_json_membern(
    ecs_strbuf_t *buf,
    const char *name,
    int32_t name_len)
{
    ecs_strbuf_list_appendch(buf, '"');
    ecs_strbuf_appendstrn(buf, name, name_len);
    ecs_strbuf_appendlit(buf, "\":");
}

void flecs_json_path(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_strbuf_appendch(buf, '"');
    ecs_get_path_w_sep_buf(world, 0, e, ".", "", buf);
    ecs_strbuf_appendch(buf, '"');
}

void flecs_json_label(
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
        ecs_strbuf_appendch(buf, '0');
    }
}

void flecs_json_color(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e)
{
    (void)world;
    (void)e;

    const char *color = NULL;
#ifdef FLECS_DOC
    color = ecs_doc_get_color(world, e);
#endif

    if (color) {
        ecs_strbuf_appendch(buf, '"');
        ecs_strbuf_appendstr(buf, color);
        ecs_strbuf_appendch(buf, '"');
    } else {
        ecs_strbuf_appendch(buf, '0');
    }
}

void flecs_json_id(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_strbuf_appendch(buf, '"');
    ecs_id_str_buf(world, id, buf);
    ecs_strbuf_appendch(buf, '"');
}

ecs_primitive_kind_t flecs_json_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind) 
{
    return kind - EcsOpPrimitive;
}

#endif
