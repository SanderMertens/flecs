#include <core.h>

void Entity_init_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void Entity_init_id_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_add_1_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = ecs_ids(TagA)
    });
    test_assert(e != 0);
    test_assert(ecs_has(world, e, TagA));

    ecs_fini(world);
}

void Entity_init_id_add_2_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = ecs_ids(TagA, TagB)
    });
    test_assert(e != 0);
    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_id_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_new(world);
    test_assert(scope != 0);

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, scope));

    ecs_fini(world);
}

void Entity_init_id_name_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child"
    });
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, scope));
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_path_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child.grandchild"
    });
    test_assert(e != 0);

    test_str(ecs_get_name(world, e), "grandchild");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child.grandchild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_fullpath_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "::parent.child.grandchild",
        .root_sep = "::"
    });
    test_assert(e != 0);

    test_str(ecs_get_name(world, e), "grandchild");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child.grandchild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_fullpath_w_scope_existing(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "::parent.child.grandchild",
        .root_sep = "::"
    });
    test_assert(e != 0);

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "::parent.child.grandchild",
        .root_sep = "::"
    });
    test_assert(r != 0);
    test_assert(r == e);

    test_str(ecs_get_name(world, e), "grandchild");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child.grandchild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_name_1_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = ecs_ids(TagA)
    });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, TagA));
    test_str(ecs_get_name(world, e), "foo");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_name_2_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = ecs_ids(TagA, TagB)
    });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, TagA));
    test_str(ecs_get_name(world, e), "foo");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_name_2_comp_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child",
        .add = ecs_ids(TagA, TagB)
    });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, TagA));
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_id_add_1_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .add = ecs_ids(TagA)
    });
    test_assert(r != 0);
    test_assert(e == r);
    test_assert(ecs_has_id(world, e, TagA));

    ecs_fini(world);
}

void Entity_id_add_2_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .add = ecs_ids(TagA, TagB)
    });
    test_assert(r != 0);
    test_assert(e == r);
    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_id_path_w_sep(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent::child",
        .sep = "::"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_find_id_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_w_existing_id_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = ecs_new(world);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = id,
        .name = "foo"
    });
    test_assert(e != 0);
    test_assert(e == id);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_name_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);   

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);    

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_path_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child.grandchild"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "grandchild");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child.grandchild");
    ecs_os_free(path);    

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child.grandchild"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_name_match(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "foo"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_name_match_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_path(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);  

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "child"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_set_scope(world, 0);

    r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "parent.child"
    });
    test_assert(r != 0);
    test_assert(r == e);     

    ecs_fini(world);
}

void Entity_find_id_path_match(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "parent.child"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_path_match_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child.grandchild"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "grandchild");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "child.grandchild"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_set_scope(world, 0);

    r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "parent.child.grandchild"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_name_mismatch(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "bar"
    });    

    ecs_log_set_level(-4);
    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "bar"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_name_mismatch_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    ecs_log_set_level(-4);
    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "parent"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_path_mismatch(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.foo"
    }); 

    ecs_log_set_level(-4);
    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "parent.foo"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_path_mismatch_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    test_assert(scope != 0);
    test_str(ecs_get_name(world, scope), "parent");

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child.grandchild"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "grandchild");

    ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "child.foo"
    });

    ecs_log_set_level(-4);
    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "child.foo"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_add_1_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = ecs_ids(TagA)
    });
    test_assert(r == e);
    test_assert(ecs_has_id(world, e, TagA));

    ecs_fini(world);
}

void Entity_find_id_add_2_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = ecs_ids(TagA, TagB)
    });
    test_assert(r == e);
    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_w_scope_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent"
    });
    ecs_entity_t foo = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.foo"
    });

    ecs_set_scope(world, foo);

    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });

    test_assert(child != 0);
    test_str(ecs_get_name(world, child), "foo");

    char *path = ecs_get_path(world, child);
    test_assert(path != NULL);
    test_str(path, "parent.foo.foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_w_core_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Prefab"
    });

    test_assert(e != 0);
    test_assert(e != EcsPrefab);

    ecs_fini(world);
}

void Entity_init_w_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void Entity_init_w_with_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){ 
        .name = "foo"
    });

    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));
    test_str(ecs_get_name(world, e), "foo");

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void Entity_init_w_with_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new(world);

    ecs_set_with(world, Tag);
    ecs_set_scope(world, parent);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));
    test_assert(ecs_has_id(world, e, ecs_pair(EcsChildOf, parent)));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void Entity_init_w_with_w_name_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new(world);

    ecs_set_with(world, Tag);
    ecs_set_scope(world, parent);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });

    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));
    test_assert(ecs_has_id(world, e, ecs_pair(EcsChildOf, parent)));
    test_str(ecs_get_name(world, e), "foo");

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void Entity_is_valid(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_bool(ecs_is_valid(world, e), true);

    ecs_fini(world);
}

