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

/** Parse element with a dot-separated qualifier ('CONTAINER.Foo') */
static
char* parse_complex_elem(
    const char *system_id,
    const char *sig,
    int column,
    char *ptr,
    ecs_sig_from_kind_t *from_kind,
    ecs_sig_oper_kind_t *oper_kind,
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
        } else if (!strncmp(bptr, "CONTAINER", dot - bptr)) {
            *from_kind = EcsFromContainer;
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

    return bptr;
}

static
bool has_refs(
    ecs_sig_t *sig)
{
    int32_t i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns);

    for (i = 0; i < count; i ++) {
        ecs_sig_from_kind_t from_kind = columns[i].from_kind;

        if (columns[i].oper_kind == EcsOperNot && from_kind == EcsFromEmpty) {
            /* Special case: if oper kind is Not and the query contained a
             * shared expression, the expression is translated to FromId to
             * prevent resolving the ref */
            return true;
        } else if (from_kind != EcsFromSelf && from_kind != EcsFromEmpty) {
            /* If the component is not from the entity being iterated over, and
             * the column is not just passing an id, it must be a reference to
             * another entity. */
            return true;
        }
    }

    return false;
}

static
void compute_sig_types(
    ecs_world_t *world,
    ecs_sig_t *sig)
{
    int i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns);

    for (i = 0; i < count; i ++) {
        ecs_sig_column_t *elem = &columns[i];
        ecs_sig_from_kind_t from = elem->from_kind;
        ecs_sig_oper_kind_t op = elem->oper_kind;

        /* AND (default) and optional columns are stored the same way */
        if (from == EcsFromEntity) {
            ecs_set_watch(world, &world->stage, elem->source);
        } else if (from == EcsCascade) {
            sig->cascade_by = i + 1;
        } else if (op == EcsOperOr) {
            /* Nothing to be done here */
        } else if (op == EcsOperNot) {
            if (from == EcsFromSelf) {
                sig->not_from_self =
                    ecs_type_add_intern(
                        world, NULL, sig->not_from_self, elem->is.component);

            } else if (from == EcsFromOwned) {
                sig->not_from_owned =
                    ecs_type_add_intern(
                        world, NULL, sig->not_from_owned, elem->is.component);

            } else if (from == EcsFromShared) {
                sig->not_from_shared =
                    ecs_type_add_intern(
                        world, NULL, sig->not_from_shared, elem->is.component);

            } else if (from == EcsFromEntity) {
                /* Nothing to be done here */

            } else if (from == EcsFromContainer) {
                sig->not_from_container =
                    ecs_type_add_intern(
                      world, NULL, sig->not_from_container, elem->is.component);
            }
        } else if (op == EcsOperAnd) {
            if (from == EcsFromSelf) {
                sig->and_from_self = ecs_type_add_intern(
                    world, NULL, sig->and_from_self, elem->is.component);

            } else if (from == EcsFromOwned) {
                sig->and_from_owned = ecs_type_add_intern(
                    world, NULL, sig->and_from_owned, elem->is.component);

            } else if (from == EcsFromShared) {
                sig->and_from_shared = ecs_type_add_intern(
                    world, NULL, sig->and_from_shared, elem->is.component);

            } else if (from == EcsFromSystem) {
                sig->and_from_system = ecs_type_add_intern(
                    world, NULL, sig->and_from_system, elem->is.component);

            } else if (from == EcsFromContainer) {
                sig->and_from_container = ecs_type_add_intern(
                    world, NULL, sig->and_from_container, elem->is.component);

            }  else if (from == EcsCascade) {
                sig->cascade_by = i + 1;
            }
        }
    }
}

static
void postprocess(
    ecs_world_t *world,
    ecs_sig_t *sig)
{
    int i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns);

    for (i = 0; i < count; i ++) {
        ecs_sig_column_t *column = &columns[i];
        ecs_sig_oper_kind_t op = column->oper_kind; 

        if (op == EcsOperOr) {
            /* If the signature explicitly indicates interest in EcsDisabled,
             * signal that disabled entities should be matched. By default,
             * disabled entities are not matched. */
            if (ecs_type_has_entity_intern(
                world, column->is.type, 
                ecs_entity(EcsDisabled), false))
            {
                sig->match_disabled = true;
            }

            /* If the signature explicitly indicates interest in EcsPrefab,
             * signal that disabled entities should be matched. By default,
             * prefab entities are not matched. */
            if (ecs_type_has_entity_intern(
                world, column->is.type, 
                ecs_entity(EcsPrefab), false))
            {
                sig->match_prefab = true;
            }            
        } else if (op == EcsOperAnd || op == EcsOperOptional) {
            if (column->is.component == ecs_entity(EcsDisabled)) {
                sig->match_disabled = true;
            } else if (column->is.component == ecs_entity(EcsPrefab)) {
                sig->match_prefab = true;
            }
        }

        if (sig->match_prefab && sig->match_disabled) {
            break;
        }
    }

    compute_sig_types(world, sig);

    sig->has_refs = has_refs(sig);
}

/* -- Private functions -- */

