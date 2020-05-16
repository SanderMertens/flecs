#include "flecs_private.h"

#define ECS_ANNOTATION_LENGTH_MAX (16)

/** Skip spaces when parsing signature */
static
const char *skip_space(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }
    return ptr;
}

/** Parse element with a dot-separated qualifier ('PARENT.Foo') */
static
char* parse_complex_elem(
    const char *system_id,
    const char *sig,
    int column,
    char *ptr,
    ecs_sig_from_kind_t *from_kind,
    ecs_sig_oper_kind_t *oper_kind,
    ecs_entity_t *flags,
    const char * *source)
{
    char *bptr = ptr;
    if (bptr[0] == '!') {
        *oper_kind = EcsOperNot;
        if (!bptr[1]) {
            ecs_parser_error(system_id, sig, column, "! must be followed by an identifier");
        }
        bptr ++;
    } else if (bptr[0] == '?') {
        *oper_kind = EcsOperOptional;
        if (!bptr[1]) {
            ecs_parser_error(system_id, sig, column, "? must be followed by an identifier");
        }
        bptr ++;
    }

    *source = NULL;

    char *dot = strchr(bptr, '.');
    if (dot) {
        if (bptr == dot) {
            *from_kind = EcsFromEmpty;
        } else if (!strncmp(bptr, "PARENT", dot - bptr)) {
            *from_kind = EcsFromParent;
        } else if (!strncmp(bptr, "SYSTEM", dot - bptr)) {
            *from_kind = EcsFromSystem;
        } else if (!strncmp(bptr, "SELF", dot - bptr)) {
            /* default */
        } else if (!strncmp(bptr, "OWNED", dot - bptr)) {
            *from_kind = EcsFromOwned;
        } else if (!strncmp(bptr, "SHARED", dot - bptr)) {
            *from_kind = EcsFromShared;
        } else if (!strncmp(bptr, "CASCADE", dot - bptr)) {
            *from_kind = EcsCascade;   
        } else {
            *from_kind = EcsFromEntity;
            *source = bptr;
        }
        
        bptr = dot + 1;

        if (!bptr[0]) {
            ecs_parser_error(
                system_id, sig, column + dot - bptr,
                 "%s must be followed by an identifier", 
                 ptr);
        }
    }

    char *or = strchr(bptr, '|');
    if (or) {
        if (!strncmp(bptr, "CHILDOF", or - bptr)) {
            *flags = ECS_CHILDOF;
        } else if (!strncmp(bptr, "INSTANCEOF", or - bptr)) {
            *flags = ECS_INSTANCEOF;
        } else if (!strncmp(bptr, "AND", or - bptr)) {
            *flags = ECS_AND;
        } else if (!strncmp(bptr, "OR", or - bptr)) {
            *flags = ECS_OR;
        } else if (!strncmp(bptr, "XOR", or - bptr)) {
            *flags = ECS_XOR;
        } else if (!strncmp(bptr, "NOT", or - bptr)) {
            *flags = ECS_NOT;
        } else {
            ecs_parser_error(
                system_id, sig, column + or - bptr,
                 "invalid flag identifier '%s'", 
                 bptr);
        }

        bptr = or + 1;

        if (!bptr[0]) {
            ecs_parser_error(
                system_id, sig, column + or - bptr,
                 "%s must be followed by an identifier", 
                 ptr);
        }        
    }

    return bptr;
}

int entity_compare(
    const void *ptr1,
    const void *ptr2)
{
    int64_t e1 = *(ecs_entity_t*)ptr1;
    int64_t e2 = *(ecs_entity_t*)ptr2;

    return (e1 - e2 < 0) 
        ? -1 
        : ((e1 - e2 > 0) 
            ? 1 
            : 0);
}

static
void vec_add_entity(
    ecs_vector_t **vec,
    ecs_entity_t entity)
{
    ecs_entity_t *e = ecs_vector_add(vec, ecs_entity_t);
    *e = entity;

    /* Keep array sorted so that we can use it in type compare operations */
    ecs_vector_sort(*vec, ecs_entity_t, entity_compare);
}


/* -- Private functions -- */