void Entity_is_recycled_valid(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_bool(ecs_is_valid(world, e), true);

    ecs_delete(world, e);
    test_bool(ecs_is_valid(world, e), false);

    e = ecs_new(world);
    test_assert(e != 0);
    test_assert((uint32_t)e != e);
    test_bool(ecs_is_valid(world, e), true);

    ecs_fini(world);
}

void Entity_is_0_valid(void) {
    ecs_world_t *world = ecs_mini();

    test_bool(ecs_is_valid(world, 0), false);

    ecs_fini(world);
}

void Entity_is_junk_valid(void) {
    ecs_world_t *world = ecs_mini();

    test_bool(ecs_is_valid(world, 1000), false);
    test_bool(ecs_is_valid(world, 0xFFFFFFFF), false);
    test_bool(ecs_is_valid(world, 0x4DCDCDCDCDCD), false);

    ecs_make_alive(world, 1000);
    ecs_make_alive(world, 0xFFFFFFFF);
    
    test_bool(ecs_is_alive(world, 1000), true);
    test_bool(ecs_is_alive(world, 0xFFFFFFFF), true);
    test_bool(ecs_is_alive(world, 0x4DCDCDCDCDCD), false);

    test_bool(ecs_is_valid(world, 0xFFFFFFFFFFFFFFFF), false);
    test_bool(ecs_is_valid(world, 0xFFFFFFFF00000000), false);
    test_bool(ecs_is_valid(world, 0x4DCDCDCDCDCDCD), false);

    ecs_fini(world);
}

void Entity_is_not_alive_valid(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_bool(ecs_is_valid(world, e), true);
    test_bool(ecs_is_alive(world, e), true);

    ecs_add_id(world, e, 500);
    test_bool(ecs_is_valid(world, e), true);
    test_bool(ecs_is_alive(world, e), true);

    ecs_delete(world, e);
    test_bool(ecs_is_valid(world, e), false);
    test_bool(ecs_is_alive(world, e), false);

    ecs_fini(world);
}

void Entity_is_nonzero_gen_valid(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = ECS_GENERATION_INC(1000);
    test_int(ECS_GENERATION(id), 1);
    test_bool(ecs_is_alive(world, id), false);
    test_bool(ecs_exists(world, id), false);
    test_bool(ecs_is_valid(world, id), false);

    ecs_fini(world);
}

void Entity_init_w_name_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_init_w_name_twice_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e1), "Foo");
    test_str(ecs_get_name(world, e2), "Foo");
    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_init_w_nested_name_twice_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Bar"
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Bar"
    });

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e1), "Bar");
    test_str(ecs_get_name(world, e2), "Bar");
    test_assert(e1 == e2);

    ecs_entity_t parent = ecs_lookup(world, "Foo");
    test_assert(parent != 0);
    test_assert(ecs_has_pair(world, e1, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_init_w_scope_name_twice_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Bar"
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Bar"
    });

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e1), "Bar");
    test_str(ecs_get_name(world, e2), "Bar");
    test_assert(e1 == e2);

    ecs_entity_t parent = ecs_lookup(world, "Foo");
    test_assert(parent != 0);
    test_assert(ecs_has_pair(world, e1, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_init_w_childof_name_twice_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t parent = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    test_assert(parent != 0);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar",
        .parent = parent
    });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar",
        .parent = parent
    });
    test_assert(e2 != 0);

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e1), "Bar");
    test_str(ecs_get_name(world, e2), "Bar");
    test_assert(e1 == e2);

    test_assert(parent == ecs_lookup(world, "Foo"));
    test_assert(ecs_has_pair(world, e1, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_init_w_childof_nested_name_twice_deferred(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t parent = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    test_assert(parent != 0);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar.Hello",
        .parent = parent
    });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar.Hello",
        .parent = parent
    });
    test_assert(e2 != 0);

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e1), "Hello");

    char *path = ecs_get_path(world, e1);
    test_str(path, "Foo.Bar.Hello");
    ecs_os_free(path);

    test_assert(parent == ecs_lookup(world, "Foo"));
    ecs_entity_t bar = ecs_lookup_child(world, parent, "Bar");
    test_assert(bar != 0);
    test_assert(ecs_has_pair(world, e1, EcsChildOf, bar));

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_init_w_name_staged(void) {
    ecs_world_t *world = ecs_mini();

    ecs_readonly_begin(world, false);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_entity_t e = ecs_entity_init(stage, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    test_assert(e != 0);

    ecs_readonly_end(world);

    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_record_find_for_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_record_t *r = ecs_record_find(world, e);
    test_assert(r != NULL);
    test_assert(r->table == NULL);

    ecs_fini(world);
}

void Entity_record_find(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new_w(world, TagA);
    test_assert(e != 0);

    ecs_record_t *r = ecs_record_find(world, e);
    test_assert(r != NULL);
    test_assert(r->table != NULL);

    ecs_fini(world);
}

void Entity_record_find_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new_w(world, TagA);
    test_assert(e != 0);

    ecs_readonly_begin(world, false);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_record_t *r = ecs_record_find(stage, e);
    test_assert(r != NULL);
    test_assert(r->table != NULL);

    ecs_readonly_end(world);

    ecs_fini(world);
}

