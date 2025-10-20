#include <script.h>

static
void fuzz(const char *expr) {
    ecs_world_t *world = ecs_init();
    ecs_log_set_level(-5);
    ecs_script_run(world, "test", expr, NULL);
    test_assert(true);
    ecs_fini(world);
}

void Fuzzing_1(void) {
    const char *expr =
    HEAD "const pi = 3.1415926"
    LINE "const pi_2 = $pi * 2"
    LINE " $pi "
    LINE "my_entity {"
    LINE "tation: {aUg/ 2}"
    ;

    fuzz(expr);
}

void Fuzzing_2(void) {
    const char *expr =
    HEAD "const pi = 3.141"
    LINE "const pi_2 =415926"
    LINE "c�\\entity ⏎"
    ;

    fuzz(expr);
}

void Fuzzing_3(void) {
    const char *expr =
    HEAD "const pi = 3.1415926"
    LINE "ct\\\\\\\\\\\\\nst pist= 3\\415926"
    LINE "consu pi_2??? = 0pi * $"
    LINE ""
    LINE "my_intity {"
    LINE "t\\\\\\\\\\\\\\\\\\\\.1"
    ;

    fuzz(expr);
}

void Fuzzing_4(void) {
    const char *expr =
    HEAD "coRRRRRRla,#44444444444444444444a,C"
    LINE "strumela,#44444444444444"
    ;

    fuzz(expr);
}

void Fuzzing_5(void) {
    const char *expr =
    HEAD "coRRRSVRRco3V.melconsta,#43444444444a,melRRcoRV.melconsta,#43444444444a,#234444"
    ;

    fuzz(expr);
}

void Fuzzing_6(void) {
    const char *expr =
    HEAD "VRRcoRcoRRRSRRRRRRRRRR,#23444nsta,#434.melRRcoRV,#23444nsta,#434..melco4"
    ;

    fuzz(expr);
}

void Fuzzing_7(void) {
    const char *expr =
    HEAD "coRRla,X"
    LINE "RRRRRR\\.mella,C"
    LINE "struct Posi {JRRRR\\.melRQRR.m :C"
    LINE "ISRR\\.m :C"
    LINE "RRRRRla,C"
    LINE "ISRR\\.v :C"
    LINE "RRRRRela,Ct pi =C"
    LINE "sts\\.RR.m :C"
    LINE "ISRR\\.v :C"
    LINE "RRRRRela,Ct pi =>3.13* 2"
    LINE ""
    LINE "my_entit}"
    LINE ""
    LINE "RRla,D"
    LINE "s"
    ;

    fuzz(expr);
}

void Fuzzing_8(void) {
    const char *expr =
    HEAD "coc,a,#44464RN,X,#4444444444444"
    LINE "RRRRRR\\.mconstella,C"
    LINE "struct Posi {JRla,Ct pf =C"
    LINE "sts\\.RR.m :C"
    LINE "ISRR\\const.vRRR\\.melRQRR.m :C"
    LINE "ISRR\\.m :C"
    LINE "RRRRRla,C"
    LINE "ISRR\\.v :C"
    LINE "RRRRRela,Ct pf =C"
    LINE "sts\\.RR.m :C"
    LINE "ISRR\\.v :C"
    LINE "RRRR= f32"
    LINE "  y  = 3.13*"
    LINE "my_t}"
    LINE "la,D"
    ;

    fuzz(expr);
}

void Fuzzing_9(void) {
    install_test_abort();

    test_expect_abort();

    const char *expr =
    HEAD "coc,a,#44464RN,X"
    LINE "RRRRRR\\.mconstella,C"
    LINE "struct Posi {JRla,Ct pf =C"
    LINE "sts\\.RR.m :C {}"
    LINE "ISRR\\const.vRRR\\.melRQRR.m :C {}"
    LINE "ISRR\\.m :C {}"
    LINE "RRRRRla,C"
    LINE "ISRR\\.v :C {}"
    LINE "RRRRRela,CtRRRSVR"
    LINE "sts\\.RR.m :C {}"
    LINE "ISRRconst\\.vRca,#434lRRcoRV.melcon pf =C"
    LINE "sts\\.RR.m :C {}"
    LINE "ISRRconst\\.v =C"
    LINE "sts\\.RR.m :C {}"
    LINE "ISRR\\.v ;C:C {}"
    LINE "RRRR= f32"
    LINE "@ y  = 3.13*"
    LINE "my_t}"
    LINE "la,D"
    ;

    fuzz(expr);
}

