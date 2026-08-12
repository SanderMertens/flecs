#include <component_inheritance.h>
#include <stdio.h>

// This example shows how queries can be used to match simple inheritance trees.

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create a simple hierarchy of unit types.

    ECS_TAG(ecs, Unit);
    ecs_entity_t CombatUnit = ecs_entity(ecs, { .name = "CombatUnit" });
    ecs_add_pair(ecs, CombatUnit, EcsIsA, Unit);
    ecs_entity_t MeleeUnit = ecs_entity(ecs, { .name = "MeleeUnit" });
    ecs_add_pair(ecs, MeleeUnit, EcsIsA, CombatUnit);
    ecs_entity_t RangedUnit = ecs_entity(ecs, { .name = "RangedUnit" });
    ecs_add_pair(ecs, RangedUnit, EcsIsA, CombatUnit);

    ecs_entity_t Warrior = ecs_entity(ecs, { .name = "Warrior" });

    ecs_add_pair(ecs, Warrior, EcsIsA, MeleeUnit);
    ecs_entity_t Wizard = ecs_entity(ecs, { .name = "Wizard" });
    ecs_add_pair(ecs, Wizard, EcsIsA, RangedUnit);
    ecs_entity_t Marksman = ecs_entity(ecs, { .name = "Marksman" });
    ecs_add_pair(ecs, Marksman, EcsIsA, RangedUnit);
    ecs_entity_t Builder = ecs_entity(ecs, { .name = "Builder" });
    ecs_add_pair(ecs, Builder, EcsIsA, Unit);

    // Create a few units
    ecs_entity_t warrior_1 = ecs_entity(ecs, { .name = "warrior_1" });
    ecs_add_id(ecs, warrior_1, Warrior);
    ecs_entity_t warrior_2 = ecs_entity(ecs, { .name = "warrior_2" });
    ecs_add_id(ecs, warrior_2, Warrior);

    ecs_entity_t marksman_1 = ecs_entity(ecs, { .name = "marksman_1" });
    ecs_add_id(ecs, marksman_1, Marksman);
    ecs_entity_t marksman_2 = ecs_entity(ecs, { .name = "marksman_2" });
    ecs_add_id(ecs, marksman_2, Marksman);

    ecs_entity_t wizard_1 = ecs_entity(ecs, { .name = "wizard_1" });
    ecs_add_id(ecs, wizard_1, Wizard);
    ecs_entity_t wizard_2 = ecs_entity(ecs, { .name = "wizard_2" });
    ecs_add_id(ecs, wizard_2, Wizard);

    ecs_entity_t builder_1 = ecs_entity(ecs, { .name = "builder_1" });
    ecs_add_id(ecs, builder_1, Builder);
    ecs_entity_t builder_2 = ecs_entity(ecs, { .name = "builder_2" });
    ecs_add_id(ecs, builder_2, Builder);

    // Create a query to find all ranged units
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {{ .id = RangedUnit }}
    });

    // Iterate the query
    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            printf("Unit %s found\n", ecs_get_name(ecs, it.entities[i]));
        }
    }

    ecs_query_fini(q);

    // Output
    //  Unit wizard_1 found
    //  Unit wizard_2 found
    //  Unit marksman_1 found
    //  Unit marksman_2 found

    return ecs_fini(ecs);
}