void Entity_make_alive_zero_gen(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = 1000;
    test_bool(ecs_is_alive(world, id), false);
    test_bool(ecs_is_valid(world, id), false);
    test_bool(ecs_exists(world, id), false);

    ecs_make_alive(world, id);
    test_bool(ecs_is_alive(world, id), true);
    test_bool(ecs_is_valid(world, id), true);
    test_bool(ecs_exists(world, id), true);

    ecs_fini(world);
}

void Entity_make_alive_nonzero_gen(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = ECS_GENERATION_INC(1000);
    test_int(ECS_GENERATION(id), 1);
    test_bool(ecs_is_alive(world, id), false);
    test_bool(ecs_is_valid(world, id), false);
    test_bool(ecs_exists(world, id), false);

    ecs_make_alive(world, id);
    test_bool(ecs_is_alive(world, id), true);
    test_bool(ecs_is_valid(world, id), true);
    test_bool(ecs_exists(world, id), true);

    ecs_fini(world);
}

void Entity_make_alive_zero_gen_exists(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    ecs_make_alive(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(ecs_is_valid(world, e));
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_make_alive_nonzero_gen_exists(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    e = ECS_GENERATION_INC(e);
    e = ECS_GENERATION_INC(e); /* one above the current value */
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    ecs_make_alive(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(ecs_is_valid(world, e));
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_make_alive_zero_gen_exists_alive(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));
    ecs_delete(world, e);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != e);
    test_assert(ecs_strip_generation(e1) == e);

    test_expect_abort();
    ecs_make_alive(world, e); // not allowed, can't ensure gen 0 if gen 1 is alive
}

void Entity_make_alive_nonzero_gen_exists_alive(void) {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));
    ecs_delete(world, e);

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != e);
    test_assert(ecs_strip_generation(e1) == e);

    e = ECS_GENERATION_INC(e);
    e = ECS_GENERATION_INC(e);

    test_expect_abort();
    ecs_make_alive(world, e); // not allowed, can't ensure gen 2 if gen 1 is alive
}

