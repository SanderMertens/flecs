#include "../../private_api.h"
#include <errno.h>

#ifdef FLECS_META

typedef struct flecs_meta_c_parser_t {
    ecs_world_t *world;
    const char *name;
    const char *code;
    const char *ptr;
    int32_t depth;
    bool is_private;
} flecs_meta_c_parser_t;

#define flecs_meta_c_error(p, ...)\
    ecs_parser_error((p)->name, (p)->code, (p)->ptr - (p)->code, __VA_ARGS__)

static bool flecs_meta_c_expect(
    flecs_meta_c_parser_t *p,
    char ch)
{
    p->ptr = flecs_parse_ws_eol(p->ptr);
    if (p->ptr[0] != ch) {
        flecs_meta_c_error(p, "expected '%c'", ch);
        return false;
    }
    p->ptr = flecs_parse_ws_eol(p->ptr + 1);
    return true;
}

static bool flecs_meta_c_identifier(
    flecs_meta_c_parser_t *p,
    char *name)
{
    p->ptr = flecs_parse_ws_eol(p->ptr);
    if (!isalpha((unsigned char)p->ptr[0]) && p->ptr[0] != '_') {
        flecs_meta_c_error(p, "expected identifier");
        return false;
    }
    int32_t length = 0;
    while (isalnum((unsigned char)p->ptr[0]) ||
        p->ptr[0] == '_' || p->ptr[0] == ':' || p->ptr[0] == '.')
    {
        if (length == 255) {
            flecs_meta_c_error(p, "identifier is too long");
            return false;
        }
        name[length ++] = *p->ptr ++;
    }
    name[length] = '\0';
    p->ptr = flecs_parse_ws_eol(p->ptr);
    return true;
}

static bool flecs_meta_c_number(
    flecs_meta_c_parser_t *p,
    int64_t *value,
    int base)
{
    p->ptr = flecs_parse_ws_eol(p->ptr);
    char *end;
    errno = 0;
    *value = strtoll(p->ptr, &end, base);
    if (end == p->ptr || errno == ERANGE) {
        flecs_meta_c_error(p, "invalid numeric value");
        return false;
    }
    p->ptr = flecs_parse_ws_eol(end);
    return true;
}

static ecs_entity_t flecs_meta_c_lookup(
    ecs_world_t *world,
    const char *name)
{
    static const struct {
        const char *name;
        const ecs_entity_t *type;
    } aliases[] = {
        {"char", &ecs_id(ecs_char_t)},
        {"bool", &ecs_id(ecs_bool_t)},
        {"_Bool", &ecs_id(ecs_bool_t)},
        {"int8_t", &ecs_id(ecs_i8_t)},
        {"int16_t", &ecs_id(ecs_i16_t)},
        {"int32_t", &ecs_id(ecs_i32_t)},
        {"int64_t", &ecs_id(ecs_i64_t)},
        {"uint8_t", &ecs_id(ecs_u8_t)},
        {"uint16_t", &ecs_id(ecs_u16_t)},
        {"uint32_t", &ecs_id(ecs_u32_t)},
        {"uint64_t", &ecs_id(ecs_u64_t)},
        {"float", &ecs_id(ecs_f32_t)},
        {"double", &ecs_id(ecs_f64_t)},
        {"ecs_entity_t", &ecs_id(ecs_entity_t)},
        {"ecs_id_t", &ecs_id(ecs_id_t)},
        {"flecs::byte", &ecs_id(ecs_byte_t)}
    };
    for (int32_t i = 0; i < (int32_t)(sizeof(aliases) / sizeof(aliases[0])); i ++) {
        if (!ecs_os_strcmp(name, aliases[i].name)) {
            return *aliases[i].type;
        }
    }
    return ecs_lookup_symbol(world, name, true, true);
}

static const char* flecs_meta_c_scope_end(
    flecs_meta_c_parser_t *p)
{
    char scopes[256];
    int32_t depth = 0;
    const char *ptr = p->ptr;
    do {
        char ch = *ptr ++;
        if (ch == '(' || ch == '<') {
            if (depth == 256) {
                break;
            }
            scopes[depth ++] = ch == '(' ? ')' : '>';
        } else if (ch == ')' || ch == '>') {
            if (!depth || ch != scopes[-- depth]) {
                break;
            }
        } else if (!ch) {
            break;
        }
        if (!depth) {
            return flecs_parse_ws_eol(ptr);
        }
    } while (true);
    flecs_meta_c_error(p, "invalid type parameters");
    return NULL;
}

