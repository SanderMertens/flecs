#include <api.h>

#define HEAD
#define LINE "\n"

/* New set of tests just for transitivity. Transitive tests from Rules suite 
 * will be moved here */

void TransitiveRules_trans_X_X() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: 0"
    LINE "vars: X=Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_X_X_2() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n"
    LINE "LocatedIn(SanFrancisco, UnitedStates)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: 0"
    LINE "vars: X=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: 0"
    LINE "vars: X=Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_X_Y() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: 0"
    LINE "vars: Y=Earth,X=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_X_Y_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)"
    LINE "LocatedIn(SanFrancisco, UnitedStates)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: 0"
    LINE "vars: Y=UnitedStates,X=SanFrancisco"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: 0"
    LINE "vars: Y=Earth,X=SanFrancisco"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: 0"
    LINE "vars: Y=Earth,X=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_pred_This_X__pred_X() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Location(Earth)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(LocatedIn, X), Location(X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Earth),Location"
    LINE "subj: 0,0"
    LINE "vars: X=Earth"
    LINE "this:"
    LINE "    - Earth"
    LINE;

    char *result = ecs_iter_str(&it);
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}