void Entity_set_scope_w_entity_init_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_readonly_begin(world, false);
    ecs_entity_t parent = ecs_set_name(stage, 0, "Parent");
    ecs_set_scope(stage, parent);
    ecs_entity_t child = ecs_entity_init(stage, &(ecs_entity_desc_t){
        .name = "Child"
    });
    ecs_set_scope(stage, 0);
    ecs_readonly_end(world);

    test_assert(parent != 0);
    test_assert(child != 0);

    test_str( ecs_get_name(world, parent), "Parent");
    test_str( ecs_get_name(world, child), "Child");

    test_assert( ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_entity_init_w_scope_twice(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    
    test_assert(ecs_set_scope(world, parent) == 0);
    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child"
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child"
    });
    test_assert(ecs_set_scope(world, 0) == parent);

    test_assert( ecs_has_pair(world, e1, EcsChildOf, parent) );
    test_str(ecs_get_name(world, e1), "Child");

    char *path = ecs_get_path(world, e1);
    test_str(path, "Parent.Child");
    ecs_os_free(path);

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_twice(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .parent = parent
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .parent = parent
    });

    test_assert( ecs_has_pair(world, e1, EcsChildOf, parent) );
    test_str(ecs_get_name(world, e1), "Child");

    char *path = ecs_get_path(world, e1);
    test_str(path, "Parent.Child");
    ecs_os_free(path);

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_nested_name_twice(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Child",
        .parent = parent
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Child",
        .parent = parent
    });

    ecs_entity_t foo = ecs_lookup_child(world, parent, "Foo");
    test_assert(foo != 0);

    test_assert( ecs_has_pair(world, e1, EcsChildOf, foo) );
    test_str(ecs_get_name(world, e1), "Child");

    char *path = ecs_get_path(world, e1);
    test_str(path, "Parent.Foo.Child");
    ecs_os_free(path);

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_and_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_set_scope(world, parent_a);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .parent = parent_b
    });
    ecs_set_scope(world, 0);

    test_assert( !ecs_has_pair(world, child, EcsChildOf, parent_a) );
    test_assert( ecs_has_pair(world, child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, child), "Child");

    char *path = ecs_get_path(world, child);
    test_str(path, "ParentB.Child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_and_scope_and_scoped_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_set_scope(world, parent_a);
    ecs_entity_t grand_child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child.GrandChild",
        .parent = parent_b
    });
    ecs_set_scope(world, 0);

    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_a) );
    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, grand_child), "GrandChild");

    ecs_entity_t child = ecs_lookup_child(world, parent_b, "Child");
    test_assert(child != 0);
    test_assert( ecs_has_pair(world, grand_child, EcsChildOf, child));

    char *path = ecs_get_path(world, grand_child);
    test_str(path, "ParentB.Child.GrandChild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_and_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .parent = parent
    });

    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_deferred_entity_init_w_childof_and_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_defer_begin(world);

    ecs_set_scope(world, parent_a);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .parent = parent_b
    });
    ecs_set_scope(world, 0);

    ecs_defer_end(world);

    test_assert( !ecs_has_pair(world, child, EcsChildOf, parent_a) );
    test_assert( ecs_has_pair(world, child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, child), "Child");

    char *path = ecs_get_path(world, child);
    test_str(path, "ParentB.Child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_deferred_entity_init_w_childof_and_scope_and_scoped_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_defer_begin(world);

    ecs_set_scope(world, parent_a);
    ecs_entity_t grand_child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child.GrandChild",
        .parent = parent_b
    });
    ecs_set_scope(world, 0);

    ecs_defer_end(world);

    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_a) );
    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, grand_child), "GrandChild");

    ecs_entity_t child = ecs_lookup_child(world, parent_b, "Child");
    test_assert(child != 0);
    test_assert( ecs_has_pair(world, grand_child, EcsChildOf, child));

    char *path = ecs_get_path(world, grand_child);
    test_str(path, "ParentB.Child.GrandChild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_deferred_entity_init_w_childof_and_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_defer_begin(world);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .parent = parent
    });
    test_assert(child != 0);
    test_assert(!ecs_has_pair(world, child, EcsChildOf, parent));
    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_entity_init_w_empty_string_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = ""
    });
    
    test_assert(e != 0);
    test_assert(!ecs_has_pair(world, e, ecs_id(EcsIdentifier), EcsName));

    ecs_fini(world);
}

void Entity_entity_init_w_empty_string_symbol(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .symbol = ""
    });
    
    test_assert(e != 0);
    test_assert(!ecs_has_pair(world, e, ecs_id(EcsIdentifier), EcsSymbol));

    ecs_fini(world);
}

void Entity_set_name_w_0_twice(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_set_name(world, 0, "Foo");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Foo");

    test_str(ecs_get_name(world, e1), "Foo");
    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_new_entity_twice(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Foo" });

    test_str(ecs_get_name(world, e1), "Foo");
    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_new_entity_scoped_twice(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_entity(world, { .name = "Foo.Bar" });
    ecs_entity_t e2 = ecs_entity(world, { .name = "Foo.Bar" });

    test_str(ecs_get_name(world, e1), "Bar");
    test_assert(e1 == e2);

    ecs_entity_t parent = ecs_lookup(world, "Foo");
    test_assert(parent != 0);
    test_assert(ecs_has_pair(world, e1, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_defer_component_init(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t c = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .type.size = ECS_SIZEOF(Position),
        .type.alignment = ECS_ALIGNOF(Position)
    });

    ecs_defer_end(world);

    test_assert(c != 0);
    test_str(ecs_get_name(world, c), "Position");
    test_assert(ecs_has(world, c, EcsComponent));

    const EcsComponent *ptr = ecs_get(world, c, EcsComponent);
    test_assert(ptr != NULL);
    test_int(ptr->size, ECS_SIZEOF(Position));
    test_int(ptr->alignment, ECS_ALIGNOF(Position));

    ecs_fini(world);
}

void Entity_defer_component_init_w_symbol(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t c = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "Position", .symbol = "Position"}),
        .type.size = ECS_SIZEOF(Position),
        .type.alignment = ECS_ALIGNOF(Position)
    });

    ecs_defer_end(world);

    test_assert(c != 0);
    test_str(ecs_get_name(world, c), "Position");
    test_str(ecs_get_symbol(world, c), "Position");
    test_assert(ecs_has(world, c, EcsComponent));

    const EcsComponent *ptr = ecs_get(world, c, EcsComponent);
    test_assert(ptr != NULL);
    test_int(ptr->size, ECS_SIZEOF(Position));
    test_int(ptr->alignment, ECS_ALIGNOF(Position));

    ecs_fini(world);
}

