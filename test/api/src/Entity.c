#include <api.h>

void Entity_init_id() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void Entity_init_id_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_path() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_add_1_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);
    test_assert(ecs_has(world, e, TagA));

    ecs_fini(world);
}

void Entity_init_id_add_2_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA, TagB}
    });
    test_assert(e != 0);
    test_assert(ecs_has(world, e, TagA));
    test_assert(ecs_has(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_id_w_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_new_id(world);
    test_assert(scope != 0);

    ecs_set_scope(world, scope);
    test_int(ecs_get_scope(world), scope);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, scope));

    ecs_fini(world);
}

void Entity_init_id_name_w_scope() {
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

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_path_w_scope() {
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

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child.grandchild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_fullpath_w_scope() {
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

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child.grandchild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_fullpath_w_scope_existing() {
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

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child.grandchild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_name_1_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = {TagA}
    });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, TagA));
    test_str(ecs_get_name(world, e), "foo");

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_name_2_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = {TagA, TagB}
    });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, TagA));
    test_str(ecs_get_name(world, e), "foo");

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_id_name_2_comp_w_scope() {
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
        .add = {TagA, TagB}
    });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, TagA));
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_id_add_1_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .add = {TagA}
    });
    test_assert(r != 0);
    test_assert(e == r);
    test_assert(ecs_has_id(world, e, TagA));

    ecs_fini(world);
}

void Entity_id_add_2_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .id = e,
        .add = {TagA, TagB}
    });
    test_assert(r != 0);
    test_assert(e == r);
    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_id_path_w_sep() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent::child",
        .sep = "::"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_fullpath(world, e);
    test_assert(path != NULL);
    test_str(path, "parent.child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_find_id_name() {
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

void Entity_find_w_existing_id_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = ecs_new_id(world);

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

void Entity_find_id_name_w_scope() {
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

    char *path = ecs_get_fullpath(world, e);
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

void Entity_find_id_path() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    char *path = ecs_get_fullpath(world, e);
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

void Entity_find_id_path_w_scope() {
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

    char *path = ecs_get_fullpath(world, e);
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

void Entity_find_id_name_match() {
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

void Entity_find_id_name_match_w_scope() {
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

    char *path = ecs_get_fullpath(world, e);
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

void Entity_find_id_path_match() {
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

void Entity_find_id_path_match_w_scope() {
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

void Entity_find_id_name_mismatch() {
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

void Entity_find_id_name_mismatch_w_scope() {
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

void Entity_find_id_path_mismatch() {
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

void Entity_find_id_path_mismatch_w_scope() {
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

void Entity_find_id_add_1_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = {TagA}
    });
    test_assert(r == e);
    test_assert(ecs_has_id(world, e, TagA));

    ecs_fini(world);
}

void Entity_find_id_add_2_comp() {
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
        .add = {TagA, TagB}
    });
    test_assert(r == e);
    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_w_scope_name() {
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

    char *path = ecs_get_fullpath(world, child);
    test_assert(path != NULL);
    test_str(path, "parent.foo.foo");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_init_w_core_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Prefab"
    });

    test_assert(e != 0);
    test_assert(e != EcsPrefab);

    ecs_fini(world);
}

void Entity_init_w_with() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void Entity_init_w_with_w_name() {
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

void Entity_init_w_with_w_scope() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new_id(world);

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

void Entity_init_w_with_w_name_scope() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new_id(world);

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

void Entity_is_valid() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    test_bool(ecs_is_valid(world, e), true);

    ecs_fini(world);
}

void Entity_is_recycled_valid() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    test_bool(ecs_is_valid(world, e), true);

    ecs_delete(world, e);
    test_bool(ecs_is_valid(world, e), false);

    e = ecs_new_id(world);
    test_assert(e != 0);
    test_assert((uint32_t)e != e);
    test_bool(ecs_is_valid(world, e), true);

    ecs_fini(world);
}

void Entity_is_0_valid() {
    ecs_world_t *world = ecs_mini();

    test_bool(ecs_is_valid(world, 0), false);

    ecs_fini(world);
}

void Entity_is_junk_valid() {
    ecs_world_t *world = ecs_mini();

    test_bool(ecs_is_valid(world, 1000), true);
    test_bool(ecs_is_valid(world, 0xFFFFFFFF), true);
    test_bool(ecs_is_valid(world, 0x4DCDCDCDCDCD), false);
    
    test_bool(ecs_is_alive(world, 1000), false);
    test_bool(ecs_is_alive(world, 0xFFFFFFFF), false);
    test_bool(ecs_is_alive(world, 0x4DCDCDCDCDCD), false);

    test_bool(ecs_is_valid(world, 0xFFFFFFFFFFFFFFFF), false);
    test_bool(ecs_is_valid(world, 0xFFFFFFFF00000000), false);
    test_bool(ecs_is_valid(world, 0x4DCDCDCDCDCDCD), false);

    ecs_fini(world);
}

void Entity_is_not_alive_valid() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
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

void Entity_is_nonzero_gen_valid() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = ECS_GENERATION_INC(1000);
    test_int(ECS_GENERATION(id), 1);
    test_bool(ecs_is_alive(world, id), false);
    test_bool(ecs_exists(world, id), false);
    test_bool(ecs_is_valid(world, id), false);

    ecs_fini(world);
}

void Entity_init_w_name_deferred() {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_init_w_name_twice_deferred() {
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

void Entity_init_w_nested_name_twice_deferred() {
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

void Entity_init_w_scope_name_twice_deferred() {
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

void Entity_init_w_childof_name_twice_deferred() {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t parent = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    test_assert(parent != 0);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar",
        .add = {ecs_pair(EcsChildOf, parent)}
    });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar",
        .add = {ecs_pair(EcsChildOf, parent)}
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

void Entity_init_w_childof_nested_name_twice_deferred() {
    ecs_world_t *world = ecs_mini();

    ecs_defer_begin(world);

    ecs_entity_t parent = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    test_assert(parent != 0);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar.Hello",
        .add = {ecs_pair(EcsChildOf, parent)}
    });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar.Hello",
        .add = {ecs_pair(EcsChildOf, parent)}
    });
    test_assert(e2 != 0);

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e1), "Hello");

    char *path = ecs_get_fullpath(world, e1);
    test_str(path, "Foo.Bar.Hello");
    ecs_os_free(path);

    test_assert(parent == ecs_lookup(world, "Foo"));
    ecs_entity_t bar = ecs_lookup_child(world, parent, "Bar");
    test_assert(bar != 0);
    test_assert(ecs_has_pair(world, e1, EcsChildOf, bar));

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_init_w_name_staged() {
    ecs_world_t *world = ecs_mini();

    ecs_readonly_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_entity_t e = ecs_entity_init(stage, &(ecs_entity_desc_t){
        .name = "Foo"
    });
    test_assert(e != 0);

    ecs_readonly_end(world);

    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_record_find_for_empty() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_record_t *r = ecs_record_find(world, e);
    test_assert(r != NULL);
    test_assert(r->table == NULL);

    ecs_fini(world);
}

void Entity_record_find() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    ecs_record_t *r = ecs_record_find(world, e);
    test_assert(r != NULL);
    test_assert(r->table != NULL);

    ecs_fini(world);
}

