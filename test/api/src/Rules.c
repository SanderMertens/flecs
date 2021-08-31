#include <api.h>

#define test_term_id(it, column_id, str) {\
    ecs_entity_t _column_entity_e = ecs_term_id(it, column_id);\
    test_assert(_column_entity_e != 0);\
    char _column_entity[512];\
    ecs_id_str_w_buf((it)->world, _column_entity_e, _column_entity, sizeof(_column_entity));\
    test_str(_column_entity, str);\
}

#define test_term_source(it, column_id, str) {\
    ecs_entity_t _column_source_e = ecs_term_source(it, column_id);\
    test_assert(_column_source_e != 0);\
    char _column_source[512];\
    ecs_id_str_w_buf((it)->world, _column_source_e, _column_source, sizeof(_column_source));\
    test_str(_column_source, str);\
}

#define test_var(it, var_id, str) {\
    ecs_entity_t _var_e = ecs_rule_variable(it, var_id);\
    test_assert(_var_e != 0);\
    char _var[512];\
    ecs_id_str_w_buf((it)->world, _var_e, _var, sizeof(_var));\
    test_str(_var, str);\
}

void Rules_invalid_rule() {
    ecs_tracing_enable(-4);
    
    ecs_world_t *world = ecs_init();

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Foo"
    });

    test_assert(r == NULL);

    ecs_fini(world);
}

void Rules_comp_recycled_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);

    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Tag, 0);

    /* Ensure recycled id */
    test_assert(Tag != (uint32_t)Tag);

    ecs_entity_t e = ecs_new_w_id(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);

    test_term_id(&it, 1, "Tag");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_comp_recycled_final_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);

    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Tag, Final);

    /* Ensure recycled id */
    test_assert(Tag != (uint32_t)Tag);

    ecs_entity_t e = ecs_new_w_id(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Tag"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "Tag");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_final_pred() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Obj, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Pred, Final);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred(., X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_pred() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Obj, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Pred, 0);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred(., X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_obj() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred(., Obj)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_pred_obj() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));
    ECS_ENTITY(world, Pred, 0);

    dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));
    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred(., Obj)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_matched_obj() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred(., X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");
    test_var(&it, x_var, "Obj");

    test_assert(!ecs_rule_next(&it));
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_matched_obj_2_terms() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Pred, 0);
    ECS_ENTITY(world, Pred_2, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Obj, 0);

    /* Ensure recycled id */
    test_assert(Obj != (uint32_t)Obj);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_add_id(world, e, ecs_pair(Pred_2, Obj));
    test_assert(ecs_has_id(world, e, ecs_pair(Pred_2, Obj)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Pred(., X), Pred_2(., X)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");
    test_term_id(&it, 2, "(Pred_2,Obj)");
    test_var(&it, x_var, "Obj");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_pair_recycled_matched_pred_2_terms() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Obj, 0);
    ECS_ENTITY(world, Obj_2, 0);

    ecs_entity_t dummy = ecs_new_id(world);
    test_assert(dummy != 0);
    ecs_delete(world, dummy);
    test_assert(!ecs_is_alive(world, dummy));

    ECS_ENTITY(world, Pred, 0);

    /* Ensure recycled id */
    test_assert(Pred != (uint32_t)Pred);

    ecs_entity_t e = ecs_new_w_id(world, ecs_pair(Pred, Obj));
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj)));

    ecs_add_id(world, e, ecs_pair(Pred, Obj_2));
    test_assert(ecs_has_id(world, e, ecs_pair(Pred, Obj_2)));

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(., Obj), X(., Obj_2)"
    });

    test_assert(r != NULL);

    int x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);
    test_term_id(&it, 1, "(Pred,Obj)");
    test_term_id(&it, 2, "(Pred,Obj_2)");
    test_var(&it, x_var, "Pred");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

static
void test_1_comp(const char *expr) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = expr
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {70, 80});
    ecs_set(world, e4, Velocity, {1, 2});

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        {e1, e2, e3, e4},
        {{ecs_id(Position)}},
        {
            (Position[]){{10, 20}, {30, 40}, {50, 60}, {70, 80}}
        },
        2
    }));

    ecs_rule_fini(r);

    ecs_fini(world);
}

