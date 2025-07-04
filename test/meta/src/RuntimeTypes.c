#include <meta.h>
#include <stdint.h>

#define UINT32_PATTERN(x) \
    (((uint32_t) (x)) + ((uint32_t) (x) << 8) + ((uint32_t) (x) << 16) + \
     ((uint32_t) (x) << 24))

static
void* test_ecs_ensure(ecs_world_t *world, ecs_entity_t entity, ecs_id_t component) {
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    test_assert(ti != NULL);
    return ecs_ensure_id(world, entity, component, ti->size);
}

static 
bool is_memory_filled_with(
    const void *ptr,
    size_t size,
    unsigned char byte)
{
    const unsigned char *byte_ptr = (const unsigned char *)ptr;

    for (size_t i = 0; i < size; i++) {
        if (byte_ptr[i] != byte) {
            return false;
        }
    }

    return true;
}

static 
bool memory_is_zero(
    const void *ptr,
    size_t size)
{
    return is_memory_filled_with(ptr, size, 0);
}

#define test_memory_zero(ptr, size) test_assert(memory_is_zero((ptr), (size)))

/* Test hooks for runtime trivial structs
   These structs have no special copy/move/dtor logic and are zero-initialized
   Trivial structs should get flecs default constructor and nothing else */
void RuntimeTypes_trivial_struct(void) {
    ecs_world_t *world = ecs_init();

    /* Create a component to use as subtype in the trivial struct
       Create a new component: */
    ecs_entity_t subtype = ecs_entity(world, {.name = "Some Subtype"});

    /* Configure as a struct: */
    ecs_struct(world, {
        .entity = subtype,
        .members = {
            {.name = "x", .type = ecs_id(ecs_i32_t)},
            {.name = "r", .type = ecs_id(ecs_f32_t)}
        }
    });

    /* Create a new component: */
    ecs_entity_t trivial_struct = ecs_entity(world, {.name = "Trivial Struct"});

    ecs_struct(world, {
        .entity = trivial_struct,
        .members = {
            {.name = "a", .type = ecs_id(ecs_i32_t)},
            {.name = "b", .type = ecs_id(ecs_f32_t)},
            {.name = "s", .type = subtype}
        }
    });

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

static
void test_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    ctor_calls++;
    ctor_ptr = (uintptr_t) ptr;
    ctor_count = count;
    ctor_component = type_info->component;
    ecs_os_memset(ptr, ctor_pattern, count * type_info->size);
}

static int dtor_calls = 0;
static uintptr_t dtor_ptr = 0;
static int32_t dtor_count = 0;
static ecs_entity_t dtor_component = 0;

static
void test_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    dtor_calls++;
    dtor_ptr = (uintptr_t) ptr;
    dtor_count = count;
    dtor_component = type_info->component;
    ecs_os_memset(ptr, 0xDE, count * type_info->size);
}

static int move_calls = 0;
static uintptr_t move_dst_ptr = 0;
static uintptr_t move_src_ptr = 0;
static int32_t move_count = 0;
static ecs_entity_t move_component = 0;

static
void test_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    move_calls++;
    move_dst_ptr = (uintptr_t) dst_ptr;
    move_src_ptr = (uintptr_t) src_ptr;
    move_count = count;
    move_component = type_info->component;
    ecs_os_memcpy(dst_ptr, src_ptr, count * type_info->size);
    ecs_os_memset(src_ptr, 0xBC, count * type_info->size);
}

static int copy_calls = 0;
static uintptr_t copy_dst_ptr = 0;
static uintptr_t copy_src_ptr = 0;
static int32_t copy_count = 0;
static ecs_entity_t copy_component = 0;

static
void test_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *type_info)
{
    copy_calls++;
    copy_dst_ptr = (uintptr_t) dst_ptr;
    copy_src_ptr = (uintptr_t) src_ptr;
    copy_count = count;
    copy_component = type_info->component;
    ecs_os_memcpy(dst_ptr, src_ptr, count * type_info->size);
}

/* Define a nested struct that will have one or more hooks depending on the test
 */
typedef struct NestedStruct {
    uint32_t a;
    uint32_t b;
} NestedStruct;

static ecs_entity_t nested_struct;

static
const ecs_type_info_t *define_nested_struct(
    ecs_world_t *world) 
{
    nested_struct = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "NestedStruct"}),
        .members = {
            { .name = "a", .type = ecs_id(ecs_u32_t)},
            { .name = "b", .type = ecs_id(ecs_u32_t)}
        }
    });

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

static
const ecs_type_info_t *define_test_struct(
    ecs_world_t *world) 
{
    test_struct = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "TestStruct"}),
        .members = {
            {.name = "x", .type = ecs_id(ecs_u32_t)},
            {.name = "s1", .type = nested_struct},
            {.name = "y", .type = ecs_id(ecs_u32_t)},
            {.name = "s2", .type = nested_struct}
        }
    });

    return ecs_get_type_info(world, test_struct);
}

/* Compares two instances of the given type */
static
int cmp(const ecs_world_t* world, ecs_entity_t id, const void *a, const void *b) {
    const ecs_type_info_t* ti = ecs_get_type_info(world, id);
    return ti->hooks.cmp(a, b, ti);
}

/* Tests that a constructor is generated for a struct if at least a member has
 * itself a constructor Also tests if the generated constructor works. */
void RuntimeTypes_ctor(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.ctor = test_ctor; /* only define a constructor for "NestedStruct" */
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * A constructor should be automatically generated for TestStruct to invoke
     * NestedStruct's specific constructor: */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should only have a constructor: */
    test_assert(test_struct_ti->hooks.ctor != NULL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.move == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    /* Now instantiate TestStruct and see the constructor for NestedStruct is
     * called twice, since TestStruct contains two NestedStructs: */
    ecs_entity_t e = ecs_new(world);

    test_int(0, ctor_calls);
    ecs_add_id(world, e, test_struct);
    test_int(2, ctor_calls); /* called twice because there are two "nested_struct" */

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

/* Tests that an illegal constructor is set for a struct if at least a member has
 * itself an illegal constructor */
void RuntimeTypes_ctor_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_CTOR_ILLEGAL; /* mark constructor for "NestedStruct" as illegal */
    hooks.ctor = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * TestStruct's constructor should be set to illegal as well. */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have an illegal constructor too: */
    test_assert(test_struct_ti->hooks.flags & ECS_TYPE_HOOK_CTOR_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.move == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    ecs_fini(world);
}

/* Tests that a destructor is generated for a struct if at least a member has
 * itself a destructor Also tests if the generated destructor works. */
void RuntimeTypes_dtor(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.dtor = test_dtor; /* only set a destructor */
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * A destructor should be automatically generated for TestStruct to invoke
     * NestedStruct's specific destructor: */
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

/* Tests that an illegal destructor is set for a struct if at least a member has
 * itself an illegal destructor */
void RuntimeTypes_dtor_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_DTOR_ILLEGAL; /* mark destructor for "NestedStruct" as illegal */
    hooks.dtor = NULL;
    
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * TestStruct's destructor should be set to illegal as well. */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have an illegal constructor too: */
    test_assert(test_struct_ti->hooks.flags & ECS_TYPE_HOOK_DTOR_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.move == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    ecs_fini(world);
}

/* Tests that a move hook is generated for a struct if at least a member has
 * itself a move hook Also tests if the generated move hook works. */
void RuntimeTypes_move(void) {
    ecs_world_t *world = ecs_init();
    define_nested_struct(world);

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);
    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.move = test_move; /* Only define a move hook. */
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * A move hook should be automatically generated for TestStruct to invoke
     * NestedStruct's specific move hook: */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have a default constructor and the generated move hook: */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.move != NULL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    ecs_entity_t e = ecs_new(world);

    test_int(0, move_calls);
    void *ptr = test_ecs_ensure(world, e, test_struct);
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

/* Tests that an illegal move hook is set for a struct if at least a member has
 * itself an illegal move hook */
void RuntimeTypes_move_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_MOVE_ILLEGAL; /* mark move hook for "NestedStruct" as illegal */
    hooks.move = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * TestStruct's constructor should be set to illegal as well. */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have an illegal move hook too: */
    test_assert(test_struct_ti->hooks.flags & ECS_TYPE_HOOK_MOVE_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.copy == NULL);

    ecs_fini(world);
}

/* Tests that a copy hook is generated for a struct if at least a member has
 * itself a copy hook Also tests if the generated copy hook works. */
void RuntimeTypes_copy(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    define_nested_struct(world);
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);
    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.copy = test_copy; /* only set a copy hook */
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * A copy hook should be automatically generated for TestStruct to invoke
     * NestedStruct's specific copy hook: */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have a default constructor and the generated copy hook: */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.copy != NULL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.move == NULL);

    /* create a prefab entity: */
    ecs_entity_t prefab = ecs_new(world);
    ecs_add_id(world, prefab, EcsPrefab);

    test_int(0, copy_calls);
    void *original_ptr = test_ecs_ensure(world, prefab, test_struct);
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

/* Tests that an illegal copy hook is set for a struct if at least a member has
 * itself an illegal copy hook */
void RuntimeTypes_copy_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_COPY_ILLEGAL; /* mark copy hook for "NestedStruct" as illegal */
    hooks.copy = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * TestStruct's copy and copy ctor should be set to illegal as well. */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have an illegal copy: */
    test_assert(test_struct_ti->hooks.flags & ECS_TYPE_HOOK_COPY_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_struct_ti->hooks.ctor != NULL);
    test_assert(test_struct_ti->hooks.dtor == NULL);
    test_assert(test_struct_ti->hooks.move == NULL);

    ecs_fini(world);
}

/* Tests that an illegal compare hook is set for a struct if at least a member has
 * itself an illegal compare hook */
void RuntimeTypes_cmp_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_CMP_ILLEGAL; /* mark cmp hook for "NestedStruct" as illegal */
    hooks.cmp = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * TestStruct's compare hook should be set to illegal as well. */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have an illegal compare hook too: */
    test_assert(test_struct_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL);

    ecs_fini(world);
}

/* Tests that an illegal equals hook is set for a struct if at least a member has
 * itself an illegal equals hook */
void RuntimeTypes_equals_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_EQUALS_ILLEGAL; /* mark equals hook for "NestedStruct" as illegal */
    hooks.equals = NULL;
    
    /* must mark cmp illegal too, otherwise equals would be autogenerated from cmp */
    hooks.flags |= ECS_TYPE_HOOK_CMP_ILLEGAL; 
    hooks.cmp = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define TestStruct, which has two "NestedStruct" members.
     * TestStruct's equals hook should be set to illegal as well. */
    const ecs_type_info_t *test_struct_ti = define_test_struct(world);

    /* TestStruct should have an illegal equals hook too: */
    test_assert(test_struct_ti->hooks.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL);

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
static ecs_vec_t resource_ids;

