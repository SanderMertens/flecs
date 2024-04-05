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
