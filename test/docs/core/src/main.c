
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <core.h>

// Testsuite 'quickstart'
void quickstart_Concepts_World_01(void);
void quickstart_Concepts_Entity_01(void);
void quickstart_Concepts_Entity_02(void);
void quickstart_Concepts_Entity_03(void);
void quickstart_Concepts_Component_01(void);
void quickstart_Concepts_Component_02(void);
void quickstart_Concepts_Component_03(void);
void quickstart_Concepts_Tag_01(void);
void quickstart_Concepts_Pair_01(void);
void quickstart_Concepts_Pair_02(void);
void quickstart_Concepts_Pair_03(void);
void quickstart_Concepts_Pair_04(void);
void quickstart_Concepts_Pair_05(void);
void quickstart_Concepts_Hierarchies_01(void);
void quickstart_Concepts_Hierarchies_02(void);
void quickstart_Concepts_Hierarchies_03(void);
void quickstart_Concepts_Type_01(void);
void quickstart_Concepts_Type_02(void);
void quickstart_Concepts_Type_03(void);
void quickstart_Concepts_Type_04(void);
void quickstart_Concepts_Query_01(void);
void quickstart_Concepts_Query_02(void);
void quickstart_Concepts_System_01(void);
void quickstart_Concepts_Pipeline_01(void);
void quickstart_Concepts_Observer_01(void);

bake_test_case quickstart_testcases[] = {
    {
        "Concepts_World_01",
        quickstart_Concepts_World_01
    },
    {
        "Concepts_Entity_01",
        quickstart_Concepts_Entity_01
    },
    {
        "Concepts_Entity_02",
        quickstart_Concepts_Entity_02
    },
    {
        "Concepts_Entity_03",
        quickstart_Concepts_Entity_03
    },
    {
        "Concepts_Component_01",
        quickstart_Concepts_Component_01
    },
    {
        "Concepts_Component_02",
        quickstart_Concepts_Component_02
    },
    {
        "Concepts_Component_03",
        quickstart_Concepts_Component_03
    },
    {
        "Concepts_Tag_01",
        quickstart_Concepts_Tag_01
    },
    {
        "Concepts_Pair_01",
        quickstart_Concepts_Pair_01
    },
    {
        "Concepts_Pair_02",
        quickstart_Concepts_Pair_02
    },
    {
        "Concepts_Pair_03",
        quickstart_Concepts_Pair_03
    },
    {
        "Concepts_Pair_04",
        quickstart_Concepts_Pair_04
    },
    {
        "Concepts_Pair_05",
        quickstart_Concepts_Pair_05
    },
    {
        "Concepts_Hierarchies_01",
        quickstart_Concepts_Hierarchies_01
    },
    {
        "Concepts_Hierarchies_02",
        quickstart_Concepts_Hierarchies_02
    },
    {
        "Concepts_Hierarchies_03",
        quickstart_Concepts_Hierarchies_03
    },
    {
        "Concepts_Type_01",
        quickstart_Concepts_Type_01
    },
    {
        "Concepts_Type_02",
        quickstart_Concepts_Type_02
    },
    {
        "Concepts_Type_03",
        quickstart_Concepts_Type_03
    },
    {
        "Concepts_Type_04",
        quickstart_Concepts_Type_04
    },
    {
        "Concepts_Query_01",
        quickstart_Concepts_Query_01
    },
    {
        "Concepts_Query_02",
        quickstart_Concepts_Query_02
    },
    {
        "Concepts_System_01",
        quickstart_Concepts_System_01
    },
    {
        "Concepts_Pipeline_01",
        quickstart_Concepts_Pipeline_01
    },
    {
        "Concepts_Observer_01",
        quickstart_Concepts_Observer_01
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