/* Initializes the available "resource ids" */
static
void initialize_resource_ids(
    int num_resources) 
{
    ecs_vec_init_t(NULL, &resource_ids, int, num_resources);
    ecs_vec_set_count_t(NULL, &resource_ids, int, num_resources);
    for (int i = 0; i < num_resources; i++) {
        *ecs_vec_get_t(&resource_ids, int, i) = num_resources - i;
    }
}

static
void free_resource_ids() {
    ecs_vec_fini_t(NULL, &resource_ids, int);
}

/* Gets a resource id from the vector. It will return 0 if no more resources
 * available. */
static
int get_resource_id() 
{
    size_t idcount = ecs_vec_count(&resource_ids);
    if (idcount == 0)
        return 0;
    int id = *ecs_vec_get_t(&resource_ids, int, idcount - 1);
    ecs_vec_set_count_t(NULL, &resource_ids, int, idcount - 1);
    return id;
}

/* Returns a resource for reuse. */
static
void return_resource_id(
    int id) 
{
    if (id == 0)
        return;
    *(ecs_vec_append_t(NULL, &resource_ids, int)) = id;
}

/* Returns true if a specific resource id is in the pool. */
static
bool resource_id_available(
    int id) 
{
    for (int i = 0; i < ecs_vec_count(&resource_ids); i++) {
        if (id == *ecs_vec_get_t(&resource_ids, int, i))
            return true;
    }
    return false;
}

/* Returns the numer of currently available resources */
static
int resources_left() 
{ 
    return ecs_vec_count(&resource_ids); 
}

/* Define a ResourceHandle constructor that sets the payload value to 0 and
 * obtains a unique resource id: */
static
void ResourceHandle_ctor(
    void *ptr, 
    int32_t count,
    const ecs_type_info_t *type_info) 
{
    for (int i = 0; i < count; i++) {
        ResourceHandle *r =
            (ResourceHandle *) ECS_ELEM_T(ptr, ResourceHandle, i);
        r->id = get_resource_id();
        test_assert(r->id);
        r->value = 0;
    }
}

/* Define a ResourceHandle destructor that returns the borrowed resource id: */
static
void ResourceHandle_dtor(
    void *ptr, 
    int32_t count,
    const ecs_type_info_t *type_info) 
{
    for (int i = 0; i < count; i++) {
        ResourceHandle *r =
            (ResourceHandle *) ECS_ELEM_T(ptr, ResourceHandle, i);
        return_resource_id(r->id);
    }
}

/* Define a ResourceHandle move operation that transfers the resource id: */
static
void ResourceHandle_move(
    void *dst_ptr, 
    void *src_ptr, 
    int32_t count,
    const ecs_type_info_t *type_info) 
{
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
static
void ResourceHandle_copy(
    void *dst_ptr, 
    const void *src_ptr, 
    int32_t count,
    const ecs_type_info_t *type_info) 
{
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

/* compares two resource handles */
static 
int ResourceHandle_comp(
    const void *a, 
    const void *b,
    const ecs_type_info_t *ti) 
{
    int va = ((const ResourceHandle*)a)->value;
    int vb = ((const ResourceHandle*)b)->value;
    return (va > vb) - (va < vb);
}

/* Defines a struct in Flecs to model the ResourceHandle struct
 * For different tests, it can set specific hooks or not. */
static ecs_entity_t resource_handle;

static
void define_resource_handle(
    ecs_world_t *world, 
    bool ctor, 
    bool dtor, 
    bool move,
    bool copy) 
{
    resource_handle = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "ResourceHandle"}),
        .members = {
            {.name = "id", .type = ecs_id(ecs_i32_t)},
            {.name = "value", .type = ecs_id(ecs_u32_t)}
        }
    });

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

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    hooks.cmp = ResourceHandle_comp;

    ecs_set_hooks_id(world, resource_handle, &hooks);
}

/* Test hooks for runtime array of trivial structs
 * These structs have no special copy/move/dtor logic and are zero-initialized
 * Trivial structs should get flecs default constructor and nothing else
 * An array of trivial struct should also only get flecs default constructor and
 * nothing else. */
void RuntimeTypes_trivial_array(void) {
    ecs_world_t *world = ecs_init();

    /* Create a new component: */
    ecs_entity_t trivial_struct = ecs_struct(world, {
            .entity = ecs_entity(world, {.name = "Trivial Struct"}),
            .members = {
                {.name = "a", .type = ecs_id(ecs_i32_t)},
                {.name = "b", .type = ecs_id(ecs_f32_t)}
        }
    });

    /* create an array of 3 trivial structs: */
    ecs_array_desc_t desc = {.entity = 0, .type = trivial_struct, .count = 3};
    ecs_entity_t arr_of_struct = ecs_array_init(world, &desc);

    const ecs_type_info_t *ti = ecs_get_type_info(world, arr_of_struct);

    test_assert(ti->hooks.ctor != NULL); /* should have got flecs default constructor. */

    /* no other hooks should've been set:  */
    test_assert(ti->hooks.copy == NULL);
    test_assert(ti->hooks.move == NULL);
    test_assert(ti->hooks.dtor == NULL);

    /* Check if the ctor initialized everything to 0.
     * Do this a couple of times to make sure we don't get a zero-initialized
     * struct out of sheer luck: */
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
 * ctor is defined for the array itself.
 * Tests that the specified constructor is called for each array element. */
void RuntimeTypes_array_ctor(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    initialize_resource_ids(10);
    test_int(10, resources_left());

    /* Define the Resource with only a constructor hook. */
    define_resource_handle(world, true, false, false, false);

    /* create an array of 3 ResourceHandles */
    ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
    ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

    const ecs_type_hooks_t *hooks =
        &ecs_get_type_info(world, arr_of_resources)->hooks;

    test_assert(hooks->ctor != NULL); /* a ctor should've been generated */

    /* no other hooks should have been generated, since the depending type,
     * "resource_handle" only has a ctor hook: */
    test_assert(hooks->dtor == NULL);
    test_assert(hooks->move == NULL);
    test_assert(hooks->copy == NULL);

    /* Test that the set ctor hook is indeed working: */
    ecs_entity_t e = ecs_new(world);
    ecs_add_id(world, e, arr_of_resources);
    test_int(7, resources_left()); /* 3 resources were used out of 10, since the array has a size of 3 */

    ecs_fini(world);

    free_resource_ids();
}

/* Tests that an illegal constructor is set for an array if its underlying type itself
 * has an illegal constructor */
void RuntimeTypes_array_ctor_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_CTOR_ILLEGAL; /* mark constructor for "NestedStruct" as illegal */
    hooks.ctor = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define test_arr, as an array of "NestedStruct".
     * TestStruct's constructor should be set to illegal as well. */
    ecs_array_desc_t desc = {.entity = 0, .type = nested_struct, .count = 3};
    ecs_entity_t test_arr = ecs_array_init(world, &desc);

    const ecs_type_info_t* test_arr_ti = ecs_get_type_info(world, test_arr);

    /* test_arr should have an illegal constructor too: */
    test_assert(test_arr_ti->hooks.flags & ECS_TYPE_HOOK_CTOR_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_arr_ti->hooks.dtor == NULL);
    test_assert(test_arr_ti->hooks.move == NULL);
    test_assert(test_arr_ti->hooks.copy == NULL);

    ecs_fini(world);
}

/* Tests that if on the array's underlying type only a dtor is defined, only a
 * dtor is defined for the array itself Tests that the specified destructor is
 * called for each array element. */
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
    test_int(10, resources_left()); /* since no special ctor is
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
    test_int(13, resources_left());
    test_assert(resource_id_available(100));
    test_assert(resource_id_available(200));
    test_assert(resource_id_available(300));

    ecs_fini(world);

    free_resource_ids();
}

/* Tests that an illegal destructor is set for an array if its underlying type itself
 * has an illegal destructor */
void RuntimeTypes_array_dtor_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_DTOR_ILLEGAL; /* mark destructor for "NestedStruct" as illegal */
    hooks.dtor = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define test_arr, as an array of "NestedStruct".
     * TestStruct's destructor should be set to illegal as well. */
    ecs_array_desc_t desc = {.entity = 0, .type = nested_struct, .count = 3};
    ecs_entity_t test_arr = ecs_array_init(world, &desc);

    const ecs_type_info_t* test_arr_ti = ecs_get_type_info(world, test_arr);

    /* test_arr should have an illegal destructor too: */
    test_assert(test_arr_ti->hooks.flags & ECS_TYPE_HOOK_DTOR_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_arr_ti->hooks.ctor != NULL);
    test_assert(test_arr_ti->hooks.move == NULL);
    test_assert(test_arr_ti->hooks.copy == NULL);

    ecs_fini(world);
}

/* Tests that if on the array's underlying type only a move hook is defined,
 * only a move hook is defined for the array itself. Tests that the specified
 * move hook is called for each array element. */
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
    test_int(10, resources_left()); /* since no special ctor is
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

    /* we should retrieve the same values: */
    test_assert(0 == cmp(
        world, resource_handle, &(ResourceHandle){.id = 100, .value = 111}, &handles[0]));
    test_assert(0 == cmp(
        world, resource_handle, &(ResourceHandle){.id = 200, .value = 222}, &handles[1]));
    test_assert(0 == cmp(
        world, resource_handle, &(ResourceHandle){.id = 300, .value = 333}, &handles[2]));

    test_int(10, resources_left()); /* pool stays the same */

    ecs_delete(world, e);

    /* since no special destructor is defined, the resources 100,200,300 are not
     * returned to the pool: */
    test_int(10, resources_left());
    test_assert(!resource_id_available(100));
    test_assert(!resource_id_available(200));
    test_assert(!resource_id_available(300));

    ecs_fini(world);

    free_resource_ids();
}

/* Tests that an illegal move hook is set for an array if its underlying type itself
 * has an illegal move hook */
