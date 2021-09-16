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

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: Earth"
    LINE "vars: X=Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

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

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: Earth"
    LINE "vars: X=Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: UnitedStates"
    LINE "vars: X=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_X_Y() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Inclusive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: UnitedStates"
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
    LINE "Inclusive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)"
    LINE "LocatedIn(SanFrancisco, UnitedStates)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: UnitedStates"
    LINE "vars: Y=Earth,X=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: SanFrancisco"
    LINE "vars: Y=UnitedStates,X=SanFrancisco"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: SanFrancisco"
    LINE "vars: Y=Earth,X=SanFrancisco"
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
    LINE "Inclusive(LocatedIn)"
    LINE "Location(Earth)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(LocatedIn, X), Location(X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Earth),Location"
    LINE "subj: Earth,0"
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

void TransitiveRules_trans_constrained_x_y() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Inclusive(LocatedIn)"
    LINE "Location(Universe)"
    LINE "Location(Earth)"
    LINE "LocatedIn(Earth, Universe)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, Y), Location(X), Location(Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Universe),Location,Location"
    LINE "subj: Earth,Earth,Universe"
    LINE "vars: X=Earth,Y=Universe"
    LINE;

    char *result = ecs_iter_str(&it);
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_entity_X_non_inclusive() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(Earth, Universe)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(Earth, X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Universe)"
    LINE "subj: Earth"
    LINE "vars: X=Universe"
    LINE;

    char *result = ecs_iter_str(&it);
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_X_entity_non_inclusive() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "LocatedIn(Earth, Universe)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(X, Universe)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Universe)"
    LINE "subj: Earth"
    LINE "vars: X=Earth"
    LINE;

    char *result = ecs_iter_str(&it);
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_entity_entity_non_inclusive() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(Earth, Universe)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(Universe, Universe)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(false, ecs_rule_next(&it));
    ecs_rule_fini(r);

    r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(Earth, Earth)"
    });
    test_assert(r != NULL);

    it = ecs_rule_iter(world, r);
    test_bool(false, ecs_rule_next(&it));
    ecs_rule_fini(r);

    r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(Earth, Universe)"
    });
    test_assert(r != NULL);

    it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Universe)"
    LINE "subj: Earth"
    LINE;

    char *result = ecs_iter_str(&it);
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_this_x_after_tag_this() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Final(Planet)"
    LINE "LocatedIn(Earth, Universe)"
    LINE "Planet(Earth)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Planet(This), LocatedIn(This, X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: Planet,(LocatedIn,Universe)"
    LINE "subj: 0,0"
    LINE "vars: X=Universe"
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

void TransitiveRules_trans_this_x_before_tag_this() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Final(Planet)"
    LINE "LocatedIn(Earth, Universe)"
    LINE "Planet(Earth)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(This, X), Planet(This)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Universe),Planet"
    LINE "subj: 0,0"
    LINE "vars: X=Universe"
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

void TransitiveRules_trans_this_x_after_tag_this_2_lvls() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Final(Planet)"
    LINE "LocatedIn(Earth, Universe)"
    LINE "LocatedIn(NorthAmerica, Earth)"
    LINE "Continent(NorthAmerica)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Continent(This), LocatedIn(This, X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Continent,(LocatedIn,Earth)"
    LINE "subj: 0,0"
    LINE "vars: X=Earth"
    LINE "this:"
    LINE "    - NorthAmerica"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Continent,(LocatedIn,Universe)"
    LINE "subj: 0,0"
    LINE "vars: X=Universe"
    LINE "this:"
    LINE "    - NorthAmerica"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_this_x_before_tag_this_2_lvls() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Final(Planet)"
    LINE "LocatedIn(Earth, Universe)"
    LINE "LocatedIn(NorthAmerica, Earth)"
    LINE "Continent(NorthAmerica)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(This, X), Continent(This)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: (LocatedIn,Earth),Continent"
    LINE "subj: 0,0"
    LINE "vars: X=Earth"
    LINE "this:"
    LINE "    - NorthAmerica"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: (LocatedIn,Universe),Continent"
    LINE "subj: 0,0"
    LINE "vars: X=Universe"
    LINE "this:"
    LINE "    - NorthAmerica"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}