void Entity_record_find_from_stage() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    ecs_readonly_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_record_t *r = ecs_record_find(stage, e);
    test_assert(r != NULL);
    test_assert(r->table != NULL);

    ecs_readonly_end(world);

    ecs_fini(world);
}

void Entity_ensure_zero_gen() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = 1000;
    test_bool(ecs_is_alive(world, id), false);
    test_bool(ecs_is_valid(world, id), true);
    test_bool(ecs_exists(world, id), false);

    ecs_ensure(world, id);
    test_bool(ecs_is_alive(world, id), true);
    test_bool(ecs_is_valid(world, id), true);
    test_bool(ecs_exists(world, id), true);

    ecs_fini(world);
}

void Entity_ensure_nonzero_gen() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t id = ECS_GENERATION_INC(1000);
    test_int(ECS_GENERATION(id), 1);
    test_bool(ecs_is_alive(world, id), false);
    test_bool(ecs_is_valid(world, id), false);
    test_bool(ecs_exists(world, id), false);

    ecs_ensure(world, id);
    test_bool(ecs_is_alive(world, id), true);
    test_bool(ecs_is_valid(world, id), true);
    test_bool(ecs_exists(world, id), true);

    ecs_fini(world);
}

void Entity_ensure_zero_gen_exists() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    ecs_ensure(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(ecs_is_valid(world, e));
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_ensure_nonzero_gen_exists() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    e = ECS_GENERATION_INC(e);
    e = ECS_GENERATION_INC(e); /* one above the current value */
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));

    ecs_ensure(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(ecs_is_valid(world, e));
    test_assert(ecs_is_alive(world, e));

    ecs_fini(world);
}

void Entity_ensure_zero_gen_exists_alive() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));
    ecs_delete(world, e);

    ecs_entity_t e1 = ecs_new_id(world);
    test_assert(e1 != e);
    test_assert(ecs_strip_generation(e1) == e);

    test_expect_abort();
    ecs_ensure(world, e); // not allowed, can't ensure gen 0 if gen 1 is alive
}