void RuntimeTypes_array_move_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_MOVE_ILLEGAL; /* mark move hook for "NestedStruct" as illegal */
    hooks.move = NULL;

    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define test_arr, as an array of "NestedStruct".
     * TestStruct's move hook should be set to illegal as well. */
    ecs_array_desc_t desc = {.entity = 0, .type = nested_struct, .count = 3};
    ecs_entity_t test_arr = ecs_array_init(world, &desc);

    const ecs_type_info_t* test_arr_ti = ecs_get_type_info(world, test_arr);

    /* test_arr should have an illegal move hook too: */
    test_assert(test_arr_ti->hooks.flags & ECS_TYPE_HOOK_MOVE_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_arr_ti->hooks.ctor != NULL);
    test_assert(test_arr_ti->hooks.dtor == NULL);
    test_assert(test_arr_ti->hooks.copy == NULL);

    ecs_fini(world);
}

/* Tests that if on the array's underlying type only a copy hook is defined,
   only a copy hook is defined for the array itself. Tests that the specified
   copy hook is called for each array element. */
void RuntimeTypes_array_copy(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    initialize_resource_ids(10);
    test_int(10, resources_left());

    /* Define the Resource with only a copy hook. */
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
    test_int(10, resources_left()); /* since no special ctor is
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
    test_int(7, resources_left()); /* should've taken 3 resources from
                                                the pool */

    ResourceHandle *handles =
        (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);

    test_assert(0 == cmp(
        world, resource_handle, &prefab_handles[0], &handles[0]));
    test_assert(0 == cmp(
        world, resource_handle, &prefab_handles[1], &handles[1]));
    test_assert(0 == cmp(
        world, resource_handle, &prefab_handles[2], &handles[2]));

    ecs_delete(world, e);
    test_int(7, resources_left()); /* resources not returned since we
                                                did not hook a destructor */

    ecs_fini(world);

    free_resource_ids();
}

/* Tests that an illegal copy hook is set for an array if its underlying type itself
 * has an illegal copy hook */
void RuntimeTypes_array_copy_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_COPY_ILLEGAL; /* mark copy hook for "NestedStruct" as illegal */
    hooks.copy = NULL;
    
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define test_arr, as an array of "NestedStruct".
     * TestStruct's copy hook should be set to illegal as well. */
    ecs_array_desc_t desc = {.entity = 0, .type = nested_struct, .count = 3};
    ecs_entity_t test_arr = ecs_array_init(world, &desc);

    const ecs_type_info_t* test_arr_ti = ecs_get_type_info(world, test_arr);

    /* test_arr should have an illegal copy hook too: */
    test_assert(test_arr_ti->hooks.flags & ECS_TYPE_HOOK_COPY_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_arr_ti->hooks.ctor != NULL);
    test_assert(test_arr_ti->hooks.dtor == NULL);
    test_assert(test_arr_ti->hooks.move == NULL);

    ecs_fini(world);
}

/* Tests that an illegal cmp hook is set for an array if its underlying type itself
 * has an illegal cmp hook */
void RuntimeTypes_array_cmp_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_CMP_ILLEGAL; /* mark compare hook 
        for "NestedStruct" as illegal */
    hooks.cmp = NULL;
    
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define test_arr, as an array of "NestedStruct".
     * TestStruct's cmp hook should be set to illegal as well. */
    ecs_array_desc_t desc = {.entity = 0, .type = nested_struct, .count = 3};
    ecs_entity_t test_arr = ecs_array_init(world, &desc);

    const ecs_type_info_t* test_arr_ti = ecs_get_type_info(world, test_arr);

    /* test_arr should have an illegal cmp hook too: */
    test_assert(test_arr_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL);

    /* No other hooks should've been set: */
    test_assert(test_arr_ti->hooks.ctor != NULL);
    test_assert(test_arr_ti->hooks.dtor == NULL);
    test_assert(test_arr_ti->hooks.move == NULL);

    ecs_fini(world);
}

