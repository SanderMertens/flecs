#include <stdint.h>
#include <meta.h>

#define UINT32_PATTERN(x) (((uint32_t) (x)) + ((uint32_t) (x) << 8) + ((uint32_t) (x) << 16) + ((uint32_t) (x) << 24))

bool is_memory_filled_with(const void *ptr, size_t size, unsigned char byte) {
  const unsigned char *byte_ptr = (const unsigned char *) ptr;

  for (size_t i = 0; i < size; i++) {
    if (byte_ptr[i] != byte) {
      return false;
    }
  }

  return true;
}

// Test hooks for runtime trivial structs
// These structs have no special copy/move/dtor logic and are zero-initialized
// Trivial structs should get flecs default constructor and nothing else
void RuntimeTypes_trivial_struct(void) {
  ecs_world_t *world = ecs_init();

  // Create a component to use as subtype in the trivial struct
  // Create a new component:
  ecs_entity_t subtype = ecs_entity(world, {.name = "Some Subtype"});

  // Configure as a struct:
  ecs_struct(world, {.entity = subtype,
                     .members = {{.name = "x", .type = ecs_id(ecs_i32_t)}, {.name = "r", .type = ecs_id(ecs_f32_t)}}});

  // Create a new component:
  ecs_entity_t trivial_struct = ecs_entity(world, {.name = "Trivial Struct"});

  ecs_struct(world, {.entity = trivial_struct,
                     .members = {{.name = "a", .type = ecs_id(ecs_i32_t)},
                                 {.name = "b", .type = ecs_id(ecs_f32_t)},
                                 {.name = "s", .type = subtype}}});

  const ecs_type_info_t *ti = ecs_get_type_info(world, trivial_struct);

  test_assert(ti->hooks.copy == NULL);
  test_assert(ti->hooks.move == NULL);
  test_assert(ti->hooks.dtor == NULL);

  // Check if the ctor initialized everything to 0.
  // Do this plenty of times to make sure we don't get a zero-initialized struct out of sheer luck:
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
static void test_ctor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
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
static void test_dtor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
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
static void test_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
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
static void test_copy(void *dst_ptr, const void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
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

// Define a nested struct that will have one or more hooks depending on the test
typedef struct NestedStruct {
  uint32_t a;
  uint32_t b;
} NestedStruct;
static ecs_entity_t nested_struct;

const ecs_type_info_t *define_nested_struct(ecs_world_t *world) {
  nested_struct = ecs_struct(
      world, {.entity = ecs_entity(world, {.name = "NestedStruct"}),
              .members = {{.name = "a", .type = ecs_id(ecs_u32_t)}, {.name = "b", .type = ecs_id(ecs_u32_t)}}});
  return ecs_get_type_info(world, nested_struct);
}

// Define a test struct that will have automatically generated hooks.
typedef struct TestStruct {
  uint32_t x;
  NestedStruct s1;
  uint32_t y;
  NestedStruct s2;
} TestStruct;
static ecs_entity_t test_struct;

const ecs_type_info_t *define_test_struct(ecs_world_t *world) {
  test_struct = ecs_struct(world, {.entity = ecs_entity(world, {.name = "TestStruct"}),
                                   .members = {{.name = "x", .type = ecs_id(ecs_u32_t)},
                                               {.name = "s1", .type = nested_struct},
                                               {.name = "y", .type = ecs_id(ecs_u32_t)},
                                               {.name = "s2", .type = nested_struct}}});
  return ecs_get_type_info(world, test_struct);
}

// Tests that a constructor is generated for a struct if at least a member has itself a constructor
// Also tests if the generated constructor works.
void RuntimeTypes_ctor(void) {
  reset_test_ctor();
  ecs_world_t *world = ecs_init();

  // Define NestedStruct:
  const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

  ecs_type_hooks_t hooks = nested_struct_ti->hooks;
  hooks.ctor = test_ctor;  // only define a constructor for "NestedStruct"
  ecs_set_hooks_id(world, nested_struct, &hooks);

  // Define TestStruct, which has two "NestedStruct" members.
  // A constructor should be automatically generated for TestStruct to invoke NestedStruct's specific constructor:
  const ecs_type_info_t *test_struct_ti = define_test_struct(world);

  // TestStruct should only have a constructor:
  test_assert(test_struct_ti->hooks.ctor != NULL);

  // No other hooks should've been set:
  test_assert(test_struct_ti->hooks.dtor == NULL);
  test_assert(test_struct_ti->hooks.move == NULL);
  test_assert(test_struct_ti->hooks.copy == NULL);

  // Now instantiate TestStruct and see the constructor for NestedStruct is called twice, since TestStruct contains two
  // NestedStructs:
  ecs_entity_t e = ecs_new(world);

  test_int(0, ctor_calls);
  ecs_add_id(world, e, test_struct);
  test_int(2, ctor_calls);  // called twice because there are two "nested_struct"

  const TestStruct *ss = (TestStruct *) ecs_get_id(world, e, test_struct);

  uint32_t test_pattern = UINT32_PATTERN(ctor_pattern);
  test_int(0, ss->x);  // trivial field was zero-initialized
  test_int(0, ss->y);
  test_int(test_pattern, ss->s1.a);  // these should've been filled by test_ctor with a pattern.
  test_int(test_pattern, ss->s1.b);
  test_int(test_pattern, ss->s2.a);
  test_int(test_pattern, ss->s2.b);

  ecs_fini(world);
}

// Tests that a destructor is generated for a struct if at least a member has itself a destructor
// Also tests if the generated destructor works.
void RuntimeTypes_dtor(void) {
  reset_test_dtor();
  ecs_world_t *world = ecs_init();

  // Define NestedStruct:
  const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);

  ecs_type_hooks_t hooks = nested_struct_ti->hooks;
  hooks.dtor = test_dtor;  // only set a destructor

  ecs_set_hooks_id(world, nested_struct, &hooks);

  // Define TestStruct, which has two "NestedStruct" members.
  // A destructor should be automatically generated for TestStruct to invoke NestedStruct's specific destructor:
  const ecs_type_info_t *test_struct_ti = define_test_struct(world);

  // TestStruct should have a default constructor and the generated destructor:
  test_assert(test_struct_ti->hooks.ctor != NULL);
  test_assert(test_struct_ti->hooks.dtor != NULL);

  // No other hooks should've been set:
  test_assert(test_struct_ti->hooks.move == NULL);
  test_assert(test_struct_ti->hooks.copy == NULL);

  // Now instantiate TestStruct to test its destructor.
  ecs_entity_t e = ecs_new(world);

  test_int(0, dtor_calls);
  ecs_add_id(world, e, test_struct);
  test_int(0, dtor_calls);  // Destructor not called yet.

  const void *data = ecs_get_id(world, e, test_struct);
  is_memory_filled_with(data, test_struct_ti->size,
                        0);  // should be zero-initialized, since there are no special ctors.

  ecs_remove_id(world, e, test_struct);
  test_int(2, dtor_calls);  // should be called twice, since there are two "struct_with_dtor"

  ecs_fini(world);
}

// Tests that a move hook is generated for a struct if at least a member has itself a move hook
// Also tests if the generated move hook works.
void RuntimeTypes_move(void) {
  reset_test_move();
  ecs_world_t *world = ecs_init();
  define_nested_struct(world);

  // Define NestedStruct:
  const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);
  ecs_type_hooks_t hooks = nested_struct_ti->hooks;

  hooks.move = test_move;  // Only define a move hook.
  ecs_set_hooks_id(world, nested_struct, &hooks);

  // Define TestStruct, which has two "NestedStruct" members.
  // A move hook should be automatically generated for TestStruct to invoke NestedStruct's specific move hook:
  const ecs_type_info_t *test_struct_ti = define_test_struct(world);

  // TestStruct should have a default constructor and the generated move hook:
  test_assert(test_struct_ti->hooks.ctor != NULL);
  test_assert(test_struct_ti->hooks.move != NULL);

  // No other hooks should've been set:
  test_assert(test_struct_ti->hooks.dtor == NULL);
  test_assert(test_struct_ti->hooks.copy == NULL);

  ecs_entity_t e = ecs_new(world);

  test_int(0, move_calls);
  void *ptr = ecs_ensure_id(world, e, test_struct);
  test_int(0, move_calls);

  // use a cursor to set the x and s1.a field to specific numbers
  ecs_meta_cursor_t cur = ecs_meta_cursor(world, test_struct, ptr);
  ecs_meta_push(&cur);
  ecs_meta_member(&cur, "x");
  ecs_meta_set_int(&cur, 82);
  ecs_meta_member(&cur, "s1");
  ecs_meta_push(&cur);
  ecs_meta_member(&cur, "a");
  ecs_meta_set_int(&cur, 79);

  // now trigger a move by adding another component to e, forcing an archetype change:
  ecs_add_id(world, e, ecs_new(world));
  test_int(2, move_calls);  // should be called twice, since there are two structs with a move hook.

  const TestStruct *ss = (TestStruct *) ecs_get_id(world, e, test_struct);

  test_assert((void *) ss != ptr);  // should be a different memory location.

  test_int(82, ss->x);  // trivial move should have copied this value.
  test_int(0, ss->y);
  test_int(79, ss->s1.a);  // these should contain the specified value, done by the move hook.
  test_int(0, ss->s1.b);   // other fields should have been 0-initialized.
  test_int(0, ss->s2.a);
  test_int(0, ss->s2.b);

  ecs_fini(world);
}

