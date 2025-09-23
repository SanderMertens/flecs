
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <core.h>

// Testsuite 'quickstart'
void quickstart_flecs_quickstart_concepts_world_01(void);
void quickstart_flecs_quickstart_concepts_entity_02(void);
void quickstart_flecs_quickstart_concepts_entity_03(void);
void quickstart_flecs_quickstart_concepts_entity_04(void);
void quickstart_flecs_quickstart_concepts_component_05(void);
void quickstart_flecs_quickstart_concepts_component_06(void);
void quickstart_flecs_quickstart_concepts_component_07(void);
void quickstart_flecs_quickstart_concepts_tag_08(void);
void quickstart_flecs_quickstart_concepts_pair_09(void);
void quickstart_flecs_quickstart_concepts_pair_10(void);
void quickstart_flecs_quickstart_concepts_pair_11(void);
void quickstart_flecs_quickstart_concepts_pair_12(void);
void quickstart_flecs_quickstart_concepts_pair_13(void);
void quickstart_flecs_quickstart_concepts_hierarchies_14(void);
void quickstart_flecs_quickstart_concepts_hierarchies_15(void);
void quickstart_flecs_quickstart_concepts_hierarchies_16(void);
void quickstart_flecs_quickstart_concepts_type_17(void);
void quickstart_flecs_quickstart_concepts_type_18(void);
void quickstart_flecs_quickstart_concepts_type_19(void);
void quickstart_flecs_quickstart_concepts_type_20(void);
void quickstart_flecs_quickstart_concepts_query_21(void);
void quickstart_flecs_quickstart_concepts_query_22(void);
void quickstart_flecs_quickstart_concepts_system_23(void);
void quickstart_flecs_quickstart_concepts_pipeline_24(void);
void quickstart_flecs_quickstart_concepts_observer_25(void);

bake_test_case quickstart_testcases[] = {
    {
        "flecs_quickstart_concepts_world_01",
        quickstart_flecs_quickstart_concepts_world_01
    },
    {
        "flecs_quickstart_concepts_entity_02",
        quickstart_flecs_quickstart_concepts_entity_02
    },
    {
        "flecs_quickstart_concepts_entity_03",
        quickstart_flecs_quickstart_concepts_entity_03
    },
    {
        "flecs_quickstart_concepts_entity_04",
        quickstart_flecs_quickstart_concepts_entity_04
    },
    {
        "flecs_quickstart_concepts_component_05",
        quickstart_flecs_quickstart_concepts_component_05
    },
    {
        "flecs_quickstart_concepts_component_06",
        quickstart_flecs_quickstart_concepts_component_06
    },
    {
        "flecs_quickstart_concepts_component_07",
        quickstart_flecs_quickstart_concepts_component_07
    },
    {
        "flecs_quickstart_concepts_tag_08",
        quickstart_flecs_quickstart_concepts_tag_08
    },
    {
        "flecs_quickstart_concepts_pair_09",
        quickstart_flecs_quickstart_concepts_pair_09
    },
    {
        "flecs_quickstart_concepts_pair_10",
        quickstart_flecs_quickstart_concepts_pair_10
    },
    {
        "flecs_quickstart_concepts_pair_11",
        quickstart_flecs_quickstart_concepts_pair_11
    },
    {
        "flecs_quickstart_concepts_pair_12",
        quickstart_flecs_quickstart_concepts_pair_12
    },
    {
        "flecs_quickstart_concepts_pair_13",
        quickstart_flecs_quickstart_concepts_pair_13
    },
    {
        "flecs_quickstart_concepts_hierarchies_14",
        quickstart_flecs_quickstart_concepts_hierarchies_14
    },
    {
        "flecs_quickstart_concepts_hierarchies_15",
        quickstart_flecs_quickstart_concepts_hierarchies_15
    },
    {
        "flecs_quickstart_concepts_hierarchies_16",
        quickstart_flecs_quickstart_concepts_hierarchies_16
    },
    {
        "flecs_quickstart_concepts_type_17",
        quickstart_flecs_quickstart_concepts_type_17
    },
    {
        "flecs_quickstart_concepts_type_18",
        quickstart_flecs_quickstart_concepts_type_18
    },
    {
        "flecs_quickstart_concepts_type_19",
        quickstart_flecs_quickstart_concepts_type_19
    },
    {
        "flecs_quickstart_concepts_type_20",
        quickstart_flecs_quickstart_concepts_type_20
    },
    {
        "flecs_quickstart_concepts_query_21",
        quickstart_flecs_quickstart_concepts_query_21
    },
    {
        "flecs_quickstart_concepts_query_22",
        quickstart_flecs_quickstart_concepts_query_22
    },
    {
        "flecs_quickstart_concepts_system_23",
        quickstart_flecs_quickstart_concepts_system_23
    },
    {
        "flecs_quickstart_concepts_pipeline_24",
        quickstart_flecs_quickstart_concepts_pipeline_24
    },
    {
        "flecs_quickstart_concepts_observer_25",
        quickstart_flecs_quickstart_concepts_observer_25
    }
};


static bake_test_suite suites[] = {
    {
        "quickstart",
        NULL,
        NULL,
        25,
        quickstart_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("core", argc, argv, suites, 1);
}
