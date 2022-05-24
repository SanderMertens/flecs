#include <api.h>

void Id_0_is_wildcard() {
    test_assert( !ecs_id_is_wildcard(0) );
}

void Id_wildcard_is_wildcard() {
    test_assert( ecs_id_is_wildcard(EcsWildcard) );
}

void Id_any_is_wildcard() {
    test_assert( ecs_id_is_wildcard(EcsAny) );
}

void Id_entity_is_wildcard() {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t e = ecs_new_id(world);
    test_assert( !ecs_id_is_wildcard(e) );
    ecs_fini(world);
}

void Id_pair_is_wildcard() {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new_id(world);
    ecs_entity_t o = ecs_new_id(world);
    test_assert( !ecs_id_is_wildcard(ecs_pair(r, o)) );
    ecs_fini(world);
}

void Id_pair_w_rel_wildcard_is_wildcard() {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t o = ecs_new_id(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsWildcard, o)) );
    ecs_fini(world);
}

void Id_pair_w_obj_wildcard_is_wildcard() {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new_id(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(r, EcsWildcard)) );
    ecs_fini(world);
}

void Id_pair_w_wildcard_wildcard_is_wildcard() {
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsWildcard, EcsWildcard)) );
}

void Id_pair_w_rel_any_is_wildcard() {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t o = ecs_new_id(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsAny, o)) );
    ecs_fini(world);
}

void Id_pair_w_obj_any_is_wildcard() {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t r = ecs_new_id(world);
    test_assert( ecs_id_is_wildcard(ecs_pair(r, EcsAny)) );
    ecs_fini(world);
}

void Id_pair_w_any_any_is_wildcard() {
    test_assert( ecs_id_is_wildcard(ecs_pair(EcsAny, EcsAny)) );
}