// Tests that a copy hook is generated for a struct if at least a member has itself a copy hook
// Also tests if the generated copy hook works.
void RuntimeTypes_copy(void) {
  reset_test_copy();
  ecs_world_t *world = ecs_init();

  // Define NestedStruct:
  define_nested_struct(world);
  const ecs_type_info_t *nested_struct_ti = define_nested_struct(world);
  ecs_type_hooks_t hooks = nested_struct_ti->hooks;

  hooks.copy = test_copy;  // only set a copy hook
  ecs_set_hooks_id(world, nested_struct, &hooks);

  // Define TestStruct, which has two "NestedStruct" members.
  // A copy hook should be automatically generated for TestStruct to invoke NestedStruct's specific copy hook:
  const ecs_type_info_t *test_struct_ti = define_test_struct(world);

  // TestStruct should have a default constructor and the generated copy hook:
  test_assert(test_struct_ti->hooks.ctor != NULL);
  test_assert(test_struct_ti->hooks.copy != NULL);

  // No other hooks should've been set:
  test_assert(test_struct_ti->hooks.dtor == NULL);
  test_assert(test_struct_ti->hooks.move == NULL);

  // create a prefab entity:
  ecs_entity_t prefab = ecs_new(world);
  ecs_add_id(world, prefab, EcsPrefab);

  test_int(0, copy_calls);
  void *original_ptr = ecs_ensure_id(world, prefab, test_struct);
  test_int(0, copy_calls);

  // use a cursor to set the x and s1.a field to specific numbers
  ecs_meta_cursor_t cur = ecs_meta_cursor(world, test_struct, original_ptr);
  ecs_meta_push(&cur);
  ecs_meta_member(&cur, "x");
  ecs_meta_set_int(&cur, 82);
  ecs_meta_member(&cur, "s1");
  ecs_meta_push(&cur);
  ecs_meta_member(&cur, "a");
  ecs_meta_set_int(&cur, 79);

  // now trigger a copy by instantiating this prefab:
  ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, prefab);
  test_int(2, copy_calls);  // should be called twice, since there are two structs with a copy hook.

  const void *copied_ptr = ecs_get_id(world, instance, test_struct);

  test_assert((void *) copied_ptr != original_ptr);  // should be a different memory location.

  test_assert(0 == ecs_os_memcmp(original_ptr, copied_ptr, test_struct_ti->size));

  ecs_fini(world);
}