void Fuzzing_10(void) {
    const char *expr =
    HEAD "coc,a,#44464RN,X"
    LINE "RRRRRR\\.mconstella,C"
    LINE "struct Posi {JRla,CCCCCCCCCCCCCCCCCt pf =C"
    LINE "sts\\.RR.m :C"
    LINE "ISRR\\const.vRRR\\.melRQRR.m :C"
    LINE "ISRR\\.m :C"
    LINE "RRTRRla,C"
    LINE "ISRR\\.v :C"
    LINE "RRRRRela,CtRRRSVRhip<:$t {"
    LINE "    Position: {"
    LINE "sts\\.RR.m :C"
    LINE "ISRRconst\fab SpaceShip {"
    LINE "    Position: {"
    LINE "sts\\. {.vRca,#434lRRcoRV.melcon pf =C"
    LINE "sts\\.RR.m :C"
    LINE "ISRRconst\\.v :C"
    LINE "RRRR> f32"
    LINE "@ y const13*"
    LINE "my_t}"
    LINE "la,D"
    ;

    fuzz(expr);
}

void Fuzzing_11(void) {
    const char *expr =
    HEAD "coRRla,X"
    LINE "RRRRRR\\.mella,C"
    LINE "struct Posi {JRRRR\\.melRQRR.m :C"
    LINE "ISRR\\.m :C"
    LINE "RRRRRla,C"
    LINE "ISRR\\.v :C"
    LINE "RRRRRela,Ct pi1=*"
    LINE "sts\\.RR.m :C"
    LINE "ISRR\\.v ;C"
    LINE "RRRRRela, #"
    LINE ""
    LINE "my_entit}"
    LINE ""
    LINE "RRla,D"
    LINE "s"
    ;

    fuzz(expr);
}

void Fuzzing_12(void) {
    const char *expr =
    HEAD "coRcoa,a,#44464RN,X"
    LINE "RR\\.la,C"
    LINE "struct Posi {JRla,Ct pU.mesu.RR.R.d {"
    LINE "  vacts\\.RR.m :C"
    LINE "ISRR\\const.v\\.melRQRR.m :C"
    LINE "ISRR\\.m :C"
    LINE "RRRconstRRla,C"
    LINE "ISRR\\.v :C"
    LINE "RRSRRela,CtJRRSVR"
    LINE "sts\\.RR.m :C"
    LINE "IS.melaMaxSpeed {"
    LINE "  value = f32"
    LINE "vRca,#4const399999999.melcon pla,CRR\\..R"
    LINE "f =C"
    LINE "lue = f32"
    LINE "}"
    LINE ""
    LINE "C"
    LINE "sts\\.RR.d {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "m :C"
    LINE "ISRRconst\\.v :C"
    LINE "RRRR= f32"
    LINE "@ y  13�  value = f32"
    LINE "}"
    LINE ""
    LINE "m :C"
    LINE "ISR*"
    LINE "my_t}"
    LINE "l"
    ;

    fuzz(expr);
}

void Fuzzing_13(void) {
    const char *expr =
    HEAD "coc,a,#44464RN,X"
    LINE "RRRRRR\\.mconstelld,C"
    LINE "struct Posi {JRla,Ct pf =C"
    LINE "sts\\.RR.m :C"
    LINE "ISRR\\const.vRRR\\.melRQRR.m :C"
    LINE "ISRR\\\\\\\\\\\\\\,C"
    LINE "ISRR\\.v :C"
    LINE "RRRRRela,CtRRRSVRhip<:$t {"
    LINE "    Position: {"
    LINE "sts\\.RR.r :C"
    LINE "ISRRconst\fab SpaceShip {.vRca,#434l :C"
    LINE "IRRcoRV.melcon pf =C"
    LINE "sts\\.RR.m :C"
    LINE "ISRRconst\\.v :C"
    LINE "RRRR> f32"
    LINE "@ y  = 3.13*"
    LINE "my_t}"
    LINE "la,D"
    ;

    fuzz(expr);
}

void Fuzzing_14(void) {
    const char *expr =
    HEAD "#299 psC"
    LINE "RRRRm###44"
    ;

    fuzz(expr);
}
