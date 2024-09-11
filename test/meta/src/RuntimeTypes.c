#include <meta.h>
#include <stdint.h>

#define UINT32_PATTERN(x) \
    (((uint32_t) (x)) + ((uint32_t) (x) << 8) + ((uint32_t) (x) << 16) + \
     ((uint32_t) (x) << 24))

bool is_memory_filled_with(const void *ptr, size_t size, unsigned char byte) {
    const unsigned char *byte_ptr = (const unsigned char *) ptr;

    for (size_t i = 0; i < size; i++) {
        if (byte_ptr[i] != byte) {
            return false;
        }
    }

    return true;
}

/* Test hooks for runtime trivial structs
   These structs have no special copy/move/dtor logic and are zero-initialized
   Trivial structs should get flecs default constructor and nothing else */
void RuntimeTypes_trivial_struct(void) {
    ecs_world_t *world = ecs_init();

    /* Create a component to use as subtype in the trivial struct
       Create a new component: */
    ecs_entity_t subtype = ecs_entity(world, {.name = "Some Subtype"});

    /* Configure as a struct: */
    ecs_struct(world, {.entity = subtype,
                       .members = {{.name = "x", .type = ecs_id(ecs_i32_t)},
                                   {.name = "r", .type = ecs_id(ecs_f32_t)}}});

    /* Create a new component: */
    ecs_entity_t trivial_struct = ecs_entity(world, {.name = "Trivial Struct"});

    ecs_struct(world, {.entity = trivial_struct,
                       .members = {{.name = "a", .type = ecs_id(ecs_i32_t)},
                                   {.name = "b", .type = ecs_id(ecs_f32_t)},
                                   {.name = "s", .type = subtype}}});

    const ecs_type_info_t *ti = ecs_get_type_info(world, trivial_struct);

    test_assert(ti->hooks.copy == NULL);
    test_assert(ti->hooks.move == NULL);
    test_assert(ti->hooks.dtor == NULL);

    /* Check if the ctor initialized everything to 0.
       Do this plenty of times to make sure we don't get a zero-initialized
       struct out of sheer luck: */
    for (int i = 0; i < 100; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_add_id(world, e, trivial_struct);
        const void *data = ecs_get_id(world, e, trivial_struct);
        test_assert(is_memory_filled_with(data, ti->size, 0));
        if (i % 3 == 0)
            ecs_delete(world, e);
    }
    ecs_fini(world);
}

static uint8_t ctor_pattern = 0xAF;
static int ctor_calls = 0;
static uintptr_t ctor_ptr = 0;
static int32_t ctor_count = 0;
static ecs_entity_t ctor_component = 0;
static void test_ctor(void *ptr, int32_t count,
                      const ecs_type_info_t *type_info) {
    ctor_calls++;
    ctor_ptr = (uintptr_t) ptr;
    ctor_count = count;
    ctor_component = type_info->component;
    ecs_os_memset(ptr, ctor_pattern, count * type_info->size);
}

void reset_test_ctor() {
    ctor_calls = 0;
    ctor_ptr = 0;
    ctor_count = 0;
    ctor_component = 0;
}

static int dtor_calls = 0;
static uintptr_t dtor_ptr = 0;
static int32_t dtor_count = 0;
static ecs_entity_t dtor_component = 0;
static void test_dtor(void *ptr, int32_t count,
                      const ecs_type_info_t *type_info) {
    dtor_calls++;
    dtor_ptr = (uintptr_t) ptr;
    dtor_count = count;
    dtor_component = type_info->component;
    ecs_os_memset(ptr, 0xDE, count * type_info->size);
}

void reset_test_dtor() {
    dtor_calls = 0;
    dtor_ptr = 0;
    dtor_count = 0;
    dtor_component = 0;
}

static int move_calls = 0;
static uintptr_t move_dst_ptr = 0;
static uintptr_t move_src_ptr = 0;
static int32_t move_count = 0;
static ecs_entity_t move_component = 0;
static void test_move(void *dst_ptr, void *src_ptr, int32_t count,
                      const ecs_type_info_t *type_info) {
    move_calls++;
    move_dst_ptr = (uintptr_t) dst_ptr;
    move_src_ptr = (uintptr_t) src_ptr;
    move_count = count;
    move_component = type_info->component;
    ecs_os_memcpy(dst_ptr, src_ptr, count * type_info->size);
    ecs_os_memset(src_ptr, 0xBC, count * type_info->size);
}

void reset_test_move() {
    move_calls = 0;
    move_dst_ptr = 0;
    move_src_ptr = 0;
    move_count = 0;
    move_component = 0;
}

static int copy_calls = 0;
static uintptr_t copy_dst_ptr = 0;
static uintptr_t copy_src_ptr = 0;
static int32_t copy_count = 0;
static ecs_entity_t copy_component = 0;
static void test_copy(void *dst_ptr, const void *src_ptr, int32_t count,
                      const ecs_type_info_t *type_info) {
    copy_calls++;
    copy_dst_ptr = (uintptr_t) dst_ptr;
    copy_src_ptr = (uintptr_t) src_ptr;
    copy_count = count;
    copy_component = type_info->component;
    ecs_os_memcpy(dst_ptr, src_ptr, count * type_info->size);
}

void reset_test_copy() {
    copy_calls = 0;
    copy_dst_ptr = 0;
    copy_src_ptr = 0;
    copy_count = 0;
    copy_component = 0;
}

/* Define a nested struct that will have one or more hooks depending on the test
 */
typedef struct NestedStruct {
    uint32_t a;
    uint32_t b;
} NestedStruct;
static ecs_entity_t nested_struct;

const ecs_type_info_t *define_nested_struct(ecs_world_t *world) {
    nested_struct = ecs_struct(
        world, {.entity = ecs_entity(world, {.name = "NestedStruct"}),
                .members = {{.name = "a", .type = ecs_id(ecs_u32_t)},
                            {.name = "b", .type = ecs_id(ecs_u32_t)}}});
    return ecs_get_type_info(world, nested_struct);
}

/* Define a test struct that will have automatically generated hooks. */
typedef struct TestStruct {
    uint32_t x;
    NestedStruct s1;
    uint32_t y;
    NestedStruct s2;
} TestStruct;
static ecs_entity_t test_struct;

const ecs_type_info_t *define_test_struct(ecs_world_t *world) {
    test_struct =
        ecs_struct(world, {.entity = ecs_entity(world, {.name = "TestStruct"}),
                           .members = {{.name = "x", .type = ecs_id(ecs_u32_t)},
                                       {.name = "s1", .type = nested_struct},
                                       {.name = "y", .type = ecs_id(ecs_u32_t)},
                                       {.name = "s2", .type = nested_struct}}});
    return ecs_get_type_info(world, test_struct);
}

