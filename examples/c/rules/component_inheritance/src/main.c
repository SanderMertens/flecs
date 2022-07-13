#include <component_inheritance.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Use convenience macros to create simple hierarchy of unit types.
    // This macro call:
    //   ECS_ENTITY(ecs, CombatUnit, (IsA, Unit))
    //
    // is the same as these C statements:
    //   ecs_entity_t CombatUnit = ecs_new_entity("CombatUnit");
    //   ecs_add_pair(ecs, CombatUnit, EcsIsA, Unit);

    ECS_TAG(ecs, Unit);
    ECS_ENTITY(ecs, CombatUnit, (IsA, Unit));
    ECS_ENTITY(ecs, MeleeUnit,  (IsA, CombatUnit));
    ECS_ENTITY(ecs, RangedUnit, (IsA, CombatUnit));

    ECS_ENTITY(ecs, Warrior,    (IsA, MeleeUnit));
    ECS_ENTITY(ecs, Wizard,     (IsA, RangedUnit));
    ECS_ENTITY(ecs, Marksman,   (IsA, RangedUnit));
    ECS_ENTITY(ecs, Builder,    (IsA, Unit));

    // Create a few units
    ecs_entity_t warrior_1 = ecs_new_entity(ecs, "warrior_1");
    ecs_add(ecs, warrior_1, Warrior);
    ecs_entity_t warrior_2 = ecs_new_entity(ecs, "warrior_2");
    ecs_add(ecs, warrior_2, Warrior);

    ecs_entity_t marksman_1 = ecs_new_entity(ecs, "marksman_1");
    ecs_add(ecs, marksman_1, Marksman);
    ecs_entity_t marksman_2 = ecs_new_entity(ecs, "marksman_2");
    ecs_add(ecs, marksman_2, Marksman);

    ecs_entity_t wizard_1 = ecs_new_entity(ecs, "wizard_1");
    ecs_add(ecs, wizard_1, Wizard);
    ecs_entity_t wizard_2 = ecs_new_entity(ecs, "wizard_2");
    ecs_add(ecs, wizard_2, Wizard);

    ecs_entity_t builder_1 = ecs_new_entity(ecs, "builder_1");
    ecs_add(ecs, builder_1, Builder);
    ecs_entity_t builder_2 = ecs_new_entity(ecs, "builder_2");
    ecs_add(ecs, builder_2, Builder);

    // Create a rule to find all ranged units
    ecs_rule_t *r = ecs_rule(ecs, {
        .terms = {{ .id = RangedUnit }}
    });

    // Iterate the rule
    ecs_iter_t it = ecs_rule_iter(ecs, r);
    while (ecs_rule_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            printf("Unit %s found\n", ecs_get_name(ecs, it.entities[i]));
        }
    }

    ecs_rule_fini(r);

    // Output
    //  Unit wizard_1 found
    //  Unit wizard_2 found
    //  Unit marksman_1 found
    //  Unit marksman_2 found

    return ecs_fini(ecs);
}
