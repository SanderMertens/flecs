#include <query.h>

void QueryStr_one_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "TagA");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_inout(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA, .inout = EcsIn }
        }
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "[in] TagA");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_two_terms(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA },
            { TagB }
        }
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "TagA, TagB");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_two_terms_w_inout(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA },
            { TagB, .inout = EcsIn }
        }
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "TagA, [in] TagB");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_three_terms_w_or(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA },
            { TagB, .oper = EcsOr },
            { TagC }
        }
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "TagA, TagB || TagC");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_three_terms_w_or_inout(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA },
            { TagB, .oper = EcsOr, .inout = EcsIn },
            { TagC, .inout = EcsIn }
        }
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "TagA, [in] TagB || TagC");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_four_terms_three_w_or_inout(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { TagA },
            { TagB, .oper = EcsOr, .inout = EcsIn },
            { TagC, .inout = EcsIn },
            { TagD, .inout = EcsIn }
        }
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "TagA, [in] TagB || TagC, [in] TagD");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, Tgt) }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "(Rel,Tgt)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_pair_entity_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Src);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = ecs_pair(Rel, Tgt), .src.id = Src }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Rel(Src,Tgt)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_self(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Foo, .src.id = EcsSelf }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Foo(self)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Foo, .src.id = EcsUp }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Foo, .src.id = EcsIsEntity }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Foo()");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .id = Foo, .src.id = Foo }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Foo($)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_final_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, Final);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Foo }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Foo");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_final_dont_inherit_pair(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, Final, DontInherit);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Foo }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Foo");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_src_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Tag, .src.id = EcsIsVariable, .src.name = "Var" }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Tag($Var)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_first_var(void) {
    ecs_world_t *world = ecs_mini();

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = EcsIsVariable, .first.name = "Var" }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "[none] $Var");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_second_var(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = Rel, .second.id = EcsIsVariable, .second.name = "Var" }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "(Rel,$Var)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_first_var_entity_src(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Src);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ .first.id = EcsIsVariable, .first.name = "Var", .src.id = Src }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "[none] $Var(Src)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_one_term_w_pair_w_0_entity(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .first.id = Rel, .second.id = Tgt, .src = {
                .id = EcsIsEntity
            } }
        }
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "Rel(0,Tgt)");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_not_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ TagA, .oper = EcsNot }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "!TagA");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_wildcard_term(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ EcsWildcard }}
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "[none] *");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_scopes(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "TagA, {TagB, {TagC}}"
    });
    test_assert(q != NULL);

    char *str = ecs_query_str(q);
    test_str(str, "TagA, {TagB, {TagC}}");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}


void QueryStr_pred_eq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == TagA"
    });

    char *str = ecs_query_str(q);
    test_str(str, "$this == TagA");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_pred_neq(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != TagA"
    });

    char *str = ecs_query_str(q);
    test_str(str, "$this != TagA");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_pred_eq_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this == \"TagA\""
    });

    char *str = ecs_query_str(q);
    test_str(str, "$this == \"TagA\"");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_pred_neq_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this != \"TagA\""
    });

    char *str = ecs_query_str(q);
    test_str(str, "$this != \"TagA\"");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_pred_eq_m(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"TagA\""
    });

    char *str = ecs_query_str(q);
    test_str(str, "$this ~= \"TagA\"");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

void QueryStr_pred_neq_m(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *q = ecs_query(world, {
        .expr = "$this ~= \"!TagA\""
    });

    char *str = ecs_query_str(q);
    test_str(str, "$this ~= \"!TagA\"");
    ecs_os_free(str);

    ecs_query_fini(q);

    ecs_fini(world);
}

