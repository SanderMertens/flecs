#include <member_ranges.h>
#include <stdio.h>

typedef struct {
    double value;
} CpuUtilization;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, CpuUtilization);

    // Register reflection data warning and error ranges. This adds metadata to
    // a type which can be used by tools (like the explorer) to visualize data.
    ecs_struct(ecs, {
        .entity = ecs_id(CpuUtilization),
        .members = {{
            .name = "value", 
            .type = ecs_id(ecs_f64_t),
            .range =         {0, 100}, // Specifics values that the member can assume
            .warning_range = {0, 60},  // Values outside this range are considerd a warning
            .error_range =   {0, 80}   // Values outside this range are considerd an error
        }}
    });

    ecs_entity_t m1 = ecs_new_entity(ecs, "MachineA");
    ecs_set(ecs, m1, CpuUtilization, {50});

    ecs_entity_t m2 = ecs_new_entity(ecs, "MachineB");
    ecs_set(ecs, m2, CpuUtilization, {75});

    ecs_entity_t m3 = ecs_new_entity(ecs, "MachineC");
    ecs_set(ecs, m3, CpuUtilization, {90});

    // Open https://www.flecs.dev/explorer?show=query&query=CpuUtilization to 
    // see how ranges affect visualization.
    return ecs_app_run(ecs, &(ecs_app_desc_t){ .enable_rest = true });
}
