/**
 * @file addons/json/serialize_value.c
 * @brief Function definition for json serialization-visitor functions.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON
#include "../serialize/serialize.h"


// Primitives

static
void flecs_json_ser_bool(bool value, void* user_data);

static
void flecs_json_ser_char_expression(char value, void* user_data);

static
void flecs_json_ser_char(char value, void* user_data);

static
void flecs_json_ser_byte(const uint8_t value, void* user_data);

static
void flecs_json_ser_u8(const uint8_t value, void* user_data);

static
void flecs_json_ser_u16(const uint16_t value, void* user_data);

static
void flecs_json_ser_u32(const uint32_t value, void* user_data);

static
void flecs_json_ser_u64(const uint64_t value, void* user_data);

static
void flecs_json_ser_i8(const int8_t value, void* user_data);

static
void flecs_json_ser_i16(const int16_t value, void* user_data);

static
void flecs_json_ser_i32(const int32_t value, void* user_data);

static
void flecs_json_ser_i64(const int64_t value, void* user_data);

static
void flecs_json_ser_f32(const float value, void* user_data);

static
void flecs_json_ser_f32_expression(const ecs_f32_t value, void* user_data);

static
void flecs_json_ser_f64(const double value, void* user_data);

static
void flecs_json_ser_f64_expression(const ecs_f64_t value, void* user_data);

static
void flecs_json_ser_iptr(const intptr_t value, void* user_data);

static
void flecs_json_ser_uptr(const uintptr_t value, void* user_data);

// String

static
void flecs_json_ser_string(const char* value, void* user_data);

// Flecs

static
void flecs_json_ser_entity(const ecs_world_t* world, ecs_entity_t e, void* user_data);

static
void flecs_json_ser_id(const ecs_world_t* world, ecs_id_t id, void* user_data);


static
void flecs_json_ser_member(const char* name, void* user_data);


// Enum

static
void flecs_json_ser_enum(uint32_t value, const char* constant_name, void* user_data);

// Bitmask

static
int flecs_json_ser_enter_bitmask(uint32_t initial_value, void* user_data);

static
int flecs_json_ser_enter_bitmask_expression(uint32_t initial_value, void* user_data);

static
void flecs_json_ser_bitmask_value(uint32_t value, const char* constant_name, void* user_data);

static
void flecs_json_ser_exit_bitmask(uint32_t found, void* user_data);

static
void flecs_json_ser_exit_bitmask_expression(uint32_t found, void* user_data);


// Array

static
void flecs_json_ser_elements_enter(uint32_t count, void* user_data);

static
void flecs_json_ser_elements_next_value(uint32_t index, void* user_data);

static
void flecs_json_ser_elements_exit(void* user_data);


// Struct

static
void flecs_json_ser_struct_enter(void* user_data);

static
void flecs_json_ser_struct_exit(void* user_data);


// Iter

static
void flecs_json_iter_result_enter(void* user_data);


// End-Control

static
void* flecs_json_ser_exit(void* user_data);

static
void* flecs_json_ser_error(void* user_data);


/*
void* ecs_ser_array(
    const ecs_world_t *world,
    ecs_entity_t type, 
    const void* ptr,
    int32_t count)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    ecs_visitor_desc_t visitor_desc = {
        .visit_char = flecs_json_ser_char_expression,
        .visit_u8 = flecs_json_ser_u8,
        .visit_u16 = flecs_json_ser_u16,
        .visit_u32 = flecs_json_ser_u32,
        .visit_u64 = flecs_json_ser_u64,
        .visit_i8 = flecs_json_ser_i8,
        .visit_i16 = flecs_json_ser_i16,
        .visit_i32 = flecs_json_ser_i32,
        .visit_i64 = flecs_json_ser_i64,
        .visit_f32 = flecs_json_ser_f32_expression,
        .visit_f64 = flecs_json_ser_f64_expression,
        .visit_bool = flecs_json_ser_bool,
        .visit_byte = flecs_json_ser_byte,
        .visit_uptr = flecs_json_ser_uptr,
        .visit_iptr = flecs_json_ser_iptr,
        .visit_string = flecs_json_ser_string_expression,
        .visit_entity = flecs_json_ser_entity,
        .visit_id = flecs_json_ser_id,

        .visit_member = flecs_json_ser_member,


        .visit_enum = flecs_json_ser_enum,
        .visit_bitmask = {
            .enter = flecs_json_ser_enter_bitmask,
            .value = flecs_json_ser_bitmask_value,
            .exit = flecs_json_ser_exit_bitmask,
        },
        .visit_array = {
            .enter = flecs_json_ser_array_enter,
            .next_value = flecs_json_ser_array_next_value,
            .exit = flecs_json_ser_array_exit,
        },
        .visit_struct {
            .enter = flecs_json_ser_struct_enter,
            .exit = flecs_json_ser_struct_exit,
        },
        .user_data = &str
    };

    if (ecs_ser_array(world, type, ptr, count, &visitor_desc) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    if (visitor_desc->exit)
        return visitor_desc->exit(visitor_desc->user_data);
    return NULL;
}

*/