static
void test_2_comp(const char *expr) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Position, Velocity"
    });

    test_assert(r != NULL);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_set(world, e1, Velocity, {1, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});
    ecs_set(world, e2, Velocity, {3, 4});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {50, 60});
    ecs_set(world, e3, Velocity, {5, 6});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {70, 80});
    ecs_set(world, e4, Velocity, {7, 8});
    ecs_set(world, e4, Mass, {5});

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        {e1, e2, e3, e4},
        {{ecs_id(Position), ecs_id(Velocity)}},
        {
            (Position[]){{10, 20}, {30, 40}, {50, 60}, {70, 80}},
            (Velocity[]){{1, 2}, {3, 4}, {5, 6}, {7, 8}}
        },
        2
    }));

    ecs_rule_fini(r);

    ecs_fini(world);    
}

void Rules_1_comp() {
    test_1_comp("Position");
}

void Rules_2_comp() {
    test_2_comp("Position, Velocity");
}

void Rules_1_comp_explicit_subject() {
    test_1_comp("Position(.)");
}

void Rules_2_comp_explicit_subject() {
    test_2_comp("Position(.), Velocity(.)");
}

const char *rules =
"IsA(CelestialBody, Thing)\n"
"IsA(Planet, CelestialBody)\n"
"IsA(Moon, CelestialBody)\n"
"IsA(Person, Thing)\n"
"IsA(Machine, Thing)\n"
"IsA(SentientMachine, Machine)\n"
"IsA(Character, Person)\n"
"IsA(Human, Character)\n"
"IsA(Droid, Character)\n"
"IsA(Droid, SentientMachine)\n"
"IsA(Creature, Character)\n"
"IsA(Wookie, Creature)\n"
"IsA(Vehicle, Machine)\n"
"IsA(Transport, Vehicle)\n"
"IsA(Transport, Container)\n"
"IsA(SpaceShip, Transport)\n"
"IsA(Speeder, Transport)\n"
"IsA(CorellianLightFreighter, SpaceShip)\n"
"IsA(MilleniumFalcon, CorellianLightFreighter)\n"
"IsA(XWing, SpaceShip)\n"
"IsA(YWing, SpaceShip)\n"
"IsA(Cyborg, SentientMachine)\n"
"IsA(Cyborg, Human)\n"
"Sentient(Droid)\n"
"Sentient(Human)\n"
"Faction(XWing, Rebellion)\n"
"IsA(Rebellion, Faction)\n"
"IsA(FirstOrder, Faction)\n"
"AtWar(FirstOrder, Rebellion)\n"
"AtWar(Rebellion, FirstOrder)\n"
"Human(Luke)\n"
"Human(Leia)\n"
"Human(Rey)\n"
"Human(HanSolo)\n"
"Human(BenSolo)\n"
"Cyborg(Grievous)\n"
"Creature(Yoda)\n"
"Jedi(Yoda)\n"
"Jedi(Luke)\n"
"Jedi(Leia)\n"
"Jedi(Rey)\n"
"Sith(DarthVader)\n"
"Sith(Palpatine)\n"
"Droid(R2D2)\n"
"Droid(C3PO)\n"
"Droid(BB8)\n"
"Wookie(Chewbacca)\n"
"HomePlanet(Yoda, Dagobah)\n"
"HomePlanet(Luke, Tatooine)\n"
"HomePlanet(Rey, Tatooine)\n"
"HomePlanet(BB8, Tatooine)\n"
"HomePlanet(DarthVader, Mustafar)\n"
"Parent(Luke, DarthVader)\n"
"Parent(Leia, DarthVader)\n"
"Parent(BenSolo, HanSolo)\n"
"Parent(BenSolo, Leia)\n"
"Enemy(Luke, Palpatine)\n"
"Enemy(Luke, DarthVader)\n"
"Enemy(Yoda, Palpatine)\n"
"Enemy(Yoda, DarthVader)\n"
"Enemy(Rey, Palpatine)\n"
"Likes(Leia, HanSolo)\n"
"Likes(HanSolo, Leia)\n"
"Likes(Fin, Rey)\n"
"Likes(Rey, Ben)\n"
;

