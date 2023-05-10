/**
 * @file json/json.c
 * @brief JSON serializer utilities.
 */

#include "json.h"
#include <ctype.h>

#ifdef FLECS_JSON

static
const char* flecs_json_token_str(
    ecs_json_token_t token_kind)
{
    switch(token_kind) {
    case JsonObjectOpen: return "{";
    case JsonObjectClose: return "}";
    case JsonArrayOpen: return "[";
    case JsonArrayClose: return "]";
    case JsonColon: return ":";
    case JsonComma: return ",";
    case JsonNumber: return "number";
    case JsonString: return "string";
    case JsonTrue: return "true";
    case JsonFalse: return "false";
    case JsonNull: return "null";
    case JsonInvalid: return "invalid";
    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
    return "invalid";
}

const char* flecs_json_parse(
    const char *json,
    ecs_json_token_t *token_kind,
    char *token)
{
    json = ecs_parse_ws_eol(json);

    char ch = json[0];

    if (ch == '{') {
        token_kind[0] = JsonObjectOpen;
        return json + 1;
    } else if (ch == '}') {
        token_kind[0] = JsonObjectClose;
        return json + 1;
    } else if (ch == '[') {
        token_kind[0] = JsonArrayOpen;
        return json + 1;
    } else if (ch == ']') {
        token_kind[0] = JsonArrayClose;
        return json + 1;
    } else if (ch == ':') {
        token_kind[0] = JsonColon;
        return json + 1;
    } else if (ch == ',') {
        token_kind[0] = JsonComma;
        return json + 1;
    } else if (ch == '"') {
        const char *start = json;
        char *token_ptr = token;
        json ++;
        for (; (ch = json[0]); ) {
            if (ch == '"') {
                json ++;
                token_ptr[0] = '\0';
                break;
            }

            if (token_ptr - token >= ECS_MAX_TOKEN_SIZE) {
                /* Token doesn't fit in buffer, signal to app to try again with
                 * dynamic buffer. */
                token_kind[0] = JsonLargeString;
                return start;
            }

            json = ecs_chrparse(json, token_ptr ++);
        }

        if (!ch) {
            token_kind[0] = JsonInvalid;
            return NULL;
        } else {
            token_kind[0] = JsonString;
            return json;
        }
    } else if (isdigit(ch) || (ch == '-')) {
        token_kind[0] = JsonNumber;
        return ecs_parse_digit(json, token);
    } else if (isalpha(ch)) {
        if (!ecs_os_strncmp(json, "null", 4)) {
            token_kind[0] = JsonNull;
            json += 4;
        } else
        if (!ecs_os_strncmp(json, "true", 4)) {
            token_kind[0] = JsonTrue;
            json += 4;
        } else
        if (!ecs_os_strncmp(json, "false", 5)) {
            token_kind[0] = JsonFalse;
            json += 5;
        }

        if (isalpha(json[0]) || isdigit(json[0])) {
            token_kind[0] = JsonInvalid;
            return NULL;
        }

        return json;
    } else {
        token_kind[0] = JsonInvalid;
        return NULL;
    }
}

const char* flecs_json_parse_large_string(
    const char *json,
    ecs_strbuf_t *buf)
{
    if (json[0] != '"') {
        return NULL; /* can only parse strings */
    }

    char ch, ch_out;
    json ++;
    for (; (ch = json[0]); ) {
        if (ch == '"') {
            json ++;
            break;
        }

        json = ecs_chrparse(json, &ch_out);
        ecs_strbuf_appendch(buf, ch_out);
    }

    if (!ch) {
        return NULL;
    } else {
        return json;
    }
}

const char* flecs_json_expect(
    const char *json,
    ecs_json_token_t token_kind,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t kind = 0;
    json = flecs_json_parse(json, &kind, token);
    if (kind == JsonInvalid) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, "invalid json");
        return NULL;
    } else if (kind != token_kind) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, "expected %s",
            flecs_json_token_str(token_kind));
        return NULL;
    }
    return json;
}

const char* flecs_json_expect_member(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    json = flecs_json_expect(json, JsonString, token, desc);
    if (!json) {
        return NULL;
    }
    json = flecs_json_expect(json, JsonColon, token, desc);
    if (!json) {
        return NULL;
    }
    return json;
}

const char* flecs_json_expect_member_name(
    const char *json,
    char *token,
    const char *member_name,
    const ecs_from_json_desc_t *desc)
{
    json = flecs_json_expect_member(json, token, desc);
    if (!json) {
        return NULL;
    }
    if (ecs_os_strcmp(token, member_name)) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "expected member '%s'", member_name);
        return NULL;
    }
    return json;
}

const char* flecs_json_skip_object(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;

    while ((json = flecs_json_parse(json, &token_kind, token))) {
        if (token_kind == JsonObjectOpen) {
            json = flecs_json_skip_object(json, token, desc);
        } else if (token_kind == JsonArrayOpen) {
            json = flecs_json_skip_array(json, token, desc);
        } else if (token_kind == JsonObjectClose) {
            return json;
        } else if (token_kind == JsonArrayClose) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected }");
            return NULL;
        }
    }

    ecs_parser_error(desc->name, desc->expr, json - desc->expr, "expected }");
    return NULL;
}

const char* flecs_json_skip_array(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;

    while ((json = flecs_json_parse(json, &token_kind, token))) {
        if (token_kind == JsonObjectOpen) {
            json = flecs_json_skip_object(json, token, desc);
        } else if (token_kind == JsonArrayOpen) {
            json = flecs_json_skip_array(json, token, desc);
        } else if (token_kind == JsonObjectClose) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected ]");
            return NULL;
        } else if (token_kind == JsonArrayClose) {
            return json;
        }
    }

    ecs_parser_error(desc->name, desc->expr, json - desc->expr, "expected ]");
    return NULL;
}

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
    ecs_strbuf_appendch(buf, '[');

    if (ECS_IS_PAIR(id)) {
        ecs_entity_t first = ecs_pair_first(world, id);
        ecs_entity_t second = ecs_pair_second(world, id);
        ecs_strbuf_appendch(buf, '"');
        ecs_get_path_w_sep_buf(world, 0, first, ".", "", buf);
        ecs_strbuf_appendch(buf, '"');
        ecs_strbuf_appendch(buf, ',');
        ecs_strbuf_appendch(buf, '"');
        ecs_get_path_w_sep_buf(world, 0, second, ".", "", buf);
        ecs_strbuf_appendch(buf, '"');
    } else {
        ecs_strbuf_appendch(buf, '"');
        ecs_get_path_w_sep_buf(world, 0, id & ECS_COMPONENT_MASK, ".", "", buf);
        ecs_strbuf_appendch(buf, '"');
    }

    ecs_strbuf_appendch(buf, ']');
}

ecs_primitive_kind_t flecs_json_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind) 
{
    return kind - EcsOpPrimitive;
}

#endif
