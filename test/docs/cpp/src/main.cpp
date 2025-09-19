
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <cpp.h>

// Testsuite 'quickstart'
void quickstart_Concepts_Entity_01(void);

// Testsuite 'componenttraits'
void componenttraits_CanToggle_trait_01(void);
void componenttraits_Cleanup_traits_01(void);
void componenttraits_Cleanup_traits_02(void);
void componenttraits_Cleanup_traits_03(void);
void componenttraits_Cleanup_traits_Examples_OnDelete_Remove_01(void);
void componenttraits_Cleanup_traits_Examples_OnDelete_Delete_01(void);
void componenttraits_Cleanup_traits_Examples_OnDeleteTarget_Delete_01(void);
void componenttraits_Cleanup_traits_Cleanup_order_01(void);
void componenttraits_DontFragment_trait_01(void);
void componenttraits_Exclusive_trait_01(void);
void componenttraits_Exclusive_trait_02(void);
void componenttraits_Final_trait_01(void);
void componenttraits_Inheritable_trait_01(void);
void componenttraits_OneOf_trait_01(void);
void componenttraits_OneOf_trait_02(void);
void componenttraits_OnInstantiate_trait_Override_01(void);
void componenttraits_OnInstantiate_trait_Inherit_01(void);
void componenttraits_OnInstantiate_trait_DontInherit_01(void);
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

// Testsuite 'entitiescomponents'
void entitiescomponents_Entities_Creation_01(void);
void entitiescomponents_Entities_Deletion_01(void);
void entitiescomponents_Entities_Deletion_02(void);
void entitiescomponents_Entities_Deletion_03(void);
void entitiescomponents_Entities_Clearing_01(void);
void entitiescomponents_Entities_Liveliness_Checking_01(void);
void entitiescomponents_Entities_Liveliness_Checking_02(void);
void entitiescomponents_Entities_Manual_Ids_01(void);
void entitiescomponents_Entities_Manual_Versioning_01(void);
void entitiescomponents_Entities_Ranges_01(void);
void entitiescomponents_Entities_Ranges_02(void);
void entitiescomponents_Entities_Ranges_03(void);
void entitiescomponents_Entities_Names_01(void);
void entitiescomponents_Entities_Names_02(void);
void entitiescomponents_Entities_Names_03(void);
void entitiescomponents_Entities_Names_04(void);
void entitiescomponents_Entities_Names_05(void);
void entitiescomponents_Entities_Names_06(void);
void entitiescomponents_Entities_Names_07(void);
void entitiescomponents_Entities_Disabling_01(void);
void entitiescomponents_Entities_Disabling_02(void);
void entitiescomponents_Entities_Disabling_03(void);
void entitiescomponents_Entities_Disabling_04(void);
void entitiescomponents_Components_Hooks_01(void);
void entitiescomponents_Components_Hooks_02(void);
void entitiescomponents_Components_Components_have_entity_handles_01(void);
void entitiescomponents_Components_Components_have_entity_handles_02(void);
void entitiescomponents_Components_Registration_01(void);
void entitiescomponents_Components_Registration_02(void);
void entitiescomponents_Components_Registration_03(void);
void entitiescomponents_Components_Registration_Runtime_Type_Registration_01(void);
void entitiescomponents_Components_Registration_Runtime_Type_Registration_02(void);
void entitiescomponents_Components_Unregistration_01(void);
void entitiescomponents_Components_Singletons_01(void);
void entitiescomponents_Components_Singletons_02(void);
void entitiescomponents_Components_Disabling_01(void);

