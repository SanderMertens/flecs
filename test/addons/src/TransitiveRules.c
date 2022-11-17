#include <addons.h>

/* New set of tests just for transitivity. Transitive tests from TransitiveRules suite 
 * will be moved here */

void TransitiveRules_trans_X_X() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $X)"
    });
    test_assert(r == NULL);

    ecs_fini(world);
}

void TransitiveRules_trans_reflexive_X_X() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Reflexive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $X)"
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

void TransitiveRules_trans_reflexive_X_X_2() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Reflexive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n"
    LINE "LocatedIn(SanFrancisco, UnitedStates)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $X)"
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

void TransitiveRules_trans_reflexive_X_Y() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Reflexive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: UnitedStates"
    LINE "vars: X=UnitedStates,Y=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: UnitedStates"
    LINE "vars: X=UnitedStates,Y=Earth"
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
    LINE "Reflexive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)"
    LINE "LocatedIn(SanFrancisco, UnitedStates)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: UnitedStates"
    LINE "vars: X=UnitedStates,Y=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: UnitedStates"
    LINE "vars: X=UnitedStates,Y=Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,SanFrancisco)"
    LINE "subj: SanFrancisco"
    LINE "vars: X=SanFrancisco,Y=SanFrancisco"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: SanFrancisco"
    LINE "vars: X=SanFrancisco,Y=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: SanFrancisco"
    LINE "vars: X=SanFrancisco,Y=Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_trans_X_Y_2_levels_nonfinal() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Reflexive(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)"
    LINE "LocatedIn(SanFrancisco, UnitedStates)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: UnitedStates"
    LINE "vars: X=UnitedStates,Y=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: UnitedStates"
    LINE "vars: X=UnitedStates,Y=Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,SanFrancisco)"
    LINE "subj: SanFrancisco"
    LINE "vars: X=SanFrancisco,Y=SanFrancisco"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: SanFrancisco"
    LINE "vars: X=SanFrancisco,Y=UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: SanFrancisco"
    LINE "vars: X=SanFrancisco,Y=Earth"
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
    LINE "Reflexive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Location(Earth)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(LocatedIn, $X), Location($X)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool(true, ecs_rule_next(&it));
    char *expect =
    HEAD "term: (LocatedIn,Earth),Location"
    LINE "subj: 0,Earth"
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
    LINE "Reflexive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Location(Universe)"
    LINE "Location(Earth)"
    LINE "LocatedIn(Earth, Universe)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $Y), Location($X), Location($Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    expect =
    HEAD "term: (LocatedIn,Earth),Location,Location"
    LINE "subj: Earth,Earth,Earth"
    LINE "vars: X=Earth,Y=Earth"
    LINE;

    result = ecs_iter_str(&it);
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    expect =
    HEAD "term: (LocatedIn,Universe),Location,Location"
    LINE "subj: Earth,Earth,Universe"
    LINE "vars: X=Earth,Y=Universe"
    LINE;

    result = ecs_iter_str(&it);
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
        .expr = "LocatedIn(Earth, $X)"
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
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(Earth, Universe)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, Universe)"
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

void TransitiveRules_trans_entity_entity_non_reflexive() {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(Earth, Universe)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(Universe, Universe)"
    });
    test_assert(r == NULL);

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
        .expr = "Planet($This), LocatedIn($This, $X)"
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
        .expr = "LocatedIn($This, $X), Planet($This)"
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
        .expr = "Continent($This), LocatedIn($This, $X)"
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
        .expr = "LocatedIn($This, $X), Continent($This)"
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