static
const char* parse_annotation(
    const char *system_id,
    const char *sig,
    int column,
    const char *ptr, 
    ecs_sig_inout_kind_t *inout_kind_out)
{
    char *bptr, buffer[ECS_ANNOTATION_LENGTH_MAX + 1];
    char ch;

    ptr = skip_space(ptr);

    for (bptr = buffer; (ch = ptr[0]); ptr ++) {        
        if (ch == ',' || ch == ']') {
            /* Even though currently only one simultaneous annotation is 
             * useful, more annotations may be added in the future. */
            bptr[0] = '\0';

            if (!strcmp(buffer, "in")) {
                *inout_kind_out = EcsIn;
            } else if (!strcmp(buffer, "out")) {
                *inout_kind_out = EcsOut;
            } else if (!strcmp(buffer, "inout")) {
                *inout_kind_out = EcsInOut;
            } else {
                ecs_parser_error(
                    system_id, sig, column, "unknown annotation '%s'", buffer);
            }

            if (ch == ']') {
                break;
            } else {
                ptr = skip_space(ptr + 1);
            }

            bptr = buffer;
        } else {
            if (bptr - buffer >= ECS_ANNOTATION_LENGTH_MAX) {
                ecs_parser_error(
                    system_id, sig, column, "annotation is too long");
            }

            bptr[0] = ch;
            bptr ++;
        }
    }

    if (!ch) {
        ecs_parser_error(system_id, sig, column,
            "annotation cannot appear at end of a column");
    }

    return ptr;
}

/** Parse type expression or signature */
int ecs_parse_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    const char *system_id,
    void *ctx)
{
    size_t len = strlen(sig);
    ecs_assert(len > 0, ECS_INVALID_SIGNATURE, NULL);
    
    const char *ptr;
    char ch, *bptr, *buffer = ecs_os_malloc(len + 1);
    ecs_assert(buffer != NULL, ECS_OUT_OF_MEMORY, NULL);

    bool complex_expr = false;
    bool prev_is_0 = false;
    ecs_sig_from_kind_t from_kind = EcsFromSelf;
    ecs_sig_oper_kind_t oper_kind = EcsOperAnd;
    ecs_sig_inout_kind_t inout_kind = EcsInOut;
    ecs_entity_t flags = 0;
    const char *source;

    for (bptr = buffer, ch = sig[0], ptr = sig; ch; ptr++) {
        ptr = skip_space(ptr);
        ch = *ptr;

        if (prev_is_0) {
            /* 0 can only apppear by itself */
            ecs_parser_error(system_id, sig, ptr - sig, "0 can only appear by itself");
        }

        if (ch == '[') {
            /* Annotations should appear at the beginning of a column */
            if (bptr != buffer) {
                ecs_parser_error(system_id, sig, ptr - sig, "[...] should appear at start of column");
            }

            ptr = parse_annotation(system_id, sig, ptr - sig, ptr + 1, &inout_kind);
            ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        } else if (ch == ',' || (ch == '|' && ptr[1] == '|') || ch == '\0') {
            /* Separators should not appear after an empty column */
            if (bptr == buffer) {
                if (ch) {
                    ecs_parser_error(
                        system_id, sig, ptr - sig, "%c unexpected here", ch);
                } else {
                    ecs_parser_error(
                        system_id, sig, ptr - sig, "unexpected end of expression");
                }
            }

            *bptr = '\0';
            bptr = buffer;

            source = NULL;

            if (complex_expr) {
                ecs_sig_oper_kind_t prev = oper_kind;
                bptr = parse_complex_elem(
                    system_id, sig, ptr - sig, bptr, &from_kind, &oper_kind, 
                    &flags, &source);

                if (oper_kind == EcsOperNot && prev == EcsOperOr) {
                    ecs_parser_error(
                        system_id, sig, ptr - sig, 
                        "cannot use ! in | expression");
                }
            }

            if (oper_kind == EcsOperOr) {
                if (from_kind == EcsFromEmpty) {
                    /* Cannot OR handles */
                    ecs_parser_error(
                        system_id, sig, ptr - sig, 
                        "cannot use | on columns without a source");
                }
            }

            if (!strcmp(bptr, "0")) {
                if (bptr != buffer) {
                    /* 0 can only appear by itself */
                    ecs_parser_error(
                        system_id, sig, ptr - sig, 
                        "0 can only appear by itself");
                }

                from_kind = EcsFromEmpty;
                prev_is_0 = true;
            }

            /* If retrieving a component from a system, only the AND operator is
             * supported. The set of system components is expected to be 
             * constant, and thus no conditional operators are needed. */
            if (from_kind == EcsFromSystem && oper_kind != EcsOperAnd) {
                ecs_parser_error(system_id, sig, ptr - sig,
                    "invalid operator for SYSTEM column");
            }     

            char *source_id = NULL;
            if (source) {
                char *dot = strchr(source, '.');
                source_id = ecs_os_malloc(dot - source + 1);
                ecs_assert(source_id != NULL, ECS_OUT_OF_MEMORY, NULL);

                strncpy(source_id, source, dot - source);
                source_id[dot - source] = '\0';
            }

            if (action(world, system_id, sig, ptr - sig, 
                from_kind, oper_kind, inout_kind, flags, bptr, source_id, ctx)) 
            {
                ecs_abort(ECS_INVALID_SIGNATURE, sig);
            }

            if (source_id) {
                ecs_os_free(source_id);
            }

            /* Reset variables for next column */
            complex_expr = false;
            from_kind = EcsFromSelf;
            oper_kind = EcsOperAnd;
            flags = 0;

            if (ch == '|') {
                if (ptr[1] == '|') {
                    ptr ++;
                    if (oper_kind == EcsOperNot) {
                        ecs_parser_error(system_id, sig, ptr - sig, 
                            "cannot use ! in | expression");
                    }
                    oper_kind = EcsOperOr;
                }
            }

            inout_kind = EcsInOut;

            bptr = buffer;
        } else {
            *bptr = ch;
            bptr ++;

            if (ch == '.' || ch == '!' || ch == '?' || ch == '|') {
                complex_expr = true;
            }
        }
    }

    ecs_os_free(buffer);
    return 0;
}