void Entity_ensure_nonzero_gen_exists_alive() {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_valid(world, e));
    test_assert(!ecs_is_alive(world, e));
    ecs_delete(world, e);

    ecs_entity_t e1 = ecs_new_id(world);
    test_assert(e1 != e);
    test_assert(ecs_strip_generation(e1) == e);

    e = ECS_GENERATION_INC(e);
    e = ECS_GENERATION_INC(e);

    test_expect_abort();
    ecs_ensure(world, e); // not allowed, can't ensure gen 2 if gen 1 is alive
}

void Entity_set_scope_w_entity_init_from_stage() {
    ecs_world_t *world = ecs_mini();

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_readonly_begin(world);
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

void Entity_entity_init_w_scope_twice() {
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

    char *path = ecs_get_fullpath(world, e1);
    test_str(path, "Parent.Child");
    ecs_os_free(path);

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_twice() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .add = { ecs_childof(parent) }
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .add = { ecs_childof(parent) }
    });

    test_assert( ecs_has_pair(world, e1, EcsChildOf, parent) );
    test_str(ecs_get_name(world, e1), "Child");

    char *path = ecs_get_fullpath(world, e1);
    test_str(path, "Parent.Child");
    ecs_os_free(path);

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_nested_name_twice() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Child",
        .add = { ecs_childof(parent) }
    });
    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo.Child",
        .add = { ecs_childof(parent) }
    });

    ecs_entity_t foo = ecs_lookup_child(world, parent, "Foo");
    test_assert(foo != 0);

    test_assert( ecs_has_pair(world, e1, EcsChildOf, foo) );
    test_str(ecs_get_name(world, e1), "Child");

    char *path = ecs_get_fullpath(world, e1);
    test_str(path, "Parent.Foo.Child");
    ecs_os_free(path);

    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_and_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_set_scope(world, parent_a);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .add = { ecs_childof(parent_b) }
    });
    ecs_set_scope(world, 0);

    test_assert( !ecs_has_pair(world, child, EcsChildOf, parent_a) );
    test_assert( ecs_has_pair(world, child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, child), "Child");

    char *path = ecs_get_fullpath(world, child);
    test_str(path, "ParentB.Child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_and_scope_and_scoped_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_set_scope(world, parent_a);
    ecs_entity_t grand_child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child.GrandChild",
        .add = { ecs_childof(parent_b) }
    });
    ecs_set_scope(world, 0);

    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_a) );
    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, grand_child), "GrandChild");

    ecs_entity_t child = ecs_lookup_child(world, parent_b, "Child");
    test_assert(child != 0);
    test_assert( ecs_has_pair(world, grand_child, EcsChildOf, child));

    char *path = ecs_get_fullpath(world, grand_child);
    test_str(path, "ParentB.Child.GrandChild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_entity_init_w_childof_and_no_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = { ecs_pair(EcsChildOf, parent) }
    });

    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_deferred_entity_init_w_childof_and_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_defer_begin(world);

    ecs_set_scope(world, parent_a);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child",
        .add = { ecs_childof(parent_b) }
    });
    ecs_set_scope(world, 0);

    ecs_defer_end(world);

    test_assert( !ecs_has_pair(world, child, EcsChildOf, parent_a) );
    test_assert( ecs_has_pair(world, child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, child), "Child");

    char *path = ecs_get_fullpath(world, child);
    test_str(path, "ParentB.Child");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_deferred_entity_init_w_childof_and_scope_and_scoped_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_defer_begin(world);

    ecs_set_scope(world, parent_a);
    ecs_entity_t grand_child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Child.GrandChild",
        .add = { ecs_childof(parent_b) }
    });
    ecs_set_scope(world, 0);

    ecs_defer_end(world);

    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_a) );
    test_assert( !ecs_has_pair(world, grand_child, EcsChildOf, parent_b) );
    test_str(ecs_get_name(world, grand_child), "GrandChild");

    ecs_entity_t child = ecs_lookup_child(world, parent_b, "Child");
    test_assert(child != 0);
    test_assert( ecs_has_pair(world, grand_child, EcsChildOf, child));

    char *path = ecs_get_fullpath(world, grand_child);
    test_str(path, "ParentB.Child.GrandChild");
    ecs_os_free(path);

    ecs_fini(world);
}

void Entity_deferred_entity_init_w_childof_and_no_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_id(world);

    ecs_defer_begin(world);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = { ecs_pair(EcsChildOf, parent) }
    });
    test_assert(child != 0);
    test_assert(!ecs_has_pair(world, child, EcsChildOf, parent));
    ecs_defer_end(world);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_entity_init_w_empty_string_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = ""
    });
    
    test_assert(e != 0);
    test_assert(!ecs_has_pair(world, e, ecs_id(EcsIdentifier), EcsName));

    ecs_fini(world);
}