void Entity_defer_entity_init_w_symbol(void) {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t c = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Position",
        .symbol = "Position",
    });

    ecs_defer_end(world);

    test_assert(c != 0);
    test_str(ecs_get_name(world, c), "Position");
    test_str(ecs_get_symbol(world, c), "Position");

    ecs_fini(world);
}

void Entity_staged_set_name_n_stages(void) {
    ecs_world_t *world = ecs_mini();

    ecs_set_stage_count(world, 2);

    ecs_readonly_begin(world, false);

    ecs_world_t *s = ecs_get_stage(world, 1);

    ecs_entity_t e = ecs_entity_init(s, &(ecs_entity_desc_t){
        .name = "Foo"
    });

    ecs_readonly_end(world);

    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_staged_set_symbol_n_stages(void) {
    ecs_world_t *world = ecs_mini();

    ecs_set_stage_count(world, 2);

    ecs_readonly_begin(world, false);

    ecs_world_t *s = ecs_get_stage(world, 1);

    ecs_entity_t e = ecs_entity_init(s, &(ecs_entity_desc_t){
        .name = "Foo",
        .symbol = "Bar"
    });

    ecs_readonly_end(world);

    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Foo");
    test_str(ecs_get_symbol(world, e), "Bar");

    ecs_fini(world);
}

void Entity_entity_init_w_add_childof_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .parent = parent
    });

    test_assert( ecs_has_pair(world, e, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_entity_w_short_notation(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    test_assert(e != 0);
    test_str("Foo", ecs_get_name(world, e));

    ecs_fini(world);
}

void Entity_override_inherited_symbol(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t Foo = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo",
        .symbol = "FooSymbol"
    });
    test_assert(Foo != 0);

    ecs_entity_t Bar = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar",
        .symbol = "BarSymbol",
        .parent = EcsIsA
    });
    test_assert(Bar != 0);

    test_str(ecs_get_name(world, Foo), "Foo");
    test_str(ecs_get_name(world, Bar), "Bar");
    test_str(ecs_get_symbol(world, Foo), "FooSymbol");
    test_str(ecs_get_symbol(world, Bar), "BarSymbol");

    ecs_fini(world);
}

void Entity_use_low_id_for_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    test_assert(ecs_id(Position) < FLECS_HI_COMPONENT_ID);

    ecs_fini(world);
}

void Entity_get_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, e3);

    test_int(0, ecs_get_depth(world, e1, EcsChildOf));
    test_int(1, ecs_get_depth(world, e2, EcsChildOf));
    test_int(2, ecs_get_depth(world, e3, EcsChildOf));
    test_int(3, ecs_get_depth(world, e4, EcsChildOf));

    ecs_fini(world);
}

void Entity_get_depth_non_acyclic(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new(world);

    test_expect_abort();
    ecs_get_depth(world, e1, Rel);
}

void Entity_get_depth_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    test_int(0, ecs_get_depth(world, e1, EcsChildOf));

    ecs_fini(world);
}

void Entity_get_depth_2_paths(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsIsA, e2);
    ecs_add_pair(world, e4, EcsIsA, e3);

    test_int(0, ecs_get_depth(world, e1, EcsIsA));
    test_int(1, ecs_get_depth(world, e2, EcsIsA));
    test_int(2, ecs_get_depth(world, e3, EcsIsA));
    test_int(3, ecs_get_depth(world, e4, EcsIsA));

    ecs_fini(world);
}

void Entity_entity_init_w_empty_sep(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo.bar",
        .sep = ""
    });
    test_assert(child != 0);
    test_str(ecs_get_name(world, child), "foo.bar");
    test_assert(!ecs_has_pair(world, child, EcsChildOf, EcsWildcard));

    ecs_entity_t l = ecs_lookup_path_w_sep(world, 0, "foo.bar", "", NULL, false);
    test_assert(l != 0);
    test_assert(l == child);

    ecs_fini(world);
}