/* Tests that a constructor is generated for a struct if at least a member has
   itself a constructor Also tests if the generated constructor works. */
void RuntimeTypes_ctor(void) {
    reset_test_ctor();
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.ctor = test_ctor; /* only define a constructor for "NestedStruct" */
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
       A constructor should be automatically generated for TestStruct to invoke
       NestedStruct's specific constructor: */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should only have a constructor: */
    test_assert(test_struct_ti->hooks.ctor != NULL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.move == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    /* Now instantiate TestStruct and see the constructor for NestedStruct is
       called twice, since TestStruct contains two

       NestedStructs: */
    ecs_entity_t e = ecs_new(world);

    test_int(0, ctor_calls);
    ecs_add_id(world, e, test_struct);
    test_int(
        2, ctor_calls); /* called twice because there are two "nested_struct" */

    const TestStruct *ss = (TestStruct *) ecs_get_id(world, e, test_struct);

    uint32_t test_pattern = UINT32_PATTERN(ctor_pattern);
    test_int(0, ss->x); /* trivial field was zero-initialized */
    test_int(0, ss->y);
    test_int(test_pattern, ss->s1.a); /* these should've been filled by
                                         test_ctor with a pattern. */
    test_int(test_pattern, ss->s1.b);
    test_int(test_pattern, ss->s2.a);
    test_int(test_pattern, ss->s2.b);

    ecs_fini(world);
}

/* Tests that a destructor is generated for a struct if at least a member has
   itself a destructor Also tests if the generated destructor works. */
void RuntimeTypes_dtor(void) {
    reset_test_dtor();
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.dtor = test_dtor; /* only set a destructor */

    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
       A destructor should be automatically generated for TestStruct to invoke
       NestedStruct's specific destructor: */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have a default constructor and the generated
     * destructor: */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.dtor != NULL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.move == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    /* Now instantiate TestStruct to test its destructor. */
    ecs_entity_t e = ecs_new(world);

    test_int(0, dtor_calls);
    ecs_add_id(world, e, test_struct);
    test_int(0, dtor_calls); /* Destructor not called yet. */

    const void *data = ecs_get_id(world, e, test_struct);
    is_memory_filled_with(
        data, test_struct_ti->size,
        0); /* should be zero-initialized, since there are no special ctors. */

    ecs_remove_id(world, e, test_struct);
    test_int(2, dtor_calls); /* should be called twice, since there are two
                                "struct_with_dtor" */

    ecs_fini(world);
}

/* Tests that a move hook is generated for a struct if at least a member has
   itself a move hook Also tests if the generated move hook works. */
void RuntimeTypes_move(void) {
    reset_test_move();
    ecs_world_t *world = ecs_init();
    define_nested_struct(world);

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);
    ecs_type_hooks_t hooks = nested_struct_ti->hooks;

    hooks.move = test_move; /* Only define a move hook. */
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
       A move hook should be automatically generated for TestStruct to invoke
       NestedStruct's specific move hook: */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have a default constructor and the generated move hook:
     */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.move != NULL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    ecs_entity_t e = ecs_new(world);

    test_int(0, move_calls);
    void *ptr = ecs_ensure_id(world, e, test_struct);
    test_int(0, move_calls);

    /* use a cursor to set the x and s1.a field to specific numbers */
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, test_struct, ptr);
    ecs_meta_push(&cur);
    ecs_meta_member(&cur, "x");
    ecs_meta_set_int(&cur, 82);
    ecs_meta_member(&cur, "s1");
    ecs_meta_push(&cur);
    ecs_meta_member(&cur, "a");
    ecs_meta_set_int(&cur, 79);

    /* now trigger a move by adding another component to e, forcing an archetype
     * change: */
    ecs_add_id(world, e, ecs_new(world));
    test_int(2, move_calls); /* should be called twice, since there are two
                                structs with a move hook. */

    const TestStruct *ss = (TestStruct *) ecs_get_id(world, e, test_struct);

    test_assert((void *) ss !=
                ptr); /* should be a different memory location. */

    test_int(82, ss->x); /* trivial move should have copied this value. */
    test_int(0, ss->y);
    test_int(79, ss->s1.a); /* these should contain the specified value, done by
                               the move hook. */
    test_int(0, ss->s1.b);  /* other fields should have been 0-initialized. */
    test_int(0, ss->s2.a);
    test_int(0, ss->s2.b);

    ecs_fini(world);
}

/* Tests that a copy hook is generated for a struct if at least a member has
   itself a copy hook Also tests if the generated copy hook works. */
void RuntimeTypes_copy(void) {
    reset_test_copy();
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    define_nested_struct(world);
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);
    ecs_type_hooks_t hooks = nested_struct_ti->hooks;

    hooks.copy = test_copy; /* only set a copy hook */
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
       A copy hook should be automatically generated for TestStruct to invoke
       NestedStruct's specific copy hook: */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have a default constructor and the generated copy hook:
     */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.copy != NULL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.move == NULL);

    /* create a prefab entity: */
    ecs_entity_t prefab = ecs_new(world);
    ecs_add_id(world, prefab, EcsPrefab);

    test_int(0, copy_calls);
    void *original_ptr = ecs_ensure_id(world, prefab, test_struct);
    test_int(0, copy_calls);

    /* use a cursor to set the x and s1.a field to specific numbers */
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, test_struct, original_ptr);
    ecs_meta_push(&cur);
    ecs_meta_member(&cur, "x");
    ecs_meta_set_int(&cur, 82);
    ecs_meta_member(&cur, "s1");
    ecs_meta_push(&cur);
    ecs_meta_member(&cur, "a");
    ecs_meta_set_int(&cur, 79);

    /* now trigger a copy by instantiating this prefab: */
    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, prefab);
    test_int(2, copy_calls); /* should be called twice, since there are two
                                structs with a copy hook. */

    const void *copied_ptr = ecs_get_id(world, instance, test_struct);

    test_assert((void *) copied_ptr !=
                original_ptr); /* should be a different memory location. */

    test_assert(0 ==
                ecs_os_memcmp(original_ptr, copied_ptr, test_struct_ti->size));

    ecs_fini(world);
}

/* For the following tests, we model a "ResourceHandle" that must allocate an id
 * from a pool of resource ids. When constructing a handle, it must get a unique
 * id. When destroying a handle, it must return the id. When copying a handle,
 * it must copy the payload value and get a new unique id for the handle. When
 * moving the handle, it should keep the same id and payload value. Two Resource
 * Handles are considered "equal" if both have a valid id (id>0) and their
 * payload value is the same
 */
typedef struct ResourceHandle {
    int id;
    uint32_t value;
} ResourceHandle;

/* This vector will hold all available ids. */
ecs_vec_t resource_ids;

