#include <meta.h>

ecs_os_api_free_t oldfree = NULL;
int free_count = 0;
const void *search_ptr = NULL;

void snoop_free(void *ptr) {
  if (ptr) {
    if (search_ptr && search_ptr == ptr) {
      free_count++;
    }
  }
  oldfree(ptr);
  // printf("snoop %p", ptr);
}

void RuntimeTypes_simple(void) {
  // set up a hook on the free() function used by Flecs
  // This will allow us to check if strings are freed properly.
  ecs_os_set_api_defaults();
  ecs_os_api_t os_api = ecs_os_get_api();
  oldfree = os_api.free_;
  os_api.free_ = snoop_free;
  ecs_os_set_api(&os_api);

  free_count = 0;

  ecs_world_t *world = ecs_init();

  // Create a new component:
  ecs_entity_t some_runtime_type = ecs_entity(world, {.name = "Some Runtime Type"});

  // Configure as a struct with one string member:
  ecs_struct(world, {.entity = some_runtime_type,
                     .members = {
                         {.name = "content", .type = ecs_id(ecs_string_t)},
                     }});

  // Create a new entity that will use the runtime type:
  ecs_entity_t a = ecs_entity(world, {.name = "test entity"});

  // Add an instance of the runtime type to our test entity and get a pointer to the underlying data:
  void *data = ecs_ensure_id(world, a, some_runtime_type);

  // Open a cursor to explore the runtime type:
  ecs_meta_cursor_t cursor = ecs_meta_cursor(world, some_runtime_type, data);

  // Navigate the struct to the 'content' string field:
  ecs_meta_push(&cursor);
  ecs_meta_member(&cursor, "content");

  // Set the string to a value. This copies the string and stores a pointer in the component:
  ecs_meta_set_string(&cursor, "This is a test string");

  // Get the pointer to the string copy, so we can check if it is freed when the component is destroyed:
  const char *str = ecs_meta_get_string(&cursor);
  search_ptr = str;  // configure our "snoop_free" hook to watch for this string being freed:

  // remove this component from the test entity. This should invoke the destructor and free the string:
  ecs_remove_id(world, a, some_runtime_type);
  test_assert(free_count == 1);

  ecs_fini(world);
  ecs_os_api.free_ = oldfree;  // remove the hook.
}