bake_test_case quickstart_testcases[] = {
    {
        "Concepts_Entity_01",
        quickstart_Concepts_Entity_01
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
        "Cleanup_traits_Examples_OnDelete_Remove_01",
        componenttraits_Cleanup_traits_Examples_OnDelete_Remove_01
    },
    {
        "Cleanup_traits_Examples_OnDelete_Delete_01",
        componenttraits_Cleanup_traits_Examples_OnDelete_Delete_01
    },
    {
        "Cleanup_traits_Examples_OnDeleteTarget_Delete_01",
        componenttraits_Cleanup_traits_Examples_OnDeleteTarget_Delete_01
    },
    {
        "Cleanup_traits_Cleanup_order_01",
        componenttraits_Cleanup_traits_Cleanup_order_01
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
        "OnInstantiate_trait_Override_01",
        componenttraits_OnInstantiate_trait_Override_01
    },
    {
        "OnInstantiate_trait_Inherit_01",
        componenttraits_OnInstantiate_trait_Inherit_01
    },
    {
        "OnInstantiate_trait_DontInherit_01",
        componenttraits_OnInstantiate_trait_DontInherit_01
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

bake_test_case entitiescomponents_testcases[] = {
    {
        "Entities_Creation_01",
        entitiescomponents_Entities_Creation_01
    },
    {
        "Entities_Deletion_01",
        entitiescomponents_Entities_Deletion_01
    },
    {
        "Entities_Deletion_02",
        entitiescomponents_Entities_Deletion_02
    },
    {
        "Entities_Deletion_03",
        entitiescomponents_Entities_Deletion_03
    },
    {
        "Entities_Clearing_01",
        entitiescomponents_Entities_Clearing_01
    },
    {
        "Entities_Liveliness_Checking_01",
        entitiescomponents_Entities_Liveliness_Checking_01
    },
    {
        "Entities_Liveliness_Checking_02",
        entitiescomponents_Entities_Liveliness_Checking_02
    },
    {
        "Entities_Manual_Ids_01",
        entitiescomponents_Entities_Manual_Ids_01
    },
    {
        "Entities_Manual_Versioning_01",
        entitiescomponents_Entities_Manual_Versioning_01
    },
    {
        "Entities_Ranges_01",
        entitiescomponents_Entities_Ranges_01
    },
    {
        "Entities_Ranges_02",
        entitiescomponents_Entities_Ranges_02
    },
    {
        "Entities_Ranges_03",
        entitiescomponents_Entities_Ranges_03
    },
    {
        "Entities_Names_01",
        entitiescomponents_Entities_Names_01
    },
    {
        "Entities_Names_02",
        entitiescomponents_Entities_Names_02
    },
    {
        "Entities_Names_03",
        entitiescomponents_Entities_Names_03
    },
    {
        "Entities_Names_04",
        entitiescomponents_Entities_Names_04
    },
    {
        "Entities_Names_05",
        entitiescomponents_Entities_Names_05
    },
    {
        "Entities_Names_06",
        entitiescomponents_Entities_Names_06
    },
    {
        "Entities_Names_07",
        entitiescomponents_Entities_Names_07
    },
    {
        "Entities_Disabling_01",
        entitiescomponents_Entities_Disabling_01
    },
    {
        "Entities_Disabling_02",
        entitiescomponents_Entities_Disabling_02
    },
    {
        "Entities_Disabling_03",
        entitiescomponents_Entities_Disabling_03
    },
    {
        "Entities_Disabling_04",
        entitiescomponents_Entities_Disabling_04
    },
    {
        "Components_Hooks_01",
        entitiescomponents_Components_Hooks_01
    },
    {
        "Components_Hooks_02",
        entitiescomponents_Components_Hooks_02
    },
    {
        "Components_Components_have_entity_handles_01",
        entitiescomponents_Components_Components_have_entity_handles_01
    },
    {
        "Components_Components_have_entity_handles_02",
        entitiescomponents_Components_Components_have_entity_handles_02
    },
    {
        "Components_Registration_01",
        entitiescomponents_Components_Registration_01
    },
    {
        "Components_Registration_02",
        entitiescomponents_Components_Registration_02
    },
    {
        "Components_Registration_03",
        entitiescomponents_Components_Registration_03
    },
    {
        "Components_Registration_Runtime_Type_Registration_01",
        entitiescomponents_Components_Registration_Runtime_Type_Registration_01
    },
    {
        "Components_Registration_Runtime_Type_Registration_02",
        entitiescomponents_Components_Registration_Runtime_Type_Registration_02
    },
    {
        "Components_Unregistration_01",
        entitiescomponents_Components_Unregistration_01
    },
    {
        "Components_Singletons_01",
        entitiescomponents_Components_Singletons_01
    },
    {
        "Components_Singletons_02",
        entitiescomponents_Components_Singletons_02
    },
    {
        "Components_Disabling_01",
        entitiescomponents_Components_Disabling_01
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
    },
    {
        "entitiescomponents",
        NULL,
        NULL,
        36,
        entitiescomponents_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("cpp", argc, argv, suites, 3);
}