void Entity_entity_init_w_empty_string_symbol() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .symbol = ""
    });
    
    test_assert(e != 0);
    test_assert(!ecs_has_pair(world, e, ecs_id(EcsIdentifier), EcsSymbol));

    ecs_fini(world);
}

void Entity_set_name_w_0_twice() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_set_name(world, 0, "Foo");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Foo");

    test_str(ecs_get_name(world, e1), "Foo");
    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_new_entity_twice() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new_entity(world, "Foo");
    ecs_entity_t e2 = ecs_new_entity(world, "Foo");

    test_str(ecs_get_name(world, e1), "Foo");
    test_assert(e1 == e2);

    ecs_fini(world);
}

void Entity_new_entity_scoped_twice() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new_entity(world, "Foo.Bar");
    ecs_entity_t e2 = ecs_new_entity(world, "Foo.Bar");

    test_str(ecs_get_name(world, e1), "Bar");
    test_assert(e1 == e2);

    ecs_entity_t parent = ecs_lookup(world, "Foo");
    test_assert(parent != 0);
    test_assert(ecs_has_pair(world, e1, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_defer_component_init() {
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

void Entity_defer_component_init_w_symbol() {
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

void Entity_defer_entity_init_w_symbol() {
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

void Entity_staged_set_name_n_stages() {
    ecs_world_t *world = ecs_mini();

    ecs_set_stage_count(world, 2);

    ecs_readonly_begin(world);

    ecs_world_t *s = ecs_get_stage(world, 1);

    ecs_entity_t e = ecs_entity_init(s, &(ecs_entity_desc_t){
        .name = "Foo"
    });

    ecs_readonly_end(world);

    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_staged_set_symbol_n_stages() {
    ecs_world_t *world = ecs_mini();

    ecs_set_stage_count(world, 2);

    ecs_readonly_begin(world);

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

void Entity_entity_init_w_add_childof_no_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = { ecs_childof(parent) }
    });

    test_assert( ecs_has_pair(world, e, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_entity_w_short_notation() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    test_assert(e != 0);
    test_str("Foo", ecs_get_name(world, e));

    ecs_fini(world);
}


void Entity_override_inherited_symbol() {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t Foo = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Foo",
        .symbol = "FooSymbol"
    });
    test_assert(Foo != 0);

    ecs_entity_t Bar = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Bar",
        .symbol = "BarSymbol",
        .add = { ecs_pair(EcsIsA, Foo) }
    });
    test_assert(Bar != 0);

    test_str(ecs_get_name(world, Foo), "Foo");
    test_str(ecs_get_name(world, Bar), "Bar");
    test_str(ecs_get_symbol(world, Foo), "FooSymbol");
    test_str(ecs_get_symbol(world, Bar), "BarSymbol");

    ecs_fini(world);
}

void Entity_use_low_id_for_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    test_assert(ecs_id(Position) < ECS_HI_COMPONENT_ID);

    ecs_fini(world);
}

void Entity_get_depth() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, e2);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, e3);

    test_int(0, ecs_get_depth(world, e1, EcsChildOf));
    test_int(1, ecs_get_depth(world, e2, EcsChildOf));
    test_int(2, ecs_get_depth(world, e3, EcsChildOf));
    test_int(3, ecs_get_depth(world, e4, EcsChildOf));

    ecs_fini(world);
}

void Entity_get_depth_non_acyclic() {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new_id(world);

    test_expect_abort();
    ecs_get_depth(world, e1, Rel);
}

void Entity_get_depth_empty() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new_id(world);
    test_int(0, ecs_get_depth(world, e1, EcsChildOf));

    ecs_fini(world);
}

void Entity_get_depth_2_paths() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new_id(world);
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

void Entity_entity_init_w_empty_sep() {
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

void Entity_entity_init_w_empty_sep_from_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_entity(world, "parent");
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

void Entity_entity_init_w_empty_sep_w_prefix() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_entity(world, "parent");
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

void Entity_set_name_w_same_ptr() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_entity(world, "foo");
    const char *name = ecs_get_name(world, e);
    test_assert(name != NULL);
    test_str(name, "foo");

    ecs_set_name(world, e, name);
    name = ecs_get_name(world, e); 
    test_assert(name != NULL);
    test_str(name, "foo");

    ecs_fini(world);
}

void Entity_set_name_w_overlapping_ptr() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_entity(world, "foo");
    const char *name = ecs_get_name(world, e);
    test_assert(name != NULL);
    test_str(name, "foo");

    ecs_set_name(world, e, &name[1]);
    name = ecs_get_name(world, e); 
    test_assert(name != NULL);
    test_str(name, "oo");

    ecs_fini(world);
}