void Entity_entity_init_w_empty_sep_from_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_set_scope(world, parent);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo.bar",
        .sep = ""
    });
    ecs_set_scope(world, 0);

    test_assert(child != 0);
    test_str(ecs_get_name(world, child), "foo.bar");
    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_entity_t l = ecs_lookup_path_w_sep(world, parent, "foo.bar", "", NULL, false);
    test_assert(l != 0);
    test_assert(l == child);

    ecs_fini(world);
}

void Entity_entity_init_w_empty_sep_w_prefix(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo.bar",
        .sep = ""
    });

    test_assert(child != 0);
    test_str(ecs_get_name(world, child), "foo.bar");
    test_assert(!ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_entity_t l = ecs_lookup_path_w_sep(world, parent, "::foo.bar", "", "::", false);
    test_assert(l != 0);
    test_assert(l == child);

    ecs_fini(world);
}

void Entity_set_name_w_same_ptr(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "foo" });
    const char *name = ecs_get_name(world, e);
    test_assert(name != NULL);
    test_str(name, "foo");

    ecs_set_name(world, e, name);
    name = ecs_get_name(world, e); 
    test_assert(name != NULL);
    test_str(name, "foo");

    ecs_fini(world);
}

void Entity_set_name_w_overlapping_ptr(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "foo" });
    const char *name = ecs_get_name(world, e);
    test_assert(name != NULL);
    test_str(name, "foo");

    ecs_set_name(world, e, &name[1]);
    name = ecs_get_name(world, e); 
    test_assert(name != NULL);
    test_str(name, "oo");

    ecs_fini(world);
}

void Entity_defer_set_name_w_overlapping_ptr(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "foo" });
    const char *name = ecs_get_name(world, e);
    test_assert(name != NULL);
    test_str(name, "foo");

    ecs_defer_begin(world);
    ecs_set_name(world, e, &name[1]);
    ecs_defer_end(world);

    name = ecs_get_name(world, e); 
    test_assert(name != NULL);
    test_str(name, "oo");

    ecs_fini(world);
}

void Entity_make_alive_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(!ecs_is_alive(world, 1000));
    ecs_make_alive(stage, 1000);
    test_assert(ecs_is_alive(world, 1000));

    ecs_fini(world);
}

void Entity_make_alive_after_deleted_1_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    ecs_delete(world, e1);
    e1 = ecs_new(world);
    ecs_delete(world, e1);

    test_assert(!ecs_is_alive(world, e1));
    ecs_make_alive(world, e1);
    test_assert(ecs_is_alive(world, e1));

    ecs_fini(world);
}

void Entity_make_alive_after_deleted_2_entities(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    ecs_delete(world, e1);
    e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_new(world);
    ecs_delete(world, e1);
    ecs_delete(world, e2);

    test_assert(!ecs_is_alive(world, e1));
    ecs_make_alive(world, e1);
    test_assert(ecs_is_alive(world, e1));

    test_assert(!ecs_is_alive(world, e2));
    ecs_make_alive(world, e2);
    test_assert(ecs_is_alive(world, e2));

    ecs_fini(world);
}

void Entity_defer_entity_init_w_set_name_w_add_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });

    ecs_defer_begin(world);
    ecs_entity(world, { .id = e, .name = "Foo" });
    ecs_set_name(world, e, "FooBar");
    ecs_add_pair(world, e, EcsChildOf, parent);
    ecs_defer_end(world);

    test_str(ecs_get_name(world, e), "FooBar");
    test_assert(e == ecs_lookup(world, "parent.FooBar"));

    ecs_fini(world);
}

void Entity_entity_w_digit_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "10000"
    });

    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "10000");
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_entity_w_existing_digit_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "7"
    });

    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "7");
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_entity_from_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "#10000"
    });

    test_assert(e != 0);
    test_uint(e, 10000);
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_entity_from_existing_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "#10000"
    });

    ecs_entity_t f = ecs_entity(world, {
        .name = "#10000"
    });

    test_assert(e != 0);
    test_assert(e == f);
    test_uint(e, 10000);
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_entity_from_digit_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "#10000.foo"
    });

    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has_pair(world, e, EcsChildOf, 10000));
    test_str(ecs_get_name(world, e), "foo");

    ecs_fini(world);
}

void Entity_entity_from_existing_digit_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "#10000.foo"
    });

    ecs_entity_t f = ecs_entity(world, {
        .name = "#10000.foo"
    });

    test_assert(e != 0);
    test_assert(e == f);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_has_pair(world, e, EcsChildOf, 10000));
    test_str(ecs_get_name(world, e), "foo");

    ecs_fini(world);
}