void Rules_1_fact_true() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Jedi(Yoda)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_1_fact_false() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sith(Yoda)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_facts_true() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Jedi(Yoda), Sith(DarthVader)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_facts_1_false() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sith(Yoda), Sith(DarthVader)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_facts_false() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sith(Yoda), Jedi(DarthVader)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_1_fact_pair_true() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Dagobah)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_1_fact_pair_false() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Tatooine)"
    });

    test_assert(r != NULL);
    
    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_fact_pairs_true() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Dagobah), HomePlanet(Luke, Tatooine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_int(it.count, 0);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_fact_pairs_1_false() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Dagobah), HomePlanet(Luke, Mustafar)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_2_fact_pairs_false() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(Yoda, Tatooine), HomePlanet(Luke, Mustafar)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_1_pair() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(., Tatooine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {"BB8", "Luke", "Rey"},
        .term_ids_expr = {{"(HomePlanet,Tatooine)"}}
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_2_pairs() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(., Tatooine), Enemy(., Palpatine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {"Luke", "Rey"},
        .term_ids_expr = {{"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}}
    }));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_w_pred_var() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(.), Jedi(.)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {
            "Luke", "Luke",
            "Yoda", "Yoda",
            "Leia", "Leia",
            "Rey", "Rey",
        },
        .term_ids_expr = {
            {"Human", "Jedi"},    {"Jedi", "Jedi"},
            {"Creature", "Jedi"}, {"Jedi", "Jedi"},
            {"Human", "Jedi"},    {"Jedi", "Jedi"},
            {"Human", "Jedi"},    {"Jedi", "Jedi"},
        },
        .variables = {
            {x_var, .entity_names = {
                "Human", "Jedi",
                "Creature", "Jedi",
                "Human", "Jedi",
                "Human", "Jedi",
            }}
        }
    }));
    
    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_w_pred_var_explicit_subject() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(Luke)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .term_ids_expr = {
            {"Human"}, {"Jedi"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Human", "Jedi"
            }}
        }
    }));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_1_pair_w_object_var() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(., X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {"BB8", "DarthVader", "Luke", "Yoda", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)"}, {"(HomePlanet,Mustafar)"}, 
            {"(HomePlanet,Tatooine)"}, {"(HomePlanet,Dagobah)"}, 
            {"(HomePlanet,Tatooine)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Tatooine", "Mustafar", "Tatooine", "Dagobah", "Tatooine"
            }}
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_2_pairs_w_object_var() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "HomePlanet(., X), Enemy(., Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);  

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {"Luke", "Luke", "Yoda", "Yoda", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)", "(Enemy,DarthVader)"}, 
            {"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}, 
            {"(HomePlanet,Dagobah)", "(Enemy,DarthVader)"},
            {"(HomePlanet,Dagobah)", "(Enemy,Palpatine)"}, 
            {"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Tatooine", "Tatooine", "Dagobah", "Dagobah", "Tatooine"
            }},
            {y_var, .entity_names = {
                "DarthVader", "Palpatine", "DarthVader", "Palpatine", "Palpatine"
            }}            
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_1_pair_w_pred_var() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(., Tatooine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {"BB8", "Luke", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)"},
        },
        .variables = {
            {x_var, .entity_names = {
                "HomePlanet"
            }}          
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);    
}

