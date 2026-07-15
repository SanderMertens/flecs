/**
 * @file addons/script/expr/format.c
 * @brief Script expression string formatting.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static const char* flecs_expr_format_skip_space(
    const char *pos)
{
    while (isspace(pos[0])) {
        pos ++;
    }
    return pos;
}

static bool flecs_expr_format_is_alignment(
    char ch)
{
    return ch == '<' || ch == '^' || ch == '>';
}

static bool flecs_expr_format_identifier_exists(
    ecs_parser_t *parser,
    const ecs_expr_eval_desc_t *desc,
    const char *start,
    char *end)
{
    bool is_variable = start[0] == '$';
    if (is_variable) {
        start ++;
    }

    char ch = end[0];
    end[0] = '\0';

    bool result = flecs_script_find_var(desc->vars, start, NULL) != NULL;
    if (!result && !is_variable && desc->lookup_action) {
        ecs_script_t *script = &parser->script->pub;
        ecs_entity_t entity = desc->lookup_action(
            script->world, start, desc->lookup_ctx);
        result = entity && ecs_get(
            script->world, entity, EcsScriptConstVar) != NULL;
    }

    end[0] = ch;
    return result;
}

static void flecs_expr_format_parse_error(
    ecs_parser_t *parser,
    const char *message)
{
    int32_t column = 0;
    if (parser->fixed_pos && parser->code &&
        parser->fixed_pos >= parser->code)
    {
        column = flecs_ito(int32_t, parser->fixed_pos - parser->code);
    }
    ecs_parser_error(parser->name, parser->code,
        column, "%s", message);
}

void flecs_expr_format_fini(
    ecs_script_t *script,
    ecs_expr_format_t *format)
{
    flecs_expr_visit_free(script, format->width);
    flecs_expr_visit_free(script, format->precision);
    ecs_os_zeromem(format);
}

const char* flecs_expr_format_parse(
    ecs_parser_t *parser,
    const char *pos,
    ecs_expr_format_t *format,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_os_zeromem(format);
    format->pos = pos;
    format->fill = ' ';
    format->alignment = '>';
    format->is_present = true;

    pos = flecs_expr_format_skip_space(pos);

    if (pos[0] && flecs_expr_format_is_alignment(pos[1])) {
        format->fill = pos[0];
        format->alignment = pos[1];
        pos += 2;
    } else if (flecs_expr_format_is_alignment(pos[0])) {
        format->alignment = pos[0];
        pos ++;
    }

    pos = flecs_expr_format_skip_space(pos);
    if (pos[0] == '+') {
        format->sign = true;
        pos ++;
    }

    pos = flecs_expr_format_skip_space(pos);
    if (pos[0] == '0') {
        format->leading_zero = true;
        pos ++;
    }

    pos = flecs_expr_format_skip_space(pos);
    bool notation_is_width = false;
    if ((pos[0] == 'e' || pos[0] == 'E') &&
        flecs_expr_format_skip_space(pos + 1)[0] == '}')
    {
        notation_is_width = flecs_expr_format_identifier_exists(
            parser, desc, pos, ECS_CONST_CAST(char*, pos + 1));
    }

    if (pos[0] && pos[0] != '.' &&
        ((pos[0] != 'e' && pos[0] != 'E') || notation_is_width) &&
        pos[0] != '}')
    {
        /* Keep the precision separator out of simple width tokens. More
         * involved width expressions must be parenthesized. */
        char *separator = NULL;
        if (isdigit(pos[0])) {
            separator = ECS_CONST_CAST(char*, pos);
            while (isdigit(separator[0])) {
                separator ++;
            }
        } else if (pos[0] == '$' || isalpha(pos[0]) || pos[0] == '_') {
            separator = ECS_CONST_CAST(char*, pos);
            if (separator[0] == '$') {
                separator ++;
            }
            while (isalnum(separator[0]) || separator[0] == '_') {
                separator ++;
            }
        }

        if (separator && separator[0] != '.') {
            separator = NULL;
        }

        if (separator) {
            separator[0] = '\0';
        }
        const char *expr_end = flecs_script_parse_expr(
            parser, pos, 0, &format->width);
        if (separator) {
            separator[0] = '.';
        }
        if (!expr_end) {
            return NULL;
        }
        pos = separator ? separator : expr_end;
    }

    pos = flecs_expr_format_skip_space(pos);
    if (pos[0] == '.') {
        pos = flecs_expr_format_skip_space(pos + 1);
        if (!pos[0] || pos[0] == '}') {
            flecs_expr_format_parse_error(
                parser, "expected precision expression");
            return NULL;
        }

        /* Split a notation suffix from a variable token. */
        char *notation = NULL;
        char notation_ch = '\0';
        if (pos[0] == '$' || isalpha(pos[0]) || pos[0] == '_') {
            char *end = ECS_CONST_CAST(char*, pos);
            if (end[0] == '$') {
                end ++;
            }
            while (isalnum(end[0]) || end[0] == '_') {
                end ++;
            }
            char *last = end - 1;
            if (last >= pos && (last[0] == 'e' || last[0] == 'E') &&
                flecs_expr_format_skip_space(end)[0] == '}' &&
                !flecs_expr_format_identifier_exists(
                    parser, desc, pos, end))
            {
                notation = last;
                notation_ch = last[0];
                last[0] = '\0';
            }
        }

        const char *expr_end = flecs_script_parse_expr(
            parser, pos, 0, &format->precision);
        if (notation) {
            notation[0] = notation_ch;
        }
        if (!expr_end) {
            return NULL;
        }
        pos = notation ? notation : expr_end;
    }

    pos = flecs_expr_format_skip_space(pos);
    if (pos[0] == 'e' || pos[0] == 'E') {
        format->notation = pos[0];
        pos ++;
    }

    pos = flecs_expr_format_skip_space(pos);
    if (pos[0] != '}') {
        flecs_expr_format_parse_error(
            parser, "invalid string format specifier");
        return NULL;
    }

    return pos;
}