void Entity_entity_from_digit_0_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "#0.foo"
    });

    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, EcsWildcard));
    test_str(ecs_get_name(world, e), "foo");

    ecs_fini(world);
}

void Entity_entity_from_conflicting_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_log_set_level(-4);

    ecs_entity_t e = ecs_entity(world, {
        .id = 6,
        .name = "#7"
    });

    test_assert(e == 0);

    ecs_fini(world);
}

void Entity_set_version_on_nonempty_entity(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t) {
        .id = 123456
    });

    ecs_add(world, e1, Tag);

    e1 |= 0x200000000ul;
    ecs_set_version(world, e1);

    ecs_iter_t it = ecs_each(world, Tag);
    ecs_entity_t first = ecs_iter_first(&it);
    test_uint(first, e1);

    ecs_fini(world);
}

void Entity_set_version_while_deferred(void) {
    install_test_abort();

    ecs_world_t* world = ecs_mini();
    ecs_entity_t e1 = ecs_new(world);

    ecs_defer_begin(world);    
    test_expect_abort();
    ecs_set_version(world, e1 |= 0x200000000ul);
}

static
void Observer(ecs_iter_t *it) {
    int *invoked = it->ctx;
    (*invoked) ++;
}

void Entity_commit_w_on_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    int invoked = 0;

    ecs_observer(world, {
        .query.terms = {
            { .id = Tag }
        },
        .events = { EcsOnAdd },
        .callback = Observer,
        .ctx = &invoked
    });

    ecs_table_t *dst = ecs_table_add_id(world, NULL, Tag);
    test_assert(dst != NULL);

    ecs_type_t added = {
        .array = (ecs_id_t[]){ Tag },
        .count = 1
    };

    ecs_entity_t e = ecs_new(world);
    ecs_commit(world, e, NULL, dst, &added, NULL);

    test_assert(ecs_has(world, e, Tag));
    test_int(invoked, 1);

    ecs_fini(world);
}

void Entity_commit_w_on_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);

    int invoked = 0;

    ecs_observer(world, {
        .query.terms = {
            { .id = Tag }
        },
        .events = { EcsOnRemove },
        .callback = Observer,
        .ctx = &invoked
    });

    ecs_type_t removed = {
        .array = (ecs_id_t[]){ Tag },
        .count = 1
    };

    ecs_table_t *dst = ecs_table_add_id(world, NULL, Foo);
    test_assert(dst != NULL);

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_add_id(world, e, Foo);
    ecs_commit(world, e, NULL, dst, NULL, &removed);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Foo));
    test_int(invoked, 1);

    ecs_fini(world);
}

static ECS_TAG_DECLARE(Foo);

static
void Observer_w_cmd(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int *invoked = it->ctx;
    (*invoked) ++;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_add(world, e, Foo);
        test_assert(!ecs_has(world, e, Foo));
    }
}

void Entity_commit_w_cmd_in_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG_DEFINE(world, Foo);

    int invoked = 0;

    ecs_observer(world, {
        .query.terms = {
            { .id = Tag }
        },
        .events = { EcsOnAdd },
        .callback = Observer_w_cmd,
        .ctx = &invoked
    });

    ecs_table_t *dst = ecs_table_add_id(world, NULL, Tag);
    test_assert(dst != NULL);

    ecs_type_t added = {
        .array = (ecs_id_t[]){ Tag },
        .count = 1
    };

    ecs_entity_t e = ecs_new(world);
    ecs_commit(world, e, NULL, dst, &added, NULL);

    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Foo));
    test_int(invoked, 1);

    ecs_fini(world);
}