/* For the following tests, we model a "ResourceHandle" that must allocate an id from a pool of resource ids.
 * When constructing a handle, it must get a unique id.
 * When destroying a handle, it must return the id.
 * When copying a handle, it must copy the payload value and get a new unique id for the handle.
 * When moving the handle, it should keep the same id and payload value.
 * Two Resource Handles are considered "equal" if both have a valid id (id>0) and their payload value is the same
 */
typedef struct ResourceHandle {
  int id;
  uint32_t value;
} ResourceHandle;

// This vector will hold all available ids.
ecs_vec_t resource_ids;

// Initializes the available "resource ids"
void initialize_resource_ids(int num_resources) {
  ecs_vec_init_t(NULL, &resource_ids, int, num_resources);
  ecs_vec_set_count_t(NULL, &resource_ids, int, num_resources);
  for (int i = 0; i < num_resources; i++) {
    *ecs_vec_get_t(&resource_ids, int, i) = num_resources - i;
  }
}

void free_resource_ids() { ecs_vec_fini_t(NULL, &resource_ids, int); }

// Gets a resource id from the vector. It will return 0 if no more resources available.
int get_resource_id() {
  size_t idcount = ecs_vec_count(&resource_ids);
  if (idcount == 0)
    return 0;
  int id = *ecs_vec_get_t(&resource_ids, int, idcount - 1);
  ecs_vec_set_count_t(NULL, &resource_ids, int, idcount - 1);
  return id;
}

// Returns a resource for reuse.
void return_resource_id(int id) {
  if (id == 0)
    return;
  *(ecs_vec_append_t(NULL, &resource_ids, int)) = id;
}

// Returns true if a specific resource id is in the pool.
bool resource_id_available(int id) {
  for (int i = 0; i < ecs_vec_count(&resource_ids); i++) {
    if (id == *ecs_vec_get_t(&resource_ids, int, i))
      return true;
  }
  return false;
}

// Returns the numer of currently available resources
int available_resource_count() { return ecs_vec_count(&resource_ids); }

// Define a ResourceHandle constructor that sets the payload value to 0 and obtains a unique resource id:
static void ResourceHandle_ctor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  for (int i = 0; i < count; i++) {
    ResourceHandle *r = (ResourceHandle *) ECS_ELEM_T(ptr, ResourceHandle, i);
    r->id = get_resource_id();
    test_assert(r->id);
    r->value = 0;
  }
}

// Define a ResourceHandle destructor that returns the borrowed resource id:
static void ResourceHandle_dtor(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
  for (int i = 0; i < count; i++) {
    ResourceHandle *r = (ResourceHandle *) ECS_ELEM_T(ptr, ResourceHandle, i);
    return_resource_id(r->id);
  }
}

// Define a ResourceHandle move operation that transfers the resource id:
static void ResourceHandle_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  for (int i = 0; i < count; i++) {
    ResourceHandle *src_r = (ResourceHandle *) ECS_ELEM_T(src_ptr, ResourceHandle, i);
    ResourceHandle *dst_r = (ResourceHandle *) ECS_ELEM_T(dst_ptr, ResourceHandle, i);
    return_resource_id(dst_r->id);
    *dst_r = *src_r;
    src_r->id = 0;
  }
}