/* Test vector types */
void RuntimeTypes_vector_lifecycle(void) {
    ecs_world_t *world = ecs_init();

    /* create some "resources" */
    int initial_resources = 10;
    initialize_resource_ids(initial_resources);
    test_int(10, resources_left());

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
        (ecs_vec_t *) test_ecs_ensure(world, prefab, vector_of_resources);
    test_assert(v != NULL);
    test_assert(v->array == NULL);
    test_assert(v->count == 0);
    test_int(initial_resources,
             resources_left()); /* still have 10 resources since
vector is empty

manually add some items to the vector. These must be constructed by hand: */
    ecs_vec_set_count_t(NULL, v, ResourceHandle, 3);
    ResourceHandle_ctor(ecs_vec_first(v), 3, NULL);
    test_int(initial_resources - 3, resources_left()); /* Used up 3
       resources that are now in the vector

       test vector copy assign by instantiating the prefab */
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
    test_int(initial_resources - 6, resources_left()); /* Used up 3
       more resources after copying the vector

       test vector move assign by forcing a move via archetype change: */
    ecs_add_id(world, e, ecs_new(world));
    test_int(initial_resources - 6,
             resources_left()); /* No more resources consumed */

    ecs_delete(world, e);
    test_int(initial_resources - 3, resources_left()); /* Frees 3 resources held
                                                          by the instance */

    ecs_delete(world, prefab);
    test_int(initial_resources, resources_left()); /* Frees another 3
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
    ecs_vec_t *v = (ecs_vec_t *) test_ecs_ensure(world, prefab, vector_of_ints);
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

/* Tests that an illegal cmp hook is set for an array if its underlying type itself
 * has an illegal cmp hook */
void RuntimeTypes_vector_cmp_illegal(void) {
    ecs_world_t *world = ecs_init();

    /* Define NestedStruct: */
    const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

    ecs_type_hooks_t hooks = nested_struct_ti->hooks;
    hooks.flags |= ECS_TYPE_HOOK_CMP_ILLEGAL; /* mark compare hook 
        for "NestedStruct" as illegal */
    hooks.cmp = NULL;
    
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;
    ecs_set_hooks_id(world, nested_struct, &hooks);

    /* Define test_vec, as a vector of "NestedStruct".
     * TestStruct's cmp hook should be set to illegal as well. */
    ecs_vector_desc_t desc = {.entity = 0, .type = nested_struct};
    ecs_entity_t test_vec = ecs_vector_init(world, &desc);

    const ecs_type_info_t* test_vec_ti = ecs_get_type_info(world, test_vec);

    /* test_vec should have an illegal cmp hook too: */
    test_assert(test_vec_ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL);

    ecs_fini(world);
}

/* Configure an opaque type that consumes resources */
ecs_entity_t define_ResourceHandle_opaque(
    ecs_world_t *world) 
{
    ECS_COMPONENT(world, ResourceHandle);

    ecs_type_hooks_t hooks = *ecs_get_hooks(world, ResourceHandle);
    hooks.ctor = ResourceHandle_ctor;
    hooks.dtor = ResourceHandle_dtor;
    hooks.move = ResourceHandle_move;
    hooks.copy = ResourceHandle_copy;
    hooks.cmp = ResourceHandle_comp;
    hooks.flags &= ~ECS_TYPE_HOOK_CMP_ILLEGAL;
    hooks.flags &= ECS_TYPE_HOOKS_ILLEGAL;

    ecs_set_hooks_id(world, ecs_id(ResourceHandle), &hooks);

    /* Create struct type that describes the structure of ResourceHandle */
    ecs_entity_t resource_handle_descriptor = ecs_struct(world, {
        .members = {
            {.name = "value", .type = ecs_id(ecs_u32_t)},
        }
    });

    /* Register ResourceHandle as opaque type. */
    ecs_opaque(world, {
        .entity = ecs_id(ResourceHandle),
        .type = {.as_type = resource_handle_descriptor}
    });

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

/* Helper function used in the tests below to invoke a type's registered
 * constructor, if any, when adding items to vectors */
static 
void invoke_type_ctor(
    ecs_world_t *world, 
    void *ptr, 
    int32_t count,
    ecs_entity_t component) 
{
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    if (ti) {
        if (ti->hooks.ctor) {
            ti->hooks.ctor(ptr, count, ti);
        } else {
            ecs_os_memset(ptr, 0, ti->size * count);
        }
    }
}

/* Test RTT combinations */
void RuntimeTypes_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t a;
        ecs_i32_t b;
    } StructWithInts;

    ecs_entity_t struct_with_ints = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithInts *ptr1 = test_ecs_ensure(world, e, struct_with_ints);
    test_memory_zero(ptr1, sizeof(StructWithInts));
    ptr1->a = 100;
    ptr1->b = 101;

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithInts *ptr2 =
        ecs_get_id(world, instance, struct_with_ints);
    test_int(100, ptr2->a);
    test_int(101, ptr2->b);

    test_assert(cmp(world, struct_with_ints, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);

    const StructWithInts *ptr3 =
        ecs_get_id(world, instance, struct_with_ints);
    test_int(100, ptr3->a);
    test_int(101, ptr3->b);

    test_assert(cmp(world, struct_with_ints, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithStrings *ptr1 = test_ecs_ensure(world, e, struct_with_strings);
    test_memory_zero(ptr1, sizeof(StructWithStrings));
    ptr1->a = ecs_os_strdup("String100");
    ptr1->b = 101;
    ptr1->c = ecs_os_strdup("String102");


    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithStrings *ptr2 =
        ecs_get_id(world, instance, struct_with_strings);
    test_str("String100", ptr2->a);
    test_int(101, ptr2->b);
    test_str("String102", ptr2->c);

    test_assert(cmp(world, struct_with_strings, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithStrings *ptr3 =
        ecs_get_id(world, instance, struct_with_strings);
    test_str("String100", ptr3->a);
    test_int(101, ptr3->b);
    test_str("String102", ptr3->c);

    test_assert(cmp(world, struct_with_strings, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_struct_with_opaque(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t resource_handle_opaque = define_ResourceHandle_opaque(world);

    typedef struct {
        ResourceHandle a;
    } StructWithOpaque;
    
    ecs_entity_t struct_with_opaque = ecs_struct(world, {
        .members = {
            {"a", resource_handle_opaque},
        }
    });

    /* struct_with_opaque consumes 1 test resources per instance */
    const int initial_resources = 4;
    initialize_resource_ids(initial_resources);

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithOpaque *ptr1 = test_ecs_ensure(world, e, struct_with_opaque);
    test_assert(ptr1->a.id != 0);
    test_int(0, ptr1->a.value);
    ptr1->a.value = 100;

    /* 1 resource(s) should have been used */
    test_int(1, initial_resources - resources_left());

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithOpaque *ptr2 =
        ecs_get_id(world, instance, struct_with_opaque);
    test_int(100, ptr2->a.value);

    test_assert(cmp(world, struct_with_opaque, ptr1, ptr2) == 0);

    /* 2 resource(s) should be in use now */
    test_int(2, initial_resources - resources_left());

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithOpaque *ptr3 =
        ecs_get_id(world, instance, struct_with_opaque);
    test_int(100, ptr3->a.value);

    test_assert(cmp(world, struct_with_opaque, ptr1, ptr3) == 0);

    /* 2 resource(s) should still be in use after a move */
    test_int(2, initial_resources - resources_left());

    /* Test deleting: */
    ecs_delete(world, e);

    /* After destroying the first entity, only 1 resource(s) should be in use */
    test_int(1, initial_resources - resources_left());
    ecs_delete(world, instance);

    /* check if all 2 test resources were returned and exactly all resource ids
     * are back: */
    test_int(initial_resources, resources_left());
    int i;
    for (i = 1; i <= initial_resources; i++) {
        test_assert(resource_id_available(i));
    }

    ecs_fini(world);

    free_resource_ids();
}

void RuntimeTypes_nested_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }});

    typedef struct {
        StructWithStrings a;
        ecs_i32_t b;
        StructWithStrings c;
    } NestedStructWithStrings;

    ecs_entity_t nested_struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", struct_with_strings},
            {"b", ecs_id(ecs_i32_t)},
            {"c", struct_with_strings},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    NestedStructWithStrings *ptr1 =
        test_ecs_ensure(world, e, nested_struct_with_strings);
    test_memory_zero(ptr1, sizeof(NestedStructWithStrings));
    ptr1->a.a = ecs_os_strdup("String100");
    ptr1->a.b = 101;
    ptr1->a.c = ecs_os_strdup("String102");
    ptr1->b = 103;
    ptr1->c.a = ecs_os_strdup("String104");
    ptr1->c.b = 105;
    ptr1->c.c = ecs_os_strdup("String106");

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const NestedStructWithStrings *ptr2 =
        ecs_get_id(world, instance, nested_struct_with_strings);
    test_str("String100", ptr2->a.a);
    test_int(101, ptr2->a.b);
    test_str("String102", ptr2->a.c);
    test_int(103, ptr2->b);
    test_str("String104", ptr2->c.a);
    test_int(105, ptr2->c.b);
    test_str("String106", ptr2->c.c);

    test_assert(cmp(world, nested_struct_with_strings, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const NestedStructWithStrings *ptr3 =
        ecs_get_id(world, instance, nested_struct_with_strings);
    test_str("String100", ptr3->a.a);
    test_int(101, ptr3->a.b);
    test_str("String102", ptr3->a.c);
    test_int(103, ptr3->b);
    test_str("String104", ptr3->c.a);
    test_int(105, ptr3->c.b);
    test_str("String106", ptr3->c.c);

    test_assert(cmp(world, nested_struct_with_strings, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_struct_with_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    typedef struct {
        ecs_string_t a[3];
        ecs_i32_t b;
    } StructWithArrayOfStrings;
   
    ecs_entity_t struct_with_array_of_strings = ecs_struct(world, {
        .members = {
            {"a", array_of_strings},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithArrayOfStrings *ptr1 =
        test_ecs_ensure(world, e, struct_with_array_of_strings);
    test_memory_zero(ptr1, sizeof(StructWithArrayOfStrings));
    ptr1->a[0] = ecs_os_strdup("String100");
    ptr1->a[1] = ecs_os_strdup("String101");
    ptr1->a[2] = ecs_os_strdup("String102");
    ptr1->b = 103;

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithArrayOfStrings *ptr2 =
        ecs_get_id(world, instance, struct_with_array_of_strings);
    test_str("String100", ptr2->a[0]);
    test_str("String101", ptr2->a[1]);
    test_str("String102", ptr2->a[2]);
    test_int(103, ptr2->b);

    test_assert(cmp(world, struct_with_array_of_strings, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithArrayOfStrings *ptr3 =
        ecs_get_id(world, instance, struct_with_array_of_strings);
    test_str("String100", ptr3->a[0]);
    test_str("String101", ptr3->a[1]);
    test_str("String102", ptr3->a[2]);
    test_int(103, ptr3->b);

    test_assert(cmp(world, struct_with_array_of_strings, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_struct_with_array_of_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    ecs_entity_t /* ecs_string_t[3][3] */ array_of_array_of_strings =
        ecs_array(world, {.type = array_of_strings, .count = 3});

    typedef struct {
        ecs_string_t a[3][3];
        ecs_string_t b;
    } StructWithArrayOfArrayOfStrings;

    ecs_entity_t struct_with_array_of_array_of_strings = ecs_struct(world, {
        .members = {
            {"a", array_of_array_of_strings},
            {"b", ecs_id(ecs_string_t)},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithArrayOfArrayOfStrings *ptr1 =
        test_ecs_ensure(world, e, struct_with_array_of_array_of_strings);
    test_memory_zero(ptr1, sizeof(StructWithArrayOfArrayOfStrings));
    int i;
    for (i = 0; i < 3; i++) {
        ptr1->a[i][0] = ecs_os_strdup("String100");
        ptr1->a[i][1] = ecs_os_strdup("String101");
        ptr1->a[i][2] = ecs_os_strdup("String102");
    }
    ptr1->b = ecs_os_strdup("String103");

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithArrayOfArrayOfStrings *ptr2 =
        ecs_get_id(world, instance, struct_with_array_of_array_of_strings);
    for (i = 0; i < 3; i++) {
        test_str("String100", ptr2->a[i][0]);
        test_str("String101", ptr2->a[i][1]);
        test_str("String102", ptr2->a[i][2]);
    }
    test_str("String103", ptr2->b);

    test_assert(cmp(world, struct_with_array_of_array_of_strings, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithArrayOfArrayOfStrings *ptr3 =
        ecs_get_id(world, instance, struct_with_array_of_array_of_strings);
    for (i = 0; i < 3; i++) {
        test_str("String100", ptr3->a[i][0]);
        test_str("String101", ptr3->a[i][1]);
        test_str("String102", ptr3->a[i][2]);
    }
    test_str("String103", ptr3->b);

    test_assert(cmp(world, struct_with_array_of_array_of_strings, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_struct_with_vector_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    typedef struct {
        ecs_vec_t a;
    } StructWithVectorOfInts;

    ecs_entity_t struct_with_vector_of_ints = ecs_struct(world, {
        .members = {
            {"a", vector_of_ints},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithVectorOfInts *ptr1 = test_ecs_ensure(world, e, struct_with_vector_of_ints);
    test_int(0, ecs_vec_count(&ptr1->a));
    ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_i32_t), 3);
    ecs_i32_t *va1 = ecs_vec_first(&ptr1->a);
    invoke_type_ctor(world, va1, 3, ecs_id(ecs_i32_t));
    test_memory_zero(va1, sizeof(ecs_i32_t) * 3);
    va1[0] = 100;
    va1[1] = 101;
    va1[2] = 102;

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithVectorOfInts *ptr2 = ecs_get_id(world, instance, struct_with_vector_of_ints);
    test_int(3, ecs_vec_count(&ptr2->a));
    const ecs_i32_t *va2 = ecs_vec_first(&ptr2->a);
    test_assert(va2 != NULL);
    test_int(100, va2[0]);
    test_int(101, va2[1]);
    test_int(102, va2[2]);

    test_assert(cmp(world, struct_with_vector_of_ints, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithVectorOfInts *ptr3 = ecs_get_id(world, instance, struct_with_vector_of_ints);
    test_int(3, ecs_vec_count(&ptr3->a));
    const ecs_i32_t *va3 = ecs_vec_first(&ptr3->a);
    test_assert(va3 != NULL);
    test_int(100, va3[0]);
    test_int(101, va3[1]);
    test_int(102, va3[2]);

    test_assert(cmp(world, struct_with_vector_of_ints, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_struct_with_vector_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    typedef struct {
        ecs_vec_t a;
    } StructWithVectorOfStrings;

    ecs_entity_t struct_with_vector_of_strings = ecs_struct(world, {
        .members = {
            {"a", vector_of_strings},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithVectorOfStrings *ptr1 =
        test_ecs_ensure(world, e, struct_with_vector_of_strings);
    test_int(0, ecs_vec_count(&ptr1->a));
    ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_string_t), 3);
    ecs_string_t *va1 = ecs_vec_first(&ptr1->a);
    invoke_type_ctor(world, va1, 3, ecs_id(ecs_string_t));
    test_memory_zero(va1, sizeof(ecs_string_t) * 3);
    va1[0] = ecs_os_strdup("String100");
    va1[1] = ecs_os_strdup("String101");
    va1[2] = ecs_os_strdup("String102");

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithVectorOfStrings *ptr2 =
        ecs_get_id(world, instance, struct_with_vector_of_strings);
    test_int(3, ecs_vec_count(&ptr2->a));
    const ecs_string_t *va2 = ecs_vec_first(&ptr2->a);
    test_assert(va2 != NULL);
    test_str("String100", va2[0]);
    test_str("String101", va2[1]);
    test_str("String102", va2[2]);

    test_assert(cmp(world, struct_with_vector_of_strings, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithVectorOfStrings *ptr3 =
        ecs_get_id(world, instance, struct_with_vector_of_strings);
    test_int(3, ecs_vec_count(&ptr3->a));
    const ecs_string_t *va3 = ecs_vec_first(&ptr3->a);
    test_assert(va3 != NULL);
    test_str("String100", va3[0]);
    test_str("String101", va3[1]);
    test_str("String102", va3[2]);

    test_assert(cmp(world, struct_with_vector_of_strings, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_nested_struct_with_vector_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    typedef struct {
        ecs_vec_t a;
        ecs_i32_t b;
        ecs_vec_t c;
    } InnerStruct1;

    ecs_entity_t inner_struct_1 = ecs_struct(world, {
        .members = {
            {"a", vector_of_ints},
            {"b", ecs_id(ecs_i32_t)},
            {"c", vector_of_ints},
        }
    });

    typedef struct {
        ecs_vec_t a;
        ecs_i32_t b;
        InnerStruct1 c;
    } NestedStructWithVectorOfInts;

    ecs_entity_t nested_struct_with_vector_of_ints = ecs_struct(world, {
        .members = {
            {"a", vector_of_ints},
            {"b", ecs_id(ecs_i32_t)},
            {"c", inner_struct_1},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    NestedStructWithVectorOfInts *ptr1 =
        test_ecs_ensure(world, e, nested_struct_with_vector_of_ints);
    {
        test_int(0, ecs_vec_count(&ptr1->a));
        ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_i32_t), 3);
        ecs_i32_t *va = ecs_vec_first(&ptr1->a);
        invoke_type_ctor(world, va, 3, ecs_id(ecs_i32_t));
        test_memory_zero(va, sizeof(ecs_i32_t) * 3);
        va[0] = 100;
        va[1] = 101;
        va[2] = 102;
    }
    test_int(0, ptr1->b);
    ptr1->b = 103;
    {
        test_int(0, ecs_vec_count(&ptr1->c.a));
        ecs_vec_set_count(NULL, &ptr1->c.a, sizeof(ecs_i32_t), 3);
        ecs_i32_t *vca = ecs_vec_first(&ptr1->c.a);
        invoke_type_ctor(world, vca, 3, ecs_id(ecs_i32_t));
        test_memory_zero(vca, sizeof(ecs_i32_t) * 3);
        vca[0] = 104;
        vca[1] = 105;
        vca[2] = 106;
    }
    test_int(0, ptr1->c.b);
    ptr1->c.b = 107;
    {
        test_int(0, ecs_vec_count(&ptr1->c.c));
        ecs_vec_set_count(NULL, &ptr1->c.c, sizeof(ecs_i32_t), 3);
        ecs_i32_t *vcc = ecs_vec_first(&ptr1->c.c);
        invoke_type_ctor(world, vcc, 3, ecs_id(ecs_i32_t));
        test_memory_zero(vcc, sizeof(ecs_i32_t) * 3);
        vcc[0] = 108;
        vcc[1] = 109;
        vcc[2] = 110;
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const NestedStructWithVectorOfInts *ptr2 =
        ecs_get_id(world, instance, nested_struct_with_vector_of_ints);
    {
        test_int(3, ecs_vec_count(&ptr2->a));
        const ecs_i32_t *va = ecs_vec_first(&ptr2->a);
        test_assert(va != NULL);
        test_int(100, va[0]);
        test_int(101, va[1]);
        test_int(102, va[2]);
    }
    test_int(103, ptr2->b);
    {
        test_int(3, ecs_vec_count(&ptr2->c.a));
        const ecs_i32_t *vca = ecs_vec_first(&ptr2->c.a);
        test_assert(vca != NULL);
        test_int(104, vca[0]);
        test_int(105, vca[1]);
        test_int(106, vca[2]);
    }
    test_int(107, ptr2->c.b);
    {
        test_int(3, ecs_vec_count(&ptr2->c.c));
        const ecs_i32_t *vcc = ecs_vec_first(&ptr2->c.c);
        test_assert(vcc != NULL);
        test_int(108, vcc[0]);
        test_int(109, vcc[1]);
        test_int(110, vcc[2]);
    }

    test_assert(cmp(world, nested_struct_with_vector_of_ints, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const NestedStructWithVectorOfInts *ptr3 =
        ecs_get_id(world, instance, nested_struct_with_vector_of_ints);
    {
        test_int(3, ecs_vec_count(&ptr3->a));
        const ecs_i32_t *va = ecs_vec_first(&ptr3->a);
        test_assert(va != NULL);
        test_int(100, va[0]);
        test_int(101, va[1]);
        test_int(102, va[2]);
    }
    test_int(103, ptr3->b);
    {
        test_int(3, ecs_vec_count(&ptr3->c.a));
        const ecs_i32_t *vca = ecs_vec_first(&ptr3->c.a);
        test_assert(vca != NULL);
        test_int(104, vca[0]);
        test_int(105, vca[1]);
        test_int(106, vca[2]);
    }
    test_int(107, ptr3->c.b);
    {
        test_int(3, ecs_vec_count(&ptr3->c.c));
        const ecs_i32_t *vcc = ecs_vec_first(&ptr3->c.c);
        test_assert(vcc != NULL);
        test_int(108, vcc[0]);
        test_int(109, vcc[1]);
        test_int(110, vcc[2]);
    }

    test_assert(cmp(world, nested_struct_with_vector_of_ints, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_nested_struct_with_vector_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    typedef struct {
        ecs_vec_t a;
        ecs_i32_t b;
        ecs_vec_t c;
    } InnerStruct2;

    ecs_entity_t inner_struct_2 = ecs_struct(world, {
        .members = {
            {"a", vector_of_strings},
            {"b", ecs_id(ecs_i32_t)},
            {"c", vector_of_strings},
        }
    });

    typedef struct {
        ecs_vec_t a;
        ecs_i32_t b;
        InnerStruct2 c;
    } NestedStructWithVectorOfStrings;

    ecs_entity_t nested_struct_with_vector_of_strings = ecs_struct(world, {
        .members = {
            {"a", vector_of_strings},
            {"b", ecs_id(ecs_i32_t)},
            {"c", inner_struct_2},
        }
    });

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    NestedStructWithVectorOfStrings *ptr1 =
        test_ecs_ensure(world, e, nested_struct_with_vector_of_strings);
    {
        test_int(0, ecs_vec_count(&ptr1->a));
        ecs_vec_set_count(NULL, &ptr1->a, sizeof(ecs_string_t), 3);
        ecs_string_t *va = ecs_vec_first(&ptr1->a);
        invoke_type_ctor(world, va, 3, ecs_id(ecs_string_t));
        test_memory_zero(va, sizeof(ecs_string_t) * 3);
        va[0] = ecs_os_strdup("String100");
        va[1] = ecs_os_strdup("String101");
        va[2] = ecs_os_strdup("String102");
    }
    test_int(0, ptr1->b);
    ptr1->b = 103;
    {
        test_int(0, ecs_vec_count(&ptr1->c.a));
        ecs_vec_set_count(NULL, &ptr1->c.a, sizeof(ecs_string_t), 3);
        ecs_string_t *vca = ecs_vec_first(&ptr1->c.a);
        invoke_type_ctor(world, vca, 3, ecs_id(ecs_string_t));
        test_memory_zero(vca, sizeof(ecs_string_t) * 3);
        vca[0] = ecs_os_strdup("String104");
        vca[1] = ecs_os_strdup("String105");
        vca[2] = ecs_os_strdup("String106");
    }
    test_int(0, ptr1->c.b);
    ptr1->c.b = 107;
    {
        test_int(0, ecs_vec_count(&ptr1->c.c));
        ecs_vec_set_count(NULL, &ptr1->c.c, sizeof(ecs_string_t), 3);
        ecs_string_t *vcc = ecs_vec_first(&ptr1->c.c);
        invoke_type_ctor(world, vcc, 3, ecs_id(ecs_string_t));
        test_memory_zero(vcc, sizeof(ecs_string_t) * 3);
        vcc[0] = ecs_os_strdup("String108");
        vcc[1] = ecs_os_strdup("String109");
        vcc[2] = ecs_os_strdup("String110");
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const NestedStructWithVectorOfStrings *ptr2 =
        ecs_get_id(world, instance, nested_struct_with_vector_of_strings);
    {
        test_int(3, ecs_vec_count(&ptr2->a));
        const ecs_string_t *va = ecs_vec_first(&ptr2->a);
        test_assert(va != NULL);
        test_str("String100", va[0]);
        test_str("String101", va[1]);
        test_str("String102", va[2]);
    }
    test_int(103, ptr2->b);
    {
        test_int(3, ecs_vec_count(&ptr2->c.a));
        const ecs_string_t *vca = ecs_vec_first(&ptr2->c.a);
        test_assert(vca != NULL);
        test_str("String104", vca[0]);
        test_str("String105", vca[1]);
        test_str("String106", vca[2]);
    }
    test_int(107, ptr2->c.b);
    {
        test_int(3, ecs_vec_count(&ptr2->c.c));
        const ecs_string_t *vcc = ecs_vec_first(&ptr2->c.c);
        test_assert(vcc != NULL);
        test_str("String108", vcc[0]);
        test_str("String109", vcc[1]);
        test_str("String110", vcc[2]);
    }

    test_assert(cmp(world, nested_struct_with_vector_of_strings, ptr1, ptr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const NestedStructWithVectorOfStrings *ptr3 =
        ecs_get_id(world, instance, nested_struct_with_vector_of_strings);
    {
        test_int(3, ecs_vec_count(&ptr3->a));
        const ecs_string_t *va = ecs_vec_first(&ptr3->a);
        test_assert(va != NULL);
        test_str("String100", va[0]);
        test_str("String101", va[1]);
        test_str("String102", va[2]);
    }
    test_int(103, ptr3->b);
    {
        test_int(3, ecs_vec_count(&ptr3->c.a));
        const ecs_string_t *vca = ecs_vec_first(&ptr3->c.a);
        test_assert(vca != NULL);
        test_str("String104", vca[0]);
        test_str("String105", vca[1]);
        test_str("String106", vca[2]);
    }
    test_int(107, ptr3->c.b);
    {
        test_int(3, ecs_vec_count(&ptr3->c.c));
        const ecs_string_t *vcc = ecs_vec_first(&ptr3->c.c);
        test_assert(vcc != NULL);
        test_str("String108", vcc[0]);
        test_str("String109", vcc[1]);
        test_str("String110", vcc[2]);
    }

    test_assert(cmp(world, nested_struct_with_vector_of_strings, ptr1, ptr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_i32_t[3] */ array_of_ints =
        ecs_array(world, {.type = ecs_id(ecs_i32_t), .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_i32_t *arr1 = test_ecs_ensure(world, e, array_of_ints);
    test_memory_zero(arr1, sizeof(ecs_i32_t[3]));
    arr1[0] = 100;
    arr1[1] = 101;
    arr1[2] = 102;

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_i32_t *arr2 = ecs_get_id(world, e, array_of_ints);
    test_int(100, arr2[0]);
    test_int(101, arr2[1]);
    test_int(102, arr2[2]);

    test_assert(cmp(world, array_of_ints, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_i32_t *arr3 = ecs_get_id(world, e, array_of_ints);
    test_int(100, arr3[0]);
    test_int(101, arr3[1]);
    test_int(102, arr3[2]);

    test_assert(cmp(world, array_of_ints, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_string_t *arr1 = test_ecs_ensure(world, e, array_of_strings);
    test_memory_zero(arr1, sizeof(ecs_string_t[3]));
    arr1[0] = ecs_os_strdup("String100");
    arr1[1] = ecs_os_strdup("String101");
    arr1[2] = ecs_os_strdup("String102");

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_string_t *arr2 = ecs_get_id(world, e, array_of_strings);
    test_str("String100", arr2[0]);
    test_str("String101", arr2[1]);
    test_str("String102", arr2[2]);

    test_assert(cmp(world, array_of_strings, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_string_t *arr3 = ecs_get_id(world, e, array_of_strings);
    test_str("String100", arr3[0]);
    test_str("String101", arr3[1]);
    test_str("String102", arr3[2]);

    test_assert(cmp(world, array_of_strings, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t a;
        ecs_i32_t b;
    } StructWithInts;

    ecs_entity_t struct_with_ints = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t /* StructWithInts[3] */ array_of_struct_with_ints =
        ecs_array(world, {.type = struct_with_ints, .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithInts *arr1 =
        test_ecs_ensure(world, e, array_of_struct_with_ints);
    test_memory_zero(arr1, sizeof(StructWithInts[3]));
    arr1[0].a = 100;
    arr1[0].b = 101;
    arr1[1].a = 102;
    arr1[1].b = 103;
    arr1[2].a = 104;
    arr1[2].b = 105;

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithInts *arr2 =
        ecs_get_id(world, e, array_of_struct_with_ints);
    test_int(100, arr2[0].a);
    test_int(101, arr2[0].b);
    test_int(102, arr2[1].a);
    test_int(103, arr2[1].b);
    test_int(104, arr2[2].a);
    test_int(105, arr2[2].b);

    test_assert(cmp(world, array_of_struct_with_ints, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithInts *arr3 =
        ecs_get_id(world, e, array_of_struct_with_ints);
    test_int(100, arr3[0].a);
    test_int(101, arr3[0].b);
    test_int(102, arr3[1].a);
    test_int(103, arr3[1].b);
    test_int(104, arr3[2].a);
    test_int(105, arr3[2].b);

    test_assert(cmp(world, array_of_struct_with_ints, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    ecs_entity_t /* StructWithStrings[3] */ array_of_struct_with_strings =
        ecs_array(world, {.type = struct_with_strings, .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithStrings *arr1 =
        test_ecs_ensure(world, e, array_of_struct_with_strings);
    test_memory_zero(arr1, sizeof(StructWithStrings[3]));
    arr1[0].a = ecs_os_strdup("String100");
    arr1[0].b = 101;
    arr1[0].c = ecs_os_strdup("String102");
    arr1[1].a = ecs_os_strdup("String103");
    arr1[1].b = 104;
    arr1[1].c = ecs_os_strdup("String105");
    arr1[2].a = ecs_os_strdup("String106");
    arr1[2].b = 107;
    arr1[2].c = ecs_os_strdup("String108");

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithStrings *arr2 =
        ecs_get_id(world, e, array_of_struct_with_strings);
    test_str("String100", arr2[0].a);
    test_int(101, arr2[0].b);
    test_str("String102", arr2[0].c);
    test_str("String103", arr2[1].a);
    test_int(104, arr2[1].b);
    test_str("String105", arr2[1].c);
    test_str("String106", arr2[2].a);
    test_int(107, arr2[2].b);
    test_str("String108", arr2[2].c);

    test_assert(cmp(world, array_of_struct_with_strings, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithStrings *arr3 =
        ecs_get_id(world, e, array_of_struct_with_strings);
    test_str("String100", arr3[0].a);
    test_int(101, arr3[0].b);
    test_str("String102", arr3[0].c);
    test_str("String103", arr3[1].a);
    test_int(104, arr3[1].b);
    test_str("String105", arr3[1].c);
    test_str("String106", arr3[2].a);
    test_int(107, arr3[2].b);
    test_str("String108", arr3[2].c);

    test_assert(cmp(world, array_of_struct_with_strings, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_struct_with_opaques(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t resource_handle_opaque = define_ResourceHandle_opaque(world);

    typedef struct {
        ResourceHandle a;
    } StructWithOpaque;

    ecs_entity_t struct_with_opaque = ecs_struct(world, {
        .members = {
            {"a", resource_handle_opaque},
        }
    });

    ecs_entity_t /* StructWithOpaque[3] */ array_of_struct_with_opaques =
        ecs_array(world, {.type = struct_with_opaque, .count = 3});

    /* array_of_struct_with_opaques consumes 3 test resources per instance */
    const int initial_resources = 12;
    initialize_resource_ids(initial_resources);

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithOpaque *arr1 =
        test_ecs_ensure(world, e, array_of_struct_with_opaques);
    test_assert(arr1[0].a.id != 0);
    test_int(0, arr1[0].a.value);
    arr1[0].a.value = 100;
    test_assert(arr1[1].a.id != 0);
    test_int(0, arr1[1].a.value);
    arr1[1].a.value = 101;
    test_assert(arr1[2].a.id != 0);
    test_int(0, arr1[2].a.value);
    arr1[2].a.value = 102;

    /* 3 resource(s) should have been used */
    test_int(3, initial_resources - resources_left());

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithOpaque *arr2 =
        ecs_get_id(world, e, array_of_struct_with_opaques);
    test_int(100, arr2[0].a.value);
    test_int(101, arr2[1].a.value);
    test_int(102, arr2[2].a.value);

    test_assert(cmp(world, array_of_struct_with_opaques, arr1, arr2) == 0);

    /* 6 resource(s) should be in use now */
    test_int(6, initial_resources - resources_left());

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithOpaque *arr3 =
        ecs_get_id(world, e, array_of_struct_with_opaques);
    test_int(100, arr3[0].a.value);
    test_int(101, arr3[1].a.value);
    test_int(102, arr3[2].a.value);

    test_assert(cmp(world, array_of_struct_with_opaques, arr1, arr3) == 0);

    /* 6 resource(s) should still be in use after a move */
    test_int(6, initial_resources - resources_left());

    /* Test deleting: */
    ecs_delete(world, e);

    /* After destroying the first entity, only 3 resource(s) should be in use */
    test_int(3, initial_resources - resources_left());
    ecs_delete(world, instance);

    /* check if all 6 test resources were returned and exactly all resource ids
     * are back: */
    test_int(initial_resources, resources_left());
    int i;
    for (i = 1; i <= initial_resources; i++) {
        test_assert(resource_id_available(i));
    }

    ecs_fini(world);

    free_resource_ids();
}

void RuntimeTypes_array_of_array_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    ecs_entity_t /* ecs_string_t[3][3] */ array_of_array_of_strings =
        ecs_array(world, {.type = array_of_strings, .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_string_t(*arr1)[3] =
        test_ecs_ensure(world, e, array_of_array_of_strings);
    test_memory_zero(arr1, sizeof(ecs_string_t[3][3]));
    int i;
    for (i = 0; i < 3; i++) {
        arr1[i][0] = ecs_os_strdup("String100");
        arr1[i][1] = ecs_os_strdup("String101");
        arr1[i][2] = ecs_os_strdup("String102");
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_string_t(*arr2)[3] = (const ecs_string_t(*)[3]) ecs_get_id(
        world, e, array_of_array_of_strings);
    for (i = 0; i < 3; i++) {
        test_str("String100", arr2[i][0]);
        test_str("String101", arr2[i][1]);
        test_str("String102", arr2[i][2]);
    }

    test_assert(cmp(world, array_of_array_of_strings, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_string_t(*arr3)[3] = (const ecs_string_t(*)[3]) ecs_get_id(
        world, e, array_of_array_of_strings);
    for (i = 0; i < 3; i++) {
        test_str("String100", arr3[i][0]);
        test_str("String101", arr3[i][1]);
        test_str("String102", arr3[i][2]);
    }

    test_assert(cmp(world, array_of_array_of_strings, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_array_of_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    ecs_entity_t /* StructWithStrings[3] */ array_of_struct_with_strings =
        ecs_array(world, {.type = struct_with_strings, .count = 3});

    ecs_entity_t /* StructWithStrings[3][3] */
        array_of_array_of_struct_with_strings = ecs_array(
            world, {.type = array_of_struct_with_strings, .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    StructWithStrings(*arr1)[3] =
        test_ecs_ensure(world, e, array_of_array_of_struct_with_strings);
    test_memory_zero(arr1, sizeof(StructWithStrings[3][3]));
    int i;
    for (i = 0; i < 3; i++) {
        arr1[i][0].a = ecs_os_strdup("String100");
        arr1[i][0].b = 101;
        arr1[i][0].c = ecs_os_strdup("String102");
        arr1[i][1].a = ecs_os_strdup("String103");
        arr1[i][1].b = 104;
        arr1[i][1].c = ecs_os_strdup("String105");
        arr1[i][2].a = ecs_os_strdup("String106");
        arr1[i][2].b = 107;
        arr1[i][2].c = ecs_os_strdup("String108");
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const StructWithStrings(*arr2)[3] =
        (const StructWithStrings(*)[3]) ecs_get_id(
            world, e, array_of_array_of_struct_with_strings);
    for (i = 0; i < 3; i++) {
        test_str("String100", arr2[i][0].a);
        test_int(101, arr2[i][0].b);
        test_str("String102", arr2[i][0].c);
        test_str("String103", arr2[i][1].a);
        test_int(104, arr2[i][1].b);
        test_str("String105", arr2[i][1].c);
        test_str("String106", arr2[i][2].a);
        test_int(107, arr2[i][2].b);
        test_str("String108", arr2[i][2].c);
    }

    test_assert(cmp(world, array_of_array_of_struct_with_strings, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const StructWithStrings(*arr3)[3] =
        (const StructWithStrings(*)[3]) ecs_get_id(
            world, e, array_of_array_of_struct_with_strings);
    for (i = 0; i < 3; i++) {
        test_str("String100", arr3[i][0].a);
        test_int(101, arr3[i][0].b);
        test_str("String102", arr3[i][0].c);
        test_str("String103", arr3[i][1].a);
        test_int(104, arr3[i][1].b);
        test_str("String105", arr3[i][1].c);
        test_str("String106", arr3[i][2].a);
        test_int(107, arr3[i][2].b);
        test_str("String108", arr3[i][2].c);
    }

    test_assert(cmp(world, array_of_array_of_struct_with_strings, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_vectors_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    ecs_entity_t /* ecs_vec_t[3] */ array_of_vectors_of_ints =
        ecs_array(world, {.type = vector_of_ints, .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *arr1 = test_ecs_ensure(world, e, array_of_vectors_of_ints);
    {
        test_int(0, ecs_vec_count(&arr1[0]));
        ecs_vec_set_count(NULL, &arr1[0], sizeof(ecs_i32_t), 3);
        ecs_i32_t *v = ecs_vec_first(&arr1[0]);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_i32_t));
        test_memory_zero(v, sizeof(ecs_i32_t) * 3);
        v[0] = 100;
        v[1] = 101;
        v[2] = 102;
    }
    {
        test_int(0, ecs_vec_count(&arr1[1]));
        ecs_vec_set_count(NULL, &arr1[1], sizeof(ecs_i32_t), 3);
        ecs_i32_t *v = ecs_vec_first(&arr1[1]);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_i32_t));
        test_memory_zero(v, sizeof(ecs_i32_t) * 3);
        v[0] = 103;
        v[1] = 104;
        v[2] = 105;
    }
    {
        test_int(0, ecs_vec_count(&arr1[2]));
        ecs_vec_set_count(NULL, &arr1[2], sizeof(ecs_i32_t), 3);
        ecs_i32_t *v = ecs_vec_first(&arr1[2]);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_i32_t));
        test_memory_zero(v, sizeof(ecs_i32_t) * 3);
        v[0] = 106;
        v[1] = 107;
        v[2] = 108;
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *arr2 = ecs_get_id(world, e, array_of_vectors_of_ints);
    {
        test_int(3, ecs_vec_count(&arr2[0]));
        const ecs_i32_t *v = ecs_vec_first(&arr2[0]);
        test_assert(v != NULL);
        test_int(100, v[0]);
        test_int(101, v[1]);
        test_int(102, v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr2[1]));
        const ecs_i32_t *v = ecs_vec_first(&arr2[1]);
        test_assert(v != NULL);
        test_int(103, v[0]);
        test_int(104, v[1]);
        test_int(105, v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr2[2]));
        const ecs_i32_t *v = ecs_vec_first(&arr2[2]);
        test_assert(v != NULL);
        test_int(106, v[0]);
        test_int(107, v[1]);
        test_int(108, v[2]);
    }

    test_assert(cmp(world, array_of_vectors_of_ints, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *arr3 = ecs_get_id(world, e, array_of_vectors_of_ints);
    {
        test_int(3, ecs_vec_count(&arr3[0]));
        const ecs_i32_t *v = ecs_vec_first(&arr3[0]);
        test_assert(v != NULL);
        test_int(100, v[0]);
        test_int(101, v[1]);
        test_int(102, v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr3[1]));
        const ecs_i32_t *v = ecs_vec_first(&arr3[1]);
        test_assert(v != NULL);
        test_int(103, v[0]);
        test_int(104, v[1]);
        test_int(105, v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr3[2]));
        const ecs_i32_t *v = ecs_vec_first(&arr3[2]);
        test_assert(v != NULL);
        test_int(106, v[0]);
        test_int(107, v[1]);
        test_int(108, v[2]);
    }

    test_assert(cmp(world, array_of_vectors_of_ints, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_vectors_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    ecs_entity_t /* ecs_vec_t[3] */ array_of_vectors_of_strings =
        ecs_array(world, {.type = vector_of_strings, .count = 3});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *arr1 = test_ecs_ensure(world, e, array_of_vectors_of_strings);
    {
        test_int(0, ecs_vec_count(&arr1[0]));
        ecs_vec_set_count(NULL, &arr1[0], sizeof(ecs_string_t), 3);
        ecs_string_t *v = ecs_vec_first(&arr1[0]);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_string_t));
        test_memory_zero(v, sizeof(ecs_string_t) * 3);
        v[0] = ecs_os_strdup("String100");
        v[1] = ecs_os_strdup("String101");
        v[2] = ecs_os_strdup("String102");
    }
    {
        test_int(0, ecs_vec_count(&arr1[1]));
        ecs_vec_set_count(NULL, &arr1[1], sizeof(ecs_string_t), 3);
        ecs_string_t *v = ecs_vec_first(&arr1[1]);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_string_t));
        test_memory_zero(v, sizeof(ecs_string_t) * 3);
        v[0] = ecs_os_strdup("String103");
        v[1] = ecs_os_strdup("String104");
        v[2] = ecs_os_strdup("String105");
    }
    {
        test_int(0, ecs_vec_count(&arr1[2]));
        ecs_vec_set_count(NULL, &arr1[2], sizeof(ecs_string_t), 3);
        ecs_string_t *v = ecs_vec_first(&arr1[2]);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_string_t));
        test_memory_zero(v, sizeof(ecs_string_t) * 3);
        v[0] = ecs_os_strdup("String106");
        v[1] = ecs_os_strdup("String107");
        v[2] = ecs_os_strdup("String108");
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *arr2 =
        ecs_get_id(world, e, array_of_vectors_of_strings);
    {
        test_int(3, ecs_vec_count(&arr2[0]));
        const ecs_string_t *v = ecs_vec_first(&arr2[0]);
        test_assert(v != NULL);
        test_str("String100", v[0]);
        test_str("String101", v[1]);
        test_str("String102", v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr2[1]));
        const ecs_string_t *v = ecs_vec_first(&arr2[1]);
        test_assert(v != NULL);
        test_str("String103", v[0]);
        test_str("String104", v[1]);
        test_str("String105", v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr2[2]));
        const ecs_string_t *v = ecs_vec_first(&arr2[2]);
        test_assert(v != NULL);
        test_str("String106", v[0]);
        test_str("String107", v[1]);
        test_str("String108", v[2]);
    }

    test_assert(cmp(world, array_of_vectors_of_strings, arr1, arr2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *arr3 =
        ecs_get_id(world, e, array_of_vectors_of_strings);
    {
        test_int(3, ecs_vec_count(&arr3[0]));
        const ecs_string_t *v = ecs_vec_first(&arr3[0]);
        test_assert(v != NULL);
        test_str("String100", v[0]);
        test_str("String101", v[1]);
        test_str("String102", v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr3[1]));
        const ecs_string_t *v = ecs_vec_first(&arr3[1]);
        test_assert(v != NULL);
        test_str("String103", v[0]);
        test_str("String104", v[1]);
        test_str("String105", v[2]);
    }
    {
        test_int(3, ecs_vec_count(&arr3[2]));
        const ecs_string_t *v = ecs_vec_first(&arr3[2]);
        test_assert(v != NULL);
        test_str("String106", v[0]);
        test_str("String107", v[1]);
        test_str("String108", v[2]);
    }

    test_assert(cmp(world, array_of_vectors_of_strings, arr1, arr3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_array_of_opaque(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t resource_handle_opaque = define_ResourceHandle_opaque(world);

    ecs_entity_t /* ResourceHandle[3] */ array_of_opaque =
        ecs_array(world, {.type = resource_handle_opaque, .count = 3});

    /* array_of_opaque consumes 3 test resources per instance */
    const int initial_resources = 12;
    initialize_resource_ids(initial_resources);

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ResourceHandle *arr1 = test_ecs_ensure(world, e, array_of_opaque);
    test_assert(arr1[0].id != 0);
    test_int(0, arr1[0].value);
    arr1[0].value = 100;
    test_assert(arr1[1].id != 0);
    test_int(0, arr1[1].value);
    arr1[1].value = 101;
    test_assert(arr1[2].id != 0);
    test_int(0, arr1[2].value);
    arr1[2].value = 102;

    /* 3 resource(s) should have been used */
    test_int(3, initial_resources - resources_left());

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ResourceHandle *arr2 = ecs_get_id(world, e, array_of_opaque);
    test_int(100, arr2[0].value);
    test_int(101, arr2[1].value);
    test_int(102, arr2[2].value);

    test_assert(cmp(world, array_of_opaque, arr1, arr2) == 0);

    /* 6 resource(s) should be in use now */
    test_int(6, initial_resources - resources_left());

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ResourceHandle *arr3 = ecs_get_id(world, e, array_of_opaque);
    test_int(100, arr3[0].value);
    test_int(101, arr3[1].value);
    test_int(102, arr3[2].value);

    test_assert(cmp(world, array_of_opaque, arr1, arr3) == 0);

    /* 6 resource(s) should still be in use after a move */
    test_int(6, initial_resources - resources_left());

    /* Test deleting: */
    ecs_delete(world, e);

    /* After destroying the first entity, only 3 resource(s) should be in use */
    test_int(3, initial_resources - resources_left());
    ecs_delete(world, instance);

    /* check if all 6 test resources were returned and exactly all resource ids
     * are back: */
    test_int(initial_resources, resources_left());
    int i;
    for (i = 1; i <= initial_resources; i++) {
        test_assert(resource_id_available(i));
    }

    ecs_fini(world);

    free_resource_ids();
}

void RuntimeTypes_vector_of_ints(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_i32_t> */ vector_of_ints =
        ecs_vector(world, {.type = ecs_id(ecs_i32_t)});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *vec1 = test_ecs_ensure(world, e, vector_of_ints);
    {
        test_int(0, ecs_vec_count(vec1));
        ecs_vec_set_count(NULL, vec1, sizeof(ecs_i32_t), 3);
        ecs_i32_t *v = ecs_vec_first(vec1);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_i32_t));
        test_memory_zero(v, sizeof(ecs_i32_t) * 3);
        v[0] = 100;
        v[1] = 101;
        v[2] = 102;
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *vec2 = ecs_get_id(world, e, vector_of_ints);
    {
        test_int(3, ecs_vec_count(vec2));
        const ecs_i32_t *v = ecs_vec_first(vec2);
        test_assert(v != NULL);
        test_int(100, v[0]);
        test_int(101, v[1]);
        test_int(102, v[2]);
    }

    test_assert(cmp(world, vector_of_ints, vec1, vec2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *vec3 = ecs_get_id(world, e, vector_of_ints);
    {
        test_int(3, ecs_vec_count(vec3));
        const ecs_i32_t *v = ecs_vec_first(vec3);
        test_assert(v != NULL);
        test_int(100, v[0]);
        test_int(101, v[1]);
        test_int(102, v[2]);
    }

    test_assert(cmp(world, vector_of_ints, vec1, vec3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_vector_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* vector<ecs_string_t> */ vector_of_strings =
        ecs_vector(world, {.type = ecs_id(ecs_string_t)});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *vec1 = test_ecs_ensure(world, e, vector_of_strings);
    {
        test_int(0, ecs_vec_count(vec1));
        ecs_vec_set_count(NULL, vec1, sizeof(ecs_string_t), 3);
        ecs_string_t *v = ecs_vec_first(vec1);
        invoke_type_ctor(world, v, 3, ecs_id(ecs_string_t));
        test_memory_zero(v, sizeof(ecs_string_t) * 3);
        v[0] = ecs_os_strdup("String100");
        v[1] = ecs_os_strdup("String101");
        v[2] = ecs_os_strdup("String102");
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *vec2 = ecs_get_id(world, e, vector_of_strings);
    {
        test_int(3, ecs_vec_count(vec2));
        const ecs_string_t *v = ecs_vec_first(vec2);
        test_assert(v != NULL);
        test_str("String100", v[0]);
        test_str("String101", v[1]);
        test_str("String102", v[2]);
    }

    test_assert(cmp(world, vector_of_strings, vec1, vec2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *vec3 = ecs_get_id(world, e, vector_of_strings);
    {
        test_int(3, ecs_vec_count(vec3));
        const ecs_string_t *v = ecs_vec_first(vec3);
        test_assert(v != NULL);
        test_str("String100", v[0]);
        test_str("String101", v[1]);
        test_str("String102", v[2]);
    }

    test_assert(cmp(world, vector_of_strings, vec1, vec3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_vector_of_struct_with_ints(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t a;
        ecs_i32_t b;
    } StructWithInts;

    ecs_entity_t struct_with_ints = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_i32_t)},
            {"b", ecs_id(ecs_i32_t)},
        }
    });

    ecs_entity_t /* vector<StructWithInts> */ vector_of_struct_with_ints =
        ecs_vector(world, {.type = struct_with_ints});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *vec1 = test_ecs_ensure(world, e, vector_of_struct_with_ints);
    {
        test_int(0, ecs_vec_count(vec1));
        ecs_vec_set_count(NULL, vec1, sizeof(StructWithInts), 3);
        StructWithInts *v = ecs_vec_first(vec1);
        invoke_type_ctor(world, v, 3, struct_with_ints);
        test_memory_zero(v, sizeof(StructWithInts) * 3);
        v[0].a = 100;
        v[0].b = 101;
        v[1].a = 102;
        v[1].b = 103;
        v[2].a = 104;
        v[2].b = 105;
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *vec2 = ecs_get_id(world, e, vector_of_struct_with_ints);
    {
        test_int(3, ecs_vec_count(vec2));
        const StructWithInts *v = ecs_vec_first(vec2);
        test_assert(v != NULL);
        test_int(100, v[0].a);
        test_int(101, v[0].b);
        test_int(102, v[1].a);
        test_int(103, v[1].b);
        test_int(104, v[2].a);
        test_int(105, v[2].b);
    }
    test_assert(cmp(world, vector_of_struct_with_ints, vec1, vec2) == 0);


    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *vec3 = ecs_get_id(world, e, vector_of_struct_with_ints);
    {
        test_int(3, ecs_vec_count(vec3));
        const StructWithInts *v = ecs_vec_first(vec2);
        test_assert(v != NULL);
        test_int(100, v[0].a);
        test_int(101, v[0].b);
        test_int(102, v[1].a);
        test_int(103, v[1].b);
        test_int(104, v[2].a);
        test_int(105, v[2].b);
    }
    test_assert(cmp(world, vector_of_struct_with_ints, vec1, vec3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_vector_of_struct_with_strings(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_string_t a;
        ecs_i32_t b;
        ecs_string_t c;
    } StructWithStrings;

    ecs_entity_t struct_with_strings = ecs_struct(world, {
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_i32_t)},
            {"c", ecs_id(ecs_string_t)},
        }
    });

    ecs_entity_t /* vector<StructWithStrings> */ vector_of_struct_with_strings =
        ecs_vector(world, {.type = struct_with_strings});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *vec1 = test_ecs_ensure(world, e, vector_of_struct_with_strings);
    {
        test_int(0, ecs_vec_count(vec1));
        ecs_vec_set_count(NULL, vec1, sizeof(StructWithStrings), 3);
        StructWithStrings *v = ecs_vec_first(vec1);
        invoke_type_ctor(world, v, 3, struct_with_strings);
        test_memory_zero(v, sizeof(StructWithStrings) * 3);
        v[0].a = ecs_os_strdup("String100");
        v[0].b = 101;
        v[0].c = ecs_os_strdup("String102");
        v[1].a = ecs_os_strdup("String103");
        v[1].b = 104;
        v[1].c = ecs_os_strdup("String105");
        v[2].a = ecs_os_strdup("String106");
        v[2].b = 107;
        v[2].c = ecs_os_strdup("String108");
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *vec2 = ecs_get_id(world, e, vector_of_struct_with_strings);
    {
        test_int(3, ecs_vec_count(vec2));
        const StructWithStrings *v = ecs_vec_first(vec2);
        test_assert(v != NULL);
        test_str("String100", v[0].a);
        test_int(101, v[0].b);
        test_str("String102", v[0].c);
        test_str("String103", v[1].a);
        test_int(104, v[1].b);
        test_str("String105", v[1].c);
        test_str("String106", v[2].a);
        test_int(107, v[2].b);
        test_str("String108", v[2].c);
    }
    test_assert(cmp(world, vector_of_struct_with_strings, vec1, vec2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *vec3 = ecs_get_id(world, e, vector_of_struct_with_strings);
    {
        test_int(3, ecs_vec_count(vec3));
        const StructWithStrings *v = ecs_vec_first(vec3);
        test_assert(v != NULL);
        test_str("String100", v[0].a);
        test_int(101, v[0].b);
        test_str("String102", v[0].c);
        test_str("String103", v[1].a);
        test_int(104, v[1].b);
        test_str("String105", v[1].c);
        test_str("String106", v[2].a);
        test_int(107, v[2].b);
        test_str("String108", v[2].c);
    }
    test_assert(cmp(world, vector_of_struct_with_strings, vec1, vec3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_vector_of_arrays_of_strings(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t /* ecs_string_t[3] */ array_of_strings =
        ecs_array(world, {.type = ecs_id(ecs_string_t), .count = 3});

    ecs_entity_t /* vector<ecs_string_t[3]> */ vector_of_arrays_of_strings =
        ecs_vector(world, {.type = array_of_strings});

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *vec1 = test_ecs_ensure(world, e, vector_of_arrays_of_strings);
    {
        test_int(0, ecs_vec_count(vec1));
        ecs_vec_set_count(NULL, vec1, sizeof(ecs_string_t[3]), 3);
        ecs_string_t(*v)[3] = ecs_vec_first(vec1);
        invoke_type_ctor(world, v, 3, array_of_strings);
        test_memory_zero(v, sizeof(ecs_string_t[3]) * 3);
        int i;
        for (i = 0; i < 3; i++) {
            v[i][0] = ecs_os_strdup("String100");
            v[i][1] = ecs_os_strdup("String101");
            v[i][2] = ecs_os_strdup("String102");
        }
    }

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *vec2 = ecs_get_id(world, e, vector_of_arrays_of_strings);
    {
        test_int(3, ecs_vec_count(vec2));
        const ecs_string_t(*v)[3] =
            (const ecs_string_t(*)[3]) ecs_vec_first(vec2);
        test_assert(v != NULL);
        int i;
        for (i = 0; i < 3; i++) {
            test_str("String100", v[i][0]);
            test_str("String101", v[i][1]);
            test_str("String102", v[i][2]);
        }
    }
    test_assert(cmp(world, vector_of_arrays_of_strings, vec1, vec2) == 0);

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *vec3 = ecs_get_id(world, e, vector_of_arrays_of_strings);
    {
        test_int(3, ecs_vec_count(vec3));
        const ecs_string_t(*v)[3] =
            (const ecs_string_t(*)[3]) ecs_vec_first(vec3);
        test_assert(v != NULL);
        int i;
        for (i = 0; i < 3; i++) {
            test_str("String100", v[i][0]);
            test_str("String101", v[i][1]);
            test_str("String102", v[i][2]);
        }
    }
    test_assert(cmp(world, vector_of_arrays_of_strings, vec1, vec3) == 0);

    /* Test deleting: */
    ecs_delete(world, e);
    ecs_delete(world, instance);

    ecs_fini(world);
}

void RuntimeTypes_vector_of_opaque(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t resource_handle_opaque = define_ResourceHandle_opaque(world);

    ecs_entity_t /* vector<ResourceHandle> */ vector_of_opaque =
        ecs_vector(world, {.type = resource_handle_opaque});

    /* vector_of_opaque consumes 3 test resources per instance */
    const int initial_resources = 12;
    initialize_resource_ids(initial_resources);

    /* Test constructor: */
    ecs_entity_t e = ecs_new(world);
    ecs_vec_t *vec1 = test_ecs_ensure(world, e, vector_of_opaque);
    {
        test_int(0, ecs_vec_count(vec1));
        ecs_vec_set_count(NULL, vec1, sizeof(ResourceHandle), 3);
        ResourceHandle *v = ecs_vec_first(vec1);
        invoke_type_ctor(world, v, 3, resource_handle_opaque);
        test_assert(v[0].id != 0);
        test_int(0, v[0].value);
        v[0].value = 100;
        test_assert(v[1].id != 0);
        test_int(0, v[1].value);
        v[1].value = 101;
        test_assert(v[2].id != 0);
        test_int(0, v[2].value);
        v[2].value = 102;
    }
    /* 3 resource(s) should have been used */
    test_int(3, initial_resources - resources_left());

    /* Test copying: */
    ecs_entity_t instance = ecs_clone(world, 0, e, true);
    const ecs_vec_t *vec2 = ecs_get_id(world, e, vector_of_opaque);
    {
        test_int(3, ecs_vec_count(vec2));
        const ResourceHandle *v = ecs_vec_first(vec2);
        test_assert(v != NULL);
        test_int(100, v[0].value);
        test_int(101, v[1].value);
        test_int(102, v[2].value);
    }
    test_assert(cmp(world, vector_of_opaque, vec1, vec2) == 0);

    /* 6 resource(s) should be in use now */
    test_int(6, initial_resources - resources_left());

    /* Test moving by forcing an archetype change: */
    ECS_TAG(world, MakeMeMove);
    ecs_add(world, e, MakeMeMove);
    const ecs_vec_t *vec3 = ecs_get_id(world, e, vector_of_opaque);
    {
        test_int(3, ecs_vec_count(vec3));
        const ResourceHandle *v = ecs_vec_first(vec3);
        test_assert(v != NULL);
        test_int(100, v[0].value);
        test_int(101, v[1].value);
        test_int(102, v[2].value);
    }
    test_assert(cmp(world, vector_of_opaque, vec1, vec3) == 0);

    /* 6 resource(s) should still be in use after a move */
    test_int(6, initial_resources - resources_left());

    /* Test deleting: */
    ecs_delete(world, e);

    /* After destroying the first entity, only 3 resource(s) should be in use */
    test_int(3, initial_resources - resources_left());
    ecs_delete(world, instance);

    /* check if all 6 test resources were returned and exactly all resource ids
     * are back: */
    test_int(initial_resources, resources_left());
    int i;
    for (i = 1; i <= initial_resources; i++) {
        test_assert(resource_id_available(i));
    }

    ecs_fini(world);

    free_resource_ids();
}