void Rules_find_2_pairs_w_pred_var() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(., Tatooine), Y(., Palpatine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);    

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {"Luke", "Rey"},
        .term_ids_expr = {
            {"(HomePlanet,Tatooine)", "(Enemy,Palpatine)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "HomePlanet"
            }},
            {y_var, .entity_names = {
                "Enemy"
            }}    
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_find_cyclic_pairs() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Likes(., X), Likes(X, .)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {"HanSolo", "Leia"},
        .term_ids_expr = {
            {"(Likes,Leia)", "(Likes,HanSolo)"},
            {"(Likes,HanSolo)", "(Likes,Leia)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Leia", "HanSolo"
            }}   
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_join_by_object() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Parent(., X), Parent(Y, X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {
            "BenSolo", "BenSolo",
            "Luke",    "Luke",
            "Leia",    "Leia"
        },
        .term_ids_expr = {
            {"(Parent,Leia)", "(Parent,Leia)"},             {"(Parent,HanSolo)", "(Parent,HanSolo)"},
            {"(Parent,DarthVader)", "(Parent,DarthVader)"}, {"(Parent,DarthVader)", "(Parent,DarthVader)"},
            {"(Parent,DarthVader)", "(Parent,DarthVader)"}, {"(Parent,DarthVader)", "(Parent,DarthVader)"}
        },
        .variables = {
            {x_var, .entity_names = {
                "Leia", "HanSolo",
                "DarthVader", "DarthVader",
                "DarthVader", "DarthVader",
            }},
            {y_var, .entity_names = {
                "BenSolo", "BenSolo",
                "Leia", "Luke",
                "Leia", "Luke",
            }}
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_join_by_predicate() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(., DarthVader), X(Y, DarthVader)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);
    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_true(test_iter(&it, ecs_rule_next, &(ecs_iter_result_t){
        .entity_names = {
            "Luke", "Luke", "Luke", "Luke",
            "Yoda", "Yoda",
            "Leia", "Leia",
        },
        .term_ids_expr = {
            {"(Parent,DarthVader)", "(Parent,DarthVader)"}, {"(Parent,DarthVader)", "(Parent,DarthVader)"},
            {"(Enemy,DarthVader)", "(Enemy,DarthVader)"},   {"(Enemy,DarthVader)", "(Enemy,DarthVader)"},

            {"(Enemy,DarthVader)", "(Enemy,DarthVader)"},   {"(Enemy,DarthVader)", "(Enemy,DarthVader)"},

            {"(Parent,DarthVader)", "(Parent,DarthVader)"}, {"(Parent,DarthVader)", "(Parent,DarthVader)"},
        },
        .variables = {
            {x_var, .entity_names = {
                "Parent", "Parent",
                "Enemy", "Enemy",
                "Enemy", "Enemy",
                "Parent", "Parent",
            }},
            {y_var, .entity_names = {
                "Leia", "Luke",
                "Luke", "Yoda",
                "Luke", "Yoda",
                "Leia", "Luke",
            }}
        }
    }));

    ecs_rule_fini(r);
    
    ecs_fini(world);
}

void Rules_join_by_predicate_from_subject() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Transitive(.), .(X, Character)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "IsA");
    test_term_id(&it, 1, "Transitive");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Creature");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "IsA");
    test_term_id(&it, 1, "Transitive");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "IsA");
    test_term_id(&it, 1, "Transitive");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_transitive() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(., Character)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Character");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Creature");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Creature)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Wookie");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Human");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_find_transitive_2_branches() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(., Thing)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Thing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 3);
    test_str(ecs_get_name(world, it.entities[0]), "CelestialBody");
    test_str(ecs_get_name(world, it.entities[1]), "Person");
    test_str(ecs_get_name(world, it.entities[2]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,CelestialBody)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "Planet");
    test_str(ecs_get_name(world, it.entities[1]), "Moon");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Person)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Character");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Creature");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Creature)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Wookie");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Human");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");
    test_str(ecs_get_name(world, it.entities[1]), "Vehicle");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Transport");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Transport)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "SpaceShip");
    test_str(ecs_get_name(world, it.entities[1]), "Speeder");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "CorellianLightFreighter");
    test_str(ecs_get_name(world, it.entities[1]), "YWing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,CorellianLightFreighter)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "MilleniumFalcon");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "XWing");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_subsets() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(., SpaceShip)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SpaceShip");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "CorellianLightFreighter");
    test_str(ecs_get_name(world, it.entities[2]), "YWing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,CorellianLightFreighter)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "MilleniumFalcon");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "XWing");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_subsets_2_terms() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(., Character), IsA(., Machine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Character)");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);    
}

void Rules_transitive_w_table_object() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Sentient(X), IsA(Y, X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Sentient");
    test_term_id(&it, 2, "(IsA,Droid)");
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Droid");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Sentient");
    test_term_id(&it, 2, "(IsA,Human)");
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Human");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Sentient");
    test_term_id(&it, 2, "(IsA,Human)");
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Cyborg");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);   
}