/* Initializes the available "resource ids" */
void initialize_resource_ids(int num_resources) {
    ecs_vec_init_t(NULL, &resource_ids, int, num_resources);
    ecs_vec_set_count_t(NULL, &resource_ids, int, num_resources);
    for (int i = 0; i < num_resources; i++) {
        *ecs_vec_get_t(&resource_ids, int, i) = num_resources - i;
    }
}

void free_resource_ids() { ecs_vec_fini_t(NULL, &resource_ids, int); }

/* Gets a resource id from the vector. It will return 0 if no more resources
 * available. */
int get_resource_id() {
    size_t idcount = ecs_vec_count(&resource_ids);
    if (idcount == 0)
        return 0;
    int id = *ecs_vec_get_t(&resource_ids, int, idcount - 1);
    ecs_vec_set_count_t(NULL, &resource_ids, int, idcount - 1);
    return id;
}

/* Returns a resource for reuse. */
void return_resource_id(int id) {
    if (id == 0)
        return;
    *(ecs_vec_append_t(NULL, &resource_ids, int)) = id;
}

/* Returns true if a specific resource id is in the pool. */
bool resource_id_available(int id) {
    for (int i = 0; i < ecs_vec_count(&resource_ids); i++) {
        if (id == *ecs_vec_get_t(&resource_ids, int, i))
            return true;
    }
    return false;
}

/* Returns the numer of currently available resources */
int available_resource_count() { return ecs_vec_count(&resource_ids); }

/* Define a ResourceHandle constructor that sets the payload value to 0 and
 * obtains a unique resource id: */
static void ResourceHandle_ctor(void *ptr, int32_t count,
                                const ecs_type_info_t *type_info) {
    for (int i = 0; i < count; i++) {
        ResourceHandle *r =
            (ResourceHandle *) ECS_ELEM_T(ptr, ResourceHandle, i);
        r->id = get_resource_id();
        test_assert(r->id);
        r->value = 0;
    }
}

/* Define a ResourceHandle destructor that returns the borrowed resource id: */
static void ResourceHandle_dtor(void *ptr, int32_t count,
                                const ecs_type_info_t *type_info) {
    for (int i = 0; i < count; i++) {
        ResourceHandle *r =
            (ResourceHandle *) ECS_ELEM_T(ptr, ResourceHandle, i);
        return_resource_id(r->id);
    }
}

/* Define a ResourceHandle move operation that transfers the resource id: */
static void ResourceHandle_move(void *dst_ptr, void *src_ptr, int32_t count,
                                const ecs_type_info_t *type_info) {
    for (int i = 0; i < count; i++) {
        ResourceHandle *src_r =
            (ResourceHandle *) ECS_ELEM_T(src_ptr, ResourceHandle, i);
        ResourceHandle *dst_r =
            (ResourceHandle *) ECS_ELEM_T(dst_ptr, ResourceHandle, i);
        return_resource_id(dst_r->id);
        *dst_r = *src_r;
        src_r->id = 0;
    }
}

/* Define a ResourceHandle copy operation that copies the payload value but
 * obtains its own resource id: */
static void ResourceHandle_copy(void *dst_ptr, const void *src_ptr,
                                int32_t count,
                                const ecs_type_info_t *type_info) {
    for (int i = 0; i < count; i++) {
        const ResourceHandle *src_r =
            (const ResourceHandle *) ECS_ELEM_T(src_ptr, ResourceHandle, i);
        ResourceHandle *dst_r =
            (ResourceHandle *) ECS_ELEM_T(dst_ptr, ResourceHandle, i);
        return_resource_id(dst_r->id);
        *dst_r = *src_r;
        dst_r->id = get_resource_id();
        test_assert(dst_r->id);
    }
}

/* Defines a struct in Flecs to model the ResourceHandle struct
   For different tests, it can set specific hooks or not. */
ecs_entity_t resource_handle;
void define_resource_handle(ecs_world_t *world, bool ctor, bool dtor, bool move,
                            bool copy) {
    resource_handle = ecs_struct(
        world, {.entity = ecs_entity(world, {.name = "ResourceHandle"}),
                .members = {{.name = "id", .type = ecs_id(ecs_i32_t)},
                            {.name = "value", .type = ecs_id(ecs_u32_t)}}});

    const ecs_type_info_t *ti = ecs_get_type_info(world, resource_handle);
    ecs_type_hooks_t hooks = ti->hooks;

    if (ctor)
        hooks.ctor = ResourceHandle_ctor;
    if (dtor)
        hooks.dtor = ResourceHandle_dtor;
    if (move)
        hooks.move = ResourceHandle_move;
    if (copy)
        hooks.copy = ResourceHandle_copy;

    ecs_set_hooks_id(world, resource_handle, &hooks);
}

/* Test hooks for runtime array of trivial structs
   These structs have no special copy/move/dtor logic and are zero-initialized
   Trivial structs should get flecs default constructor and nothing else
   An array of trivial struct should also only get flecs default constructor and
   nothing else. */
void RuntimeTypes_trivial_array(void) {
    ecs_world_t *world = ecs_init();

    /* Create a new component: */
    ecs_entity_t trivial_struct = ecs_struct(
        world, {.entity = ecs_entity(world, {.name = "Trivial Struct"}),
                .members = {{.name = "a", .type = ecs_id(ecs_i32_t)},
                            {.name = "b", .type = ecs_id(ecs_f32_t)}}});

    /* create an array of 3 trivial structs: */
    ecs_array_desc_t desc = {.entity = 0, .type = trivial_struct, .count = 3};
    ecs_entity_t arr_of_struct = ecs_array_init(world, &desc);

    const ecs_type_info_t *ti = ecs_get_type_info(world, arr_of_struct);

    test_assert(ti->hooks.ctor !=
                NULL); /* should have got flecs default constructor.

no other hooks should've been set: */
    test_assert(ti->hooks.copy == NULL);
    test_assert(ti->hooks.move == NULL);
    test_assert(ti->hooks.dtor == NULL);

    /* Check if the ctor initialized everything to 0.
       Do this a couple of times to make sure we don't get a zero-initialized
       struct out of sheer luck: */
    for (int i = 0; i < 30; i++) {
        ecs_entity_t e = ecs_new(world);
        ecs_add_id(world, e, arr_of_struct);
        const void *data = ecs_get_id(world, e, arr_of_struct);
        test_assert(is_memory_filled_with(data, ti->size, 0));
        if (i % 3 == 0)
            ecs_delete(world, e);
    }
    ecs_fini(world);
}

/* Tests that if on the array's underlying type only a ctor is defined, only a
   ctor is defined for the array itself Tests that the specified constructor is
   called for each array element. */
