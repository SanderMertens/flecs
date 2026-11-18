#include <addons.h>

void Doc_get_set_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_name(world, e, "Human readable name");

    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsName));
    test_str( ecs_doc_get_name(world, e), "Human readable name");

    ecs_fini(world);
}

void Doc_get_entity_name(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_name(world, 0, "Entity name");

    test_assert( ecs_has_pair(world, e, ecs_id(EcsIdentifier), EcsName));
    test_str( ecs_doc_get_name(world, e), "Entity name");

    ecs_fini(world);
}

void Doc_get_set_brief(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_doc_set_brief(world, MyTag, "Brief description");

    test_assert( ecs_has_pair(world, MyTag, ecs_id(EcsDocDescription), EcsDocBrief));
    test_str( ecs_doc_get_brief(world, MyTag), "Brief description");

    ecs_fini(world);
}

void Doc_get_set_detail(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_doc_set_detail(world, MyTag, "Detailed description");

    test_assert( ecs_has_pair(world, MyTag, ecs_id(EcsDocDescription), EcsDocDetail));
    test_str( ecs_doc_get_detail(world, MyTag), "Detailed description");

    ecs_fini(world);
}

void Doc_get_set_link(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_doc_set_link(world, MyTag, "http://www.example.com");

    test_assert( ecs_has_pair(world, MyTag, ecs_id(EcsDocDescription), EcsDocLink));
    test_str( ecs_doc_get_link(world, MyTag), "http://www.example.com");

    ecs_fini(world);
}

void Doc_get_set_uuid(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_doc_set_uuid(world, MyTag, "81f50b40-09ff-4ce0-a388-4a52a14052c7");

    test_assert( ecs_has_pair(world, MyTag, ecs_id(EcsDocDescription), EcsDocUuid));
    test_str( ecs_doc_get_uuid(world, MyTag), "81f50b40-09ff-4ce0-a388-4a52a14052c7");

    ecs_fini(world);
}

void Doc_set_name_nullptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_name(world, e, "foo");
    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsName));

    ecs_doc_set_name(world, e, NULL);
    test_assert( !ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsName));

    ecs_fini(world);
}

void Doc_set_brief_nullptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_brief(world, e, "foo");
    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocBrief));

    ecs_doc_set_brief(world, e, NULL);
    test_assert( !ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocBrief));

    ecs_fini(world);
}

void Doc_set_detail_nullptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_detail(world, e, "foo");
    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocDetail));

    ecs_doc_set_detail(world, e, NULL);
    test_assert( !ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocDetail));

    ecs_fini(world);
}

void Doc_set_link_nullptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_link(world, e, "foo");
    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocLink));

    ecs_doc_set_link(world, e, NULL);
    test_assert( !ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocLink));

    ecs_fini(world);
}

void Doc_set_color_nullptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_color(world, e, "foo");
    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocColor));

    ecs_doc_set_color(world, e, NULL);
    test_assert( !ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocColor));

    ecs_fini(world);
}

void Doc_set_uuid_nullptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_uuid(world, e, "foo");
    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocUuid));

    ecs_doc_set_uuid(world, e, NULL);
    test_assert( !ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsDocUuid));

    ecs_fini(world);
}

void Doc_get_set_source_location(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    test_assert(ecs_doc_get_source_location(world, e) == NULL);

    ecs_doc_set_source_location(world, e, "foo.c", 10);

    test_assert( ecs_has(world, e, EcsDocSourceLocation));
    const EcsDocSourceLocation *loc = ecs_doc_get_source_location(world, e);
    test_assert(loc != NULL);
    test_str(loc->file, "foo.c");
    test_int(loc->line, 10);

    ecs_doc_set_source_location(world, e, "bar.c", 20);

    loc = ecs_doc_get_source_location(world, e);
    test_assert(loc != NULL);
    test_str(loc->file, "bar.c");
    test_int(loc->line, 20);

    ecs_fini(world);
}

void Doc_set_source_location_copies_file(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    char *file = ecs_os_strdup("foo.c");
    ecs_doc_set_source_location(world, e, file, 10);
    test_assert(ecs_doc_get_source_location(world, e)->file != file);
    ecs_os_free(file);

    const EcsDocSourceLocation *loc = ecs_doc_get_source_location(world, e);
    test_assert(loc != NULL);
    test_str(loc->file, "foo.c");
    test_int(loc->line, 10);

    ecs_fini(world);
}

void Doc_set_source_location_nullptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    ecs_doc_set_source_location(world, e, "foo.c", 10);
    test_assert( ecs_has(world, e, EcsDocSourceLocation));

    ecs_doc_set_source_location(world, e, NULL, 0);
    test_assert( !ecs_has(world, e, EcsDocSourceLocation));
    test_assert(ecs_doc_get_source_location(world, e) == NULL);

    ecs_fini(world);
}

void Doc_set_source_location_here(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);

    int32_t line = __LINE__; ecs_doc_set_source_location_here(world, e);

    const EcsDocSourceLocation *loc = ecs_doc_get_source_location(world, e);
    test_assert(loc != NULL);
    test_str(loc->file, __FILE__);
    test_int(loc->line, line);

    ecs_fini(world);
}

static void Dummy(ecs_iter_t *it) { }

void Doc_set_source_location_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_doc_set_source_location(world, Dummy, "systems.c", 42);

    const EcsDocSourceLocation *loc = ecs_doc_get_source_location(world, Dummy);
    test_assert(loc != NULL);
    test_str(loc->file, "systems.c");
    test_int(loc->line, 42);

    ecs_fini(world);
}

void Doc_set_source_location_observer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_OBSERVER(world, Dummy, EcsOnAdd, Position);

    ecs_doc_set_source_location(world, Dummy, "observers.c", 7);

    const EcsDocSourceLocation *loc = ecs_doc_get_source_location(world, Dummy);
    test_assert(loc != NULL);
    test_str(loc->file, "observers.c");
    test_int(loc->line, 7);

    ecs_fini(world);
}

void Doc_source_location_not_inherited(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_doc_set_source_location(world, base, "foo.c", 10);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_doc_get_source_location(world, inst) == NULL);

    ecs_fini(world);
}

void Doc_source_location_reflection(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world);
    ecs_doc_set_source_location(world, e, "foo.c", 10);

    const EcsDocSourceLocation *loc = ecs_doc_get_source_location(world, e);
    char *json = ecs_ptr_to_json(world, ecs_id(EcsDocSourceLocation), loc);
    test_str(json, "{\"file\":\"foo.c\", \"line\":10}");
    ecs_os_free(json);

    ecs_fini(world);
}