void Rules_transitive_supersets() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, .)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SpaceShip");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Transport)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Transport");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Vehicle");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Thing");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Container)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Container");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_superset_w_subj_var() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = ".(\\R2D2), IsA(., X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));

    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Droid)");
    test_var(&it, x_var, "Droid");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_var(&it, x_var, "SentientMachine");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_var(&it, x_var, "Machine");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Character");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Person)");
    test_var(&it, x_var, "Person");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");
    
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Droid");    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_superset_w_subj_var_2_term() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SentientMachine, .), IsA(., X)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_term_id(&it, 2, "(IsA,SentientMachine)");
    test_var(&it, x_var, "SentientMachine");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_var(&it, x_var, "Machine");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "SentientMachine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_var(&it, x_var, "Machine");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Machine)");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Machine");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Thing)");
    test_term_id(&it, 2, "(IsA,Thing)");
    test_var(&it, x_var, "Thing");   
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Thing");    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_supersets_2_terms() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, .), IsA(Container, .)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Container)");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Container");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_constraint_on_superset_var() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(\\C3PO), IsA(X, Y), Sentient(Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Droid)");
    test_term_id(&it, 3, "Sentient");
    test_var(&it, x_var, "Droid");
    test_var(&it, x_var, "Droid");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_instances() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X, IsA(X, Character)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Yoda");
    test_term_id(&it, 1, "Creature");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Creature");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Chewbacca");
    test_term_id(&it, 1, "Wookie");
    test_term_id(&it, 2, "(IsA,Creature)");
    test_var(&it, x_var, "Wookie");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "R2D2");
    test_str(ecs_get_name(world, it.entities[1]), "C3PO");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "BB8");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "BenSolo");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Luke");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");    

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Leia");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "HanSolo");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Rey");
    test_term_id(&it, 1, "Human");
    test_term_id(&it, 2, "(IsA,Character)");
    test_var(&it, x_var, "Human");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");
    test_term_id(&it, 1, "Cyborg");
    test_term_id(&it, 2, "(IsA,Human)");
    test_var(&it, x_var, "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_instances_2_terms() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X, IsA(X, Character), IsA(X, Machine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 2);    
    test_str(ecs_get_name(world, it.entities[0]), "R2D2");
    test_str(ecs_get_name(world, it.entities[1]), "C3PO");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_term_id(&it, 3, "(IsA,SentientMachine)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "BB8");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_term_id(&it, 3, "(IsA,SentientMachine)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 2);    
    test_str(ecs_get_name(world, it.entities[0]), "R2D2");
    test_str(ecs_get_name(world, it.entities[1]), "C3PO");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_term_id(&it, 3, "(IsA,Droid)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "BB8");
    test_term_id(&it, 1, "Droid");
    test_term_id(&it, 2, "(IsA,Character)");
    test_term_id(&it, 3, "(IsA,Droid)");
    test_var(&it, x_var, "Droid");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");
    test_term_id(&it, 1, "Cyborg");
    test_term_id(&it, 2, "(IsA,Human)");
    test_term_id(&it, 3, "(IsA,SentientMachine)");
    test_var(&it, x_var, "Cyborg");

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);    
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");
    test_term_id(&it, 1, "Cyborg");
    test_term_id(&it, 2, "(IsA,Human)");
    test_term_id(&it, 3, "(IsA,Cyborg)");
    test_var(&it, x_var, "Cyborg");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_implicit_isa_instances() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Machine"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Cyborg");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_int(it.count, 2);
    test_str(ecs_get_name(world, it.entities[0]), "R2D2");
    test_str(ecs_get_name(world, it.entities[1]), "C3PO");

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Droid");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "BB8");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_implicit_isa_instances_2_terms() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Human, Machine"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "Cyborg");
    test_term_id(&it, 2, "Cyborg");
    test_int(it.count, 1);
    test_str(ecs_get_name(world, it.entities[0]), "Grievous");

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_same_pred_obj() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);

    ecs_entity_t t1 = ecs_pair(Foo, Bar);
    ecs_entity_t t2 = ecs_pair(Foo, Foo);

    ecs_add_id(world, e1, t1);
    ecs_add_id(world, e2, t2);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(., X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_same_pred_obj_explicit_subject() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ECS_ENTITY(world, Ent, 0);

    ecs_entity_t t1 = ecs_pair(Foo, Bar);

    ecs_add_id(world, Ent, t1);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(Ent, X)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_1() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, SpaceShip)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_false() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Creature)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_2() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Transport)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);    

    test_assert(!ecs_rule_next(&it));
    
    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_3() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Vehicle)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);        

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_4() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Machine)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_depth_5() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(XWing, Thing)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,XWing)");
    test_int(it.count, 0);    

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_true_2_relationships() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(Cyborg, Thing)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Human)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Cyborg)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SentientMachine)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,Cyborg)");
    test_int(it.count, 0);        

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_all() {
    ecs_world_t *world = ecs_init();

    /* Use smaller rule set so testcase doens't blow up */
    const char *small_ruleset = 
        "IsA(CelestialBody, Thing)\n"
        "IsA(Planet, CelestialBody)\n"
        "IsA(Moon, CelestialBody)\n"
        "IsA(Character, Thing)\n"
        "IsA(Machine, Thing)\n"
        "IsA(Human, Character)\n"
        "IsA(Droid, Character)\n"
        "IsA(Droid, Machine)\n"
        "IsA(Creature, Character)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_ruleset) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(X, Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);

    /* All permutations of all valid IsA relationships */

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Character");
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Creature");
    test_var(&it, y_var, "Character");
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Human");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Creature");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Character");
    test_term_id(&it, 1, "(IsA,Character)");
    test_int(it.count, 0);    

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Machine");
    test_term_id(&it, 1, "(IsA,Machine)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Droid");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);          

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "CelestialBody");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Character");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);    

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Machine");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);    

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Planet");
    test_var(&it, y_var, "CelestialBody");
    test_term_id(&it, 1, "(IsA,CelestialBody)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Moon");
    test_var(&it, y_var, "CelestialBody");
    test_term_id(&it, 1, "(IsA,CelestialBody)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Planet");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Moon");
    test_var(&it, y_var, "Thing");
    test_term_id(&it, 1, "(IsA,Thing)");
    test_int(it.count, 0);

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_same_subj_obj() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, SpaceShip)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_int(it.count, 0); 

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_fact_subset_superset() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "IsA(SpaceShip, X), IsA(X, Machine)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    /* Results appear twice because transitive terms are inclusive by default */

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "SpaceShip");
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_term_id(&it, 2, "(IsA,Transport)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "SpaceShip");
    test_term_id(&it, 1, "(IsA,SpaceShip)");
    test_term_id(&it, 2, "(IsA,SpaceShip)");
    test_int(it.count, 0);     

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Transport");
    test_term_id(&it, 1, "(IsA,Transport)");
    test_term_id(&it, 2, "(IsA,Vehicle)");
    test_int(it.count, 0);  

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Transport");
    test_term_id(&it, 1, "(IsA,Transport)");
    test_term_id(&it, 2, "(IsA,Transport)");
    test_int(it.count, 0);

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Vehicle");
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_int(it.count, 0); 

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Vehicle");
    test_term_id(&it, 1, "(IsA,Vehicle)");
    test_term_id(&it, 2, "(IsA,Vehicle)");
    test_int(it.count, 0);   

    test_assert(ecs_rule_next(&it));
    test_var(&it, x_var, "Machine");
    test_term_id(&it, 1, "(IsA,Machine)");
    test_term_id(&it, 2, "(IsA,Machine)");
    test_int(it.count, 0);                  

    test_assert(!ecs_rule_next(&it));

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_nonfinal_fact() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Transitive(PartOf)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "PartOf(Painting, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_transitive_nonfinal_fact_w_implicit_pred_subset() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Transitive(ConnectedWith)\n"
        "IsA(PartOf, ConnectedWith)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "ConnectedWith(Painting, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact_2_lvls() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact_3_lvls() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(PaintingTmpl_3, PaintingTmpl_2)\n"
        "IsA(MonaLisa, PaintingTmpl_3)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);    
}

void Rules_implicit_is_a_tag_fact_2_lvls_both_matching() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "Painting(PaintingTmpl_2)\n"
        "IsA(PaintingTmpl, Painting)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "PaintingTmpl_2");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_tag_fact_2_lvls_entity_matching() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Painting(PaintingTmpl)\n"
        "Painting(MonaLisa)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "Painting(MonaLisa)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "MonaLisa");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_fact() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_2_lvls() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_3_lvls() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(PaintingTmpl_3, PaintingTmpl_2)\n"
        "IsA(MonaLisa, PaintingTmpl_3)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_2_lvls_both_matching() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "CreatedBy(PaintingTmpl_2, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl_2");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_2_lvls_entity_matching() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl, Painter)\n"
        "CreatedBy(MonaLisa, Painter)\n"
        "IsA(PaintingTmpl_2, PaintingTmpl)\n"
        "IsA(MonaLisa, PaintingTmpl_2)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "CreatedBy(MonaLisa, Painter)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "MonaLisa");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pred_var() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Canvas(PaintingTmpl)\n"
        "Painting(MonaLisa)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(MonaLisa)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Painting");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "Canvas");
    test_term_source(&it, 1, "PaintingTmpl");
    test_var(&it, x_var, "Canvas");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_pair_var() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "CreatedBy(PaintingTmpl,Painter)\n"
        "PaintedBy(MonaLisa,LeonardoDaVinci)\n"
        "IsA(MonaLisa, PaintingTmpl)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "X(MonaLisa, Y)"
    });

    test_assert(r != NULL);

    int32_t x_var = ecs_rule_find_variable(r, "X");
    test_assert(x_var != -1);

    int32_t y_var = ecs_rule_find_variable(r, "Y");
    test_assert(y_var != -1);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(Identifier,Name)");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "Identifier");
    test_var(&it, y_var, "Name");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(IsA,PaintingTmpl)");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "IsA");
    test_var(&it, y_var, "PaintingTmpl");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PaintedBy,LeonardoDaVinci)");
    test_term_source(&it, 1, "MonaLisa");
    test_var(&it, x_var, "PaintedBy");
    test_var(&it, y_var, "LeonardoDaVinci");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(Identifier,Name)");
    test_term_source(&it, 1, "PaintingTmpl");
    test_var(&it, x_var, "Identifier");
    test_var(&it, y_var, "Name");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(CreatedBy,Painter)");
    test_term_source(&it, 1, "PaintingTmpl");
    test_var(&it, x_var, "CreatedBy");
    test_var(&it, y_var, "Painter");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_transitive_pair_fact() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Transitive(PartOf)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n"
        "IsA(MonaLisa, Painting)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "PartOf(MonaLisa, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_subset() {
    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Transitive(ConnectedWith)\n"
        "IsA(PartOf, ConnectedWith)\n"
        "PartOf(ArtCollection, Museum)\n"
        "PartOf(Painting, ArtCollection)\n"
        "IsA(MonaLisa, Painting)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "ConnectedWith(MonaLisa, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}

void Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_obj() {
    test_quarantine("Aug 31 2021");

    ecs_world_t *world = ecs_init();

    const char *small_rules = 
        "Transitive(PartOf)\n"
        "PartOf(LouvreExhibit, Louvre)\n"
        "PartOf(LouvrePainting, LouvreExhibit)\n"
        "IsA(Louvre, Museum)\n"
        "IsA(MonaLisa, LouvrePainting)\n";

    test_assert(ecs_plecs_from_str(world, NULL, small_rules) == 0);

    ecs_rule_t *r = ecs_rule_init(world, &(ecs_filter_desc_t){
        .expr = "PartOf(MonaLisa, Museum)"
    });

    test_assert(r != NULL);

    ecs_iter_t it = ecs_rule_iter(r);
    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,ArtCollection)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == true);
    test_term_id(&it, 1, "(PartOf,Painting)");
    test_term_source(&it, 1, "Painting");

    test_assert(ecs_rule_next(&it) == false);

    ecs_rule_fini(r);

    ecs_fini(world);
}
