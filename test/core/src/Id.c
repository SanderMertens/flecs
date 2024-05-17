#include <core.h>

void Id_0_is_wildcard(void) {
    test_assert( !ecs_id_is_wildcard(0) );
}

void Id_wildcard_is_wildcard(void) {
    test_assert( ecs_id_is_wildcard(EcsWildcard) );
}

void Id_any_is_wildcard(void) {
    test_assert( ecs_id_is_wildcard(EcsAny) );
}

void Id_entity_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t e = ecs_new(world);
    test_assert( !ecs_id_is_wildcard(e) );
    ecs_fini(world);
}

void Id_pair_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    test_assert( !ecs_id_is_wildcard(ecs_pair(r, o)) );
    ecs_fini(world);
}

void Id_pair_w_rel_wildcard_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t o = ecs_new(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsWildcard, o)) );
    ecs_fini(world);
}

void Id_pair_w_obj_wildcard_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(r, EcsWildcard)) );
    ecs_fini(world);
}

void Id_pair_w_wildcard_wildcard_is_wildcard(void) {
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsWildcard, EcsWildcard)) );
}

void Id_pair_w_rel_any_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t o = ecs_new(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsAny, o)) );
    ecs_fini(world);
}

void Id_pair_w_obj_any_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(r, EcsAny)) );
    ecs_fini(world);
}

void Id_pair_w_any_any_is_wildcard(void) {
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsAny, EcsAny)) );
}

void Id_pair_w_override_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    test_assert( !ecs_id_is_wildcard(ECS_AUTO_OVERRIDE | ecs_pair(r, EcsWildcard)));
    ecs_fini(world);
}

void Id_pair_w_toggle_is_wildcard(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    test_assert( !ecs_id_is_wildcard(ECS_TOGGLE | ecs_pair(r, EcsWildcard)));
    ecs_fini(world);
}

void Id_tag_id_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t t = ecs_new(world);
    test_assert( ecs_id_is_tag(world, t));
    ecs_fini(world);
}

void Id_component_id_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    test_assert( !ecs_id_is_tag(world, ecs_id(Position)));
    ecs_fini(world);
}

void Id_pair_id_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    ecs_entity_t o = ecs_new(world);
    test_assert( ecs_id_is_tag(world, ecs_pair(r, o)));
    ecs_fini(world);
}

void Id_pair_id_w_rel_component_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    ecs_entity_t o = ecs_new(world);
    test_assert( !ecs_id_is_tag(world, ecs_pair(ecs_id(Position), o)));
    ecs_fini(world);
}

void Id_pair_id_w_obj_component_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    ecs_entity_t r = ecs_new(world);
    test_assert( !ecs_id_is_tag(world, ecs_pair(r, ecs_id(Position))));
    ecs_fini(world);
}

void Id_pair_id_w_rel_component_obj_wildcard_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    test_assert( !ecs_id_is_tag(world, ecs_pair(ecs_id(Position), EcsWildcard)));
    ecs_fini(world);
}

void Id_pair_id_w_obj_wildcard_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    test_assert( !ecs_id_is_tag(world, ecs_pair(r, EcsWildcard)));
    ecs_fini(world);
}

void Id_pair_id_w_tag_property_w_obj_component_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    ecs_entity_t r = ecs_new_w_id(world, EcsPairIsTag);
    test_assert( ecs_id_is_tag(world, ecs_pair(r, ecs_id(Position))));
    ecs_fini(world);
}

void Id_pair_id_w_tag_property_w_obj_wildcard_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    ecs_entity_t r = ecs_new_w_id(world, EcsPairIsTag);
    test_assert( ecs_id_is_tag(world, ecs_pair(r, EcsWildcard)));
    ecs_fini(world);
}

void Id_pair_w_rel_wildcard_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t o = ecs_new(world);
    test_assert( !ecs_id_is_tag(world, ecs_pair(EcsWildcard, o)));
    ecs_fini(world);
}

void Id_pair_w_obj_wildcard_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    test_assert( !ecs_id_is_tag(world, ecs_pair(r, EcsWildcard)));
    ecs_fini(world);
}

void Id_pair_w_rel_tag_obj_wildcard_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    ecs_add_id(world, r, EcsPairIsTag);
    test_assert( ecs_id_is_tag(world, ecs_pair(r, EcsWildcard)));
    ecs_fini(world);
}

void Id_pair_w_wildcard_wildcard_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    test_assert( !ecs_id_is_tag(world, ecs_pair(EcsWildcard, EcsWildcard)));
    ecs_fini(world);
}

void Id_pair_w_rel_any_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t o = ecs_new(world);
    test_assert( !ecs_id_is_tag(world, ecs_pair(EcsAny, o)));
    ecs_fini(world);
}

void Id_pair_w_obj_any_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    test_assert( !ecs_id_is_tag(world, ecs_pair(r, EcsAny)));
    ecs_fini(world);
}

void Id_pair_w_rel_tag_obj_any_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new(world);
    ecs_add_id(world, r, EcsPairIsTag);
    test_assert( ecs_id_is_tag(world, ecs_pair(r, EcsAny)));
    ecs_fini(world);
}

void Id_pair_w_any_any_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    test_assert( !ecs_id_is_tag(world, ecs_pair(EcsAny, EcsAny)));
    ecs_fini(world);
}

void Id_id_w_override_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    test_assert( ecs_id_is_tag(world, ECS_AUTO_OVERRIDE | ecs_id(Position)));
    ecs_fini(world);
}

void Id_id_w_toggle_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    test_assert( ecs_id_is_tag(world, ECS_TOGGLE | ecs_id(Position)));
    ecs_fini(world);
}

void Id_pair_id_override_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    ecs_entity_t o = ecs_new(world);
    test_assert( ecs_id_is_tag(world, ECS_AUTO_OVERRIDE | ecs_pair(ecs_id(Position), o)));
    ecs_fini(world);
}

void Id_pair_id_toggle_is_tag(void) {
    ecs_world_t *world = ecs_mini();
    ECS_COMPONENT(world, Position);
    ecs_entity_t o = ecs_new(world);
    test_assert( ecs_id_is_tag(world, ECS_TOGGLE | ecs_pair(ecs_id(Position), o)));
    ecs_fini(world);
}

void Id_make_pair(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t t = ecs_new(world);
    ecs_id_t id = ecs_make_pair(r, t);

    test_assert( ecs_pair_first(world, id) == r);
    test_assert( ecs_pair_second(world, id) == t);

    ecs_fini(world);
}

void Id_make_pair_of_pair(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t t = ecs_new(world);
    ecs_id_t id = ecs_make_pair(r, t);

    test_expect_abort();
    ecs_make_pair(id, t);
}

void Id_make_pair_of_pair_tgt(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new(world);
    ecs_entity_t t = ecs_new(world);
    ecs_id_t id = ecs_make_pair(r, t);

    test_expect_abort();
    ecs_make_pair(r, id);
}