/** Count components in a signature */
int32_t ecs_columns_count(
    const char *sig)
{
    const char *ptr = sig;
    int32_t count = 1;

    while ((ptr = strchr(ptr + 1, ','))) {
        count ++;
    }

    return count;
}

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

        } else if (ch == ',' || ch == '|' || ch == '\0') {
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
                bptr = parse_complex_elem(system_id, sig, ptr - sig, bptr, &from_kind, &oper_kind, &source);

                if (oper_kind == EcsOperNot && prev == EcsOperOr) {
                    ecs_parser_error(system_id, sig, ptr - sig, "cannot use ! in | expression");
                }
            }

            if (oper_kind == EcsOperOr) {
                if (from_kind == EcsFromEmpty) {
                    /* Cannot OR handles */
                    ecs_parser_error(system_id, sig, ptr - sig, "cannot use | on columns without a source");
                }
            }

            if (!strcmp(bptr, "0")) {
                if (bptr != buffer) {
                    /* 0 can only appear by itself */
                    ecs_parser_error(system_id, sig, ptr - sig, "0 can only appear by itself");
                }

                from_kind = EcsFromEmpty;
                prev_is_0 = true;
            }

            /* If retrieving a component from a system, only the AND operator is
             * supported. The set of system components is expected to be constant, and
             * thus no conditional operators are needed. */
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
                from_kind, oper_kind, inout_kind, bptr, source_id, ctx)) 
            {
                ecs_abort(ECS_INVALID_SIGNATURE, sig);
            }

            if (source_id) {
                ecs_os_free(source_id);
            }

            /* Reset variables for next column */
            complex_expr = false;
            from_kind = EcsFromSelf;

            if (ch == '|') {
                if (oper_kind == EcsOperNot) {
                    ecs_parser_error(system_id, sig, ptr - sig, 
                        "cannot use ! in | expression");
                }
                oper_kind = EcsOperOr;
            } else {
                oper_kind = EcsOperAnd;
            }

            inout_kind = EcsInOut;

            bptr = buffer;
        } else {
            *bptr = ch;
            bptr ++;

            if (ch == '.' || ch == '!' || ch == '?') {
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
    const char *component_id,
    const char *source_id,
    void *data)
{
    ecs_sig_t *sig = data;
    ecs_sig_column_t *elem;

    if (from_kind == EcsFromSelf || 
        from_kind == EcsFromOwned ||
        from_kind == EcsFromShared ||
        from_kind == EcsFromContainer) 
    {
        sig->needs_tables = true;
    }

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

    /* AND (default) and optional columns are stored the same way */
    if (oper_kind == EcsOperAnd || oper_kind == EcsOperOptional) {
        elem = ecs_vector_add(&sig->columns, ecs_sig_column_t);
        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;
        elem->inout_kind = inout_kind;
        elem->is.component = component;

        if (from_kind == EcsFromEntity) {
            elem->source = ecs_lookup(world, source_id);
            if (!elem->source) {
                ecs_parser_error(system_id, expr, column, 
                    "unresolved source identifier '%s'", source_id);
            }

            ecs_set_watch(world, &world->stage, elem->source);
        }

    /* OR columns store a type id instead of a single component */
    } else if (oper_kind == EcsOperOr) {
        ecs_assert(inout_kind != EcsOut, ECS_INVALID_SIGNATURE, NULL);

        elem = ecs_vector_last(sig->columns, ecs_sig_column_t);
        if (elem->oper_kind == EcsOperAnd) {
            elem->is.type = ecs_type_add_intern(
                world, NULL, 0, elem->is.component);

        } else {
            if (elem->from_kind != from_kind) {
                /* Cannot mix FromEntity and FromComponent in OR */
                ecs_parser_error(system_id, expr, column, 
                    "cannot use mixed source kinds in | expression");
                goto error;
            }
        }

        elem->is.type = ecs_type_add_intern(
            world, NULL, elem->is.type, component);
        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;

    /* A system stores two NOT familes; one for entities and one for components.
     * These can be quickly & efficiently used to exclude tables with
     * ecs_type_contains. */
    } else if (oper_kind == EcsOperNot) {
        elem = ecs_vector_add(&sig->columns, ecs_sig_column_t);
        elem->from_kind = EcsFromEmpty;
        elem->oper_kind = oper_kind;
        elem->inout_kind = inout_kind;
        elem->is.component = component;

        if (from_kind == EcsFromSelf) {
            sig->not_from_self = ecs_type_add_intern(
                world, NULL, sig->not_from_self, component);

        } else if (from_kind == EcsFromOwned) {
            sig->not_from_owned = ecs_type_add_intern(
                world, NULL, sig->not_from_owned, component);

        } else if (from_kind == EcsFromShared) {
            sig->not_from_shared = ecs_type_add_intern(
                world, NULL, sig->not_from_shared, component); 

        } else if (from_kind == EcsFromEntity) {
            elem->from_kind = EcsFromEntity;
            elem->source = ecs_lookup(world, source_id);

        } else {
            sig->not_from_container =
              ecs_type_add_intern(
                  world, NULL, sig->not_from_container, component);
        }
    }

    return 0;
error:
    return -1;
}

void ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig)
{
    sig->expr = ecs_os_strdup(expr);

    ecs_parse_expr(
        world, expr, ecs_parse_signature_action, name, sig);

    postprocess(world, sig);
}

void ecs_sig_deinit(
    ecs_sig_t *sig)
{
    ecs_os_free(sig->expr);
    ecs_vector_free(sig->columns);
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