void TransitiveRules_transitive_not_w_var() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "Final(City)"
    LINE "LocatedIn(SanFrancisco, UnitedStates)"
    LINE "LocatedIn(Seattle, UnitedStates)"
    LINE "LocatedIn(Amsterdam, Netherlands)"
    LINE "LocatedIn(Utrecht, Netherlands)"
    LINE "City(SanFrancisco)"
    LINE "City(Seattle)"
    LINE "City(Amsterdam)"
    LINE "City(Utrecht)"
    ;

    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "City($X), !LocatedIn($X, UnitedStates)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: City,(LocatedIn,UnitedStates)"
    LINE "subj: Amsterdam,Amsterdam"
    LINE "vars: X=Amsterdam"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: City,(LocatedIn,UnitedStates)"
    LINE "subj: Utrecht,Utrecht"
    LINE "vars: X=Utrecht"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_transitive_w_not_nonfinal() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    LINE "IsA(RockyPlanet, Planet)"
    LINE "IsA(HabitablePlanet, RockyPlanet)"
    LINE "IsA(GasGiant, Planet)"
    LINE "Final(Foo)"
    LINE "Foo(Earth)"
    LINE "Foo(Jupiter)"
    LINE "HabitablePlanet(Earth)"
    LINE "GasGiant(Jupiter)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Foo, !RockyPlanet"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Foo,RockyPlanet"
    LINE "subj: 0,0"
    LINE "this:"
    LINE "    - Jupiter"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_transitive_w_optional_nonfinal() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    LINE "IsA(RockyPlanet, Planet)"
    LINE "Final(Foo)"
    LINE "Foo(Earth)"
    LINE "Foo(Jupiter)"
    LINE "Foo(Moon)"
    LINE "RockyPlanet(Earth)"
    LINE "Planet(Jupiter)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Foo, ?Planet"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Foo,Planet"
    LINE "subj: 0,0"
    LINE "this:"
    LINE "    - Moon"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Foo,RockyPlanet"
    LINE "subj: 0,0"
    LINE "this:"
    LINE "    - Earth"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Foo,Planet"
    LINE "subj: 0,0"
    LINE "this:"
    LINE "    - Jupiter"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}

void TransitiveRules_transitive_w_optional_nonfinal_w_var() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    LINE "IsA(Loves, Likes)"
    LINE "Final(Foo)"
    LINE "Foo(Alice)"
    LINE "Foo(Bob)"
    LINE "Foo(Jane)"
    LINE "Likes(Alice, Bob)"
    LINE "Loves(Bob, Alice)";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Foo($X), ?Likes($X, $Y)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Foo,(Likes,*)"
    LINE "subj: Jane,Jane"
    LINE "vars: X=Jane,Y=*"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Foo,(Likes,Bob)"
    LINE "subj: Alice,Alice"
    LINE "vars: X=Alice,Y=Bob"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it), expect =
    HEAD "term: Foo,(Loves,Alice)"
    LINE "subj: Bob,Bob"
    LINE "vars: X=Bob,Y=Alice"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}


void TransitiveRules_rule_iter_set_transitive_variable() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, LocatedIn, Transitive, Final);
    ECS_ENTITY(world, Earth, 0);
    ECS_ENTITY(world, NorthAmerica, (LocatedIn, Earth));
    ECS_ENTITY(world, UnitedStates, (LocatedIn, NorthAmerica));
    ECS_ENTITY(world, SanFrancisco, (LocatedIn, UnitedStates));
    ECS_ENTITY(world, Soma, (LocatedIn, SanFrancisco));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(SanFrancisco, $X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, UnitedStates));
    test_int(ecs_iter_get_var(&it, x_var), UnitedStates);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, Earth));
    test_int(ecs_iter_get_var(&it, x_var), Earth);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, Soma);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, UnitedStates);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, UnitedStates));
    test_int(ecs_iter_get_var(&it, x_var), UnitedStates);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, NorthAmerica);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, Earth);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, Earth));
    test_int(ecs_iter_get_var(&it, x_var), Earth);
    test_bool( ecs_rule_next(&it), false );

    ecs_rule_fini(r);

    ecs_fini(world);
}

void TransitiveRules_rule_iter_set_transitive_self_variable() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, LocatedIn, Transitive, Reflexive, Final);
    ECS_ENTITY(world, Earth, 0);
    ECS_ENTITY(world, NorthAmerica, (LocatedIn, Earth));
    ECS_ENTITY(world, UnitedStates, (LocatedIn, NorthAmerica));
    ECS_ENTITY(world, SanFrancisco, (LocatedIn, UnitedStates));
    ECS_ENTITY(world, Soma, (LocatedIn, SanFrancisco));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn(SanFrancisco, $X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, SanFrancisco));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, Soma);
    test_bool( ecs_rule_next(&it), false );

    ecs_rule_fini(r);

    ecs_fini(world);
}