void RuntimeTypes_array_ctor(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    initialize_resource_ids(10);
    test_int(10, available_resource_count());

    /* Define the Resource with only a constructor hook. */
    define_resource_handle(world, true, false, false, false);

    /* create an array of 3 ResourceHandles */
    ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
    ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

    const ecs_type_hooks_t *hooks =
        &ecs_get_type_info(world, arr_of_resources)->hooks;

    test_assert(hooks->ctor != NULL); /* a ctor should've been generated

       no other hooks should have been generated, since the depending type,
       "resource_handle" only has a ctor hook: */
    test_assert(hooks->dtor == NULL);
    test_assert(hooks->move == NULL);
    test_assert(hooks->copy == NULL);

    /* Test that the set ctor hook is indeed working: */
    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, arr_of_resources);
    test_int(7,
             available_resource_count()); /* 3 resources were used out of 10,
                                             since the array has a size of 3 */

    ecs_fini(world);
    free_resource_ids();
}

/* Tests that if on the array's underlying type only a dtor is defined, only a
   dtor is defined for the array itself Tests that the specified destructor is
   called for each array element. */
void RuntimeTypes_array_dtor(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    initialize_resource_ids(10);

    /* Define the Resource with only a destructor hook. */
    define_resource_handle(world, false, true, false, false);

    /* create an array of ResourceHandle */
    ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
    ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

    const ecs_type_hooks_t *hooks =
        &ecs_get_type_info(world, arr_of_resources)->hooks;

    test_assert(hooks->ctor != NULL); /* ctor should exist and set to flecs'
                                         default memset-to-zero constructor */
    test_assert(hooks->dtor != NULL); /* should be set to call the resource
       handle dtor for each item in the array.

       no other hooks should have been generated, since the depending type,
       "resource_handle" only has a dtor hook: */
    test_assert(hooks->move == NULL);
    test_assert(hooks->copy == NULL);

    /* Test that the set dtor hook is indeed working. */
    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, arr_of_resources);
    test_int(10, available_resource_count()); /* since no special ctor is
       defined, it won't pick up resources from the pool, and it stays the same.

       The default constructor should've set the whole array to zero: */
    ResourceHandle *handles =
        (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);
    test_assert(is_memory_filled_with(handles, sizeof(ResourceHandle) * 3, 0));

    /* Make up some resource ids so they're artificially returned to the pool by
     * the destructor: */
    handles[0].id = 100;
    handles[1].id = 200;
    handles[2].id = 300;

    ecs_delete(world, e); /* destroy the entity, which will in turn destroy the
       component, invoking the destructors

       check that made-up resources are now in the pool: */
    test_int(13, available_resource_count());
    test_assert(resource_id_available(100));
    test_assert(resource_id_available(200));
    test_assert(resource_id_available(300));

    ecs_fini(world);
    free_resource_ids();
}

static bool resource_handle_compare(ecs_world_t *world, void *a, void *b,
                                    int32_t count, const ecs_type_info_t *ti);

/* Tests that if on the array's underlying type only a move hook is defined,
   only a move hook is defined for the array itself. Tests that the specified
   move hook is called for each array element. */
void RuntimeTypes_array_move(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    initialize_resource_ids(10);

    /* Define the Resource with only a move hook. */
    define_resource_handle(world, false, false, true, false);

    /* create an array of ResourceHandle */
    ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
    ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

    const ecs_type_hooks_t *hooks =
        &ecs_get_type_info(world, arr_of_resources)->hooks;

    test_assert(hooks->ctor != NULL); /* ctor should exist and set to flecs'
                                         default memset-to-zero constructor */
    test_assert(hooks->move !=
                NULL); /* should be set to call the resource handle move hook
       for each item in the array.

       no other hooks should have been generated, since the underlying type,
       "resource_handle" only has a move hook: */
    test_assert(hooks->dtor == NULL);
    test_assert(hooks->copy == NULL);

    /* Test that the set move hook is indeed working. */
    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, arr_of_resources);
    test_int(10, available_resource_count()); /* since no special ctor is
       defined, it won't pick up resources from the pool, and it stays the same.

       The default constructor should've set the whole array to zero: */
    ResourceHandle *handles =
        (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);
    test_assert(is_memory_filled_with(handles, sizeof(ResourceHandle) * 3, 0));

    /* Make up some resource ids so we can test they moved: */
    handles[0] = (ResourceHandle){.id = 100, .value = 111};
    handles[1] = (ResourceHandle){.id = 200, .value = 222};
    handles[2] = (ResourceHandle){.id = 300, .value = 333};

    /* add another entity to the test entity to trigger an archetype change and
     * thus a move: */
    ecs_add_id(world, e, ecs_new(world));
    handles = (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);

    const ecs_type_info_t *rh_ti = ecs_get_type_info(world, resource_handle);
    /* we should retrieve the same values: */
    test_assert(resource_handle_compare(
        world, &(ResourceHandle){.id = 100, .value = 111}, &handles[0], 1,
        rh_ti));
    test_assert(resource_handle_compare(
        world, &(ResourceHandle){.id = 200, .value = 222}, &handles[1], 1,
        rh_ti));
    test_assert(resource_handle_compare(
        world, &(ResourceHandle){.id = 300, .value = 333}, &handles[2], 1,
        rh_ti));

    test_int(10, available_resource_count()); /* pool stays the same */

    ecs_delete(world, e);

    /* since no special destructor is defined, the resources 100,200,300 are not
     * returned to the pool: */
    test_int(10, available_resource_count());
    test_assert(!resource_id_available(100));
    test_assert(!resource_id_available(200));
    test_assert(!resource_id_available(300));

    ecs_fini(world);
    free_resource_ids();
}

/* Tests that if on the array's underlying type only a copy hook is defined,
   only a copy hook is defined for the array itself. Tests that the specified
   copy hook is called for each array element. */
void RuntimeTypes_array_copy(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    initialize_resource_ids(10);
    test_int(10, available_resource_count());

    /* Define the Resource with only a move hook. */
    define_resource_handle(world, false, false, false, true);

    /* create an array of ResourceHandle */
    ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
    ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

    const ecs_type_hooks_t *hooks =
        &ecs_get_type_info(world, arr_of_resources)->hooks;

    test_assert(hooks->ctor != NULL); /* ctor should exist and set to flecs'
                                         default memset-to-zero constructor */
    test_assert(hooks->copy !=
                NULL); /* should be set to call the resource handle copy hook
       for each item in the array.

       no other hooks should have been generated, since the underlying type,
       "resource_handle" only has a move hook: */
    test_assert(hooks->dtor == NULL);
    test_assert(hooks->move == NULL);

    /* Test that the set copy hook is indeed working. */
    ecs_entity_t prefab = ecs_new(world);
    ecs_add_id(world, prefab, arr_of_resources);
    test_int(10, available_resource_count()); /* since no special ctor is
       defined, it won't pick up resources from the pool, and it stays the same.

       The default constructor should've set the whole array to zero: */
    ResourceHandle *prefab_handles =
        (ResourceHandle *) ecs_get_mut_id(world, prefab, arr_of_resources);
    test_assert(
        is_memory_filled_with(prefab_handles, sizeof(ResourceHandle) * 3, 0));

    /* Make up some resource ids so we can test they are copied: */
    prefab_handles[0] = (ResourceHandle){.id = 100, .value = 111};
    prefab_handles[1] = (ResourceHandle){.id = 200, .value = 222};
    prefab_handles[2] = (ResourceHandle){.id = 300, .value = 333};

    /* Instantiate a prefab to trigger a copy: */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
    test_int(7, available_resource_count()); /* should've taken 3 resources from
                                                the pool */

    ResourceHandle *handles =
        (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);

    const ecs_type_info_t *rh_ti = ecs_get_type_info(world, resource_handle);
    test_assert(resource_handle_compare(world, &prefab_handles[0], &handles[0],
                                        1, rh_ti));
    test_assert(resource_handle_compare(world, &prefab_handles[1], &handles[1],
                                        1, rh_ti));
    test_assert(resource_handle_compare(world, &prefab_handles[2], &handles[2],
                                        1, rh_ti));

    ecs_delete(world, e);
    test_int(7, available_resource_count()); /* resources not returned since we
                                                did not hook a destructor */

    ecs_fini(world);
    free_resource_ids();
}