void Entity_entity_init_existing_no_sep(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .name = "foo",
        .sep = ""
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_2_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( 
            ecs_value(Position, {10, 20}),
            ecs_value(Velocity, {1, 2})
        )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_1_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity(world, {
        .add = ecs_ids(TagA),
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, TagA));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_2_comp_2_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity(world, {
        .add = ecs_ids(TagA, TagB),
        .set = ecs_values(
            ecs_value(Position, {10, 20}),
            ecs_value(Velocity, {1, 2})
        )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_entity(world, {
        .name = "Foo",
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, e), "Foo");

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_existing(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    ecs_entity_t e2 = ecs_entity(world, {
        .id = e1,
        .set = ecs_values( ecs_value(Velocity, {1, 2}) )
    });

    test_assert(e1 == e2);
    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e1, Velocity));

    const Position *p = ecs_get(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_existing_empty(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_entity(world, {
        .id = e1,
        .set = ecs_values( ecs_value(Velocity, {1, 2}) )
    });

    test_assert(e1 == e2);
    test_assert(ecs_has(world, e1, Velocity));

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

static int add_position_invoked = 0;
static int set_position_invoked = 0;

static void add_position_hook(ecs_iter_t *it) {
    add_position_invoked ++;
}

static void set_position_hook(ecs_iter_t *it) {
    set_position_invoked ++;
}

void Entity_entity_init_w_set_w_hook(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .on_add = add_position_hook,
        .on_set = set_position_hook
    });

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_int(add_position_invoked, 1);
    test_int(set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_w_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }},
        .events = { EcsOnAdd },
        .callback = add_position_hook
    });

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }},
        .events = { EcsOnSet },
        .callback = set_position_hook
    });

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_int(add_position_invoked, 1);
    test_int(set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_1_tag_w_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}), {TagA} )
    });

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, TagA));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Position));
    
    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_2_comp_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( 
            ecs_value(Position, {10, 20}),
            ecs_value(Velocity, {1, 2})
        )
    });

    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_1_tag_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity(world, {
        .add = ecs_ids(TagA),
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, TagA));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, TagA));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_2_comp_2_tag_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity(world, {
        .add = ecs_ids(TagA, TagB),
        .set = ecs_values(
            ecs_value(Position, {10, 20}),
            ecs_value(Velocity, {1, 2})
        )
    });

    test_assert(e != 0);
    
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, TagA));
    test_assert(!ecs_has(world, e, TagB));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_w_name_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity(world, {
        .name = "Foo",
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    test_str(ecs_get_name(world, e), "Foo");

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_str(ecs_get_name(world, e), "Foo");

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_existing_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_defer_begin(world);

    ecs_entity_t e1 = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}) )
    });

    ecs_entity_t e2 = ecs_entity(world, {
        .id = e1,
        .set = ecs_values( ecs_value(Velocity, {1, 2}) )
    });

    test_assert(e1 == e2);
    test_assert(!ecs_has(world, e1, Position));
    test_assert(!ecs_has(world, e1, Velocity));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e1, Velocity));

    const Position *p = ecs_get(world, e1, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_existing_empty_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_defer_begin(world);

    ecs_entity_t e1 = ecs_new(world);
    ecs_entity_t e2 = ecs_entity(world, {
        .id = e1,
        .set = ecs_values( ecs_value(Velocity, {1, 2}) )
    });

    test_assert(e1 == e2);
    test_assert(!ecs_has(world, e1, Velocity));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e1, Velocity));

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_entity_init_w_set_1_comp_1_tag_w_set_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity(world, {
        .set = ecs_values( ecs_value(Position, {10, 20}), {TagA} )
    });

    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, TagA));

    ecs_defer_end(world);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, TagA));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_insert_1_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_insert(world, ecs_value(Position, {10, 20}));
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_insert_2_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        ecs_value(Velocity, {1, 2})
    );
    
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, e, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Entity_insert_1_comp_1_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_insert(world, 
        ecs_value(Position, {10, 20}),
        {TagA}
    );
    
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, TagA));

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Entity_entity_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e = ecs_entity(world, {
        .parent = parent
    });
    
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_entity_w_parent_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e = ecs_entity(world, {
        .parent = parent,
        .name = "Foo"
    });
    
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_entity_w_parent_w_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e = ecs_entity(world, {
        .parent = parent,
        .add = (ecs_id_t[]){TagA, 0}
    });
    
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));
    test_assert(ecs_has(world, e, TagA));

    ecs_fini(world);
}

void Entity_entity_w_parent_w_add_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e = ecs_entity(world, {
        .parent = parent,
        .name = "Foo",
        .add = (ecs_id_t[]){TagA, 0}
    });
    
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));
    test_assert(ecs_has(world, e, TagA));
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_entity_w_parent_w_set(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e = ecs_entity(world, {
        .parent = parent,
        .set = (ecs_value_t[]){{ .type = TagA}, {0}}
    });
    
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));
    test_assert(ecs_has(world, e, TagA));

    ecs_fini(world);
}

void Entity_entity_w_parent_w_set_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e = ecs_entity(world, {
        .parent = parent,
        .name = "Foo",
        .set = (ecs_value_t[]){{ .type = TagA}, {0}}
    });
    
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));
    test_assert(ecs_has(world, e, TagA));
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}
