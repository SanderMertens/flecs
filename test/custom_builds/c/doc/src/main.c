#include <doc.h>
#include <string.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    ecs_doc_set_brief(world, e, "A foo");

    const char *brief = ecs_doc_get_brief(world, e);
    assert(brief != NULL);
    assert(!strcmp(brief, "A foo"));

    return ecs_fini(world);
}
