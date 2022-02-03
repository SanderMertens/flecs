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

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
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

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
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
    test_bool(ecs_is_valid(world, 0x4DCDCDCDCDCD), false);
    
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

void Entity_is_nonzero_gen_valid() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t id = ECS_GENERATION_INC(1000);
    test_int(ECS_GENERATION(id), 1);
    test_bool(ecs_is_alive(world, id), false);
    test_bool(ecs_exists(world, id), false);
    test_bool(ecs_is_valid(world, id), false);

    ecs_fini(world);
}

void Entity_init_w_name_deferred() {
    ecs_world_t *world = ecs_init();

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t) {
        .name = "Foo"
    });

    test_assert(e != 0);
    test_str(ecs_get_name(world, e), NULL);

    ecs_defer_end(world);

    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_init_w_name_staged() {
    ecs_world_t *world = ecs_init();

    ecs_staging_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_entity_t e = ecs_entity_init(stage, &(ecs_entity_desc_t) {
        .name = "Foo"
    });

    test_assert(e != 0);
    test_str(ecs_get_name(stage, e), NULL);

    ecs_staging_end(world);

    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Entity_record_find_for_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_record_t *r = ecs_record_find(world, e);
    test_assert(r != NULL);
    test_assert(r->table == NULL);

    ecs_fini(world);
}

void Entity_record_find() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    ecs_record_t *r = ecs_record_find(world, e);
    test_assert(r != NULL);
    test_assert(r->table != NULL);

    ecs_fini(world);
}

void Entity_record_find_from_stage() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);

    ecs_entity_t e = ecs_new(world, TagA);
    test_assert(e != 0);

    ecs_staging_begin(world);

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_record_t *r = ecs_record_find(stage, e);
    test_assert(r != NULL);
    test_assert(r->table != NULL);

    ecs_staging_end(world);

    ecs_fini(world);
}

void Entity_ensure_zero_gen() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

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

    ecs_world_t *world = ecs_init();

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
    
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_staging_begin(world);
    ecs_entity_t parent = ecs_set_name(stage, 0, "Parent");
    ecs_set_scope(stage, parent);
    ecs_entity_t child = ecs_entity_init(stage, &(ecs_entity_desc_t) {
        .name = "Child"
    });
    ecs_set_scope(stage, 0);
    ecs_staging_end(world);

    test_assert(parent != 0);
    test_assert(child != 0);

    test_str( ecs_get_name(world, parent), "Parent");
    test_str( ecs_get_name(world, child), "Child");

    test_assert( ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Entity_entity_init_w_childof_and_scope() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_set_scope(world, parent_a);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t) {
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
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_set_scope(world, parent_a);
    ecs_entity_t grand_child = ecs_entity_init(world, &(ecs_entity_desc_t) {
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

void Entity_deferred_entity_init_w_childof_and_scope() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_defer_begin(world);

    ecs_set_scope(world, parent_a);
    ecs_entity_t child = ecs_entity_init(world, &(ecs_entity_desc_t) {
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
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_a = ecs_set_name(world, 0, "ParentA");
    ecs_entity_t parent_b = ecs_set_name(world, 0, "ParentB");

    ecs_defer_begin(world);

    ecs_set_scope(world, parent_a);
    ecs_entity_t grand_child = ecs_entity_init(world, &(ecs_entity_desc_t) {
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


void Entity_deferred_entity_init_w_name_twice() {
    // ecs_world_t *world = ecs_init();

    // ecs_defer_begin(world);
    // ecs_entity_t child_1 = ecs_entity_init(world, &(ecs_entity_desc_t) {
    //     .name = "Child"
    // });
    // ecs_entity_t child_2 = ecs_entity_init(world, &(ecs_entity_desc_t) {
    //     .name = "Child"
    // });
    // ecs_defer_end(world);

    // test_str(ecs_get_name(world, child_1), "Child");
    // test_assert(child_1 == child_2);

    // ecs_fini(world);
}
