#include <addons.h>

void Doc_get_set_name() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);

    ecs_doc_set_name(world, e, "Human readable name");

    test_assert( ecs_has_pair(world, e, ecs_id(EcsDocDescription), EcsName));
    test_str( ecs_doc_get_name(world, e), "Human readable name");

    ecs_fini(world);
}

void Doc_get_entity_name() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_name(world, 0, "Entity name");

    test_assert( ecs_has_pair(world, e, ecs_id(EcsIdentifier), EcsName));
    test_str( ecs_doc_get_name(world, e), "Entity name");

    ecs_fini(world);
}

void Doc_get_set_brief() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_doc_set_brief(world, MyTag, "Brief description");

    test_assert( ecs_has_pair(world, MyTag, ecs_id(EcsDocDescription), EcsDocBrief));
    test_str( ecs_doc_get_brief(world, MyTag), "Brief description");

    ecs_fini(world);
}

void Doc_get_set_detail() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_doc_set_detail(world, MyTag, "Detailed description");

    test_assert( ecs_has_pair(world, MyTag, ecs_id(EcsDocDescription), EcsDocDetail));
    test_str( ecs_doc_get_detail(world, MyTag), "Detailed description");

    ecs_fini(world);
}

void Doc_get_set_link() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, MyTag);

    ecs_doc_set_link(world, MyTag, "http://www.example.com");

    test_assert( ecs_has_pair(world, MyTag, ecs_id(EcsDocDescription), EcsDocLink));
    test_str( ecs_doc_get_link(world, MyTag), "http://www.example.com");

    ecs_fini(world);
}