/* Test vector types */
void RuntimeTypes_vector_lifecycle(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    int initial_resources = 10;
    initialize_resource_ids(initial_resources);
    test_int(10, available_resource_count());

    define_resource_handle(world, true, true, true, true);

    /* create a vector of ResourceHandles */
    ecs_vector_desc_t desc = {.entity = 0, .type = resource_handle};
    ecs_entity_t vector_of_resources = ecs_vector_init(world, &desc);

    const ecs_type_hooks_t *hooks =
        &ecs_get_type_info(world, vector_of_resources)->hooks;

    /* a vector type requires al 4 hooks: */
    test_assert(hooks->ctor != NULL);
    test_assert(hooks->dtor != NULL);
    test_assert(hooks->move != NULL);
    test_assert(hooks->copy != NULL);

    /* Test the vector type is working: */
    ecs_entity_t prefab = ecs_new(world);
    ecs_add_id(world, prefab, EcsPrefab);
    ecs_vec_t *v =
        (ecs_vec_t *) ecs_ensure_id(world, prefab, vector_of_resources);
    test_assert(v != NULL);
    test_assert(v->array == NULL);
    test_assert(v->count == 0);
    test_int(initial_resources,
             available_resource_count()); /* still have 10 resources since
vector is empty

manually add some items to the vector. These must be constructed by hand: */
    ecs_vec_set_count_t(NULL, v, ResourceHandle, 3);
    ResourceHandle_ctor(ecs_vec_first(v), 3, NULL);
    test_int(initial_resources - 3, available_resource_count()); /* Used up 3
       resources that are now in the vector

       test vector copy assign by instantiating the prefab */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
    test_int(initial_resources - 6, available_resource_count()); /* Used up 3
       more resources after copying the vector

       test vector move assign by forcing a move via archetype change: */
    ecs_add_id(world, e, ecs_new(world));
    test_int(initial_resources - 6,
             available_resource_count()); /* No more resources consumed */

    ecs_delete(world, e);
    test_int(initial_resources - 3,
             available_resource_count()); /* Frees 3 resources held by the
                                             instance */

    ecs_delete(world, prefab);
    test_int(initial_resources, available_resource_count()); /* Frees another 3
       resources held by the prefab

       check if all resources were returned: */
    for (int i = 0; i < initial_resources; i++) {
        test_assert(resource_id_available(i + 1));
    }
    ecs_fini(world);
    free_resource_ids();
}

/* Test vectors of trivial types */
void RuntimeTypes_vector_lifecycle_trivial_type(void) {
    ecs_world_t *world = ecs_init();

    /* create a vector of ints */
    ecs_vector_desc_t desc = {.entity = 0, .type = ecs_id(ecs_i32_t)};
    ecs_entity_t vector_of_ints = ecs_vector_init(world, &desc);

    const ecs_type_hooks_t *hooks =
        &ecs_get_type_info(world, vector_of_ints)->hooks;

    /* a vector type requires al 4 hooks, even for trivial types: */
    test_assert(hooks->ctor != NULL);
    test_assert(hooks->dtor != NULL);
    test_assert(hooks->move != NULL);
    test_assert(hooks->copy != NULL);

    /* Test the vector type is working: */
    ecs_entity_t prefab = ecs_new(world);
    ecs_add_id(world, prefab, EcsPrefab);
    ecs_vec_t *v = (ecs_vec_t *) ecs_ensure_id(world, prefab, vector_of_ints);
    test_assert(v != NULL);
    test_assert(v->array == NULL);
    test_assert(v->count == 0);

    /* manually add some items to the vector. These must be constructed by hand:
     */
    for (int i = 0; i < 3; i++) {
        *ecs_vec_append_t(NULL, v, int) = 79;
    }

    /* test vector copy assign by instantiating the prefab */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);

    /* verify we got a copy: */
    const ecs_vec_t *vcopy =
        (const ecs_vec_t *) ecs_get_id(world, e, vector_of_ints);
    int32_t count = ecs_vec_count(vcopy);
    test_int(ecs_vec_count(v), count);
    test_assert(0 == ecs_os_memcmp(ecs_vec_first(v), ecs_vec_first(vcopy),
                                   sizeof(int32_t) * count));

    /* test vector move assign by forcing a move via archetype change: */
    ecs_add_id(world, e, ecs_new(world));
    const ecs_vec_t *vcopy_moved =
        (const ecs_vec_t *) ecs_get_id(world, e, vector_of_ints);
    count = ecs_vec_count(vcopy_moved);
    test_assert(vcopy != vcopy_moved);
    test_int(ecs_vec_count(v), count);
    test_assert(0 == ecs_os_memcmp(ecs_vec_first(v), ecs_vec_first(vcopy_moved),
                                   sizeof(int32_t) * count));

    ecs_delete(world, e);
    ecs_delete(world, prefab);

    ecs_fini(world);
    free_resource_ids();
}

/* Configure an opaque type that consumes resources */
ecs_entity_t define_ResourceHandle_opaque(ecs_world_t *world) {
    ECS_COMPONENT(world, ResourceHandle);

    ecs_type_hooks_t hooks = *ecs_get_hooks(world, ResourceHandle);
    hooks.ctor = ResourceHandle_ctor;
    hooks.dtor = ResourceHandle_dtor;
    hooks.move = ResourceHandle_move;
    hooks.copy = ResourceHandle_copy;
    ecs_set_hooks_id(world, ecs_id(ResourceHandle), &hooks);

    /* Create struct type that describes the structure of ResourceHandle */
    ecs_entity_t resource_handle_descriptor =
        ecs_struct(world, {.members = {
                               {.name = "value", .type = ecs_id(ecs_u32_t)},
                           }});
    /* Register ResourceHandle as opaque type. */
    ecs_opaque(world, {.entity = ecs_id(ResourceHandle),
                       .type = {.as_type = resource_handle_descriptor}});

    return ecs_id(ResourceHandle);
}

