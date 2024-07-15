/**
 * @file addons/json/json.c
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
    case JsonLargeInt: return "large integer";
    case JsonLargeString:
    case JsonString: return "string";
    case JsonBoolean: return "bool";
    case JsonTrue: return "true";
    case JsonFalse: return "false";
    case JsonNull: return "null";
    case JsonInvalid: return "invalid";
    default:
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }
error:
    return "<<invalid token kind>>";
}

const char* flecs_json_parse(
    const char *json,
    ecs_json_token_t *token_kind,
    char *token)
{
    ecs_assert(json != NULL, ECS_INTERNAL_ERROR, NULL);
    json = flecs_parse_ws_eol(json);

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
            if (token_ptr - token >= ECS_MAX_TOKEN_SIZE) {
                /* Token doesn't fit in buffer, signal to app to try again with
                 * dynamic buffer. */
                token_kind[0] = JsonLargeString;
                return start;
            }

            if (ch == '"') {
                json ++;
                token_ptr[0] = '\0';
                break;
            }

            json = flecs_chrparse(json, token_ptr ++);
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
        const char *result = flecs_parse_digit(json, token);
        
        /* Cheap initial check if parsed token could represent large int */
        if (result - json > 15) {
            /* Less cheap secondary check to see if number is integer */
            if (!strchr(token, '.')) {
                token_kind[0] = JsonLargeInt;
            }
        }

        return result;
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

        json = flecs_chrparse(json, &ch_out);
        ecs_strbuf_appendch(buf, ch_out);
    }

    if (!ch) {
        return NULL;
    } else {
        return json;
    }
}

const char* flecs_json_parse_next_member(
    const char *json,
    char *token,
    ecs_json_token_t *token_kind,
    const ecs_from_json_desc_t *desc)
{
    json = flecs_json_parse(json, token_kind, token);
    if (*token_kind == JsonObjectClose) {
        return json;
    }

    if (*token_kind != JsonComma) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "expecteded } or ,");
        return NULL;
    }

    json = flecs_json_parse(json, token_kind, token);
    if (*token_kind != JsonString) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "expecteded member name");
        return NULL;
    }

    char temp_token[ECS_MAX_TOKEN_SIZE];
    ecs_json_token_t temp_token_kind;

    json = flecs_json_parse(json, &temp_token_kind, temp_token);
    if (temp_token_kind != JsonColon) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "expecteded :");
        return NULL;
    }

    return json;
}

const char* flecs_json_expect(
    const char *json,
    ecs_json_token_t token_kind,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    /* Strings must be handled by flecs_json_expect_string for LargeString */
    ecs_assert(token_kind != JsonString, ECS_INTERNAL_ERROR, NULL);

    ecs_json_token_t kind = 0;
    const char *lah = flecs_json_parse(json, &kind, token);

    if (kind == JsonInvalid) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "invalid json");
        return NULL;
    } else if (kind != token_kind) {
        if (token_kind == JsonBoolean && 
            (kind == JsonTrue || kind == JsonFalse)) 
        {
            /* ok */
        } else {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected %s, got %s",
                flecs_json_token_str(token_kind), flecs_json_token_str(kind));
            return NULL;
        }
    }

    return lah;
}

const char* flecs_json_expect_string(
    const char *json,
    char *token,
    char **out,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    json = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonInvalid) {
        ecs_parser_error(
            desc->name, desc->expr, json - desc->expr, "invalid json");
        return NULL;
    } else if (token_kind != JsonString && token_kind != JsonLargeString) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "expected string");
        return NULL;
    }

    if (token_kind == JsonLargeString) {
        ecs_strbuf_t large_token = ECS_STRBUF_INIT;
        json = flecs_json_parse_large_string(json, &large_token);
        if (!json) {
            return NULL;
        }

        if (out) {
            *out = ecs_strbuf_get(&large_token);
        } else {
            ecs_strbuf_reset(&large_token);
        }
    } else if (out) {
        *out = token;
    }

    return json;
}

const char* flecs_json_expect_member(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    char *out = NULL;
    json = flecs_json_expect_string(json, token, &out, desc);
    if (!json) {
        return NULL;
    }

    if (out != token) {
        ecs_os_free(out);
    }

    json = flecs_json_expect(json, JsonColon, token, desc);
    if (!json) {
        return NULL;
    }
    return json;
}