void* flecs_json_init_visitor_desc(void* visitor_desc_ptr, ecs_strbuf_t* str) {
    ecs_visitor_desc_t visitor_desc = {
        .visit_char = flecs_json_ser_char_expression,
        .visit_u8 = flecs_json_ser_u8,
        .visit_u16 = flecs_json_ser_u16,
        .visit_u32 = flecs_json_ser_u32,
        .visit_u64 = flecs_json_ser_u64,
        .visit_i8 = flecs_json_ser_i8,
        .visit_i16 = flecs_json_ser_i16,
        .visit_i32 = flecs_json_ser_i32,
        .visit_i64 = flecs_json_ser_i64,
        .visit_f32 = flecs_json_ser_f32_expression,
        .visit_f64 = flecs_json_ser_f64_expression,
        .visit_bool = flecs_json_ser_bool,
        .visit_byte = flecs_json_ser_byte,
        .visit_uptr = flecs_json_ser_uptr,
        .visit_iptr = flecs_json_ser_iptr,
        .visit_string = flecs_json_ser_string,
        .visit_entity = flecs_json_ser_entity,
        .visit_id = flecs_json_ser_id,

        .visit_member = flecs_json_ser_member,


        .visit_enum = flecs_json_ser_enum,
        .visit_bitmask = {
            .enter = flecs_json_ser_enter_bitmask,
            .value = flecs_json_ser_bitmask_value,
            .exit = flecs_json_ser_exit_bitmask,
        },
        .visit_array = {
            .enter = flecs_json_ser_elements_enter,
            .next_value = flecs_json_ser_elements_next_value,
            .exit = flecs_json_ser_elements_exit,
        },
        .visit_struct = {
            .enter = flecs_json_ser_struct_enter,
            .exit = flecs_json_ser_struct_exit,
        },

        .visit_iter_result = {
            .enter = flecs_json_iter_result_enter,
            .exit = flecs_json_ser_elements_exit,
        },

        .exit = flecs_json_ser_exit,
        .error = flecs_json_ser_error,
        .user_data = str
    };
    *(ecs_visitor_desc_t*)visitor_desc_ptr = visitor_desc;
    return visitor_desc_ptr;
}




// Primitives

static
void flecs_json_ser_bool(bool value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        ecs_strbuf_appendlit(str, "true");
    } else {
        ecs_strbuf_appendlit(str, "false");
    }
}

static
void flecs_json_ser_char_expression(char value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        char chbuf[3];
        flecs_chresc(chbuf, value, '"');
        ecs_strbuf_appendch(str, '"');
        ecs_strbuf_appendstr(str, chbuf);
        ecs_strbuf_appendch(str, '"');
    } else {
        ecs_strbuf_appendch(str, '0');
    }
}

static
void flecs_json_ser_char(char value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        char chbuf[3];
        flecs_chresc(chbuf, value, '"');
        ecs_strbuf_appendstr(str, chbuf);
    } else {
        ecs_strbuf_appendch(str, '0');
    }
}