/* Tests that RTT ignores opaque types */
void RuntimeTypes_opaque(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t resource_handle_opaque = define_ResourceHandle_opaque(world);

    /* Now test that hooks defined for the opaque type are intact: */
    ecs_type_hooks_t hooks = *ecs_get_hooks_id(world, resource_handle_opaque);
    test_assert(hooks.ctor == ResourceHandle_ctor);
    test_assert(hooks.dtor == ResourceHandle_dtor);
    test_assert(hooks.move == ResourceHandle_move);
    test_assert(hooks.copy == ResourceHandle_copy);

    ecs_fini(world);
}

/*

RTT universal type tester

The following code runs tests on any run-time type. It has the following
elements:

* Universal comparer that takes two type instances and determines if they
contain the same data, taking into account if these are arrays, structs, vectors
or primitive types.
* Comparer function registry for comparing special types like strings
* Universal initializer that takes a component instance and fills it with test
data, for testing purposes
* Initializer function registry for initializing special types like strings
* The RTT tester itself that runs the same test for each type.
* The test function where all types under test (tuts) are defined.

*/

/* Comparison function pointer interface. Returns true if all `count` elements
 * are equal */
typedef bool (*comparer)(ecs_world_t *world, const void *a, const void *b,
                         int32_t count, const ecs_type_info_t *ti);

/* comprarer map registry allows to register a comparison function for specific
 * types */
ecs_map_t comparer_map; /* map of ecs_entity_t -> comparer */

/* Compares two component instances
   returns true if both instances are the same according to the comparer
   functions registered for each type. if no comparison function is registered,
   it will do a memory comparison */
