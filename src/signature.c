#include "private_api.h"

#define ECS_ANNOTATION_LENGTH_MAX (16)

#define TOK_SOURCE ':'
#define TOK_AND ','
#define TOK_OR "||"
#define TOK_NOT '!'
#define TOK_OPTIONAL '?'
#define TOK_ROLE '|'
#define TOK_NAME_SEP '.'
#define TOK_ANNOTATE_OPEN '['
#define TOK_ANNOTATE_CLOSE ']'

#define TOK_ANY "ANY"
#define TOK_OWNED "OWNED"
#define TOK_SHARED "SHARED"
#define TOK_SYSTEM "SYSTEM"
#define TOK_PARENT "PARENT"
#define TOK_CASCADE "CASCADE"

#define TOK_ROLE_CHILDOF "CHILDOF"
#define TOK_ROLE_INSTANCEOF "INSTANCEOF"
#define TOK_ROLE_TRAIT "TRAIT"
#define TOK_ROLE_AND "AND"
#define TOK_ROLE_OR "OR"
#define TOK_ROLE_XOR "XOR"
#define TOK_ROLE_NOT "NOT"

#define TOK_IN "in"
#define TOK_OUT "out"
#define TOK_INOUT "inout"

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

/** Parse element with a dot-separated qualifier ('PARENT:Foo') */
static
char* parse_complex_elem(
    const char *name,
    const char *sig,
    int64_t column,
    char *ptr,
    ecs_sig_from_kind_t *from_kind,
    ecs_sig_oper_kind_t *oper_kind,
    ecs_entity_t *flags,
    const char * *source)
{
    char *bptr = ptr;
    if (bptr[0] == TOK_NOT) {
        *oper_kind = EcsOperNot;
        if (!bptr[1]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(name, sig, column, 
                "not must be followed by an identifier");
        }
        bptr ++;

    } else if (bptr[0] == TOK_OPTIONAL) {
        *oper_kind = EcsOperOptional;
        if (!bptr[1]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(name, sig, column, 
                "optional must be followed by an identifier");
        }
        bptr ++;
    }

    *source = NULL;

    char *src = strchr(bptr, TOK_SOURCE);
    if (src) {
        size_t token_len = ecs_to_size_t(src - bptr);
        if (bptr == src) {
            *from_kind = EcsFromEmpty;
        } else if (!strncmp(bptr, TOK_PARENT, token_len)) {
            *from_kind = EcsFromParent;
        } else if (!strncmp(bptr, TOK_SYSTEM, token_len)) {
            *from_kind = EcsFromSystem;
        } else if (!strncmp(bptr, TOK_ANY, token_len)) {
            *from_kind = EcsFromAny;
        } else if (!strncmp(bptr, TOK_OWNED, token_len)) {
            *from_kind = EcsFromOwned;
        } else if (!strncmp(bptr, TOK_SHARED, token_len)) {
            *from_kind = EcsFromShared;
        } else if (!strncmp(bptr, TOK_CASCADE, token_len)) {
            *from_kind = EcsCascade;   
        } else {
            *from_kind = EcsFromEntity;
            *source = bptr;
        }
        
        bptr = src + 1;

        if (!bptr[0]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(
                name, sig, column + src - bptr,
                 "%s must be followed by an identifier", 
                 ptr);
        }
    }

    char *or = strchr(bptr, TOK_ROLE);
    if (or) {
        size_t token_len = ecs_to_size_t(or - bptr);
        if (!strncmp(bptr, TOK_ROLE_CHILDOF, token_len)) {
            *flags = ECS_CHILDOF;
        } else if (!strncmp(bptr, TOK_ROLE_INSTANCEOF, token_len)) {
            *flags = ECS_INSTANCEOF;
        } else if (!strncmp(bptr, TOK_ROLE_TRAIT, token_len)) {
            *flags = ECS_TRAIT;            
        } else if (!strncmp(bptr, TOK_ROLE_AND, token_len)) {
            *flags = ECS_AND;
        } else if (!strncmp(bptr, TOK_ROLE_OR, token_len)) {
            *flags = ECS_OR;
        } else if (!strncmp(bptr, TOK_ROLE_XOR, token_len)) {
            *flags = ECS_XOR;
        } else if (!strncmp(bptr, TOK_ROLE_NOT, token_len)) {
            *flags = ECS_NOT;
        } else {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(
                name, sig, column + or - bptr,
                 "invalid flag identifier '%s'", 
                 bptr);
        }

        bptr = or + 1;

        if (!bptr[0]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(
                name, sig, column + or - bptr,
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
    ecs_entity_t e1 = *(ecs_entity_t*)ptr1;
    ecs_entity_t e2 = *(ecs_entity_t*)ptr2;
    return (e1 > e2) - (e1 < e2);
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
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr, 
    ecs_sig_inout_kind_t *inout_kind_out)
{
    char *bptr, buffer[ECS_ANNOTATION_LENGTH_MAX + 1];
    char ch;

    ptr = skip_space(ptr);

    for (bptr = buffer; (ch = ptr[0]); ptr ++) {        
        if (ch == TOK_AND || ch == TOK_ANNOTATE_CLOSE) {
            /* Even though currently only one simultaneous annotation is 
             * useful, more annotations may be added in the future. */
            bptr[0] = '\0';

            if (!strcmp(buffer, TOK_IN)) {
                *inout_kind_out = EcsIn;
            } else if (!strcmp(buffer, TOK_OUT)) {
                *inout_kind_out = EcsOut;
            } else if (!strcmp(buffer, TOK_INOUT)) {
                *inout_kind_out = EcsInOut;
            } else {
                if (!name) {
                    return NULL;
                }

                ecs_parser_error(
                    name, sig, column, "unknown annotation '%s'", buffer);
            }

            if (ch == TOK_ANNOTATE_CLOSE) {
                break;
            } else {
                ptr = skip_space(ptr + 1);
            }

            bptr = buffer;
        } else {
            if (bptr - buffer >= ECS_ANNOTATION_LENGTH_MAX) {
                if (!name) {
                    return NULL;
                }

                ecs_parser_error(
                    name, sig, column, "annotation is too long");
            }

            bptr[0] = ch;
            bptr ++;
        }
    }

    if (!ch) {
        if (!name) {
            return NULL;
        }

        ecs_parser_error(name, sig, column,
            "annotation cannot appear at end of a column");
    }

    return ptr;
}

/** Parse type expression or signature */
int ecs_parse_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    const char *name,
    void *ctx)
{
    ecs_size_t len = ecs_os_strlen(sig);
    ecs_assert(len > 0, ECS_INVALID_SIGNATURE, NULL);
    
    const char *ptr;
    char ch, *bptr, *buffer = ecs_os_malloc(len + 1);
    ecs_assert(buffer != NULL, ECS_OUT_OF_MEMORY, NULL);

    bool complex_expr = false;
    bool prev_is_0 = false;
    ecs_sig_from_kind_t from_kind = EcsFromOwned;
    ecs_sig_oper_kind_t oper_kind = EcsOperAnd;
    ecs_sig_inout_kind_t inout_kind = EcsInOut;
    ecs_entity_t flags = 0;
    const char *source;

    for (bptr = buffer, ch = sig[0], ptr = sig; ch; ptr++) {
        ptr = skip_space(ptr);
        ch = *ptr;

        if (prev_is_0) {
            if (!name) {
                return -1;
            }

            /* 0 can only apppear by itself */
            ecs_parser_error(
                name, sig, ptr - sig, "0 can only appear by itself");
        }

        if (ch == TOK_ANNOTATE_OPEN) {
            /* Annotations should appear at the beginning of a column */
            if (bptr != buffer) {
                if (!name) {
                    return -1;
                }

                ecs_parser_error(name, sig, ptr - sig, 
                    "[...] should appear at start of column");
            }

            ptr = parse_annotation(name, sig, ptr - sig, ptr + 1, &inout_kind);
            if (!ptr) {
                return -1;
            }

        } else if (ch == TOK_AND || (ch == TOK_OR[0] && ptr[1] == TOK_OR[1]) || ch == '\0') {
            /* Separators should not appear after an empty column */
            if (bptr == buffer) {
                if (ch) {
                    if (!name) {
                        return -1;
                    }

                    ecs_parser_error(
                        name, sig, ptr - sig, "%c unexpected here", ch);
                } else {
                    if (!name) {
                        return -1;
                    }

                    ecs_parser_error(
                        name, sig, ptr - sig, "unexpected end of expression");
                }
            }

            *bptr = '\0';
            bptr = buffer;

            source = NULL;

            if (complex_expr) {
                ecs_sig_oper_kind_t prev = oper_kind;
                bptr = parse_complex_elem(
                    name, sig, ptr - sig, bptr, &from_kind, &oper_kind, 
                    &flags, &source);
                if (!bptr) {
                    return -1;
                }

                if (oper_kind == EcsOperNot && prev == EcsOperOr) {
                    if (!name) {
                        return -1;
                    }

                    ecs_parser_error(
                        name, sig, ptr - sig, 
                        "cannot use ! in | expression");
                }
            }

            if (oper_kind == EcsOperOr) {
                if (from_kind == EcsFromEmpty) {
                    if (!name) {
                        return -1;
                    }

                    /* Cannot OR handles */
                    ecs_parser_error(
                        name, sig, ptr - sig, 
                        "cannot use | on columns without a source");
                }
            }

            if (!strcmp(bptr, "0")) {
                if (bptr != buffer) {
                    if (!name) {
                        return -1;
                    }

                    /* 0 can only appear by itself */
                    ecs_parser_error(
                        name, sig, ptr - sig, 
                        "0 can only appear by itself");
                }

                from_kind = EcsFromEmpty;
                prev_is_0 = true;
            }

            /* If retrieving a component from a system, only the AND operator is
             * supported. The set of system components is expected to be 
             * constant, and thus no conditional operators are needed. */
            if (from_kind == EcsFromSystem && oper_kind != EcsOperAnd) {
                if (!name) {
                    return -1;
                }

                ecs_parser_error(name, sig, ptr - sig,
                    "invalid operator for SYSTEM column");
            }     

            char *source_id = NULL;
            if (source) {
                char *src = strchr(source, TOK_SOURCE);
                source_id = ecs_os_malloc(ecs_to_i32(src - source + 1));
                ecs_assert(source_id != NULL, ECS_OUT_OF_MEMORY, NULL);

                ecs_os_strncpy(source_id, source, ecs_to_i32(src - source));
                source_id[src - source] = '\0';
            }

            if (action(world, name, sig, ptr - sig, 
                from_kind, oper_kind, inout_kind, flags, bptr, source_id, ctx)) 
            {
                if (!name) {
                    return -1;
                }
                
                ecs_abort(ECS_INVALID_SIGNATURE, sig);
            }

            if (source_id) {
                ecs_os_free(source_id);
            }

            /* Reset variables for next column */
            complex_expr = false;
            from_kind = EcsFromOwned;
            oper_kind = EcsOperAnd;
            flags = 0;

            if (ch == TOK_ROLE) {
                if (ptr[1] == TOK_OR[1]) {
                    ptr ++;
                    if (oper_kind == EcsOperNot) {
                        if (!name) {
                            return -1;
                        }

                        ecs_parser_error(name, sig, ptr - sig, 
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

            if (ch == TOK_SOURCE || ch == TOK_NOT || ch == TOK_OPTIONAL || ch == TOK_ROLE) {
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
    int64_t column,
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
    ecs_entity_t component = ecs_lookup_fullpath(world, component_id);
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
            source = ecs_lookup_fullpath(world, source_id);
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
    if (expr && ecs_os_strlen(expr)) {
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
    ecs_vector_each(sig->columns, ecs_sig_column_t, elem, {
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
    });

    return true;
}