static ecs_entity_t flecs_meta_c_type(
    flecs_meta_c_parser_t *p)
{
    char name[256];
    if (!flecs_meta_c_identifier(p, name)) {
        return 0;
    }
    if (!ecs_os_strcmp(name, "ECS_PRIVATE")) {
        p->is_private = p->depth == 0;
        return 0;
    }
    if (!ecs_os_strcmp(name, "const") && !flecs_meta_c_identifier(p, name)) {
        return 0;
    }
    ecs_world_t *world = p->world;
    ecs_entity_t type = 0;
    char close = p->ptr[0] == '(' ? ')' : p->ptr[0] == '<' ? '>' : 0;
    if (close) {
        ecs_type_kind_t kind = EcsOpaqueType;
        if (!ecs_os_strcmp(name, "ecs_array")) {
            kind = EcsArrayType;
        } else if (!ecs_os_strcmp(name, "ecs_vector") ||
            !ecs_os_strcmp(name, "ecs_vec") ||
            !ecs_os_strcmp(name, "flecs::vector"))
        {
            kind = EcsVectorType;
        } else if (!ecs_os_strcmp(name, "ecs_map")) {
            kind = EcsMapType;
        } else if (!ecs_os_strcmp(name, "flecs::bitmask")) {
            kind = EcsBitmaskType;
        }
        const char *end = flecs_meta_c_scope_end(p);
        if (!end) {
            return 0;
        }
        if (*end == '*' || kind == EcsOpaqueType) {
            p->ptr = end;
            if (*end == '*') {
                p->ptr = flecs_parse_ws_eol(end + 1);
                return ecs_id(ecs_uptr_t);
            }
            return flecs_meta_c_lookup(world, name);
        }
        if (++ p->depth >= 256) {
            flecs_meta_c_error(p, "maximum level of nesting reached");
            return 0;
        }
        p->ptr ++;
        ecs_entity_t first = flecs_meta_c_type(p), second = 0;
        if (!first) {
            return 0;
        }
        int64_t count = 0;
        if (p->ptr[0] == ',') {
            p->ptr = flecs_parse_ws_eol(p->ptr + 1);
            if (isdigit((unsigned char)p->ptr[0])) {
                if (!flecs_meta_c_number(p, &count, 0) ||
                    count <= 0 || count > INT32_MAX)
                {
                    flecs_meta_c_error(p, "invalid array size");
                    return 0;
                }
            } else {
                second = flecs_meta_c_type(p);
                if (!second) {
                    return 0;
                }
            }
        }
        if (!flecs_meta_c_expect(p, close)) {
            return 0;
        }
        p->depth --;
        if (kind == EcsArrayType && count && !second) {
            type = ecs_insert(world, ecs_value(EcsArray, {first, (int32_t)count}));
        } else if (kind == EcsVectorType && !second) {
            type = ecs_insert(world, ecs_value(EcsVector, {first}));
        } else if (kind == EcsMapType && second) {
            type = ecs_insert(world, ecs_value(EcsMap, {.key_type = first, .type = second}));
        } else if (kind == EcsBitmaskType && !second && !count &&
            ecs_has(world, first, EcsBitmask))
        {
            type = ecs_insert(world, ecs_value(EcsArray, {first, 1}));
        } else {
            flecs_meta_c_error(p, "invalid collection type '%s'", name);
            return 0;
        }
    } else if (p->ptr[0] == '*') {
        p->ptr = flecs_parse_ws_eol(p->ptr + 1);
        type = !ecs_os_strcmp(name, "char") ? ecs_id(ecs_string_t) : ecs_id(ecs_uptr_t);
    } else {
        type = flecs_meta_c_lookup(world, name);
    }
    if (!type) {
        flecs_meta_c_error(p, "unknown type '%s'", name);
    }
    return type;
}

