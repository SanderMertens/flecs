#include <script_shell.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsScriptShell);

    assert(ecs_lookup(world, "flecs.script.shell.commands.core.ls") != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    (void)e;

    ecs_entity_t session = ecs_shell_session_init(world, NULL);
    assert(session != 0);

    ecs_shell_result_t r = ecs_shell_run(world, session, "new foo");
    assert(ecs_vec_count(&r.errors) == 0);
    assert(r.result.type == ecs_id(ecs_entity_t));
    assert(*(ecs_entity_t*)r.result.ptr == ecs_lookup(world, "foo"));
    ecs_shell_result_fini(world, &r);

    r = ecs_shell_run(world, session, "ls");
    assert(ecs_vec_count(&r.errors) == 0);
    ecs_shell_result_fini(world, &r);

    return ecs_fini(world);
}