static void flecs_expr_format_append_fill(
    ecs_strbuf_t *buf,
    char fill,
    int32_t count)
{
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_appendch(buf, fill);
    }
}

int flecs_expr_format_value(
    const ecs_script_t *script,
    const ecs_expr_node_t *node,
    const ecs_value_t *value,
    const ecs_expr_format_t *format,
    int32_t width,
    int32_t precision,
    ecs_strbuf_t *buf)
{
    if (format->width && width < 0) {
        flecs_expr_visit_error(script, node,
            "minimum width must not be negative");
        return -1;
    }
    if (format->width && width > 1024) {
        flecs_expr_visit_error(script, node,
            "minimum width must not exceed 1024");
        return -1;
    }
    if (format->precision && precision < 0) {
        flecs_expr_visit_error(script, node,
            "precision must not be negative");
        return -1;
    }
    if (format->precision && precision > 1024) {
        flecs_expr_visit_error(script, node,
            "precision must not exceed 1024");
        return -1;
    }

    double number;
    if (value->type == ecs_id(ecs_f32_t)) {
        number = (double)*(ecs_f32_t*)value->ptr;
    } else if (value->type == ecs_id(ecs_f64_t)) {
        number = *(ecs_f64_t*)value->ptr;
    } else {
        flecs_expr_visit_error(script, node,
            "format specifiers require an f32 or f64 value");
        return -1;
    }

    char printf_fmt[8];
    char *fmt_ptr = printf_fmt;
    fmt_ptr[0] = '%';
    fmt_ptr ++;
    if (format->sign) {
        fmt_ptr[0] = '+';
        fmt_ptr ++;
    }
    if (precision >= 0) {
        fmt_ptr[0] = '.';
        fmt_ptr[1] = '*';
        fmt_ptr += 2;
    }
    fmt_ptr[0] = format->notation ? format->notation : 'f';
    fmt_ptr[1] = '\0';

    char *str;
    if (precision >= 0) {
        str = flecs_asprintf(printf_fmt, precision, number);
    } else {
        str = flecs_asprintf(printf_fmt, number);
    }
    if (!str) {
        return -1;
    }

    int32_t length = flecs_ito(int32_t, ecs_os_strlen(str));
    int32_t padding = width > length ? width - length : 0;

    if (format->leading_zero && padding) {
        const char *number_ptr = str;
        if (number_ptr[0] == '+' || number_ptr[0] == '-') {
            ecs_strbuf_appendch(buf, number_ptr[0]);
            number_ptr ++;
        }
        flecs_expr_format_append_fill(buf, '0', padding);
        ecs_strbuf_appendstr(buf, number_ptr);
    } else if (format->alignment == '<') {
        ecs_strbuf_appendstr(buf, str);
        flecs_expr_format_append_fill(buf, format->fill, padding);
    } else if (format->alignment == '^') {
        int32_t left = padding / 2;
        flecs_expr_format_append_fill(buf, format->fill, left);
        ecs_strbuf_appendstr(buf, str);
        flecs_expr_format_append_fill(buf, format->fill, padding - left);
    } else {
        flecs_expr_format_append_fill(buf, format->fill, padding);
        ecs_strbuf_appendstr(buf, str);
    }

    ecs_os_free(str);
    return 0;
}

#endif