/** Parse callback that adds component to the components array for a system */
int ecs_parse_signature_action(
    ecs_world_t *world,
    const char *system_id,
    const char *expr,
    int column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t flags,
    const char *component_id,
    const char *source_id,
    void *data)
{
    ecs_sig_t *sig = data;

    ecs_assert(sig != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Lookup component handly by string identifier */
    ecs_entity_t component = ecs_lookup(world, component_id);
    if (!component) {
        /* "0" is a valid expression used to indicate that a system matches no
         * components */
        if (strcmp(component_id, "0")) {
            ecs_parser_error(system_id, expr, column, 
                "unresolved component identifier '%s'", component_id);
        } else {
            /* No need to add 0 component to signature */
            return 0;
        }
    }

    component |= flags;

    ecs_entity_t source = 0;
    if (from_kind == EcsFromEntity) {
        if (from_kind == EcsFromEntity) {
            source = ecs_lookup(world, source_id);
            if (!source) {
                ecs_parser_error(system_id, expr, column, 
                    "unresolved source identifier '%s'", source_id);
            }
        }
    }

    return ecs_sig_add(
        sig, from_kind, oper_kind, inout_kind, component, source);
}

void ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig)
{
    if (expr && strlen(expr)) {
        sig->expr = ecs_os_strdup(expr);
    } else {
        sig->expr = ecs_os_strdup("0");
    }

    ecs_parse_expr(
        world, sig->expr, ecs_parse_signature_action, name, sig);
}

int ecs_sig_add(
    ecs_sig_t *sig,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t component,
    ecs_entity_t source)
{
    ecs_sig_column_t *elem;

    /* AND (default) and optional columns are stored the same way */
    if (oper_kind != EcsOperOr) {
        elem = ecs_vector_add(&sig->columns, ecs_sig_column_t);
        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;
        elem->inout_kind = inout_kind;
        elem->is.component = component;
        elem->source = source;

    /* OR columns store a type id instead of a single component */
    } else {
        ecs_assert(inout_kind != EcsOut, ECS_INVALID_SIGNATURE, NULL);
        elem = ecs_vector_last(sig->columns, ecs_sig_column_t);

        if (elem->oper_kind == EcsOperAnd) {
            ecs_entity_t prev = elem->is.component;
            elem->is.type = NULL;
            vec_add_entity(&elem->is.type, prev);
            vec_add_entity(&elem->is.type, component);
        } else {
            if (elem->from_kind != from_kind) {
                /* Cannot mix FromEntity and FromComponent in OR */
                ecs_parser_error(sig->name, sig->expr, 0, 
                    "cannot mix source kinds in || expression");
                goto error;
            }

            if (elem->oper_kind != EcsOperAnd && elem->oper_kind != EcsOperOr) {
                ecs_parser_error(sig->name, sig->expr, 0, 
                    "cannot mix operators in || expression");                
            }

            vec_add_entity(&elem->is.type, component);
        }

        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;
    }

    return 0;
error:
    return -1;

}

void ecs_sig_deinit(
    ecs_sig_t *sig)
{   
    ecs_vector_each(sig->columns, ecs_sig_column_t, column, {
        if (column->oper_kind == EcsOperOr) {
            ecs_vector_free(column->is.type);
        }
    });

    ecs_vector_free(sig->columns);
    ecs_os_free(sig->expr);
}

/* Check if system meets constraints of non-table columns */
bool ecs_sig_check_constraints(
    ecs_world_t *world,
    ecs_sig_t *sig)
{
    int32_t i, column_count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *buffer = ecs_vector_first(sig->columns);

    for (i = 0; i < column_count; i ++) {
        ecs_sig_column_t *elem = &buffer[i];
        ecs_sig_from_kind_t from_kind = elem->from_kind;
        ecs_sig_oper_kind_t oper_kind = elem->oper_kind;

        if (from_kind == EcsFromEntity) {
            ecs_type_t type = ecs_get_type(world, elem->source);
            if (ecs_type_has_entity(world, type, elem->is.component)) {
                if (oper_kind == EcsOperNot) {
                    return false;
                }
            } else {
                if (oper_kind != EcsOperNot) {
                    return false;
                }
            }
        }
    }

    return true;
}