// Define a ResourceHandle copy operation that copies the payload value but obtains its own resource id:
static void ResourceHandle_copy(void *dst_ptr, const void *src_ptr, int32_t count, const ecs_type_info_t *type_info) {
  for (int i = 0; i < count; i++) {
    const ResourceHandle *src_r = (const ResourceHandle *) ECS_ELEM_T(src_ptr, ResourceHandle, i);
    ResourceHandle *dst_r = (ResourceHandle *) ECS_ELEM_T(dst_ptr, ResourceHandle, i);
    return_resource_id(dst_r->id);
    *dst_r = *src_r;
    dst_r->id = get_resource_id();
    test_assert(dst_r->id);
  }
}

// Defines a struct in Flecs to model the ResourceHandle struct
// For different tests, it can set specific hooks or not.
ecs_entity_t resource_handle;
void define_resource_handle(ecs_world_t *world, bool ctor, bool dtor, bool move, bool copy) {
  resource_handle = ecs_struct(
      world, {.entity = ecs_entity(world, {.name = "ResourceHandle"}),
              .members = {{.name = "id", .type = ecs_id(ecs_i32_t)}, {.name = "value", .type = ecs_id(ecs_u32_t)}}});

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

// The below structs contain handles, which in turn may or may not have hooks depending on each test:
typedef struct ResNestedStruct {
  int32_t x;
  ResourceHandle handle;
  int32_t y;
} ResNestedStruct;

typedef struct ResTestStruct {
  int32_t a;
  ResourceHandle handle;
  ResNestedStruct nested;
  int32_t b;
} ResTestStruct;

bool compare_resource_handle(const ResourceHandle *r1, const ResourceHandle *r2) {
  return r1->value == r2->value && r1->id != 0 && r2->id != 0;
}

bool compare_test_struct(const ResTestStruct *m, const ResTestStruct *n) {
  return m->a == n->a && m->b == n->b && m->nested.x == n->nested.x && m->nested.y == n->nested.y &&
         compare_resource_handle(&m->handle, &n->handle) &&
         compare_resource_handle(&m->nested.handle, &n->nested.handle);
}

// Defines Flecs structs to mirror the C structs above:
void define_nested_struct_with_resources(ecs_world_t *world) {
  nested_struct = ecs_struct(world, {.entity = ecs_new(world),
                                     .members = {{.name = "x", .type = ecs_id(ecs_i32_t)},
                                                 {.name = "handle", .type = resource_handle},
                                                 {.name = "y", .type = ecs_id(ecs_i32_t)}}});
  test_struct = ecs_struct(world, {.entity = ecs_new(world),
                                   .members = {{.name = "a", .type = ecs_id(ecs_i32_t)},
                                               {.name = "handle", .type = resource_handle},
                                               {.name = "nested", .type = nested_struct},
                                               {.name = "b", .type = ecs_id(ecs_i32_t)}}});
}

// Tests that hooks work as expected in a nested struct scenario
// Only ResourceHandle has actual hooks. Hooks for other structs are generated automatically
void RuntimeTypes_combined(void) {
  ecs_world_t *world = ecs_init();

  define_resource_handle(world, true, true, true, true);
  define_nested_struct_with_resources(world);

  // create some "resources"
  initialize_resource_ids(10);
  test_int(10, available_resource_count());

  // create a prefab entity:
  ecs_entity_t prefab = ecs_new(world);
  ecs_add_id(world, prefab, EcsPrefab);
  ResTestStruct *ts = (ResTestStruct *) ecs_ensure_id(world, prefab, test_struct);
  ts->a = 10;
  ts->b = 20;
  ts->handle.value = 100;
  ts->nested.x = 30;
  ts->nested.y = 40;
  ts->nested.handle.value = 200;

  // After adding a test struct to the prefab, it should have used up two "resources"
  test_int(8, available_resource_count());

  // Instantiating the prefab copies the component, using another two resources:
  ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
  test_int(6, available_resource_count());

  // Make sure we got an identical copy:
  test_assert(compare_test_struct(ts, (ResTestStruct *) ecs_get_id(world, e, test_struct)));

  // Add another component to the test entity, to force an archetype change and thus a move:
  ecs_add_id(world, e, ecs_new(world));
  test_int(6, available_resource_count());  // resource usage stays the same
  test_assert(compare_test_struct(ts, (ResTestStruct *) ecs_get_id(world, e, test_struct)));

  // remove the component, this should return two resources to the pool:
  ecs_remove_id(world, e, test_struct);
  test_int(8, available_resource_count());

  // remove the component from the prefab, all resources should be back:
  ecs_remove_id(world, prefab, test_struct);
  test_int(10, available_resource_count());

  // check if all resources were actually returned:
  for (int i = 0; i < 10; i++) {
    test_assert(resource_id_available(i + 1));
  }

  ecs_fini(world);
  free_resource_ids();
}

// Test hooks for runtime array of trivial structs
// These structs have no special copy/move/dtor logic and are zero-initialized
// Trivial structs should get flecs default constructor and nothing else
// An array of trivial struct should also only get flecs default constructor and nothing else.
void RuntimeTypes_trivial_array(void) {
  ecs_world_t *world = ecs_init();

  // Create a new component:
  ecs_entity_t trivial_struct = ecs_struct(
      world, {.entity = ecs_entity(world, {.name = "Trivial Struct"}),
              .members = {{.name = "a", .type = ecs_id(ecs_i32_t)}, {.name = "b", .type = ecs_id(ecs_f32_t)}}});

  // create an array of 3 trivial structs:
  ecs_array_desc_t desc = {.entity = 0, .type = trivial_struct, .count = 3};
  ecs_entity_t arr_of_struct = ecs_array_init(world, &desc);

  const ecs_type_info_t *ti = ecs_get_type_info(world, arr_of_struct);

  test_assert(ti->hooks.ctor != NULL);  // should have got flecs default constructor.

  // no other hooks should've been set:
  test_assert(ti->hooks.copy == NULL);
  test_assert(ti->hooks.move == NULL);
  test_assert(ti->hooks.dtor == NULL);

  // Check if the ctor initialized everything to 0.
  // Do this a couple of times to make sure we don't get a zero-initialized struct out of sheer luck:
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

// Tests that if on the array's underlying type only a ctor is defined, only a ctor is defined for the array itself
// Tests that the specified constructor is called for each array element.
void RuntimeTypes_array_ctor(void) {
  ecs_world_t *world = ecs_init();

  // create some "resources"
  initialize_resource_ids(10);
  test_int(10, available_resource_count());

  // Define the Resource with only a constructor hook.
  define_resource_handle(world, true, false, false, false);

  // create an array of 3 ResourceHandles
  ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
  ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

  const ecs_type_hooks_t *hooks = &ecs_get_type_info(world, arr_of_resources)->hooks;

  test_assert(hooks->ctor != NULL);  // a ctor should've been generated

  // no other hooks should have been generated, since the depending type, "resource_handle" only has a ctor hook:
  test_assert(hooks->dtor == NULL);
  test_assert(hooks->move == NULL);
  test_assert(hooks->copy == NULL);

  // Test that the set ctor hook is indeed working:
  ecs_entity_t e = ecs_new(world);
  ecs_add_id(world, e, arr_of_resources);
  test_int(7, available_resource_count());  // 3 resources were used out of 10, since the array has a size of 3

  ecs_fini(world);
  free_resource_ids();
}

// Tests that if on the array's underlying type only a dtor is defined, only a dtor is defined for the array itself
// Tests that the specified destructor is called for each array element.
void RuntimeTypes_array_dtor(void) {
  ecs_world_t *world = ecs_init();

  // create some "resources"
  initialize_resource_ids(10);

  // Define the Resource with only a destructor hook.
  define_resource_handle(world, false, true, false, false);

  // create an array of ResourceHandle
  ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
  ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

  const ecs_type_hooks_t *hooks = &ecs_get_type_info(world, arr_of_resources)->hooks;

  test_assert(hooks->ctor != NULL);  // ctor should exist and set to flecs' default memset-to-zero constructor
  test_assert(hooks->dtor != NULL);  // should be set to call the resource handle dtor for each item in the array.

  // no other hooks should have been generated, since the depending type, "resource_handle" only has a dtor hook:
  test_assert(hooks->move == NULL);
  test_assert(hooks->copy == NULL);

  // Test that the set dtor hook is indeed working.
  ecs_entity_t e = ecs_new(world);
  ecs_add_id(world, e, arr_of_resources);
  test_int(10, available_resource_count());  // since no special ctor is defined, it won't pick up resources from the
                                             // pool, and it stays the same.

  // The default constructor should've set the whole array to zero:
  ResourceHandle *handles = (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);
  test_assert(is_memory_filled_with(handles, sizeof(ResourceHandle) * 3, 0));

  // Make up some resource ids so they're artificially returned to the pool by the destructor:
  handles[0].id = 100;
  handles[1].id = 200;
  handles[2].id = 300;

  ecs_delete(world, e);  // destroy the entity, which will in turn destroy the component, invoking the destructors

  // check that made-up resources are now in the pool:
  test_int(13, available_resource_count());
  test_assert(resource_id_available(100));
  test_assert(resource_id_available(200));
  test_assert(resource_id_available(300));

  ecs_fini(world);
  free_resource_ids();
}

// Tests that if on the array's underlying type only a move hook is defined, only a move hook is defined for the array
// itself.
// Tests that the specified move hook is called for each array element.
void RuntimeTypes_array_move(void) {
  ecs_world_t *world = ecs_init();

  // create some "resources"
  initialize_resource_ids(10);

  // Define the Resource with only a move hook.
  define_resource_handle(world, false, false, true, false);

  // create an array of ResourceHandle
  ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
  ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

  const ecs_type_hooks_t *hooks = &ecs_get_type_info(world, arr_of_resources)->hooks;

  test_assert(hooks->ctor != NULL);  // ctor should exist and set to flecs' default memset-to-zero constructor
  test_assert(hooks->move != NULL);  // should be set to call the resource handle move hook for each item in the array.

  // no other hooks should have been generated, since the underlying type, "resource_handle" only has a move hook:
  test_assert(hooks->dtor == NULL);
  test_assert(hooks->copy == NULL);

  // Test that the set move hook is indeed working.
  ecs_entity_t e = ecs_new(world);
  ecs_add_id(world, e, arr_of_resources);
  test_int(10, available_resource_count());  // since no special ctor is defined, it won't pick up resources from the
                                             // pool, and it stays the same.

  // The default constructor should've set the whole array to zero:
  ResourceHandle *handles = (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);
  test_assert(is_memory_filled_with(handles, sizeof(ResourceHandle) * 3, 0));

  // Make up some resource ids so we can test they moved:
  handles[0] = (ResourceHandle){.id = 100, .value = 111};
  handles[1] = (ResourceHandle){.id = 200, .value = 222};
  handles[2] = (ResourceHandle){.id = 300, .value = 333};

  // add another entity to the test entity to trigger an archetype change and thus a move:
  ecs_add_id(world, e, ecs_new(world));
  handles = (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);

  // we should retrieve the same values:
  test_assert(compare_resource_handle(&(ResourceHandle){.id = 100, .value = 111}, &handles[0]));
  test_assert(compare_resource_handle(&(ResourceHandle){.id = 200, .value = 222}, &handles[1]));
  test_assert(compare_resource_handle(&(ResourceHandle){.id = 300, .value = 333}, &handles[2]));

  test_int(10, available_resource_count());  // pool stays the same

  ecs_delete(world, e);

  // since no special destructor is defined, the resources 100,200,300 are not returned to the pool:
  test_int(10, available_resource_count());
  test_assert(!resource_id_available(100));
  test_assert(!resource_id_available(200));
  test_assert(!resource_id_available(300));

  ecs_fini(world);
  free_resource_ids();
}

// Tests that if on the array's underlying type only a copy hook is defined, only a copy hook is defined for the array
// itself.
// Tests that the specified copy hook is called for each array element.
void RuntimeTypes_array_copy(void) {
  ecs_world_t *world = ecs_init();

  // create some "resources"
  initialize_resource_ids(10);
  test_int(10, available_resource_count());

  // Define the Resource with only a move hook.
  define_resource_handle(world, false, false, false, true);

  // create an array of ResourceHandle
  ecs_array_desc_t desc = {.entity = 0, .type = resource_handle, .count = 3};
  ecs_entity_t arr_of_resources = ecs_array_init(world, &desc);

  const ecs_type_hooks_t *hooks = &ecs_get_type_info(world, arr_of_resources)->hooks;

  test_assert(hooks->ctor != NULL);  // ctor should exist and set to flecs' default memset-to-zero constructor
  test_assert(hooks->copy != NULL);  // should be set to call the resource handle copy hook for each item in the array.

  // no other hooks should have been generated, since the underlying type, "resource_handle" only has a move hook:
  test_assert(hooks->dtor == NULL);
  test_assert(hooks->move == NULL);

  // Test that the set copy hook is indeed working.
  ecs_entity_t prefab = ecs_new(world);
  ecs_add_id(world, prefab, arr_of_resources);
  test_int(10, available_resource_count());  // since no special ctor is defined, it won't pick up resources from the
                                             // pool, and it stays the same.

  // The default constructor should've set the whole array to zero:
  ResourceHandle *prefab_handles = (ResourceHandle *) ecs_get_mut_id(world, prefab, arr_of_resources);
  test_assert(is_memory_filled_with(prefab_handles, sizeof(ResourceHandle) * 3, 0));

  // Make up some resource ids so we can test they are copied:
  prefab_handles[0] = (ResourceHandle){.id = 100, .value = 111};
  prefab_handles[1] = (ResourceHandle){.id = 200, .value = 222};
  prefab_handles[2] = (ResourceHandle){.id = 300, .value = 333};

  // Instantiate a prefab to trigger a copy:
  ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
  test_int(7, available_resource_count());  // should've taken 3 resources from the pool

  ResourceHandle *handles = (ResourceHandle *) ecs_get_mut_id(world, e, arr_of_resources);

  test_assert(compare_resource_handle(&prefab_handles[0], &handles[0]));
  test_assert(compare_resource_handle(&prefab_handles[1], &handles[1]));
  test_assert(compare_resource_handle(&prefab_handles[2], &handles[2]));

  ecs_delete(world, e);
  test_int(7, available_resource_count());  // resources not returned since we did not hook a destructor

  ecs_fini(world);
  free_resource_ids();
}

typedef struct BigStruct {
  ResNestedStruct ns;
  int32_t x;
  ResourceHandle arr[3];
  ecs_vec_t vec;  // vector<ResourceHandle>
} BigStruct;

void initialize_big_struct(BigStruct *arr_bs, size_t num_elements) {
  for (size_t i = 0; i < num_elements; i++) {
    arr_bs[i].ns.x = (int32_t) (10 + i * 30);
    arr_bs[i].ns.y = (int32_t) (20 + i * 30);
    arr_bs[i].ns.handle.value = 100 + i * 100;  // do not set ns.handle.id
    arr_bs[i].x = (int32_t) (30 + i * 30);
    arr_bs[i].arr[0].value = 110 + i * 100;  // do not set arr[0].id
    arr_bs[i].arr[1].value = 120 + i * 100;  // do not set arr[1].id
    arr_bs[i].arr[2].value = 130 + i * 100;  // do not set arr[2].id
    ecs_vec_set_count_t(NULL, &arr_bs[i].vec, ResourceHandle, 3);
    ResourceHandle_ctor(ecs_vec_first(&arr_bs[i].vec), 3, NULL);
  }
}
bool compare_res_nested_struct(const ResNestedStruct *m, const ResNestedStruct *n) {
  return m->x == n->x && m->y == n->y && compare_resource_handle(&m->handle, &n->handle);
}

bool compare_big_struct(const BigStruct *m, const BigStruct *n, size_t num_elements) {
  for (size_t i = 0; i < num_elements; i++) {
    // Compare nested struct
    if (!compare_res_nested_struct(&m[i].ns, &n[i].ns)) {
      return false;
    }

    // Compare x field
    if (m[i].x != n[i].x) {
      return false;
    }

    // Compare array of ResourceHandles
    for (int j = 0; j < 3; j++) {
      if (!compare_resource_handle(&m[i].arr[j], &n[i].arr[j])) {
        return false;
      }
    }

    // Compare vector
    if (ecs_vec_count(&m[i].vec) != ecs_vec_count(&n[i].vec))
      return false;
    ResourceHandle *mh = ecs_vec_first_t(&m[i].vec, ResourceHandle);
    ResourceHandle *nh = ecs_vec_first_t(&n[i].vec, ResourceHandle);
    for (int j = 0; j < ecs_vec_count(&m[i].vec); j++) {
      if (!compare_resource_handle(&mh[j], &nh[j]))
        return false;
    }
  }

  // All comparisons passed
  return true;
}

void RuntimeTypes_vector_lifecycle(void) {
  ecs_world_t *world = ecs_init();

  // create some "resources"
  int initial_resources = 10;
  initialize_resource_ids(initial_resources);
  test_int(10, available_resource_count());

  define_resource_handle(world, true, true, true, true);

  // create a vector of ResourceHandles
  ecs_vector_desc_t desc = {.entity = 0, .type = resource_handle};
  ecs_entity_t vector_of_resources = ecs_vector_init(world, &desc);

  const ecs_type_hooks_t *hooks = &ecs_get_type_info(world, vector_of_resources)->hooks;

  // a vector type requires al 4 hooks:
  test_assert(hooks->ctor != NULL);
  test_assert(hooks->dtor != NULL);
  test_assert(hooks->move != NULL);
  test_assert(hooks->copy != NULL);

  // Test the vector type is working:
  ecs_entity_t prefab = ecs_new(world);
  ecs_add_id(world, prefab, EcsPrefab);
  ecs_vec_t *v = (ecs_vec_t *) ecs_ensure_id(world, prefab, vector_of_resources);
  test_assert(v != NULL);
  test_assert(v->array == NULL);
  test_assert(v->count == 0);
  test_int(initial_resources, available_resource_count());  // still have 10 resources since vector is empty

  // manually add some items to the vector. These must be constructed by hand:
  ecs_vec_set_count_t(NULL, v, ResourceHandle, 3);
  ResourceHandle_ctor(ecs_vec_first(v), 3, NULL);
  test_int(initial_resources - 3, available_resource_count());  // Used up 3 resources that are now in the vector

  // test vector copy assign by instantiating the prefab
  ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
  test_int(initial_resources - 6, available_resource_count());  // Used up 3 more resources after copying the vector

  // test vector move assign by forcing a move via archetype change:
  ecs_add_id(world, e, ecs_new(world));
  test_int(initial_resources - 6, available_resource_count());  // No more resources consumed

  ecs_delete(world, e);
  test_int(initial_resources - 3, available_resource_count());  // Frees 3 resources held by the instance

  ecs_delete(world, prefab);
  test_int(initial_resources, available_resource_count());  // Frees another 3 resources held by the prefab

  // check if all resources were returned:
  for (int i = 0; i < initial_resources; i++) {
    test_assert(resource_id_available(i + 1));
  }
  ecs_fini(world);
  free_resource_ids();
}

void RuntimeTypes_vector_lifecycle_trivial_type(void) {
  ecs_world_t *world = ecs_init();

  // create a vector of ints
  ecs_vector_desc_t desc = {.entity = 0, .type = ecs_id(ecs_i32_t)};
  ecs_entity_t vector_of_ints = ecs_vector_init(world, &desc);

  const ecs_type_hooks_t *hooks = &ecs_get_type_info(world, vector_of_ints)->hooks;

  // a vector type requires al 4 hooks, even for trivial types:
  test_assert(hooks->ctor != NULL);
  test_assert(hooks->dtor != NULL);
  test_assert(hooks->move != NULL);
  test_assert(hooks->copy != NULL);

  // Test the vector type is working:
  ecs_entity_t prefab = ecs_new(world);
  ecs_add_id(world, prefab, EcsPrefab);
  ecs_vec_t *v = (ecs_vec_t *) ecs_ensure_id(world, prefab, vector_of_ints);
  test_assert(v != NULL);
  test_assert(v->array == NULL);
  test_assert(v->count == 0);

  // manually add some items to the vector. These must be constructed by hand:
  for (int i = 0; i < 3; i++) {
    *ecs_vec_append_t(NULL, v, int) = 79;
  }

  // test vector copy assign by instantiating the prefab
  ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);

  // verify we got a copy:
  const ecs_vec_t *vcopy = (const ecs_vec_t *) ecs_get_id(world, e, vector_of_ints);
  int32_t count = ecs_vec_count(vcopy);
  test_int(ecs_vec_count(v), count);
  test_assert(0 == ecs_os_memcmp(ecs_vec_first(v), ecs_vec_first(vcopy), sizeof(int32_t) * count));

  // test vector move assign by forcing a move via archetype change:
  ecs_add_id(world, e, ecs_new(world));
  const ecs_vec_t *vcopy_moved = (const ecs_vec_t *) ecs_get_id(world, e, vector_of_ints);
  count = ecs_vec_count(vcopy_moved);
  test_assert(vcopy != vcopy_moved);
  test_int(ecs_vec_count(v), count);
  test_assert(0 == ecs_os_memcmp(ecs_vec_first(v), ecs_vec_first(vcopy_moved), sizeof(int32_t) * count));

  ecs_delete(world, e);
  ecs_delete(world, prefab);

  ecs_fini(world);
  free_resource_ids();
}

// Create a mixed type of array, vectors and nested structs and test all hooks together.
void RuntimeTypes_mixed(void) {
  ecs_world_t *world = ecs_init();

  // create some "resources"
  const int initial_resources = 50;
  initialize_resource_ids(initial_resources);

  define_resource_handle(world, true, true, true, true);
  define_nested_struct_with_resources(world);

  // create an array of 3 ResourceHandles
  ecs_array_desc_t arr_desc = (ecs_array_desc_t){.entity = 0, .type = resource_handle, .count = 3};
  ecs_entity_t arr_of_resources = ecs_array_init(world, &arr_desc);
  ecs_set_name(world, arr_of_resources, "ArrOfResources_3");

  // create a vector of ResourceHandle
  ecs_vector_desc_t vector_desc = {.entity = 0, .type = resource_handle};
  ecs_entity_t vec_of_resources = ecs_vector_init(world, &vector_desc);

  // define BigStruct
  ecs_entity_t big_struct = ecs_struct(world, {.entity = ecs_new(world),
                                               .members = {{.name = "ns", .type = nested_struct},
                                                           {.name = "x", .type = ecs_id(ecs_i32_t)},
                                                           {.name = "arr", .type = arr_of_resources},
                                                           {.name = "vec", .type = vec_of_resources}}});

  // create an array of BigStruct
  arr_desc = (ecs_array_desc_t){.entity = 0, .type = big_struct, .count = 2};
  ecs_entity_t arr_of_big_struct = ecs_array_init(world, &arr_desc);
  ecs_set_name(world, arr_of_big_struct, "ArrOfBigStruct_5");

  ecs_entity_t prefab = ecs_new(world);

  BigStruct *arr_bs = (BigStruct *) ecs_ensure_id(world, prefab, arr_of_big_struct);
  // set the array of BigStruct to some test values:
  initialize_big_struct(arr_bs, arr_desc.count);

  test_int(initial_resources - 14, available_resource_count());  // 14 resources were consumed

  // now instantiate this prefab to trigger a copy
  ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, prefab);
  test_int(initial_resources - 28, available_resource_count());  // 28 resources should be in use
  const BigStruct *instance_bs = (const BigStruct *) ecs_get_id(world, e, arr_of_big_struct);

  test_assert(compare_big_struct(arr_bs, instance_bs, arr_desc.count));

  // add another entity to have `e` change archetypes and trigger a move:
  ecs_add_id(world, e, ecs_new(world));
  test_int(initial_resources - 28, available_resource_count());  // 28 resources should still be in use

  instance_bs = (const BigStruct *) ecs_get_id(world, e, arr_of_big_struct);
  test_assert(compare_big_struct(arr_bs, instance_bs, arr_desc.count));

  // destroy world:
  ecs_fini(world);

  test_int(initial_resources, available_resource_count());  // all resources should be back

  // check if all resources were returned:
  for (int i = 0; i < initial_resources; i++) {
    test_assert(resource_id_available(i + 1));
  }
  free_resource_ids();
}