const char* flecs_json_expect_next_member(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    json = flecs_json_expect(json, JsonComma, token, desc);
    if (!json) {
        return NULL;
    }

    return flecs_json_expect_member(json, token, desc);
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

static
const char* flecs_json_skip_string(
    const char *json)
{
    if (json[0] != '"') {
        return NULL; /* can only skip strings */
    }

    char ch, ch_out;
    json ++;
    for (; (ch = json[0]); ) {
        if (ch == '"') {
            json ++;
            break;
        }

        json = flecs_chrparse(json, &ch_out);
    }

    if (!ch) {
        return NULL;
    } else {
        return json;
    }
}

const char* flecs_json_skip_object(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    ecs_assert(json != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_json_token_t token_kind = 0;

    while ((json = flecs_json_parse(json, &token_kind, token))) {
        if (token_kind == JsonObjectOpen) {
            json = flecs_json_skip_object(json, token, desc);
        } else if (token_kind == JsonArrayOpen) {
            json = flecs_json_skip_array(json, token, desc);
        } else if (token_kind == JsonLargeString) {
            json = flecs_json_skip_string(json);
        } else if (token_kind == JsonObjectClose) {
            return json;
        } else if (token_kind == JsonArrayClose) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected }, got ]");
            return NULL;
        }

        ecs_assert(json != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_parser_error(desc->name, desc->expr, 0, 
        "expected }, got end of string");
    return NULL;
}

const char* flecs_json_skip_array(
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    ecs_assert(json != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_json_token_t token_kind = 0;

    while ((json = flecs_json_parse(json, &token_kind, token))) {
        if (token_kind == JsonObjectOpen) {
            json = flecs_json_skip_object(json, token, desc);
        } else if (token_kind == JsonArrayOpen) {
            json = flecs_json_skip_array(json, token, desc);
        } else if (token_kind == JsonLargeString) {
            json = flecs_json_skip_string(json);
        } else if (token_kind == JsonObjectClose) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected ]");
            return NULL;
        } else if (token_kind == JsonArrayClose) {
            return json;
        }

        ecs_assert(json != NULL, ECS_INTERNAL_ERROR, NULL);
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

void flecs_json_u32(
    ecs_strbuf_t *buf,
    uint32_t value)
{
    ecs_strbuf_appendint(buf, flecs_uto(int64_t, value));
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

void flecs_json_null(
    ecs_strbuf_t *buf)
{
    ecs_strbuf_appendlit(buf, "null");
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
    ecs_size_t length = flecs_stresc(NULL, 0, '"', value);
    if (length == ecs_os_strlen(value)) {
        ecs_strbuf_appendch(buf, '"');
        ecs_strbuf_appendstrn(buf, value, length);
        ecs_strbuf_appendch(buf, '"');
    } else {
        char *out = ecs_os_malloc(length + 3);
        flecs_stresc(out + 1, length, '"', value);
        out[0] = '"';
        out[length + 1] = '"';
        out[length + 2] = '\0';
        ecs_strbuf_appendstr(buf, out);
        ecs_os_free(out);
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

static
const char* flecs_json_entity_label(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    const char *lbl = NULL;
    if (!e) {
        return "#0";
    }
#ifdef FLECS_DOC
    lbl = ecs_doc_get_name(world, e);
#else
    lbl = ecs_get_name(world, e);
#endif
    return lbl;
}

void flecs_json_label(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e)
{
    const char *lbl = flecs_json_entity_label(world, e);
    if (lbl) {
        flecs_json_string_escape(buf, lbl);
    } else {
        ecs_strbuf_appendch(buf, '#');
        ecs_strbuf_appendint(buf, (uint32_t)e);
    }
}

void flecs_json_path_or_label(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_entity_t e,
    bool path)
{
    if (!path) {
        flecs_json_label(buf, world, e);
    } else {
        flecs_json_path(buf, world, e);
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

static
void flecs_json_id_member_fullpath(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        ecs_strbuf_appendch(buf, '(');
        ecs_entity_t first = ecs_pair_first(world, id);
        ecs_entity_t second = ecs_pair_second(world, id);
        ecs_get_path_w_sep_buf(world, 0, first, ".", "", buf);
        ecs_strbuf_appendch(buf, ',');
        ecs_get_path_w_sep_buf(world, 0, second, ".", "", buf);
        ecs_strbuf_appendch(buf, ')');
    } else {
        ecs_get_path_w_sep_buf(world, 0, id & ECS_COMPONENT_MASK, ".", "", buf);
    }
}

void flecs_json_id_member(
    ecs_strbuf_t *buf,
    const ecs_world_t *world,
    ecs_id_t id,
    bool fullpath)
{
    if (fullpath) {
        flecs_json_id_member_fullpath(buf, world, id);
        return;
    }

    if (ECS_IS_PAIR(id)) {
        ecs_strbuf_appendch(buf, '(');
        ecs_entity_t first = ecs_pair_first(world, id);
        ecs_entity_t second = ecs_pair_second(world, id);
        {
            const char *lbl = flecs_json_entity_label(world, first);
            if (lbl) {
                ecs_strbuf_appendstr(buf, lbl);
            }
        }
        ecs_strbuf_appendch(buf, ',');
        {
            const char *lbl = flecs_json_entity_label(world, second);
            if (lbl) {
                ecs_strbuf_appendstr(buf, lbl);
            }
        }
        ecs_strbuf_appendch(buf, ')');
    } else {
        const char *lbl = flecs_json_entity_label(world, id & ECS_COMPONENT_MASK);
        if (lbl) {
            ecs_strbuf_appendstr(buf, lbl);
        }
    }
}

ecs_primitive_kind_t flecs_json_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind) 
{
    return kind - EcsOpPrimitive;
}

#endif