void TransitiveRules_rule_iter_set_transitive_2_variables_set_one() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, LocatedIn, Transitive, Final);
    ECS_ENTITY(world, Earth, 0);
    ECS_ENTITY(world, NorthAmerica, (LocatedIn, Earth));
    ECS_ENTITY(world, UnitedStates, (LocatedIn, NorthAmerica));
    ECS_ENTITY(world, SanFrancisco, (LocatedIn, UnitedStates));
    ECS_ENTITY(world, Soma, (LocatedIn, SanFrancisco));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $Y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, UnitedStates));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), UnitedStates);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, Earth));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), Earth);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, y_var, NorthAmerica);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), UnitedStates);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), Soma);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), false );

    ecs_rule_fini(r);

    ecs_fini(world);
}

void TransitiveRules_rule_iter_set_transitive_2_variables_set_both() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, LocatedIn, Transitive, Final);
    ECS_ENTITY(world, Earth, 0);
    ECS_ENTITY(world, NorthAmerica, (LocatedIn, Earth));
    ECS_ENTITY(world, UnitedStates, (LocatedIn, NorthAmerica));
    ECS_ENTITY(world, SanFrancisco, (LocatedIn, UnitedStates));
    ECS_ENTITY(world, Soma, (LocatedIn, SanFrancisco));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $Y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, UnitedStates);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, UnitedStates));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), UnitedStates);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, NorthAmerica);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, Earth);
    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, Earth));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), Earth);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, Soma);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, SanFrancisco);
    test_bool( ecs_rule_next(&it), false );

    ecs_rule_fini(r);

    ecs_fini(world);
}

void TransitiveRules_rule_iter_set_transitive_self_2_variables_set_both() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, LocatedIn, Transitive, Reflexive, Final);
    ECS_ENTITY(world, NorthAmerica, 0);
    ECS_ENTITY(world, UnitedStates, (LocatedIn, NorthAmerica));
    ECS_ENTITY(world, SanFrancisco, (LocatedIn, UnitedStates));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "LocatedIn($X, $Y)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_var(r, "X");
    test_assert(x_var != -1);
    int y_var = ecs_rule_find_var(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(world, r);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, UnitedStates));
    test_int(ecs_iter_get_var(&it, x_var), UnitedStates);
    test_int(ecs_iter_get_var(&it, y_var), UnitedStates);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), UnitedStates);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, SanFrancisco));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), SanFrancisco);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, UnitedStates));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), UnitedStates);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);

    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, SanFrancisco);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, SanFrancisco));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), SanFrancisco);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, UnitedStates);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, UnitedStates));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), UnitedStates);
    test_bool( ecs_rule_next(&it), false );

    it = ecs_rule_iter(world, r);
    ecs_iter_set_var(&it, x_var, SanFrancisco);
    ecs_iter_set_var(&it, y_var, NorthAmerica);

    test_bool( ecs_rule_next(&it), true );
    test_int(it.count, 0);
    test_int(ecs_field_id(&it, 1), ecs_pair(LocatedIn, NorthAmerica));
    test_int(ecs_iter_get_var(&it, x_var), SanFrancisco);
    test_int(ecs_iter_get_var(&it, y_var), NorthAmerica);
    test_bool( ecs_rule_next(&it), false );

    ecs_rule_fini(r);

    ecs_fini(world);
}

void TransitiveRules_transitive_w_self_tgt() {
    ecs_world_t *world = ecs_init();

    const char *ruleset = 
    HEAD "Transitive(LocatedIn)"
    LINE "Final(LocatedIn)"
    LINE "LocatedIn(UnitedStates, Earth)\n"
    LINE "LocatedIn(SanFrancisco, UnitedStates)\n";
    test_int(ecs_plecs_from_str(world, NULL, ruleset), 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "(LocatedIn, $X:self)"
    });
    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(world, r);
    char *result, *expect;

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,Earth)"
    LINE "subj: 0"
    LINE "vars: X=Earth"
    LINE "this:"
    LINE "    - UnitedStates"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(true, ecs_rule_next(&it));
    result = ecs_iter_str(&it); expect =
    HEAD "term: (LocatedIn,UnitedStates)"
    LINE "subj: 0"
    LINE "vars: X=UnitedStates"
    LINE "this:"
    LINE "    - SanFrancisco"
    LINE;
    test_str(result, expect);
    ecs_os_free(result);

    test_bool(false, ecs_rule_next(&it));

    ecs_rule_fini(r);
    ecs_fini(world);
}