bool compare(ecs_world_t *world, const void *a, const void *b, int32_t count,
             const ecs_type_info_t *ti) {
    const EcsType *type = ecs_get(world, ti->component, EcsType);

    if (type) {
        if (type->kind == EcsStructType) {
            const EcsStruct *struct_info =
                ecs_get(world, ti->component, EcsStruct);
            ecs_assert(struct_info != NULL, ECS_INTERNAL_ERROR, NULL);
            for (int j = 0; j < count; j++) {
                const void *element_a = ECS_ELEM(a, ti->size, j);
                const void *element_b = ECS_ELEM(b, ti->size, j);
                for (int i = 0; i < ecs_vec_count(&struct_info->members); i++) {
                    ecs_member_t *m =
                        ecs_vec_get_t(&struct_info->members, ecs_member_t, i);
                    const ecs_type_info_t *member_ti =
                        ecs_get_type_info(world, m->type);
                    if (!compare(world, ECS_OFFSET(element_a, m->offset),
                                 ECS_OFFSET(element_b, m->offset), m->count,
                                 member_ti)) {
                        return false;
                    }
                }
            }
            return true;
        } else if (type->kind == EcsArrayType) {
            const EcsArray *array_info =
                ecs_get(world, ti->component, EcsArray);
            ecs_assert(array_info != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_info_t *array_ti =
                ecs_get_type_info(world, array_info->type);
            for (int j = 0; j < count; j++) {
                const void *element_a = ECS_ELEM(a, ti->size, j);
                const void *element_b = ECS_ELEM(b, ti->size, j);
                if (!compare(world, element_a, element_b, array_info->count,
                             array_ti)) {
                    return false;
                }
            }
            return true;
        } else if (type->kind == EcsVectorType) {
            const EcsVector *vector_info =
                ecs_get(world, ti->component, EcsVector);
            ecs_assert(vector_info != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_info_t *vector_ti =
                ecs_get_type_info(world, vector_info->type);
            for (int j = 0; j < count; j++) {
                const void *element_a = ECS_ELEM(a, ti->size, j);
                const void *element_b = ECS_ELEM(b, ti->size, j);
                ecs_vec_t *va = (ecs_vec_t *) element_a;
                ecs_vec_t *vb = (ecs_vec_t *) element_b;
                int32_t vector_count = ecs_vec_count(va);
                if (vector_count != ecs_vec_count(vb)) {
                    return false;
                }
                if (!compare(world, ecs_vec_first(va), ecs_vec_first(vb),
                             vector_count, vector_ti)) {
                    return false;
                }
            }
            return true;
        }
    }

    ecs_map_val_t *comp_ptr = ecs_map_get(&comparer_map, ti->component);
    if (comp_ptr) {
        comparer comp = (comparer) (uintptr_t) (*comp_ptr);
        return comp(world, a, b, count, ti);
    }

    return 0 == ecs_os_memcmp(a, b, ti->size * count);
}

/* Initializer function pointer. */
typedef void (*initializer)(ecs_world_t *world, void *ptr, int32_t count,
                            const ecs_type_info_t *ti);

/* Allows to register an initializer with test data for the given type */
ecs_map_t initializer_map; /* map of ecs_entity_t -> initializer */

/* Initializes the given component instance with test data according to the test
   data initializers registered for each type. If no initializer is registered,
   it leaves what the component's registered constructor already did, which
   could be zeroed memory if no constructor was defined. */
static void initialize(ecs_world_t *world, void *ptr, int32_t count,
                       const ecs_type_info_t *ti) {
    const EcsType *type = ecs_get(world, ti->component, EcsType);

    if (type) {
        if (type->kind == EcsStructType) {
            const EcsStruct *struct_info =
                ecs_get(world, ti->component, EcsStruct);
            ecs_assert(struct_info != NULL, ECS_INTERNAL_ERROR, NULL);
            for (int j = 0; j < count; j++) {
                void *element = ECS_ELEM(ptr, ti->size, j);
                for (int i = 0; i < ecs_vec_count(&struct_info->members); i++) {
                    ecs_member_t *m =
                        ecs_vec_get_t(&struct_info->members, ecs_member_t, i);
                    const ecs_type_info_t *member_ti =
                        ecs_get_type_info(world, m->type);
                    initialize(world, ECS_OFFSET(element, m->offset), m->count,
                               member_ti);
                }
            }
            return;
        }
        if (type->kind == EcsArrayType) {
            const EcsArray *array_info =
                ecs_get(world, ti->component, EcsArray);
            ecs_assert(array_info != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_info_t *array_ti =
                ecs_get_type_info(world, array_info->type);
            for (int j = 0; j < count; j++) {
                void *element = ECS_ELEM(ptr, ti->size, j);
                initialize(world, element, array_info->count, array_ti);
            }
            return;
        }
        if (type->kind == EcsVectorType) {
            const EcsVector *vector_info =
                ecs_get(world, ti->component, EcsVector);
            ecs_assert(vector_info != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_info_t *vector_ti =
                ecs_get_type_info(world, vector_info->type);
            for (int j = 0; j < count; j++) {
                const void *element = ECS_ELEM(ptr, ti->size, j);
                ecs_vec_t *v = (ecs_vec_t *) element;
                /* arbitrarly size the vector to have 3 items: */
                ecs_vec_set_count(NULL, v, vector_ti->size, 3);
                void *vec_first = ecs_vec_first(v);
                int32_t vec_count = ecs_vec_count(v);
                if (vector_ti->hooks.ctor) {
                    vector_ti->hooks.ctor(vec_first, vec_count, vector_ti);
                } else {
                    flecs_default_ctor(vec_first, vec_count, vector_ti);
                }
                initialize(world, vec_first, vec_count, vector_ti);
            }
            return;
        }
    }

    ecs_map_val_t *init_ptr = ecs_map_get(&initializer_map, ti->component);
    if (init_ptr) {
        initializer init = (initializer) (uintptr_t) (*init_ptr);
        init(world, ptr, count, ti);
    }
    /* do nothing (leave to what the component's ctor did) */
}

/* Initializes an int type with test data */
static void int_initialize(ecs_world_t *world, void *ptr, int32_t count,
                           const ecs_type_info_t *ti) {
    for (int j = 0; j < count; j++) {
        int *element = (int *) ECS_ELEM(ptr, ti->size, j);
        *element = 100 + j;
    }
}

/* Initializes a string type with test data */
static void string_initialize(ecs_world_t *world, void *ptr, int32_t count,
                              const ecs_type_info_t *ti) {
    for (int j = 0; j < count; j++) {
        char **element = (char **) ECS_ELEM(ptr, ti->size, j);
        char buf[100];
        ecs_os_snprintf(buf, sizeof(buf), "String%d", j);
        *element = ecs_os_strdup(buf);
    }
}

/* Compares two string component instances */
static bool string_compare(ecs_world_t *world, void *a, void *b, int32_t count,
                           const ecs_type_info_t *ti) {
    for (int j = 0; j < count; j++) {
        const char **st_a = (const char **) ECS_ELEM(a, ti->size, j);
        const char **st_b = (const char **) ECS_ELEM(b, ti->size, j);
        if (*st_a == *st_b) {
            continue;
        }
        if (*st_a == NULL || *st_b == NULL) {
            return false;
        }
        if (0 != strcmp(*st_a, *st_b)) {
            return false;
        }
    }
    return true;
}

/* Initializes a resource handle type with test data */
static void resource_handle_initialize(ecs_world_t *world, void *ptr,
                                       int32_t count,
                                       const ecs_type_info_t *ti) {
    for (int j = 0; j < count; j++) {
        ResourceHandle *rh = (ResourceHandle *) ECS_ELEM(ptr, ti->size, j);
        rh->value = 200 + j;
    }
}

/* compares two resource handles */
static bool resource_handle_compare(ecs_world_t *world, void *a, void *b,
                                    int32_t count, const ecs_type_info_t *ti) {
    for (int j = 0; j < count; j++) {
        const ResourceHandle *ra =
            (const ResourceHandle *) ECS_ELEM(a, ti->size, j);
        const ResourceHandle *rb =
            (const ResourceHandle *) ECS_ELEM(b, ti->size, j);
        if (ra->value != rb->value) {
            return false;
        }
    }
    return true;
}

/* Asserts the condition showing a helpful message in case it fails */
static void test_assert_fmt(bool condition, const char *fmt, ...) {
    if (!condition) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        test_assert(false);
    }
}

/* Asserts equality of two ints showing a helpful message in case it fails */
static void test_int_fmt(int expected, int actual, const char *fmt, ...) {
    if (expected != actual) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        test_assert(false);
    }
}

/* Asserts expected resource usage showing a helpful message in case it fails */
static void test_resource_usage(int32_t initial, int32_t expected_usage,
                                const char *tut_name, const char *action) {
    test_int_fmt(initial - expected_usage, available_resource_count(),
                 "When testing %s after %s, expected resource usage is %d, "
                 "however %d resources were used.\n",
                 tut_name, action, expected_usage,
                 initial - available_resource_count());
}

/* Component used to mark an entity as a type under test (tut) and also indicate
 * how many "test resources" is expected to consume during the test */
typedef struct TestTypeInfo {
    int resources;
} TestTypeInfo;

/* Applies a test to a runtime type. Constructs the type, copies the type
 * instantiating a prefab, forces a move with an archetype change and then
 * destroys it, verifying the data matches in all steps and that resources are
 * consumed / returned properly and in the right amount. */
static void test_runtime_type(ecs_world_t *world, ecs_entity_t tut) {
    ECS_COMPONENT(world, TestTypeInfo);
    const ecs_type_info_t *ti = ecs_get_type_info(world, tut);
    const char *tut_name = ecs_get_name(world, tut);
    int resources_required = ecs_get(world, tut, TestTypeInfo)->resources;
    ecs_entity_t prefab = ecs_new(world);
    printf("testing %s\n", tut_name);

    int32_t initial_resources = ecs_vec_count(&resource_ids);
    ecs_add_id(world, prefab, tut);
    void *prefab_data = ecs_get_mut_id(world, prefab, tut);
    initialize(world, prefab_data, 1, ti);
    test_resource_usage(initial_resources, resources_required, tut_name,
                        "creating prefab");

    /* now instantiate this prefab to trigger a copy */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
    test_resource_usage(initial_resources, resources_required * 2, tut_name,
                        "instancing prefab");

    const void *instance_data = ecs_get_id(world, e, tut);

    test_assert_fmt(
        compare(world, prefab_data, instance_data, 1, ti),
        "When testing %s, prefab data did not match with instance data\n",
        tut_name);

    ecs_entity_t changer = ecs_new(world);
    /* add another entity to have `e` change archetypes and trigger a move: */
    ecs_add_id(world, e, changer);
    test_resource_usage(initial_resources, resources_required * 2, tut_name,
                        "moving instance");

    const void *instance_data2 = ecs_get_id(world, e, tut);
    test_assert(instance_data != instance_data2);

    test_assert_fmt(compare(world, prefab_data, instance_data2, 1, ti),
                    "When testing %s, prefab data did not match with instance "
                    "data after move\n",
                    tut_name);

    ecs_delete(world, e);
    test_resource_usage(initial_resources, resources_required, tut_name,
                        "destroying instance");

    ecs_delete(world, prefab);
    test_resource_usage(initial_resources, 0, tut_name, "destroying prefab");

    ecs_delete(world, changer);

    /* check if all resources were returned: */
    for (int i = 0; i < initial_resources; i++) {
        test_assert_fmt(resource_id_available(i + 1),
                        "When testing %s, could not find resource with id %d, "
                        "it should have been returned by destructors\n",
                        tut_name, i + 1);
    }
}

/* Creates a test entity with a name and expected test resource usage */
ecs_entity_t test_entity(ecs_world_t *world, int resources, const char *name) {
    ECS_COMPONENT(world, TestTypeInfo);
    ecs_entity_t tut = ecs_new(world);
    ecs_ensure(world, tut, TestTypeInfo)->resources = resources;
    ecs_set_name(world, tut, name);
    return tut;
}

/*
Define all Types under Test inside this function. When using `test_entity()` to
create the entity, the tut is marked so it can be found later to run tests on
it.
*/
void define_types_under_test(ecs_world_t *world) {
    ecs_entity_t resource_handle_opaque = define_ResourceHandle_opaque(world);

    /* Register comparers for special types */
    ecs_map_insert(&comparer_map, ecs_id(ecs_string_t),
                   (ecs_map_val_t) (uintptr_t) string_compare);
    ecs_map_insert(&comparer_map, resource_handle_opaque,
                   (ecs_map_val_t) (uintptr_t) resource_handle_compare);

    /* Register initializers for special types */
    ecs_map_insert(&initializer_map, ecs_id(ecs_string_t),
                   (ecs_map_val_t) (uintptr_t) string_initialize);
    ecs_map_insert(&initializer_map, ecs_id(ecs_i32_t),
                   (ecs_map_val_t) (uintptr_t) int_initialize);
    ecs_map_insert(&initializer_map, resource_handle_opaque,
                   (ecs_map_val_t) (uintptr_t) resource_handle_initialize);

    /* Actually define the tuts below: */

    ecs_entity_t struct_with_strings = ecs_struct(
        world, {.entity = test_entity(world, 0, "struct_with_strings"),
                .members = {{.name = "a", .type = ecs_id(ecs_string_t)},
                            {.name = "b", .type = ecs_id(ecs_string_t)}}});

    ecs_struct(world,
               {.entity = test_entity(world, 0, "nested_struct_with_strings"),
                .members = {{.name = "a", .type = struct_with_strings},
                            {.name = "b", .type = ecs_id(ecs_i32_t)},
                            {.name = "c", .type = struct_with_strings}}});

    ecs_entity_t array_of_strings =
        ecs_array(world, {.entity = test_entity(world, 0, "array_of_strings"),
                          .type = ecs_id(ecs_string_t),
                          .count = 3});

    ecs_struct(world,
               {.entity = test_entity(world, 0, "struct_with_array_of_strings"),
                .members = {{.name = "a", .type = array_of_strings},
                            {.name = "b", .type = ecs_id(ecs_i32_t)}}});

    ecs_struct(world,
               {.entity = test_entity(world, 1, "struct_with_opaque"),
                .members = {{.name = "a", .type = resource_handle_opaque}}});

    ecs_entity_t vector_of_ints =
        ecs_vector(world, {.entity = test_entity(world, 0, "vector_of_ints"),
                           .type = ecs_id(ecs_i32_t)});

    ecs_struct(world,
               {.entity = test_entity(world, 0, "struct_with_vector_of_ints"),
                .members = {{.name = "a", .type = vector_of_ints}}});

    ecs_entity_t vector_of_strings =
        ecs_vector(world, {.entity = test_entity(world, 0, "vector_of_strings"),
                           .type = ecs_id(ecs_string_t)});

    ecs_struct(world, {.entity = test_entity(world, 0,
                                             "struct_with_vector_of_strings"),
                       .members = {{.name = "a", .type = vector_of_strings}}});

    ecs_struct(
        world,
        {.entity = test_entity(world, 0, "nested_struct_with_vector_of_ints"),
         .members = {
             {.name = "a", .type = vector_of_ints},
             {.name = "b", .type = ecs_id(ecs_i32_t)},
             {.name = "c",
              .type = ecs_struct(
                  world,
                  {.members = {{.name = "a", .type = vector_of_ints},
                               {.name = "b", .type = ecs_id(ecs_i32_t)},
                               {.name = "c", .type = vector_of_ints}}})}}});

    ecs_struct(
        world,
        {.entity =
             test_entity(world, 0, "nested_struct_with_vector_of_strings"),
         .members = {
             {.name = "a", .type = vector_of_strings},
             {.name = "b", .type = ecs_id(ecs_i32_t)},
             {.name = "c",
              .type = ecs_struct(
                  world,
                  {.members = {{.name = "a", .type = vector_of_strings},
                               {.name = "b", .type = ecs_id(ecs_i32_t)},
                               {.name = "c", .type = vector_of_strings}}})}}});

    ecs_entity_t array_of_struct_with_strings = ecs_array(
        world, {.entity = test_entity(world, 0, "array_of_struct_with_strings"),
                .type = struct_with_strings,
                .count = 3});

    ecs_array(world,
              {.entity = test_entity(world, 0, "array_of_array_of_strings"),
               .type = array_of_strings,
               .count = 3});

    ecs_array(world, {.entity = test_entity(
                          world, 0, "array_of_array_of_struct_with_strings"),
                      .type = array_of_struct_with_strings,
                      .count = 3});

    ecs_array(world,
              {.entity = test_entity(world, 0, "array_of_vectors_of_ints"),
               .type = vector_of_ints,
               .count = 3});

    ecs_array(world,
              {.entity = test_entity(world, 0, "array_of_vectors_of_strings"),
               .type = vector_of_strings,
               .count = 3});

    ecs_array(world, {.entity = test_entity(world, 3, "array_of_opaque"),
                      .type = resource_handle_opaque,
                      .count = 3});

    ecs_vector(world, {.entity = test_entity(world, 0,
                                             "vector_of_structs_with_strings"),
                       .type = struct_with_strings});

    ecs_vector(world,
               {.entity = test_entity(world, 0, "vector_of_arrays_of_strings"),
                .type = array_of_strings});

    ecs_vector(world, {.entity = test_entity(world, 3, "vector_of_opaque"),
                       .type = resource_handle_opaque});
}

/* Actual universal type tester */
void RuntimeTypes_universal_type_tester(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT(world, TestTypeInfo);

    /* create some "resources" */
    const int initial_resources = 50;
    initialize_resource_ids(initial_resources);
    ecs_map_init(&comparer_map, NULL);
    ecs_map_init(&initializer_map, NULL);

    define_types_under_test(world);

    /* This query finds all tuts and runs tests on them */
    ecs_query_t *q = ecs_query(world, {.terms = {{ecs_id(TestTypeInfo)}}});
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            test_runtime_type(world, it.entities[i]);
        }
    }
    ecs_query_fini(q);

    ecs_map_fini(&comparer_map);
    ecs_map_fini(&initializer_map);
    free_resource_ids();
    ecs_fini(world);
}