static
void flecs_json_ser_byte(const uint8_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

static
void flecs_json_ser_u8(const uint8_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

static
void flecs_json_ser_u16(const uint16_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

static
void flecs_json_ser_u32(const uint32_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_uto(int64_t, value));
}

static
void flecs_json_ser_u64(const uint64_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    bool large_int = value >= 2147483648;
    if (large_int) {
        ecs_strbuf_appendch(str, '"');
    }
    ecs_strbuf_append(str, "%llu", value);
    if (large_int) {
        ecs_strbuf_appendch(str, '"');
    }
}

static
void flecs_json_ser_i8(const int8_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

static
void flecs_json_ser_i16(const int16_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

static
void flecs_json_ser_i32(const int32_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

static
void flecs_json_ser_i64(const int64_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    bool large_int = value >= 2147483648;
    if (large_int) {
        ecs_strbuf_appendch(str, '"');
    }
    ecs_strbuf_appendint(str, value);
    if (large_int) {
        ecs_strbuf_appendch(str, '"');
    }
}

static
void flecs_json_ser_f32(const float value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, (double)value, 0);
}

static
void flecs_json_ser_f32_expression(const ecs_f32_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, (ecs_f64_t)value, '"');
}

static
void flecs_json_ser_f64(const double value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, value, 0);
}

static
void flecs_json_ser_f64_expression(const ecs_f64_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendflt(str, value, '"');
}

static
void flecs_json_ser_iptr(const intptr_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendint(str, flecs_ito(int64_t, value));
}

static
void flecs_json_ser_uptr(const uintptr_t value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_append(str, "%u", value);
}

// String

static
void flecs_json_ser_string(const char* value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        ecs_size_t length = flecs_stresc(NULL, 0, '"', value);
        if (length == ecs_os_strlen(value)) {
            ecs_strbuf_appendch(str, '"');
            ecs_strbuf_appendstrn(str, value, length);
            ecs_strbuf_appendch(str, '"');
        } else {
            char *out = ecs_os_malloc(length + 3);
            flecs_stresc(out + 1, length, '"', value);
            out[0] = '"';
            out[length + 1] = '"';
            out[length + 2] = '\0';
            ecs_strbuf_appendstr(str, out);
            ecs_os_free(out);
        }
    } else {
        ecs_strbuf_appendlit(str, "null");
    }
}
/*
 * static
void flecs_json_ser_string(const char* value, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (value) {
        ecs_strbuf_appendstr(str, value);
    } else {
        ecs_strbuf_appendlit(str, "null");
    }
}
*/

// Flecs

static
void flecs_json_ser_entity(const ecs_world_t* world, ecs_entity_t e, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendch(str, '"');
    if (!e) {
        ecs_strbuf_appendlit(str, "#0");
    } else {
        ecs_get_path_w_sep_buf(world, 0, e, ".", "", str, true);
    }
    ecs_strbuf_appendch(str, '"');
}
/*
 * static
void flecs_json_ser_entity_expression(const ecs_world_t* world, ecs_entity_t e, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    if (!e) {
        ecs_strbuf_appendlit(str, "#0");
    } else {
        ecs_get_path_w_sep_buf(world, 0, e, ".", NULL, str, false);
    }
}
*/


static
void flecs_json_ser_id(const ecs_world_t* world, ecs_id_t id, void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendch(str, '"');
    if (!id) {
        ecs_strbuf_appendlit(str, "#0");
    } else {
        ecs_id_str_buf(world, id, str);
    }
    ecs_strbuf_appendch(str, '"');
}


static
void flecs_json_ser_member(
    const char *name,
    void* user_data)
{
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    flecs_json_membern(str, name, ecs_os_strlen(name));
}

// Enum

static
void flecs_json_ser_enum(
    uint32_t value,
    const char* constant_name,
    void* user_data)
{
    (void)value;
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;

    ecs_strbuf_appendch(str, '"');
    ecs_strbuf_appendstr(str, constant_name);
    ecs_strbuf_appendch(str, '"');
}


// Bitmask

static
int flecs_json_ser_enter_bitmask(uint32_t initial_value, void* user_data) {
    (void)initial_value;
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    if (!initial_value) {
        ecs_strbuf_appendch(str, '0');
        return false;
    }
    ecs_strbuf_list_push(str, "\"", "|");
    return true;
}

static
int flecs_json_ser_enter_bitmask_expression(uint32_t initial_value, void* user_data) {
    (void)initial_value;
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_list_push(str, "", "|");
    return true;
}

static
void flecs_json_ser_bitmask_value(uint32_t value, const char* constant_name, void* user_data) {
    (void)value;
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_list_appendstr(str, constant_name);
}

static
void flecs_json_ser_exit_bitmask(uint32_t found, void* user_data) {
    (void)found;
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_list_pop(str, "\"");
}

static
void flecs_json_ser_exit_bitmask_expression(uint32_t found, void* user_data) {
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    if (!found) {
        ecs_strbuf_appendch(str, '0');
    }
    ecs_strbuf_list_pop(str, "");
}


// Array

static
void flecs_json_ser_elements_enter(uint32_t count, void* user_data) {
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_list_push(str, "[", ", ");
}

static
void flecs_json_ser_elements_next_value(uint32_t index, void* user_data) {
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_list_next(str);
}

static
void flecs_json_ser_elements_exit(void* user_data) {
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_list_pop(str, "]");
}


// Struct

static
void flecs_json_ser_struct_enter(void* user_data) {
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    flecs_json_object_push(str);
}

static
void flecs_json_ser_struct_exit(void* user_data) {
    ecs_strbuf_t *str = (ecs_strbuf_t*)user_data;
    flecs_json_object_pop(str);
}


// Iter
static
void flecs_json_iter_result_enter(void* user_data) {
    ecs_strbuf_t *buf = (ecs_strbuf_t*)user_data;
    ecs_strbuf_appendlit(buf, "\"values\":");
    flecs_json_array_push(buf);
}


// End-Control

static
void* flecs_json_ser_exit(void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    return ecs_strbuf_get(str);
}

static
void* flecs_json_ser_error(void* user_data) {
    ecs_strbuf_t* str = (ecs_strbuf_t*)user_data;
    ecs_strbuf_reset(str);
    return NULL;
}

#endif
