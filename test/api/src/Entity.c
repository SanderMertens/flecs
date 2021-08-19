#include <api.h>

void Entity_init_id() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void Entity_init_id_name() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {TagA}
    });
    test_assert(e != 0);
    test_assert(ecs_has(world, e, TagA));

    ecs_fini(world);
}

void Entity_init_id_add_2_comp() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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

void Entity_init_id_name_1_comp() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .add = {TagA}
    });
    test_assert(r != 0);
    test_assert(e == r);
    test_assert(ecs_has_id(world, e, TagA));

    ecs_fini(world);
}

void Entity_id_add_2_comp() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .add = {TagA, TagB}
    });
    test_assert(r != 0);
    test_assert(e == r);
    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Entity_id_remove_1_comp() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    ecs_add_id(world, e, TagA);
    test_assert(ecs_has_id(world, e, TagA));

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .remove = {TagA}
    });
    test_assert(r != 0);
    test_assert(e == r);

    test_assert(!ecs_has_id(world, e, TagA));

    ecs_fini(world);
}

void Entity_id_remove_2_comp() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    ecs_add_id(world, e, TagA);
    ecs_add_id(world, e, TagB);
    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));    

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .remove = {TagA, TagB}
    });
    test_assert(r != 0);
    test_assert(e == r);
    test_assert(!ecs_has_id(world, e, TagA));
    test_assert(!ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_id_path_w_sep() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ecs_entity_t id = ecs_new_id(world);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = id,
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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "foo"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_name_match_w_scope() {
    ecs_world_t *world = ecs_init();

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
        .entity = e,
        .name = "child"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_set_scope(world, 0);

    r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "parent.child"
    });
    test_assert(r != 0);
    test_assert(r == e);     

    ecs_fini(world);
}

void Entity_find_id_path_match() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "parent.child"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_path_match_w_scope() {
    ecs_world_t *world = ecs_init();

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
        .entity = e,
        .name = "child.grandchild"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_set_scope(world, 0);

    r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "parent.child.grandchild"
    });
    test_assert(r != 0);
    test_assert(r == e);

    ecs_fini(world);
}

void Entity_find_id_name_mismatch() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "bar"
    });    

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "bar"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_name_mismatch_w_scope() {
    ecs_world_t *world = ecs_init();

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

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "parent"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_path_mismatch() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.child"
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "child");

    ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "parent.foo"
    }); 

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "parent.foo"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_path_mismatch_w_scope() {
    ecs_world_t *world = ecs_init();

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

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .entity = e,
        .name = "child.foo"
    });
    test_assert(r == 0);

    ecs_fini(world);
}

void Entity_find_id_add_1_comp() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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

void Entity_find_id_remove_1_comp() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = {TagA}
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");
    test_assert(ecs_has_id(world, e, TagA));

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .remove = {TagA}
    });
    test_assert(r == e);
    test_assert(!ecs_has_id(world, e, TagA));

    ecs_fini(world);
}

void Entity_find_id_remove_2_comp() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .add = {TagA, TagB}
    });
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");
    test_assert(ecs_has_id(world, e, TagA));
    test_assert(ecs_has_id(world, e, TagB));

    ecs_entity_t r = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "foo",
        .remove = {TagA, TagB}
    });
    test_assert(r == e);
    test_assert(!ecs_has_id(world, e, TagA));
    test_assert(!ecs_has_id(world, e, TagB));

    ecs_fini(world);
}

void Entity_init_w_scope_name() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Prefab"
    });

    test_assert(e != 0);
    test_assert(e != EcsPrefab);

    ecs_fini(world);
}

void Entity_init_w_with() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){ });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void Entity_init_w_with_w_name() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t parent = ecs_new_id(world);

    ecs_set_with(world, Tag);
    ecs_set_scope(world, parent);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){ });
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));
    test_assert(ecs_has_id(world, e, ecs_pair(EcsChildOf, parent)));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void Entity_init_w_with_w_name_scope() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    test_bool(ecs_is_valid(world, e), true);

    ecs_fini(world);
}

void Entity_is_recycled_valid() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    test_bool(ecs_is_valid(world, 0), false);

    ecs_fini(world);
}

void Entity_is_junk_valid() {
    ecs_world_t *world = ecs_init();

    test_bool(ecs_is_valid(world, 500), true);
    test_bool(ecs_is_valid(world, 0xFFFFFFFF), true);
    test_bool(ecs_is_valid(world, 0x4DCDCDCDCDCD), true);
    
    test_bool(ecs_is_alive(world, 500), false);
    test_bool(ecs_is_alive(world, 0xFFFFFFFF), false);
    test_bool(ecs_is_alive(world, 0x4DCDCDCDCDCD), false);

    test_bool(ecs_is_valid(world, 0xFFFFFFFFFFFFFFFF), false);
    test_bool(ecs_is_valid(world, 0xFFFFFFFF00000000), false);
    test_bool(ecs_is_valid(world, 0x4DCDCDCDCDCDCD), false);

    ecs_fini(world);
}

void Entity_is_not_alive_valid() {
    ecs_world_t *world = ecs_init();

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