static int flecs_meta_c_struct(
    flecs_meta_c_parser_t *p,
    ecs_entity_t component)
{
    ecs_struct_desc_t desc = {.entity = component};
    int32_t count = 0;
    int result = -1;
    if (!flecs_meta_c_expect(p, '{')) {
        return -1;
    }
    while (p->ptr[0] != '}') {
        ecs_entity_t type = flecs_meta_c_type(p);
        if (p->is_private) {
            break;
        }
        if (!type) {
            goto done;
        }
        char name[256];
        if (!flecs_meta_c_identifier(p, name)) {
            goto done;
        }
        int64_t elem_count = 0;
        if (p->ptr[0] == '[') {
            p->ptr ++;
            if (!flecs_meta_c_number(p, &elem_count, 10) ||
                elem_count <= 0 || elem_count > INT32_MAX ||
                !flecs_meta_c_expect(p, ']'))
            {
                flecs_meta_c_error(p, "invalid array size");
                goto done;
            }
        }
        if (!flecs_meta_c_expect(p, ';')) {
            goto done;
        }
        if (count == ECS_MEMBER_DESC_CACHE_SIZE - 1) {
            flecs_meta_c_error(p, "too many struct members");
            goto done;
        }
        desc.members[count ++] = (ecs_member_t){
            .name = ecs_os_strdup(name), .type = type, .count = (int32_t)elem_count
        };
    }
    if (!p->is_private && (!flecs_meta_c_expect(p, '}') || p->ptr[0])) {
        flecs_meta_c_error(p, "stray characters after struct definition");
        goto done;
    }
    result = ecs_struct_init(p->world, &desc) ? 0 : -1;
done:
    for (int32_t i = 0; i < count; i ++) {
        ecs_os_free(ECS_CONST_CAST(char*, desc.members[i].name));
    }
    return result;
}

static int flecs_meta_c_constants(
    flecs_meta_c_parser_t *p,
    ecs_entity_t component,
    bool bitmask)
{
    ecs_world_t *world = p->world;
    ecs_add_id(world, component, bitmask ? ecs_id(EcsBitmask) : ecs_id(EcsEnum));
    if (!flecs_meta_c_expect(p, '{')) {
        return -1;
    }
    const char *prefix = ecs_get_world_info(world)->name_prefix;
    int32_t prefix_len = prefix ? ecs_os_strlen(prefix) : 0;
    int32_t name_len = ecs_os_strlen(p->name);
    int64_t value = 0;
    ecs_entity_t old_scope = ecs_set_scope(world, component);
    int result = -1;
    while (p->ptr[0] != '}') {
        char name[256];
        if (!flecs_meta_c_identifier(p, name)) {
            goto done;
        }
        if (p->ptr[0] == '=') {
            p->ptr ++;
            if (!flecs_meta_c_number(p, &value, 0)) {
                goto done;
            }
        } else if (bitmask) {
            flecs_meta_c_error(p, "bitmask requires explicit value assignment");
            goto done;
        }
        if (p->ptr[0] != '}' && p->ptr[0] != ',') {
            flecs_meta_c_error(p, "missing , after enum constant");
            goto done;
        }
        const char *constant = name;
        if (prefix && !ecs_os_strncmp(constant, prefix, prefix_len)) {
            constant += prefix_len;
        }
        if (!ecs_os_strncmp(constant, p->name, name_len)) {
            constant += name_len;
        }
        ecs_entity_t e = ecs_entity(world, {.name = constant});
        if (bitmask) {
            ecs_set_pair_second(world, e, EcsConstant, ecs_u32_t, {(ecs_u32_t)value});
        } else {
            ecs_set_pair_second(world, e, EcsConstant, ecs_i32_t, {(ecs_i32_t)value});
        }
        value ++;
        if (p->ptr[0] == ',') {
            p->ptr = flecs_parse_ws_eol(p->ptr + 1);
        }
    }
    if (!flecs_meta_c_expect(p, '}') || p->ptr[0]) {
        flecs_meta_c_error(p, "stray characters after enum definition");
        goto done;
    }
    result = 0;
done:
    ecs_set_scope(world, old_scope);
    return result;
}

int ecs_meta_from_desc(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_kind_t kind,
    const char *desc)
{
    flecs_meta_c_parser_t p = {
        .world = world, .name = ecs_get_name(world, component),
        .code = desc, .ptr = desc
    };
    switch (kind) {
    case EcsStructType:
        return flecs_meta_c_struct(&p, component);
    case EcsEnumType:
    case EcsBitmaskType:
        return flecs_meta_c_constants(&p, component, kind == EcsBitmaskType);
    case EcsPrimitiveType:
    case EcsArrayType:
    case EcsVectorType:
    case EcsMapType:
    case EcsValueType:
    case EcsOpaqueType:
        return 0;
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid type kind");
    }
}

#endif
