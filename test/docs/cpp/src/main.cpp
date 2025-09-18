
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <cpp.h>

// Testsuite 'quickstart'
void quickstart_Entity_01(void);

// Testsuite 'componenttraits'
void componenttraits_CanToggle_trait_01(void);
void componenttraits_Cleanup_traits_01(void);
void componenttraits_Cleanup_traits_02(void);
void componenttraits_Cleanup_traits_03(void);
void componenttraits_OnDelete_Remove_01(void);
void componenttraits_OnDelete_Delete_01(void);
void componenttraits_OnDeleteTarget_Delete_01(void);
void componenttraits_Cleanup_order_01(void);
void componenttraits_DontFragment_trait_01(void);
void componenttraits_Exclusive_trait_01(void);
void componenttraits_Exclusive_trait_02(void);
void componenttraits_Final_trait_01(void);
void componenttraits_Inheritable_trait_01(void);
void componenttraits_OneOf_trait_01(void);
void componenttraits_OneOf_trait_02(void);
void componenttraits_Override_01(void);
void componenttraits_Inherit_01(void);
void componenttraits_DontInherit_01(void);
void componenttraits_OrderedChildren_trait_01(void);
void componenttraits_PairIsTag_trait_01(void);
void componenttraits_PairIsTag_trait_02(void);
void componenttraits_Relationship_trait_01(void);
void componenttraits_Relationship_trait_02(void);
void componenttraits_Singleton_trait_01(void);
void componenttraits_Singleton_trait_02(void);
void componenttraits_Sparse_trait_01(void);
void componenttraits_Symmetric_trait_01(void);
void componenttraits_Target_trait_01(void);
void componenttraits_Trait_trait_01(void);
void componenttraits_Transitive_trait_01(void);
void componenttraits_Transitive_trait_02(void);
void componenttraits_With_trait_01(void);
void componenttraits_With_trait_02(void);

bake_test_case quickstart_testcases[] = {
    {
        "Entity_01",
        quickstart_Entity_01
    }
};

bake_test_case componenttraits_testcases[] = {
    {
        "CanToggle_trait_01",
        componenttraits_CanToggle_trait_01
    },
    {
        "Cleanup_traits_01",
        componenttraits_Cleanup_traits_01
    },
    {
        "Cleanup_traits_02",
        componenttraits_Cleanup_traits_02
    },
    {
        "Cleanup_traits_03",
        componenttraits_Cleanup_traits_03
    },
    {
        "OnDelete_Remove_01",
        componenttraits_OnDelete_Remove_01
    },
    {
        "OnDelete_Delete_01",
        componenttraits_OnDelete_Delete_01
    },
    {
        "OnDeleteTarget_Delete_01",
        componenttraits_OnDeleteTarget_Delete_01
    },
    {
        "Cleanup_order_01",
        componenttraits_Cleanup_order_01
    },
    {
        "DontFragment_trait_01",
        componenttraits_DontFragment_trait_01
    },
    {
        "Exclusive_trait_01",
        componenttraits_Exclusive_trait_01
    },
    {
        "Exclusive_trait_02",
        componenttraits_Exclusive_trait_02
    },
    {
        "Final_trait_01",
        componenttraits_Final_trait_01
    },
    {
        "Inheritable_trait_01",
        componenttraits_Inheritable_trait_01
    },
    {
        "OneOf_trait_01",
        componenttraits_OneOf_trait_01
    },
    {
        "OneOf_trait_02",
        componenttraits_OneOf_trait_02
    },
    {
        "Override_01",
        componenttraits_Override_01
    },
    {
        "Inherit_01",
        componenttraits_Inherit_01
    },
    {
        "DontInherit_01",
        componenttraits_DontInherit_01
    },
    {
        "OrderedChildren_trait_01",
        componenttraits_OrderedChildren_trait_01
    },
    {
        "PairIsTag_trait_01",
        componenttraits_PairIsTag_trait_01
    },
    {
        "PairIsTag_trait_02",
        componenttraits_PairIsTag_trait_02
    },
    {
        "Relationship_trait_01",
        componenttraits_Relationship_trait_01
    },
    {
        "Relationship_trait_02",
        componenttraits_Relationship_trait_02
    },
    {
        "Singleton_trait_01",
        componenttraits_Singleton_trait_01
    },
    {
        "Singleton_trait_02",
        componenttraits_Singleton_trait_02
    },
    {
        "Sparse_trait_01",
        componenttraits_Sparse_trait_01
    },
    {
        "Symmetric_trait_01",
        componenttraits_Symmetric_trait_01
    },
    {
        "Target_trait_01",
        componenttraits_Target_trait_01
    },
    {
        "Trait_trait_01",
        componenttraits_Trait_trait_01
    },
    {
        "Transitive_trait_01",
        componenttraits_Transitive_trait_01
    },
    {
        "Transitive_trait_02",
        componenttraits_Transitive_trait_02
    },
    {
        "With_trait_01",
        componenttraits_With_trait_01
    },
    {
        "With_trait_02",
        componenttraits_With_trait_02
    }
};


static bake_test_suite suites[] = {
    {
        "quickstart",
        NULL,
        NULL,
        1,
        quickstart_testcases
    },
    {
        "componenttraits",
        NULL,
        NULL,
        33,
        componenttraits_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("cpp", argc, argv, suites, 2);
}
