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

/* crash=out/fuzzer02/crashes/id:000000,sig:11,src:000009,time:34419,execs:4908,op:havoc,rep:1, sha1=28c972a5c8e7fc949987fd9cb73b615aea9410b9
 * asan_stack:
 *     #0 0x000105783128 in __asan_memcpy (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x3b128)
 *     #1 0x000104e28d3c in ecs_value_copy_w_type_info flecs/src/value.c:142:9
 *     #2 0x000104e28d3c in ecs_value_copy flecs/src/value.c:159:12
 *     #3 0x000104ccd128 in flecs_expr_initializer_post_fold flecs/src/addons/script/expr/visit_fold.c:314:13
 *     #4 0x000104ccb60c in flecs_expr_initializer_visit_fold flecs/src/addons/script/expr/visit_fold.c:345:13
 *     #5 0x000104ccb60c in flecs_expr_visit_fold flecs/src/addons/script/expr/visit_fold.c:563:13
 *     #6 0x000104cff5fc in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:527:13
 *     #7 0x000104d032f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #8 0x000104d032f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #9 0x000104cfb370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #10 0x000104cff9a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #11 0x000104d010d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 */
void Fuzzing_15(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE "  left = Controller"
    LINE "  right = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: i32, count: 3}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE "    }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer02/crashes/id:000001,sig:11,src:000009,time:37726,execs:5396,op:havoc,rep:10, sha1=38e3f6971329bed18ce181e0cb1a85b3b172b8f0
 * asan_stack:
 *     #0 0x000100204a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100204a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000100202b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x00010028bad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x00010028e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x00010028a530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #6 0x00010028fa38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #7 0x000100291544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x0001001a8a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x0001001a8a7c in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_16(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = fffffffffffffffffffffffffffffffff32"
    LINE "}"
    LINE ""
    LINE "vtruct PI\\ {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct Cont{type: i32, coun\216: 7}"
    LINE "}"
    LINE ""
    LINE "FibVerolPair {"
    LINE "  left = Controller"
    LINE "  rig\203t = dontr\214ller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: i32, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains.5e1"
    LINE ": {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      di\177"
    LINE ""
    LINE ""
    LINE "ion: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE "   \240}"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer02/crashes/id:000002,sig:11,src:000009,time:39381,execs:5638,op:havoc,rep:1, sha1=428b7e010e7d903e0ccce91f61b9aacd54b50124
 * asan_stack:
 *     #0 0x000101479748 in __sanitizer::StackDepotBase<__sanitizer::StackDepotNode, 1, 20>::Put(__sanitizer::StackTrace, bool*) (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x65748)
 *     #1 0x000101416be8 in __asan::Allocator::Allocate(unsigned long, unsigned long, __sanitizer::BufferedStackTrace*, __asan::AllocType, bool) (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x2be8)
 *     #2 0x0001014168e4 in __asan::asan_malloc(unsigned long, __sanitizer::BufferedStackTrace*) (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x28e4)
 *     #3 0x000101451390 in malloc (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x3d390)
 *     #4 0x0001007d2038 in flecs_map_bucket_add flecs/src/datastructures/map.c:78:37
 *     #5 0x0001007d2038 in ecs_map_ensure flecs/src/datastructures/map.c:281:24
 *     #6 0x0001007d007c in flecs_hashmap_ensure_ flecs/src/datastructures/hashmap.c:140:27
 *     #7 0x0001007d44cc in flecs_name_index_ensure flecs/src/datastructures/name_index.c:233:34
 *     #8 0x0001006d50f8 in flecs_meta_serialize_struct flecs/src/addons/meta/serializer.c:501:9
 *     #9 0x0001006d50f8 in flecs_meta_serialize_type flecs/src/addons/meta/serializer.c:533:32
 *     #10 0x0001006d4f04 in flecs_meta_serialize_struct flecs/src/addons/meta/serializer.c:483:17
 *     #11 0x0001006d4f04 in flecs_meta_serialize_type flecs/src/addons/meta/serializer.c:533:32
 */
void Fuzzing_17(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "troller_enControlPair {"
    LINE "  left = Controller"
    LINE "  right = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: i32, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "con}"
    LINE ""
    LINE "struct tity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE "    }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer02/crashes/id:000003,sig:11,src:000005,time:140104,execs:20199,op:havoc,rep:2, sha1=379eaa226792fb1cff0c5e03c85a3442b8289331
 * asan_stack:
 *     #0 0x000100f86234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x000100f845ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x000100f837c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x000100fb35e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x000100fb72f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x000100fb72f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x000100faf370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x000100fb39a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x000100fb50d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x000100faf648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x000100fb6b24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x000100fb6b24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_18(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Mass {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "root {"
    LINE "  Nameplate: {\"root\"}with Mass {"
    LINE "cd_a { Mass: {1.5} }"
    LINE "child_b { Mass: {2.5} }"
    LINE "  }"
    LINE ""
    LINE "  { Nameplate: {\"anonymo\\s child\"}"
    LINE "Mass: {3.5}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer02/crashes/id:000004,sig:11,src:000005+000008,time:232662,execs:33440,op:splice,rep:7, sha1=7e9d39714e27ad7f44e11bbba722c6c30399be9c
 * asan_stack:
 *     #0 0x00010074ca08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x00010074ca08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x00010074ab50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001007d24d0 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #4 0x0001007d7a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #5 0x0001007d9544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x0001006f0a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x0001006f0a7c in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_19(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Nameplatnt: $count, bastrin\020"
    LINE "}"
    LINE ""
    LINE "struct Mass {"
    LINE "  value = fass: {1.5} {"
    LINE "  Nameplate: {\"root\"}with Mass {"
    LINE "child_a { M32"
    LINE "}"
    LINE ""
    LINE "root }"
    LINE "chilusing flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE " $y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.me\204a.f32: 0"
    LINE "  prop offset = flecs.meta.f32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Leaf: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster\177 {coue {"
    LINE "  v\020"
    LINE "ue = se: $base, offs"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer02/crashes/id:000005,sig:11,src:000007,time:606033,execs:88526,op:havoc,rep:2, sha1=85d042cd61fd1b88875c6f6dfac410a4dc6681dc
 */
void Fuzzing_20(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE " xed = Vec3"
    LINE "z = f64"
    LINE "}"
    LINE ""
    LINE "struct MathSample {"
    LINE "  trig = f64"
    LINE "  power = f64"
    LINE "  root = f64"
    LINE "  dot_value = f64"
    LINE "  len_value = f64"
    LINE "  mixed = Vec3"
    LINE "  unit = Vec3"
    LINE "}"
    LINE ""
    LINE "const a = Vec3: {1.0e1, -2.5e1, 3.1}"
    LINE "const b = Vec3: {2.0, 4.0, -6.0}"
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE ""
    LINE "const trig: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const power: polog10(1.0e3)"
    LINE "const root: sqrt(1.6e1) + abs(-2.5)"
    LINE "const mixed: lerp($a, $b, 0.25)"
    LINE "const unit: normalize($axis)"
    LINE "const dot_value: dot($a, $b)"
    LINE "const len_value: length($axis)"
    LINE ""
    LINE "mentity {"
    LINE "  MathSample: { trig: $trig"
    LINE "   $power"
    LINE "    root: $root"
    LINE "    dot_value: $dot_value len_value: $len_value"
    LINE "    mixed: $mixed"
    LINE "    unit: $unit"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer03/crashes/id:000000,sig:11,src:000008,time:632949,execs:90081,op:havoc,rep:12, sha1=7be89e2b7a3ce314e7af1e5580876db28b04176b
 * asan_stack:
 *     #0 0x000100b9ca08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100b9ca08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000100b9ab50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000100c23ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000100c26288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000100c21078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x000100c26288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000100c21648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #8 0x000100c27a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000100c29544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000100b40a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000100b40a7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_21(void) {
    const char *expr =
    HEAD "using flecs.mQt3"
    LINE ""
    LINE "struct Position {"
    LINE "  x5= f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaunt: $count, ba Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = ffffffffffff32: 3"
    LINE "  pqop b,ase, offset: 3} prop offset = flecs.meth.f32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Leaf: s.meta.f32:= e: $base +  {"
    LINE "    Lea }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster: {count: $count, base: $base, offset: -2} }"
    LINE "  right { Cluster: {cof_{$idx}\" {"
    LINE "   se: $base, of\231set: 3} }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer04/crashes/id:000000,sig:11,src:000005,time:19403,execs:2948,op:havoc,rep:1, sha1=61bb719881a454a38dee9b845e4b11c2694d2b05
 * asan_stack:
 *     #0 0x000100c0e234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x000100c0c5ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x000100c0b7c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x000100c3b5e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x000100c3f2f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x000100c3f2f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x000100c37370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x000100c3b9a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x000100c3d0d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x000100c37648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x000100c3eb24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x000100c3eb24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_22(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Mass {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "root {"
    LINE "  Nameplate: {\"root\"}with Mass {"
    LINE "child_a { Mass: {1.5} }"
    LINE "child_b { Mass: {2.5} }"
    LINE "  }"
    LINE ""
    LINE "  { Nameplate: {\"anon\\mous child\"}"
    LINE "Mass: {3.5}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer04/crashes/id:000001,sig:11,src:000395,time:383902,execs:55897,op:havoc,rep:1, sha1=ac7f719e1e85bb6feb4fd80d79b0a33e818b1dbf
 * asan_stack:
 *     #0 0x00010484a234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x0001048485ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x0001048477c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x0001048775e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x00010487b2f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x00010487b2f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x000104873370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x0001048779a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x0001048790d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x000104873648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x00010487ab24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x00010487ab24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_23(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Mass {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "root {"
    LINE "  Nameplate: {\"Ma{1.5} }"
    LINE "chroot\"}with Mass {"
    LINE "child_a { Mass: {1.5} }"
    LINE "child_b { Mass: {2.5} }"
    LINE "  }"
    LINE ""
    LINE "  { Nameplate: {\"an\026nymous ch\\ld\"}"
    LINE "Mass: {3.5}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000000,sig:11,src:000008,time:93641,execs:13452,op:havoc,rep:2, sha1=fddfbe493953395362308fc1fe8e86a92b4be5b0
 * asan_stack:
 *     #0 0x0001041a4a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x0001041a4a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x0001041a2b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x00010422bad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x00010422e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104229078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x00010422e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000104229648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #8 0x00010422fa38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000104231544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000104148a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000104148a7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_24(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 0"
    LINE "  prop offset = flecs.meta.f32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Leaf: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster: {count: $count, base: $base, offset: -2} se: $base,\037offset: -2} }unt: $count, base: $base, offset: 3} }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000001,sig:11,src:000509,time:173806,execs:24720,op:havoc,rep:7, sha1=482c77f460748f2c4f0c3d9ed30daaa39188b0f4
 * asan_stack:
 *     #0 0x000104170a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104170a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x00010416eb50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001041f7ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x0001041fba38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #5 0x0001041fd544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x000104114a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x000104114a7c in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_25(void) {
    const char *expr =
    HEAD "uunt: $count, bas\225\225\225\225\225\225\225e: $tion {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct\240Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$dx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "\023 prop base = flecs.meta.\231(2: 0"
    LINE "  prop offset = flecs.meta.count, base: $base, offset: -2} }"
    LINE "  right { CZuster: {count: $count, base: 9base, offset: 3} }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, ba"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000002,sig:11,src:000002,time:411640,execs:59754,op:havoc,rep:13, sha1=e66f20148e9861145fa450af7dffc0f1d5b9d919
 * asan_stack:
 *     #0 0x00010289ca08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x00010289ca08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x00010289ab50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000102923ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000102926288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000102922530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #6 0x000102927a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #7 0x000102929544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000102840a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000102840a7c in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_26(void) {
    const char *expr =
    HEAD "using flecN.meta"
    LINE ""
    LINE "struct Attack {"
    LINE "  valuse: $base, \177\377fseb: -2ense {"
    LINE "  value < f32"
    LINE "}"
    LINE ""
    LINE "prefab Ship {"
    LINE "  Attab Tank : Ship {ack\324 {10}"
    LINE "  Def\377"
    LINE "ct Defense {"
    LINE "  value = fnse: {5}"
    LINE "}"
    LINE ""
    LINE "prefab Tank;: Ship {"
    LINE "  Attack: {20}"
    LINE "  Defense: {25}"
    LINE "}"
    LINE ""
    LINE "with Ship {"
    LINE "  scou\177_a {}"
    LINE "  scout_b {}"
    LINE "}"
    LINE ""
    LINE "battle_tank : Tank {"
    LINE "  At]ack:\037{30}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000003,sig:11,src:000002,time:416163,execs:60428,op:havoc,rep:11, sha1=8fcb77b93b15994a8e2c956123fd7b24937fe7af
 * asan_stack:
 *     #0 0x000104648a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104648a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000104646b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001046ce4d0 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #4 0x0001046d2288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x0001046cd078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x0001046d2288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x0001046ce530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #8 0x0001046d3a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x0001046d5544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x0001045eca7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x0001045eca7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_27(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct At{ack {"
    LINE "  value ase: $base,\034offset: -2} }"
    LINE "  \207= f32"
    LINE "}"
    LINE ""
    LINE "struUt 10}"
    LINE "  Defense: {5}"
    LINE "}"
    LINE "Defenseht {"
    LINE "  vafab Ship {"
    LINE "  Attack: t, base: $ba{10}"
    LINE "  Defense: {5}"
    LINE "}"
    LINE ""
    LINE "prefab Tank : Ship {"
    LINE "  Attack: {20}"
    LINE "  Defense: {25}"
    LINE "}"
    LINE ""
    LINE "wi"
    LINE ""
    LINE ""
    LINE "\020hip { {"
    LINE "  value ase: "
    LINE "  scout_a {}"
    LINE "  scout_b {}"
    LINE "}"
    LINE ""
    LINE "battle_tank : Tank {"
    LINE "  Attack: {30}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000000,sig:11,src:000008,time:96417,execs:13968,op:havoc,rep:14, sha1=67ab09a48e2ec9d3ad63fbcfd5d2be2fa892bf1a
 * asan_stack:
 *     #0 0x000100654a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100654a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000100652b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001006dbad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x0001006de288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x0001006d9078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x0001006de288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x0001006d9648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #8 0x0001006dfa38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x0001006e1544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x0001005f8a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x0001005f8a7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_28(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = C32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = fkecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "     base: $base, off\212et: 3  0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop cVun 1.5, $baseeta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 0"
    LINE "  prop offset = flecs\030meta.f32: 0"
    LINE ""
    LINE "  for\032i in 0..$count {"
    LINE "  \020 Leaf: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "templ\020te Branch {"
    LINE "  prop count = fle\200"
    LINE ""
    LINE ""
    LINE "eta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster: {count: $count, qase: $base, offset: -2} }"
    LINE "  right { Cluster: {count: $count,Weight: {$base + ($idx *Leaf: {idx: $Y, banch: {count: 4, base: 1.5} }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000001,sig:11,src:000008,time:108052,execs:15668,op:havoc,rep:12, sha1=3ac258ba549953a7f8db958044b4505d0ad03a92
 * asan_stack:
 *     #0 0x000100920a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100920a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x00010091eb50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001009a7ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x0001009aba38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #5 0x0001009ad544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x0001008c4a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x0001008c4a7c in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_29(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "templaidx: $i, bas\367: $base + $offste Leaf {"
    LINE "  prop idx = flecs.meta.i3t { Cl2: 0"
    LINE "  prop base = f\223ecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template luster {"
    LINE "  prop co\177\377t = flecs.meta.i32: 3"
    LINE "  p\001op base = flecs.meta.f\3142: 0"
    LINE "  prop offset = flecs.mf32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Leaf: {idx: $i, bas\367: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  p = f32"
    LINE "}rop ba32: 2"
    LINE ""
    LINE "  left { Cluster: {count: $count, base: $base, offset: -2} }"
    LINE "  right { Cluster: {count: $count, base: $base, offset: 3} }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} }"
    LINE "trde_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000002,sig:11,src:000002,time:288101,execs:40399,op:havoc,rep:5, sha1=f7386dfbec580c98268fce17c8b45e9f022414dc
 * asan_stack:
 *     #0 0x000104a80a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104a80a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000104a7eb50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000104b07ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000104b0ba38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #5 0x000104b0d544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x000104a24a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x000104a24a7c in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_30(void) {
    const char *expr =
    HEAD "usingbase: $base, offs\204t: -2} }"
    LINE "  rivalue = f32"
    LINE "}"
    LINE ""
    LINE "struct Defense {"
    LINE "  vaaaaaaaaaaaaaaaaaaaaaaaaaaaaaalue = f32"
    LINE "}"
    LINE ""
    LINE "prefab Ship {"
    LINE "  Attack: {10}"
    LINE "  Defense: {5}"
    LINE "}"
    LINE ""
    LINE "prefab Tank : Ship {"
    LINE "  Attack: {20}"
    LINE "  Defense: {25}"
    LINE "}"
    LINE ""
    LINE "with Ship {"
    LINE "  scout_a {}"
    LINE "  scout_b {}"
    LINE "}"
    LINE ""
    LINE "battle_tunt: ank : Tank {"
    LINE "  Atck: {20}"
    LINE "  Defense:tack: {30}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000003,sig:11,src:000456,time:589757,execs:84831,op:havoc,rep:3, sha1=b648eca4ecbf1faf13f5841b7a9cecb11d4fe2da
 * asan_stack:
 *     #0 0x0001001cca08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x0001001cca08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x0001001cab50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001002524d0 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #4 0x000100256288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000100251078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x000100256288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000100251648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #8 0x000100257a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000100259544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000100170a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000100170a7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_31(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 0"
    LINE "  prop offset = flecs.meta.f32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Leaf: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster: {count: $count, base: $base, offset: -2} }"
    LINE "  right { Cluster base: $base, offse\200: 3} }"
    LINE "}"
    LINE ""
    LINE "tree.meta_a { Branch: {count: 4, base: 1.5"
    LINE " }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer07/crashes/id:000000,sig:11,src:000293,time:138515,execs:19966,op:havoc,rep:1, sha1=046d53ff73f610573d9474f433d7991460644769
 * asan_stack:
 *     #0 0x000100bba234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x000100bb85ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x000100bb77c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x000100be75e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x000100beb2f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x000100beb2f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x000100be3370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x000100be79a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x000100be90d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x000100be3648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x000100beab24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x000100beab24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_32(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Nameplate {alue = string"
    LINE "}"
    LINE ""
    LINE "struct Mass { Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "root {"
    LINE "  Nameplate: {\"\\oot\"}with Mass {"
    LINE "child_a { Mass: {1.5} }"
    LINE "child_b { Mass: {2.5} }"
    LINE "  }"
    LINE ""
    LINE "  { Nameplate: {\"anonymous child\"}"
    LINE "Mass: {3.5}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer07/crashes/id:000001,sig:11,src:000008,time:548382,execs:79917,op:havoc,rep:6, sha1=a785c0aefe1afb3eacdd46e1d8bd3df68dd4a8fb
 * asan_stack:
 *     #0 0x000104304a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104304a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000104302b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x00010438bad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x00010438e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104387b54 in flecs_script_stmt flecs/src/addons/script/parser.c:424:12
 *     #6 0x00010438e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000104389078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #8 0x00010438e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #9 0x000104389648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #10 0x00010438fa38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #11 0x000104391544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 */
void Fuzzing_33(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    {count: $count, base\372"
    LINE ""
    LINE "\372base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32:plate Cluster {"
    LINE " flecs.meta.f32: 0"
    LINE "  prop offset = flecs.meta.f32: 0"
    LINE ""
    LINE "  for i in 0 .$count {"
    LINE "    Leaf: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster: {count: $count, base: $base, of32: 0"
    LINE "  prop offsht { Cluster: {count: $count, base: $base, offset: 3\224 }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer08/crashes/id:000000,sig:11,src:000555,time:188612,execs:27167,op:havoc,rep:4, sha1=67e77e9088c8982c6cb1ab8fece7d9c8e74b3520
 * asan_stack:
 *     #0 0x0001027dca08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x0001027dca08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x0001027dab50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000102863ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000102867a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #5 0x000102869544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x000102780a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x000102780a7c in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_34(void) {
    const char *expr =
    HEAD "Planet {}"
    LINE "Sp{}"
    LINE "Fon {}"
    LINE "En {}"
    LINE "ght {}"
    LINE ""
    LINE "h { et }"
    LINE ""
    LINE "my_spaip {ip }"
    LINE ""
    LINE "my_aZtle_tank : Tanon,  Fas\206er {ip }"
    LINE ""
    LINE "mnLight"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer08/crashes/id:000001,sig:11,src:000008,time:354349,execs:51526,op:havoc,rep:5, sha1=8973da00874e70e4480b1f27243730f5661bd10b
 * asan_stack:
 *     #0 0x000100abca08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100abca08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000100abab50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000100b43ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000100b46288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000100b43b1c in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #6 0x000100b46288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000100b41078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #8 0x000100b46288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #9 0x000100b41648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #10 0x000100b47a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #11 0x000100b49544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 */
void Fuzzing_35(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 0"
    LINE "  prop offset = flecs.meta.f32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Leaf: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster:  pr{coant: $count, base> $base, offset: -2} }"
    LINE "  right { Cluster: {count: $count, base: $base: T: {20}"
    LINE "   }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} set}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "te}"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer08/crashes/id:000002,sig:11,src:000004,time:474765,execs:69183,op:havoc,rep:2, sha1=4e49bbb35547bd496c1ad2af8da104f751cdad01
 * asan_stack:
 *     #0 0x000104988a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104988a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000104986b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000104a0fad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000104a12288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104a0d078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x000104a12288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000104a0d648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #8 0x000104a13a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000104a15544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x00010492ca7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x00010492ca7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_36(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Size {"
    LINE "  w = f32"
    LINE " ght = f32"
    LINE "}"
    LINE ""
    LINE "template Marker {"
    LINE "  prop width = flecs.meta.f32: 4"
    LINE "  prop height = flecs.meta.f32: 2"
    LINE "  prop x = flecs.meta.f32: 0"
    LINE "  prop y = flecs.meta.f32: 0"
    LINE ""
    LINE "  marker {"
    LINE "Position: {$x, $y}"
    LINE "unt: $count, ba\322\322\322\322\322\322    Size: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "entity_a {"
    LINE "  Marker: {x: 10, y: 20}"
    LINE "}"
    LINE ""
    LINE "entiity_b {"
    LINE "  Marker: {ty_b {"
    LINE "  Marker: {x: -5, y: 3, width: 7}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer09/crashes/id:000000,sig:11,src:000003,time:7087,execs:1109,op:havoc,rep:3, sha1=f87c0b98252b4c24353814263369ea063ee08a39
 * asan_stack:
 *     #0 0x000102906234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x0001029045ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x0001029037c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x0001029335e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x0001029372f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x0001029372f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x00010292f370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x0001029339a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x0001029350d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x00010292f648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x000102936b24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x000102936b24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_37(void) {
    const char *expr =
    HEAD "using flecs.Teta"
    LINE ""
    LINE "Vertex {}"
    LINE "Fragment {}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "my_pipeline {"
    LINE "  (Shader, Vertex): {"
    LINE "filename: \"vglsl\","
    LINE "    code: `"
    LINE " in() \\{"
    LINE "        gl_Position = pos;"
    LINE "  '   }`"
    LINE "}"
    LINE ""
    LINE "  (Shader, Fragment): {"
    LINE "    filename: \"frag.glsl\","
    LINE "    code: `"
    LINE "      void main() \\z\372        frag_color = color;"
    LINE "      }`"
    LINE "}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer09/crashes/id:000001,sig:11,src:000003,time:33215,execs:4770,op:havoc,rep:1, sha1=b952e2dc74b7f20ac842210984d0d085354d2370
 * asan_stack:
 *     #0 0x0001044ae234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x0001044ac5ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x0001044ab7c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x0001044db5e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x0001044df2f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x0001044df2f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x0001044d7370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x0001044db9a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x0001044dd0d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x0001044d7648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x0001044deb24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x0001044deb24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_38(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "Vertex {}"
    LINE "Fragment {}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "my_pipeline {"
    LINE "  (Shader, Vertex): {"
    LINE "filename: \"v\202g\\sl\",    code: `"
    LINE " in() \\{"
    LINE "        gl_Position = pos;"
    LINE "      }`"
    LINE "}"
    LINE ""
    LINE "  (Shader, Fragment): {"
    LINE "    filename: \"frag.glsl\","
    LINE "    code: `"
    LINE "      void main() \\{"
    LINE "        frag_color = color;"
    LINE "      }`"
    LINE "}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer09/crashes/id:000002,sig:11,src:000003,time:84164,execs:12306,op:havoc,rep:2, sha1=921e761e3296d17dc70253929f9e76841a6ed910
 * asan_stack:
 *     #0 0x00010491e234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x00010491c5ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x00010491b7c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x00010494b5e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x00010494f2f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x00010494f2f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x000104947370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x00010494b9a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x00010494d0d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x000104947648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x00010494eb24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x00010494eb24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_39(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "Vertex {}"
    LINE "Fragment {}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "my_pipeline {"
    LINE "  (Shader, Vertex): {"
    LINE "filename: \"vglsl\","
    LINE "    code: `"
    LINE " in() \\_"
    LINE "        gl_Position = pos;"
    LINE "      }`"
    LINE "}"
    LINE ""
    LINE "  (Shader, Fragment): {"
    LINE "    f\\lename: \"frag.glsl\","
    LINE "    code: `"
    LINE "      void main() \\{"
    LINE "        frag_color = color;"
    LINE "      }`"
    LINE "}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer09/crashes/id:000003,sig:11,src:000002+000009,time:229485,execs:33232,op:splice,rep:6, sha1=884ca5e930c910f8b727d6cbe58ce1ea6c9b20e9
 * asan_stack:
 *     #0 0x000104c14a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104c14a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000104c12b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000104c9bad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000104c9e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104c9a530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #6 0x000104c9e288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000104c9a530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #8 0x000104c9fa38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000104ca1544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000104bb8a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000104bb8a7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_40(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Attack {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "struct Defense {"
    LINE "  value = f32using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE "  left = Controller"
    LINE "  r: i32,\030couight = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  a\206ray: {\200\377pe: i32, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 888888888888888888888888888888, 13, A1]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE "    }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3,"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer09/crashes/id:000004,sig:11,src:000864,time:527224,execs:77139,op:havoc,rep:2, sha1=f770c7786548cb5545d694cce4d46f7799f55360
 * asan_stack:
 *     #0 0x000102234a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000102234a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000102232b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001022bbad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x0001022be288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x0001022ba530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #6 0x0001022bfa38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #7 0x0001022c1544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x0001021d8a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x0001021d8a7c in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_41(void) {
    const char *expr =
    HEAD "using flecsLmfta"
    LINE ""
    LINE "struct Attack {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "struct Defense {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "prefab Ship {"
    LINE "  Attack: {10}"
    LINE "  Defense: {5}"
    LINE "}"
    LINE ""
    LINE "prefab Tank : Ship {"
    LINE "  Attack:er, Frag\200\200\200   {20}"
    LINE ": {5}"
    LINE "}"
    LINE ""
    LINE "p"
    LINE "  scout_b {}"
    LINE "}"
    LINE ""
    LINE "battle_tank : Tank {"
    LINE "  Attack: {30}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer11/crashes/id:000000,sig:11,src:000005,time:37508,execs:5380,op:havoc,rep:1, sha1=2c801e604a9b1c0e52c09d77fef539a5d1cf59d9
 * asan_stack:
 *     #0 0x000104c62234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x000104c605ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x000104c5f7c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x000104c8f5e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x000104c932f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x000104c932f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x000104c8b370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x000104c8f9a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x000104c910d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x000104c8b648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x000104c92b24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x000104c92b24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_42(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Mass {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "root {"
    LINE "  Nameplate: {\"root\"}with Mass {"
    LINE "child_a { Mass: {1.5} }"
    LINE "child_b { Mass: {2.5} }"
    LINE "  }"
    LINE ""
    LINE "  { Nameplate: {\"anonym\\us child\"}"
    LINE "Mass: {3.5}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer11/crashes/id:000001,sig:11,src:000719+000605,time:257192,execs:37030,op:splice,rep:4, sha1=a05bf58f7158129cfddf154c8290e84c188178ec
 * asan_stack:
 *     #0 0x000100a3ca08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100a3ca08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000100a3ab50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000100ac3ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000100ac6288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000100ac1078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x000100ac7a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #7 0x000100ac9544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x0001009e0a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x0001009e0a7c in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_43(void) {
    const char *expr =
    HEAD "using fl.meta"
    LINE ""
    LINE "struct Nameplecs.meta"
    LINE ""
    LINE "struct Nameplate {alue = string"
    LINE "}"
    LINE ""
    LINE "struct Mass {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "root {count: $count,aceshi\201 {"
    LINE " "
    LINE "  Nameplate: {\"root\"}with Mass {"
    LINE "child_a { Mass: {1.5} }"
    LINE "child_b { Mass: {2.5} }"
    LINE "  }{ Nameplate: {\"anonymous child\"}"
    LINE "using f\213eplame\200"
    LINE ""
    LINE ""
    LINE "struct Namecs.tf {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "using flecs.meta"
    LINE ""
    LINE "struct ExpressionSample {"
    LINE "  base = f64"
    LINE "  shifted = i32"
    LINE "  mixed = f64"
    LINE "  ratio = f64"
    LINE "  gate = bool"
    LINE "}"
    LINE ""
    LINE "const base: 1.25e2"
    LINE "const\001"
    LINE "hifted: (3 << 4) + (64 "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer11/crashes/id:000002,sig:11,src:000004,time:472309,execs:68451,op:havoc,rep:13, sha1=a21db0b39a02f3f72586f656d4632739e49af1b6
 * asan_stack:
 *     #0 0x000104280a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104280a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x00010427eb50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x0001043064d0 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #4 0x00010430a288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104305078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x00010430ba38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #7 0x00010430d544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000104224a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000104224a7c in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_44(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Size "
    LINE "  ker {"
    LINE "Position: {$x, $y}"
    LINE " plate Mar: $base, o\210p width = \200lecs.me|a.f32: 4"
    LINE "  prop height = flecs.meta.f32> h, $hei2"
    LINE " \200"
    LINE ""
    LINE ""
    LINE "p x = flecs.meta.f32: 0"
    LINE "  prop y\350\003"
    LINE ""
    LINE "lecs.me\213a.f32: 0"
    LINE ""
    LINE "  marDefense: {ght = f32"
    LINE "}"
    LINE ""
    LINE "tem   Size: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "entity_a {"
    LINE "  Marker: {x: 10, y: 20}"
    LINE "}"
    LINE ""
    LINE "entity_b {"
    LINE "  Marker: {\277\277x: -5, y: 3, wid\221h: 7}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer12/crashes/id:000000,sig:11,src:000008,time:463446,execs:67534,op:havoc,rep:2, sha1=264e56f6a91e4b0b2734e2a412c8586d2eaf565c
 * asan_stack:
 *     #0 0x0001044b0a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x0001044b0a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x0001044aeb50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000104537ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x00010453a288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104533b54 in flecs_script_stmt flecs/src/addons/script/parser.c:424:12
 *     #6 0x00010453a288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000104535078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #8 0x00010453a288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #9 0x000104535648 in flecs_script_stmt flecs/src/addons/script/parser.c:567:5
 *     #10 0x00010453ba38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #11 0x00010453d544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 */
void Fuzzing_45(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 0"
    LINE "  prop offset = flecs.meta.f32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Leaf: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { {count: $count, b\177"
    LINE ""
    LINE ""
    LINE " $base, offset: -2} }"
    LINE "  right { Cluster: {count: $count, base: $base, offset: 3} }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer12/crashes/id:000001,sig:11,src:000008,time:494869,execs:72178,op:havoc,rep:4, sha1=b3b5454d192cf46c1d695dd2ede6cd5e485f2dbc
 * asan_stack:
 *     #0 0x000102d74a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000102d74a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000102d72b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000102dfbad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000102dfe288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000102df7b54 in flecs_script_stmt flecs/src/addons/script/parser.c:424:12
 *     #6 0x000102dfe288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000102dfa530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #8 0x000102dffa38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000102e01544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000102d18a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000102d18a7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_46(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  {count: $count,\337base: $ba x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Weight {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "template Leaf {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop base = flecs.meta.f32: 1"
    LINE ""
    LINE "  \"leaf_{$idx}\" {"
    LINE "    Position: {$idx * 1.5, $base + $idx}"
    LINE "    Weight: {$base + ($idx * 0.5)}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Cluster {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 0"
    LINE "  prop offset = flecs.meta.f32: 0"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    Le`f: {idx: $i, base: $base + $offset}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Branch {"
    LINE "  prop \001ount = flecs.meta.i32: 3"
    LINE "  prop base = flecs.meta.f32: 2"
    LINE ""
    LINE "  left { Cluster: {count: $count, base: $base, offset: -2} }"
    LINE "  right { Cluster: {count: $count, base: $base, offset: 3} }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer12/crashes/id:000002,sig:11,src:000007,time:618162,execs:89842,op:havoc,rep:1, sha1=0247ed2d99b7e3980b632debb1fd127caee949bc
 * asan_stack:
 *     #0 0x000105463edc in __asan::RZSize2Log(unsigned int) (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x7edc)
 *     #1 0x00010545f080 in __asan::asan_calloc(unsigned long, unsigned long, __sanitizer::BufferedStackTrace*) (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x3080)
 *     #2 0x000105499684 in calloc (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x3d684)
 *     #3 0x0001048d5178 in flecs_map_rehash flecs/src/datastructures/map.c:153:20
 *     #4 0x0001048d6008 in ecs_map_ensure flecs/src/datastructures/map.c:277:9
 *     #5 0x0001048d407c in flecs_hashmap_ensure_ flecs/src/datastructures/hashmap.c:140:27
 *     #6 0x0001048d84cc in flecs_name_index_ensure flecs/src/datastructures/name_index.c:233:34
 *     #7 0x0001047d90f8 in flecs_meta_serialize_struct flecs/src/addons/meta/serializer.c:501:9
 *     #8 0x0001047d90f8 in flecs_meta_serialize_type flecs/src/addons/meta/serializer.c:533:32
 *     #9 0x0001047d8f04 in flecs_meta_serialize_struct flecs/src/addons/meta/serializer.c:483:17
 *     #10 0x0001047d8f04 in flecs_meta_serialize_type flecs/src/addons/meta/serializer.c:533:32
 *     #11 0x0001047d8f04 in flecs_meta_serialize_struct flecs/src/addons/meta/serializer.c:483:17
 */
void Fuzzing_47(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE ""
    LINE "  trig = f64"
    LINE "  power = f64"
    LINE "  root = f64"
    LINE "  dot_value = f64"
    LINE "  len_value = f64"
    LINE "  mixed = Vec3"
    LINE "  unit = Vec3"
    LINE "}"
    LINE ""
    LINE "const a = Vec3: {1.0e1, -2.5e1, 3.1}"
    LINE "const b = Vec3: {2.0, 4.0, -6.0}"
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE ""
    LINE "const trig: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const power: pow(1.0e2, 0.5) + exp2(3.0) - log10(1.0e3)"
    LINE "const root: sqrt(1.6e1) + abs(-2.5)"
    LINE "const mixed: lerp($a, $b, 0.25)"
    LINE "const unit: normalize($axis)"
    LINE "const dot_value: dot($a, $b)"
    LINE "const len_value: length($axis)"
    LINE ""
    LINE "mentity {"
    LINE "  MathSample: { trig: $trig"
    LINE "   $power"
    LINE "    root: $root"
    LINE "    dot_value: $dot_value len_value: $len_value"
    LINE "    mixed: $mixed"
    LINE "    unit: $unit"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer13/crashes/id:000000,sig:11,src:000004,time:36920,execs:5279,op:havoc,rep:4, sha1=6ad9bf7ca317be45e70a2a580a25df358260c992
 * asan_stack:
 *     #0 0x000100e38a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100e38a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000100e36b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000100ebfad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000100ec2288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000100ebd078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x000100ec3a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #7 0x000100ec5544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000100ddca7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000100ddca7c in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_48(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Size {"
    LINE "  w = f32"
    LINE " ght = f32"
    LINE "}"
    LINE ""
    LINE "template Marker {"
    LINE "  prop width = flecs.meta.f32: 4"
    LINE "  prop height = fleeeeeeeeeeeeeeeeeeeeeeeeeeeeeecs.meta.f32: 0"
    LINE "  prop y = flecs.meta.f32: 0"
    LINE ""
    LINE "  marker {"
    LINE "Position: {$x, $y}"
    LINE "    Size: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "entity_a {"
    LINE "  Marker:  Marke, y\305 20}"
    LINE "}"
    LINE ""
    LINE "entity_b {"
    LINE "  Marke\211: {x: -5, y: 3, width: 7}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer13/crashes/id:000001,sig:11,src:000005,time:262599,execs:37684,op:havoc,rep:1, sha1=2c977ccf0300865d11c0cc61f6a971162f261b72
 * asan_stack:
 *     #0 0x00010223a234 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:974:26
 *     #1 0x0001022385ec in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2191:13
 *     #2 0x0001022377c4 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2318:16
 *     #3 0x0001022675e8 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:524:13
 *     #4 0x00010226b2f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #5 0x00010226b2f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #6 0x000102263370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #7 0x0001022679a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #8 0x0001022690d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 *     #9 0x000102263648 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #10 0x00010226ab24 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:796:9
 *     #11 0x00010226ab24 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1665:16
 */
void Fuzzing_49(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Mass {"
    LINE "  value = f32"
    LINE "}"
    LINE ""
    LINE "root {"
    LINE "  Nameplate: {\"root\"}with Mass {"
    LINE "child_a { Mass: {1.5} }"
    LINE "child_b { Mass: {2.5} }"
    LINE "  }"
    LINE ""
    LINE "  { Nameplate: {\"a\\onymous child\"}"
    LINE "Mass: {3.5}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer13/crashes/id:000002,sig:11,src:000009,time:371283,execs:53539,op:havoc,rep:2, sha1=a52337520e477fba7ce84a26ef11d77e18e36aff
 * asan_stack:
 *     #0 0x0001027e8a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x0001027e8a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x0001027e6b50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x00010286fad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000102872288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x00010286d078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x000102872288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x00010286d078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #8 0x000102873a38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000102875544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x00010278ca7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x00010278ca7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_50(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE "  left = Controller"
    LINE "  right = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array9 {type: i32,\007count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE "    }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer13/crashes/id:000003,sig:11,src:000009,time:379807,execs:54788,op:havoc,rep:1, sha1=12aad1271444c55165b1088b3db0a7b2eff46c72
 * asan_stack:
 *     #0 0x000104dcf128 in __asan_memcpy (/Library/Developer/CommandLineTools/usr/lib/clang/17/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64e+0x3b128)
 *     #1 0x000104278d3c in ecs_value_copy_w_type_info flecs/src/value.c:142:9
 *     #2 0x000104278d3c in ecs_value_copy flecs/src/value.c:159:12
 *     #3 0x00010411d128 in flecs_expr_initializer_post_fold flecs/src/addons/script/expr/visit_fold.c:314:13
 *     #4 0x00010411b60c in flecs_expr_initializer_visit_fold flecs/src/addons/script/expr/visit_fold.c:345:13
 *     #5 0x00010411b60c in flecs_expr_visit_fold flecs/src/addons/script/expr/visit_fold.c:563:13
 *     #6 0x00010414f5fc in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:527:13
 *     #7 0x0001041532f8 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:977:13
 *     #8 0x0001041532f8 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1624:16
 *     #9 0x00010414b370 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #10 0x00010414f9a8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:574:18
 *     #11 0x0001041510d0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1618:16
 */
void Fuzzing_51(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE "  left = Controller"
    LINE "  right = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: i32, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE "    }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer13/crashes/id:000004,sig:11,src:000009,time:395313,execs:57049,op:havoc,rep:6, sha1=c7df4fdd2bfc6d4e6e56d97607bcd39c462cb225
 * asan_stack:
 *     #0 0x000100cf0a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000100cf0a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x000100ceeb50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000100d77ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x000100d7a288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000100d73b54 in flecs_script_stmt flecs/src/addons/script/parser.c:424:12
 *     #6 0x000100d7a288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000100d76530 in flecs_script_stmt flecs/src/addons/script/parser.c:897:5
 *     #8 0x000100d7ba38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #9 0x000100d7d544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000100c94a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000100c94a7c in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_52(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "   {type: i32, count\222 7}"
    LINE "}"
    LINE ""
    LINE "Fi= f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE "  "
    LINE "\004ft = Controller"
    LINE "  right = Iontroller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: i32, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "                vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE " 6  }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer16/crashes/id:000000,sig:11,src:000001,time:164695,execs:23678,op:havoc,rep:5, sha1=362e168f6aded33d27aca34b2d921d0bdfb299ed
 * asan_stack:
 *     #0 0x000104200a08 in flecs_scan_whitespace flecs/src/addons/parser/tokenizer.c
 *     #1 0x000104200a08 in flecs_scan_whitespace_and_comment flecs/src/addons/parser/tokenizer.c:200:11
 *     #2 0x0001041feb50 in flecs_token flecs/src/addons/parser/tokenizer.c:606:11
 *     #3 0x000104287ad4 in flecs_script_stmt flecs/src/addons/script/parser.c:798:5
 *     #4 0x00010428a288 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104285078 in flecs_script_stmt flecs/src/addons/script/parser.c:443:5
 *     #6 0x00010428ba38 in ecs_script_parse flecs/src/addons/script/parser.c:1042:15
 *     #7 0x00010428d544 in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x0001041a4a7c in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x0001041a4a7c in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_53(void) {
    const char *expr =
    HEAD "using fqecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "stQuct Velocity {"
    LINE "osition: {30, 40}"
    LINE "  }"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const speed = 3.5"
    LINE ""
    LINE "ship_1 {"
    LINE "  Position: {x: 10, y: 20}"
    LINE "  Velocity: {$s{Blue}0}"
    LINE "  Team: {Red}"
    LINE "}"
    LINE ""
    LINE "ship_2 {"
    LINE "  Position: {30, 40}"
    LINE "  Velocity: {-$speed, 1}"
    LINE "  Team: peed, "
    LINE "\377\200"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer02/crashes/id:000000,sig:11,src:000013,time:9379,execs:2468,op:havoc,rep:1, sha1=a480a69251313d8d9dca0f7bc73a45c3c4f955ae
 * asan_stack:
 *     #0 0x000104d110a8 in __asan_region_is_poisoned (/opt/homebrew/Cellar/llvm/20.1.8/lib/clang/20/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64+0x550a8)
 *     #1 0x0001042c81c8 in ecs_id_str /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/id.c:284:18
 *     #2 0x0001042bd18c in ecs_get_parent /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity.c:2874:9
 *     #3 0x0001042b98b0 in ecs_get_target /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity.c:2774:16
 *     #4 0x0001042c3ba4 in flecs_path_append /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity_name.c:34:19
 *     #5 0x0001042c43d4 in ecs_get_path_w_sep /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity_name.c:596:5
 *     #6 0x0001042bd1b0 in ecs_get_parent /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity.c:2874:9
 *     #7 0x0001042b98b0 in ecs_get_target /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity.c:2774:16
 *     #8 0x0001042c3ba4 in flecs_path_append /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity_name.c:34:19
 *     #9 0x0001042c43d4 in ecs_get_path_w_sep /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity_name.c:596:5
 *     #10 0x0001042bd1b0 in ecs_get_parent /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity.c:2874:9
 *     #11 0x0001042b98b0 in ecs_get_target /Users/sandermertens/GitHub/SanderMertens/flecs_fuzzer/flecs/src/entity.c:2774:16
 */
void Fuzzing_54(void) {
    const char *expr =
    HEAD "@tree Parent"
    LINE ""
    LINE "root {"
    LINE "  parent_a {"
    LINE "    @tree ChildOf"
    LINE "childof_branch {   level_2 { @tree Parent"
    LINE "    parent_branch {"
    LINE "  final_a {}"
    LINE "  final_b {}"
    LINE "}"
    LINE "  }"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  @tree ChildOf"
    LINE "  childof_sibling {"
    LINE "    child_a {Parent"
    LINE "      parent_nested {"
    LINE "    leaf {}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000000,sig:11,src:001310,time:1043436,execs:262772,op:havoc,rep:1, sha1=2759f4a6b67a2dba5248e4e087da9e38b02b0443, grouped_crashes=6
 * asan_stack:
 *     #0 0x0001001dea8c in ecs_script_params_free flecs/src/addons/script/function.c:17:9
 *     #1 0x0001001dea8c in EcsScriptFunction_dtor flecs/src/addons/script/function.c:60:1
 *     #2 0x00010030d3e8 in flecs_table_dtor_all flecs/src/storage/table.c:1138:13
 *     #3 0x00010030d3e8 in flecs_table_fini_data flecs/src/storage/table.c:1187:5
 *     #4 0x00010030e4d8 in flecs_table_fini flecs/src/storage/table.c:1305:5
 *     #5 0x00010032f9dc in flecs_clean_tables flecs/src/world.c:567:9
 *     #6 0x00010032f9dc in flecs_fini_store flecs/src/world.c:680:5
 *     #7 0x00010032f9dc in ecs_fini flecs/src/world.c:1303:5
 *     #8 0x000100108a50 in fuzz_one fuzz/flecs_script_harness.c:108:5
 *     #9 0x000100108a50 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_55(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EnginePren: length($axis)"
    LINE ""
    LINE "Earth { Plciency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "      axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: { attack: $attack + ($i * 0.75)"
    LINE " $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parent"
    LINE "parent_root {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_mix}"
    LINE ""
    LINE " lpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180, efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenance bay\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_root {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "  Position: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8, orbit_phase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1, py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE ""
    LINE "    @tree Parent"
    LINE "    parent_section {"
    LINE "      nested_parent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "      nested_parent_b {"
    LINE "        Position: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate: {\"mixed/default hierarchy root\"}"
    LINE "  Orbit: {radius: $orbit_radius + $axis_len, phase: $trig_mix + $damping}"
    LINE "  Combat: {"
    LINE "    attack: ($cruise_speed / 3.0) + 1"
    LINE "    defense: (abs(-4.0) + 2.0)"
    LINE "  }"
    LINE "oller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) + ($axis_len / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: `"
    LINE " () \\{"
    LINE "  gl_Position = vec4(0.0);"
    LINE "        }`"
    LINE "      }"
    LINE "  }"
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "          link_a {}"
    LINE "          link_b {}"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  gate_marker {"
    LINE "    Nameplate: {\"fast gate marker\"}"
    LINE "    Flags: {fast: $gat+_fast}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000001,sig:11,src:002097,time:1517583,execs:379774,op:havoc,rep:3, sha1=91b3f458415d0aeba4587763d3ec568dbf5fb4c6, grouped_crashes=2
 * asan_stack:
 *     #0 0x000102bd274c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102c60914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x000102c5b22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #3 0x000102c60a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x000102c5b22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #5 0x000102c624e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #6 0x000102c6432c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #7 0x000102b78a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #8 0x000102b78a48 in main fuzz/flecs_script_harness.c:132:9
 *     #9 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_56(void) {
    const char *expr =
    HEAD "@tree Parent"
    LINE "t_branch {"
    LINE "  a {"
    LINE "    /*o*o`"
    LINE "`"
    LINE "   \374lent t_branch {"
    LINE "  l_pe: i32, a {}"
    LINE " al_b {}"
    LINE "}"
    LINE "  }"
    LINE "    }"
    LINE "  }g {}ruct er {"
    LINE " _sng {ld_a {}"
    LINE "child_b {"
    LINE "  @tree Pa\377\377\377\377  y = f32"
    LINE "}"
    LINE ""
    LINE "enum T{rent"
    LINE "   t_ndsted {"
    LINE "    l{}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000002,sig:11,src:002842,time:2105300,execs:532190,op:havoc,rep:4, sha1=59ea8d34dcc4dc3dbd58d53c5573503828ca1229, grouped_crashes=1
 * asan_stack:
 *     #0 0x000104b1a74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104ba2bf4 in flecs_script_stmt flecs/src/addons/script/parser.c:887:5
 *     #2 0x000104ba8a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x000104ba561c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #4 0x000104baa4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #5 0x000104bac32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x000104ac0a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x000104ac0a48 in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_57(void) {
    const char *expr =
    HEAD "using flecs."
    LINE ""
    LINE "sd Expe {"
    LINE " \tbng flecs."
    LINE ""
    LINE "sd Exl /*\200\3771.4*/"
    LINE "  /*\253\253\253\253\253\253\253\253\253\253\253\253pe {"
    LINE "\003\350base ese = f64"
    LINE "  sed =--------------------0-000000000E-0.0) &t ra"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000003,sig:11,src:002842,time:2109616,execs:533271,op:havoc,rep:1, sha1=3138d9a1d5a63c0ed4d76cb6136c5f3771555470, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010283674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001028bc3ac in flecs_script_stmt flecs/src/addons/script/parser.c:447:5
 *     #2 0x0001028c4a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x0001028c161c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #4 0x0001028c64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #5 0x0001028c832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x0001027dca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x0001027dca48 in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_58(void) {
    const char *expr =
    HEAD "using flecs."
    LINE ""
    LINE "sd Expe {"
    LINE " \tbase = f64"
    LINE "  sl /*\200\3771.4*/"
    LINE "  /*//******\026"
    LINE "***ed =--------------------0-000000000E-0.0) &t ra"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000004,sig:11,src:002875,time:2151145,execs:544193,op:havoc,rep:1, sha1=4f9181f80200db70b28b29c0da41e814c5af74b7, grouped_crashes=27
 * asan_stack:
 *     #0 0x000102a6e74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102af43ac in flecs_script_stmt flecs/src/addons/script/parser.c:447:5
 *     #2 0x000102afe4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x000102b0032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x000102a14a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x000102a14a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_59(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Size {"
    LINE "  w = f32"
    LINE " ght = f32"
    LINE "}"
    LINE ""
    LINE "template Marker {"
    LINE "  prop width = flecs.meta.f32: 4"
    LINE "  prop height = flecs.meta.f32: 2"
    LINE "  prop x = flecs.meta.f32: 0"
    LINE "  prop y = flecs.meta.{"
    LINE "Position: {$x,!$y}"
    LINE "    Size: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE "ity_a {"
    LINE "  Marker: {x: 10, y: 20}"
    LINE "}"
    LINE ""
    LINE "ty2"
    LINE "/*\200  Marker: {x: -5, y: 3, width: 7}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000007,sig:11,src:002957,time:2176342,execs:550780,op:havoc,rep:3, sha1=cad226870d08b713f63dcb4eec5f9d9ea7e51fd9, grouped_crashes=5
 * asan_stack:
 *     #0 0x0001004ba74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000100542b34 in flecs_script_stmt flecs/src/addons/script/parser.c:784:9
 *     #2 0x00010054a4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x00010054c32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x000100460a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x000100460a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_60(void) {
    const char *expr =
    HEAD "ColPai$: //ller \236{"
    LINE " /*\200\377\177\031"
    LINE "_1.4*// r"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000008,sig:11,src:002957+001009,time:2176657,execs:550867,op:splice,rep:3, sha1=a9e29b3e74467f5c452a6c194c7cbd4f32bbcbd0, grouped_crashes=3
 * asan_stack:
 *     #0 0x00010217274c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001021faca4 in flecs_script_stmt flecs/src/addons/script/parser.c:887:5
 *     #2 0x0001022024e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x00010220432c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x000102118a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x000102118a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_61(void) {
    const char *expr =
    HEAD "Contr lPai$: //ller {{"
    LINE " /*\200\377\177\377\377@trwe Parent"
    LINE ""
    LINE "sc{"
    LINE "  {"
    LINE "    branch_a {   "
    LINE "      nested_anonymous _ {"
    LINE "    leaf_a {}   }"
    LINE "      {"
    LINE "    leaf_b {}"
    LINE "      }"
    LINE "    \204"
    LINE "  }"
    LINE ""
    LINE "  _ {"
    LINE "    n}   }"
    LINE "      {"
    LINE "    lamed_under_placeholder {}"
    LINE "    {"
    LINE "      n"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000010,sig:11,src:002974+000363,time:2181175,execs:552039,op:splice,rep:14, sha1=fb6b7b5364ce6cd86f44ac085ab41c8bde413b87, grouped_crashes=2
 * asan_stack:
 *     #0 0x0001043de74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104469ea8 in flecs_script_stmt flecs/src/addons/script/parser.c:806:5
 *     #2 0x00010446e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x00010447032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x000104384a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x000104384a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_62(void) {
    const char *expr =
    HEAD "Con1111111111111111111ate:trol /*\200\3771.4*/"
    LINE "  /*//****using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE " 2d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.\377}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: Rerp($probe, $unit_axis, 0.35)"
    LINE "codst axis_len: length($axis)\356"
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "tWmplate EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "      axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "emplate PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "  &     x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: {"
    LINE "        attack: $attack + ($i * 0.75)"
    LINE "        defennnnnnnnnense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32:70"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree lecs.meta.f32: 3"
    LINE ""
    LINE "  for Parent"
    LINE "parent_root {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_mix}"
    LINE ""
    LINE "  fleet_alpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180, efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenance bay\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_rooW {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "  Position: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8, orbit_phase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1, py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE ""
    LINE "    @tree Parent"
    LINE "    parent_section {"
    LINE "      nested_parent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "      nested_parent_b {"
    LINE "        PoVition: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate: {\"mixed/default hierarc$orbit_radius + $axis_len, phase: $trig_mix + $damping}"
    LINE "  Combat: {"
    LINE "    attack: ($cruise_speed / 3.0)axis_len: length($axis)"
    LINE ""
    LINE "E0) + 2.0)"
    LINE "  }\177"
    LINE "  controller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) + ($axis_len / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: `"
    LINE "        void main() \\{"
    LINE "          gl_Position = vec4(0.0);"
    LINE "        }`"
    LINE "      }\350\003"
    LINE ""
    LINE ""
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "          link_a {}"
    LINE "          link_b {}"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  gate_marker {"
    LINE "    Nameplate: {\"fast gate marker\"}"
    LINE "    Flag"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000011,sig:11,src:002983,time:2183511,execs:552664,op:havoc,rep:2, sha1=42b42838f058e41e7a6fe89fce27d5ffd3680cd0, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001024ce74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102555e40 in flecs_script_stmt flecs/src/addons/script/parser.c:643:5
 *     #2 0x00010255e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x00010256032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x000102474a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x000102474a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_63(void) {
    const char *expr =
    HEAD "(ontrol /*\200\0342.4*/"
    LINE "  /*n: {x: 10, y: 20}"
    LINE "  Velo*\200\377\177\377\377(O.4*222222: J/*"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000012,sig:11,src:000940+002438,time:2275982,execs:576154,op:splice,rep:3, sha1=c2178505eb7d3f4ccc6d4a2990e9e6bf7b6cd3a7, grouped_crashes=1
 * asan_stack:
 *     #0 0x000102a8674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102b1694c in flecs_script_parse_var flecs/src/addons/script/parser.c:307:5
 *     #2 0x000102b0c6d8 in flecs_script_stmt flecs/src/addons/script/parser.c
 *     #3 0x000102b14a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x000102b0e144 in flecs_script_stmt flecs/src/addons/script/parser.c:575:5
 *     #5 0x000102b14a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #6 0x000102b1161c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #7 0x000102b164e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #8 0x000102b1832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #9 0x000102a2ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #10 0x000102a2ca48 in main fuzz/flecs_script_harness.c:132:9
 *     #11 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_64(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PIusing flecs.meta"
    LINE "using nlecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Co {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency =!f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < O0.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop e"
    LINE "/*,us[/fficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Epgine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      ef      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: { attack: $attack + ($i * 0.75)"
    LINE " $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Zed}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parent"
    LINE "parent_root {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radbit_radiu\216, phas_mix}"
    LINE ""
    LINE " lpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180, efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenance bay\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_root {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "  Position: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8phase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1, py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE "\t    @tree Parent"
    LINE "    parent_section {"
    LINE "      nested_parent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "      nested_parent_b {"
    LINE "        Position: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate: {\"mixed/default hierarchy root\"}"
    LINE "  Orbit: {radiut_radius + $axis_len, phig_mix +g}"
    LINE "  Combat: {"
    LINE " k: ($cruise_speed / 3.0) + 1"
    LINE "    defense: (abs(-4.0) + 2.0)"
    LINE "  }"
    LINE ""
    LINE "  controller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) + ($axis_len / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: `"
    LINE " () \\{"
    LINE "  gl_Position = vec4(0.0);"
    LINE "        }`"
    LINE "      }"
    LINE "  }"
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "          link_a {}"
    LINE "    "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000013,sig:11,src:003017+001623,time:2333589,execs:590298,op:splice,rep:2, sha1=a25d29eb8020d3fd6b5da8ae4135e5ab32d6b0d3, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010290674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x00010298c3ac in flecs_script_stmt flecs/src/addons/script/parser.c:447:5
 *     #2 0x000102994a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x00010299161c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #4 0x000102994a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000102995608 in flecs_script_if_stmt flecs/src/addons/script/parser.c:245:5
 *     #6 0x00010298c3f0 in flecs_script_stmt flecs/src/addons/script/parser.c:613:12
 *     #7 0x0001029964e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #8 0x00010299832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #9 0x0001028aca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #10 0x0001028aca48 in main fuzz/flecs_script_harness.c:132:9
 *     #11 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_65(void) {
    const char *expr =
    HEAD "if  af_b {using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE " ng"
    LINE "/*//}  left = Controller"
    LINE "  right = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: i32, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      se\377\377\377\377nt: 2.5e2"
    LINE "    }"
    LINE "    right: {{{{{{{{{{{{{{{{{{{{{{{{{{{0e2, i: 1.0 / 1.0e3, d: 7.5 / 1>0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "  "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000015,sig:11,src:000009,time:2363010,execs:597432,op:havoc,rep:3, sha1=d6c2526783c987d2fbc8cc26f808033722066e75, grouped_crashes=2
 * asan_stack:
 *     #0 0x0001022e8130 in flecs_script_stmt flecs/src/addons/script/parser.c:408
 *     #1 0x0001022f0a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x0001022e9d18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #3 0x0001022f0a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x0001022e9d18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #5 0x0001022f0a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #6 0x0001022e9d18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #7 0x0001022f0a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #8 0x0001022e9d18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #9 0x0001022f0a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #10 0x0001022e9d18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #11 0x0001022f0a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 */
void Fuzzing_66(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "      axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) -!1"
    LINE "      }"
    LINE "      Combat: { $attack + ($i * 0.75)"
    LINE " $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = fldcs.meta.i32: {{{{{{{{{{{{{{{{{{{{{{{{{ { Cluster: {count: $count, base: $base, offset:\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314\314 3} }"
    LINE "}"
    LINE ""
    LINE "tree_a { Branch: {count: 4, base: 1.5} }"
    LINE "tree_b { Branch: {count: 2, base: 4.0} }"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000016,sig:11,src:003027+003140,time:2413463,execs:610189,op:splice,rep:2, sha1=ec61a458cb99a92ccb0b2fc5a449a6c99e9a71b0, grouped_crashes=4
 * asan_stack:
 *     #0 0x000104f6274c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104fea1cc in flecs_script_stmt flecs/src/addons/script/parser.c:553:5
 *     #2 0x000104ff24e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x000104ff432c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x000104f08a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x000104f08a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_67(void) {
    const char *expr =
    HEAD "using/*\200us[/*/"
    LINE "/*,\213s[A*"
    LINE "/*,us[/*[/["
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000017,sig:11,src:003176,time:2417803,execs:611326,op:havoc,rep:2, sha1=432625e4e33e2aaa2fcf2ffbe911f7f6813fe64e, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001037fba18 in __asan_memcpy (/opt/homebrew/Cellar/llvm/20.1.8/lib/clang/20/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64+0x4fa18)
 *     #1 0x000102da90f4 in flecs_meta_parse_member flecs/src/addons/meta/cursor.c:436:5
 *     #2 0x000102da90f4 in flecs_meta_dotmember flecs/src/addons/meta/cursor.c:459:19
 *     #3 0x000102e48138 in flecs_expr_initializer_visit_type flecs/src/addons/script/expr/visit_type.c:973:17
 *     #4 0x000102e46670 in flecs_expr_visit_type_priv flecs/src/addons/script/expr/visit_type.c:2202:13
 *     #5 0x000102e45874 in flecs_expr_visit_type flecs/src/addons/script/expr/visit_type.c:2329:16
 *     #6 0x000102e76324 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:546:13
 *     #7 0x000102e7a034 in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:1006:13
 *     #8 0x000102e7a034 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1655:16
 *     #9 0x000102e71f24 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #10 0x000102e766e8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #11 0x000102e77e10 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 */
void Fuzzing_68(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Size {"
    LINE "  w = f32"
    LINE " ght = f32"
    LINE "}"
    LINE ""
    LINE "template Marker {"
    LINE "  prop width = flecs.meta.f32: 4"
    LINE "  prop height = flecs.meta.f32: 2"
    LINE "  prop x = flecs.meta.f32: 0"
    LINE "  prop y = flecs.meta.f32: 0"
    LINE ""
    LINE "  mark$$$er {"
    LINE "Position: {$x, $y}"
    LINE "    Size: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "entity_a {"
    LINE "  Marker: {x: 10, y: 20}"
    LINE "}"
    LINE ""
    LINE "entity_b {"
    LINE "  Marker: {xTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTDTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT: -5, y: 3, width: 7}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000018,sig:11,src:003240,time:2431130,execs:614809,op:havoc,rep:4, sha1=87022f6194b5b842334ce4bff6709d4f2481b01e, grouped_crashes=1
 * asan_stack:
 *     #0 0x000104629df4 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:448
 *     #1 0x000104629c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #2 0x00010462a6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #3 0x000104629c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #4 0x00010462a6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #5 0x000104629c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #6 0x00010462a6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #7 0x000104629c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #8 0x00010462a6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #9 0x000104629c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #10 0x00010462a6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #11 0x000104629c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 */
void Fuzzing_69(void) {
    const char *expr =
    HEAD "usin4"
    LINE "  sed =-----------------------------------------------------------------------L-------------------------------------------------------------------------------------------------------------------------------------------------A---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------se_speed <=30.0)"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000019,sig:11,src:003245,time:2478849,execs:627344,op:havoc,rep:2, sha1=313e36e76b9ef06f1b0d3fd231a87dcfed513cce, grouped_crashes=6
 * asan_stack:
 *     #0 0x000104cba74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104d402d4 in flecs_script_stmt flecs/src/addons/script/parser.c:413:5
 *     #2 0x000104d4a4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x000104d4c32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x000104c60a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x000104c60a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_70(void) {
    const char *expr =
    HEAD "/*\200\3771{4*/"
    LINE "  /*n-Con = l "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000020,sig:11,src:003255+002949,time:2489093,execs:630124,op:splice,rep:12, sha1=4c82bcae57843dfcd1d896db865c675c350f9997, grouped_crashes=4
 * asan_stack:
 *     #0 0x00010436674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001043eef44 in flecs_script_stmt flecs/src/addons/script/parser.c:833:5
 *     #2 0x0001043f64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x0001043f832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x00010430ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x00010430ca48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_71(void) {
    const char *expr =
    HEAD "using flecs;"
    LINE " sint$cs;"
    LINE " sint$= f6g"
    LINE "/* strRame!=//}                 &(1./(1.\016\016\016\016\016ing flecs;"
    LINE " sint$cs;"
    LINE " sint$= f6g"
    LINE "/* strRame =//}    \350\003"
    LINE ""
    LINE "   \020"
    LINE "    &(1./(1.\016\016\016\016\016\016\016\016\016\016\016\016"
    LINE "\367\016\016"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000021,sig:11,src:003255+002949,time:2489115,execs:630130,op:splice,rep:1, sha1=c4b433f4fdeca18dac65865b6035c34c0e362bab, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001001ea74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000100272f44 in flecs_script_stmt flecs/src/addons/script/parser.c:833:5
 *     #2 0x000100278a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x00010027322c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x000100278a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000100273940 in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #6 0x00010027a4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #7 0x00010027c32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000100190a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000100190a48 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_72(void) {
    const char *expr =
    HEAD "using flecs;meta"
    LINE "  {"
    LINE "  afYb:#10,  {}"
    LINE "ader {"
    LINE "  eilename = string"
    LINE "/*//}                 &(1./(1.\016\016\016\016\016\016\016\016\016\016\016\016\016\016\016\016\016"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000024,sig:11,src:003268,time:2524325,execs:639337,op:havoc,rep:2, sha1=80264d95615eb9abd3611c2695566bd1f0864d93, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001002263b4 in flecs_script_parse_collection_initializer flecs/src/addons/script/expr/parser.c:236
 *     #1 0x000100222088 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #2 0x000100226718 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #3 0x000100226718 in flecs_script_parse_collection_initializer flecs/src/addons/script/expr/parser.c:260:15
 *     #4 0x000100222088 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #5 0x000100226718 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #6 0x000100226718 in flecs_script_parse_collection_initializer flecs/src/addons/script/expr/parser.c:260:15
 *     #7 0x000100222088 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #8 0x000100226718 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #9 0x000100226718 in flecs_script_parse_collection_initializer flecs/src/addons/script/expr/parser.c:260:15
 *     #10 0x000100222088 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #11 0x000100226718 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 */
void Fuzzing_73(void) {
    const char *expr =
    HEAD "if  ! ! \ttm[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]Uq !  t \ttm[[[]U: {radius: $fle\ttm[[[]Uq !  t \ttm[[][Uq[[[[[tm[[[]Uq !  t \ttm\\[[]U: {radiu[[[!["
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000025,sig:11,src:003294,time:2865815,execs:650661,op:havoc,rep:3, sha1=085956f524cbf8447b925819b7709ab6bbae6072, grouped_crashes=4
 * asan_stack:
 *     #0 0x00010263674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001026c4914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x0001026c161c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #3 0x0001026c64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #4 0x0001026c832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #5 0x0001025dca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #6 0x0001025dca48 in main fuzz/flecs_script_harness.c:132:9
 *     #7 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_74(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  Salue = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = fQ2"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  {}"
    LINE "Rel "
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Cosentroller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 1"
    LINE "const shift_factor: (3 << 3) using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct VelociQy {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radjus = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags"
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  // &(}"
    LINE "//"
    LINE " /*\200\377\202i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct Engine"
    LINE " ofile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "e"
    LINE "um Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.muta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine}"
    LINE ""
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficieiciency"
    LINE "      axis: $mixe|_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    V) +city: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = fleGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGcs.meta.i32: 3"
    LINE "  prop sprection: {x: 1, y: 0}"
    LINE "      setp attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: {"
    LINE "        attack: $attack + ($i * 0.75)"
    LINE "        defense: $defense + ($i  {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  proq px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parent"
    LINE "parent_root {"
    LINE "  \020ameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_{"
    LINE "      nested_parent_a {"
    LINE "     ion, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180, efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, effi"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer02/crashes/id:000001,sig:11,src:002848+002413,time:2192068,execs:543603,op:splice,rep:2, sha1=22ebd78e6b98b58f34dae05733079ba51bb184ae, grouped_crashes=10
 * asan_stack:
 *     #0 0x00010451a74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001045a8914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x0001045a322c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #3 0x0001045aa4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #4 0x0001045ac32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #5 0x0001044c0a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #6 0x0001044c0a48 in main fuzz/flecs_script_harness.c:132:9
 *     #7 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_75(void) {
    const char *expr =
    HEAD "usiue"
    LINE ""
    LINE "a {"
    LINE " /*// \377\177"
    LINE "\335/ +"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer03/crashes/id:000002,sig:11,src:001805,time:1146265,execs:288822,op:havoc,rep:1, sha1=abfe4f6be67b5bf4ac3a84f3671f8ca800e7ea56, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001043f674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104484914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x00010447f22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #3 0x000104484a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x00010447dd18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #5 0x0001044864e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #6 0x00010448832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #7 0x00010439ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #8 0x00010439ca48 in main fuzz/flecs_script_harness.c:132:9
 *     #9 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_76(void) {
    const char *expr =
    HEAD "@tree Parent"
    LINE ""
    LINE "sc{"
    LINE ""
    LINE " a {}   }   { leaf_b {"
    LINE "name}   }   { leaf_b {"
    LINE "/*////////"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer03/crashes/id:000003,sig:11,src:001805,time:1159034,execs:292062,op:havoc,rep:2, sha1=113d19fe1d99f2099d22966ce1ca7f8a11f34e4e, grouped_crashes=3
 * asan_stack:
 *     #0 0x00010484674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001048cc660 in flecs_script_stmt flecs/src/addons/script/parser.c:534:5
 *     #2 0x0001048d64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x0001048d832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x0001047eca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x0001047eca48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_77(void) {
    const char *expr =
    HEAD "@tree Parent"
    LINE "/*//// a {)   }   { leaf_b {"
    LINE "name"
    LINE "sc{"
    LINE ""
    LINE "////"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer03/crashes/id:000005,sig:11,src:001854,time:1331190,execs:336127,op:havoc,rep:2, sha1=8c24dc2f614e0e4f304e42c73bfee61d64578501, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001043ae74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104436f44 in flecs_script_stmt flecs/src/addons/script/parser.c:833:5
 *     #2 0x00010443ca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x00010443722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x00010443e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #5 0x00010444032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x000104354a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x000104354a48 in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_78(void) {
    const char *expr =
    HEAD "plate PaGro///////u@tre\232 eme PaGro//M//"
    LINE "tPosition {"
    LINE "  x = f32"
    LINE "  /*////////"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer03/crashes/id:000006,sig:11,src:001854+000688,time:1334923,execs:337059,op:splice,rep:13, sha1=b83c9b5783df0c92d9feb9feabffe3b2fe2a8519, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010474a74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001047d20e4 in flecs_script_stmt flecs/src/addons/script/parser.c:575:5
 *     #2 0x0001047da4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #3 0x0001047dc32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #4 0x0001046f0a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #5 0x0001046f0a48 in main fuzz/flecs_script_harness.c:132:9
 *     #6 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_79(void) {
    const char *expr =
    HEAD "@tre\232 eme PaGro//M//"
    LINE "template PaGro///////u{"
    LINE "/*///using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "suruht Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Rei,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust [ ($\223ruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "   }"
    LINE "  }"
    LINE ""
    LINE "  gate_marker \223"
    LINE "       axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ( trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in "
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: { attack: $attack + ($i * 0.75)"
    LINE " $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" d"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensrr node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parent"
    LINE "parent_root {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_mix}"
    LINE ""
    LINE " lpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180,ine {"
    LINE "iency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenance bay\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_root {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "  Position: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8, orbit_ph    nested_parent_a {"
    LINE "      ase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1, py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE ""
    LINE "    @tree Parent"
    LINE "    parent_section {"
    LINE "      nested_parent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "     pe {"
    LINE "  base = f64"
    LINE "  s\335d =----sition: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate: {\"mixed/default hierarchy root\"}"
    LINE "  Orbit: {radius: $orbit_radius + $axis_len, phase: $trig_mix + $damping}"
    LINE "  Combis)"
    LINE "conat: pe    attack: ($cruise_speed / 3.0) + 1"
    LINE "    defense: (abs(-4.0) + 2.0)"
    LINE "  }"
    LINE "oller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) + ($axis_lel / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: `"
    LINE " () \\{"
    LINE "  gl_Position = vec4(0.0);"
    LINE "        }`"
    LINE "      }"
    LINE "  }"
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "          link_a {}"
    LINE "          link_b {}"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  gate_marker \223"
    LINE "    Nameplate: {\"fast "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer03/crashes/id:000007,sig:11,src:001731+000488,time:1683433,execs:425667,op:splice,rep:2, sha1=c3b2488866fd19fc2c59eaa33b865630c4dbb1a0, grouped_crashes=2
 * asan_stack:
 *     #0 0x00010415274c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001041e0914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x0001041db22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #3 0x0001041e0a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x0001041dd61c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #5 0x0001041e24e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #6 0x0001041e432c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #7 0x0001040f8a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #8 0x0001040f8a48 in main fuzz/flecs_script_harness.c:132:9
 *     #9 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_80(void) {
    const char *expr =
    HEAD "using flecs."
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Stat{}"
    LINE "Faction {}"
    LINE "Engine {}gate_fas"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "      axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: { attack: $attack + ($i * 0.75)"
    LINE " $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parent"
    LINE "parent_root {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_mix}"
    LINE ""
    LINE " lpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180, efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenanc\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_root {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "/**osition: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8, orbit_phase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1,8py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE ""
    LINE "    @tree Parent"
    LINE "    parent_section {"
    LINE "      nested_parent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "      nested_parent_b {"
    LINE "        Position: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate: {\"mixed/default hierarchy root\"}"
    LINE "  Orbit: {radius: $orbit_radius + $axis_len, phase: $trig_mix + $damping}"
    LINE "  Combat: {"
    LINE "    attack: ($cruise_speed / 3.0) + 1"
    LINE "    defense: (abs(-4.0) + 2.0)"
    LINE "  }"
    LINE "oller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) + ($axis_len / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: `"
    LINE " () \\{"
    LINE "  gl_Position = vec4(0.0);"
    LINE "        }`"
    LINE "      }"
    LINE "  }"
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "          link_a {}"
    LINE "          l"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer03/crashes/id:000008,sig:11,src:002393,time:2004204,execs:504907,op:havoc,rep:2, sha1=6170c3d644cd052d288fc924516ace76b73680bf, grouped_crashes=2
 * asan_stack:
 *     #0 0x0001011927d8 in __asan::Allocator::Deallocate(void*, unsigned long, unsigned long, __sanitizer::BufferedStackTrace*, __asan::AllocType) (/opt/homebrew/Cellar/llvm/20.1.8/lib/clang/20/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64+0x27d8)
 *     #1 0x0001011e29a0 in free (/opt/homebrew/Cellar/llvm/20.1.8/lib/clang/20/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64+0x529a0)
 *     #2 0x0001007f2a44 in ecs_string_t_copy flecs/src/addons/meta/type_support/primitive_ts.c:12:8
 *     #3 0x0001009cbdc4 in ecs_value_copy_w_type_info flecs/src/value.c:140:9
 *     #4 0x000100899068 in flecs_script_template_ctor flecs/src/addons/script/template.c:89:13
 *     #5 0x0001009b3bdc in flecs_table_invoke_add_hooks flecs/src/storage/table.c:1055:9
 *     #6 0x0001009b2c6c in flecs_table_move flecs/src/storage/table.c:2035:17
 *     #7 0x0001008f8d48 in flecs_move_entity flecs/src/entity.c:218:5
 *     #8 0x0001008f85c4 in flecs_commit flecs/src/entity.c:279:5
 *     #9 0x0001008fad98 in flecs_add_id_w_record flecs/src/entity.c:460:5
 *     #10 0x0001008fad98 in flecs_ensure flecs/src/entity.c:580:5
 *     #11 0x000100905d24 in ecs_ensure_id flecs/src/entity.c:2034:20
 */
void Fuzzing_81(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Size {"
    LINE "  w = f32"
    LINE " ght = f32"
    LINE "}"
    LINE ""
    LINE "template Marker {"
    LINE "  prop width = flecs.meta.f32: 4"
    LINE "  prop height = flecs.meta.f32: 2"
    LINE "  prop x = flecs.meta.f32: 0"
    LINE "  prop y =\"fle&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&cs.meta!f32: 0"
    LINE ""
    LINE "\" marker {"
    LINE "Position: {$x, $y}"
    LINE "    Size: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "entity_a {"
    LINE "  Marker: {x: 10, y: 20}"
    LINE "}"
    LINE "ity_b {"
    LINE "  Marker: {x: -5, cs.meta.f32: 0}"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer04/crashes/id:000000,sig:11,src:000010,time:238025,execs:61106,op:havoc,rep:1, sha1=280d0b336cbae154a684fcb9decf1000fa94576b, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001003574d8 in flecs_set_array flecs/src/addons/meta/type_support/array_ts.c:33:23
 *     #1 0x0001004a1dd0 in flecs_observer_invoke flecs/src/observer.c:381:9
 *     #2 0x0001004a1dd0 in flecs_uni_observer_invoke flecs/src/observer.c:425:9
 *     #3 0x0001004a1dd0 in flecs_observers_invoke flecs/src/observer.c:518:13
 *     #4 0x00010049bebc in flecs_emit flecs/src/observable.c:1493:13
 *     #5 0x00010044f554 in flecs_notify_on_set flecs/src/component_actions.c:634:9
 *     #6 0x000100472ae4 in ecs_modified_id flecs/src/entity.c:2303:5
 *     #7 0x0001004120dc in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:1010:9
 *     #8 0x0001004120dc in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1655:16
 *     #9 0x000100409f24 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #10 0x00010040e6e8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #11 0x00010040fe10 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 */
void Fuzzing_82(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE "  left = Controller"
    LINE "  right = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: 332, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type: i32}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3, 5, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: 2.5e2"
    LINE "    }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000000,sig:11,src:001249,time:809022,execs:205800,op:havoc,rep:1, sha1=ee4f179341903578cd5ecf93453f960fac7bcb68, grouped_crashes=2
 * asan_stack:
 *     #0 0x000102c47a3c in flecs_rtt_init_default_hooks_vector flecs/src/addons/meta/rtt_lifecycle.c:892:45
 *     #1 0x000102c47a3c in flecs_rtt_init_default_hooks flecs/src/addons/meta/rtt_lifecycle.c:969:13
 *     #2 0x000102d99dd0 in flecs_observer_invoke flecs/src/observer.c:381:9
 *     #3 0x000102d99dd0 in flecs_uni_observer_invoke flecs/src/observer.c:425:9
 *     #4 0x000102d99dd0 in flecs_observers_invoke flecs/src/observer.c:518:13
 *     #5 0x000102d93ebc in flecs_emit flecs/src/observable.c:1493:13
 *     #6 0x000102d47554 in flecs_notify_on_set flecs/src/component_actions.c:634:9
 *     #7 0x000102d72014 in flecs_modified_id_if flecs/src/entity.c:2262:5
 *     #8 0x000102d41208 in flecs_defer_end flecs/src/commands.c:1262:21
 *     #9 0x000102d6ab00 in ecs_modified_id flecs/src/entity.c:2307:5
 *     #10 0x000102d0a0dc in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:1010:9
 *     #11 0x000102d0a0dc in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1655:16
 */
void Fuzzing_83(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {\013  x = f32, 8"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  sint$= f64"
    LINE "}"
    LINE ""
    LINE "struct ControlPair {"
    LINE "  left = Controller"
    LINE "  right = Controller"
    LINE "}"
    LINE ""
    LINE "FibArray {"
    LINE "  array: {type: i32, count: 7}"
    LINE "}"
    LINE ""
    LINE "FibVector {"
    LINE "  vector: {type:  52}"
    LINE "}"
    LINE ""
    LINE "controller_entity {"
    LINE "  FibArray: [1, 2, 3,i3, 8, 13, 21]"
    LINE "  FibVector: [1, 2, 3, 5, 8, 13, 21]"
    LINE ""
    LINE "  ControlPair: {"
    LINE "    left: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      sett: 2.5e2"
    LINE "    }"
    LINE "    right: {"
    LINE "      gains: {p: 3.0 / 1.0e2, i: 1.0 / 1.0e3, d: 7.5 / 1.0e4}"
    LINE "      direction: {x: 0, y: 1}"
    LINE "      setpoint: 7.5e1"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000001,sig:06,src:000885,time:1137824,execs:288020,op:havoc,rep:1, sha1=95df6db40779938f9ec387d58efd622e2593f5e9, grouped_crashes=1
 * asan_stack:
 *     #0 0x000100a8e93c in free (/opt/homebrew/Cellar/llvm/20.1.8/lib/clang/20/lib/darwin/libclang_rt.asan_osx_dynamic.dylib:arm64+0x5293c)
 *     #1 0x0001000e2704 in EcsScriptConstVar_dtor flecs/src/addons/script/function.c:42:1
 *     #2 0x0001002113e8 in flecs_table_dtor_all flecs/src/storage/table.c:1138:13
 *     #3 0x0001002113e8 in flecs_table_fini_data flecs/src/storage/table.c:1187:5
 *     #4 0x0001002124d8 in flecs_table_fini flecs/src/storage/table.c:1305:5
 *     #5 0x0001002339dc in flecs_clean_tables flecs/src/world.c:567:9
 *     #6 0x0001002339dc in flecs_fini_store flecs/src/world.c:680:5
 *     #7 0x0001002339dc in ecs_fini flecs/src/world.c:1303:5
 *     #8 0x00010000ca50 in fuzz_one fuzz/flecs_script_harness.c:108:5
 *     #9 0x00010000ca50 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_84(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "      axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: { attack: $attack + ($i * 0.75)"
    LINE "        defense: $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parent"
    LINE "parent_root {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_mix}"
    LINE ""
    LINE "  fleet_alpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180, efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenance bay\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_root {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "  Position: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8, orbit_phase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1, py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE ""
    LINE "    @tree Parent"
    LINE "    parent_section {"
    LINE "      nested_parent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "      nested_parent_b {"
    LINE "        Position: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate:E{\"mixed/default hierarchy root\"}"
    LINE "  Orbit: {radius: $orbit_radius + $axis_len, phase: $trig_mix + $damping}"
    LINE "  Combat: {"
    LINE "    attack: ($cruise_speed / 3.0) + 1"
    LINE "    defense: (abs(-4.0) + 2.0)"
    LINE "  }"
    LINE ""
    LINE "  controller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) +  {}"
    LINE "          link_b {}"
    LINE "  ($axis_len / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: ` void main() \\{"
    LINE "ition = vec4(0.0);"
    LINE "        }`"
    LINE "      }"
    LINE "  }"
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "  link_a {}"
    LINE "     k_b {}"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  gate_marker {"
    LINE "    Nameplate: {\"fast gate marker\"}"
    LINE "    Flags: {fastfast}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000006,sig:11,src:001834,time:1307813,execs:331973,op:havoc,rep:1, sha1=62ba6ae35f49e8f9f7386545999131e6e91217a8, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010085674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x00010087b2b8 in flecs_script_parse_rhs flecs/src/addons/script/expr/parser.c:329:9
 *     #2 0x00010087b2b8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:707:12
 *     #3 0x000100879424 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #4 0x000100879424 in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:196:15
 *     #5 0x0001008e1af4 in flecs_script_stmt flecs/src/addons/script/parser.c:868:5
 *     #6 0x0001008e64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #7 0x0001008e832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x0001007fca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x0001007fca48 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_85(void) {
    const char *expr =
    HEAD "us = 3.5"
    LINE "/*/"
    LINE "}"
    LINE ""
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000008,sig:11,src:001850+000958,time:1341710,execs:340778,op:splice,rep:5, sha1=d755b653034cfbd93055a2febc8f55346bba0b64, grouped_crashes=4
 * asan_stack:
 *     #0 0x00010261674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x00010269ef44 in flecs_script_stmt flecs/src/addons/script/parser.c:833:5
 *     #2 0x0001026a4a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x0001026a161c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #4 0x0001026a64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #5 0x0001026a832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x0001025bca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x0001025bca48 in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_86(void) {
    const char *expr =
    HEAD "usi/*/us[/*/"
    LINE "      gl_using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE " /*/"
    LINE "      gl_ucing flecs.meta"
    LINE ""
    LINE "s y = f32"
    LINE "}"
    LINE ""
    LINE "struct Venocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "ity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "enu.5"
    LINE ""
    LINE "ship_1"
    LINE "\020"
    LINE "  Position: {x: 10, y: 20}"
    LINE "  Velocity: {$speed, 0}\026  Team: {Red}"
    LINE "}"
    LINE ""
    LINE "ship_2 {"
    LINE "  Position8 {30, 40}"
    LINE "  Velocity: {-$speed"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000019,sig:11,src:002009+000193,time:1538200,execs:389937,op:splice,rep:12, sha1=dccaffde1492205e463daac84b2a7904bbd8b225, grouped_crashes=1
 * asan_stack:
 *     #0 0x000102e3274c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102e55f64 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #2 0x000102e57474 in flecs_script_parse_rhs flecs/src/addons/script/expr/parser.c:329:9
 *     #3 0x000102e57474 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:707:12
 *     #4 0x000102e55424 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #5 0x000102e55424 in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:196:15
 *     #6 0x000102ebdaf4 in flecs_script_stmt flecs/src/addons/script/parser.c:868:5
 *     #7 0x000102ec24e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #8 0x000102ec432c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #9 0x000102dd8a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #10 0x000102dd8a48 in main fuzz/flecs_script_harness.c:132:9
 *     #11 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_87(void) {
    const char *expr =
    HEAD "usii =us[/*/"
    LINE "/*,*,us[9*/"
    LINE "/*,un"
    LINE "{1.5} }"
    LINE "child_st "
    LINE ""
    LINE "\002"
    LINE "e {"
    LINE "  v = sg"
    LINE "\227"
    LINE ""
    LINE "\"}w{\"root\"}with Mass sXusing flecs."
    LINE ""
    LINE "baxis_le\025e_fast: \177ss"
    LINE "\200|"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000020,sig:11,src:001337,time:1571170,execs:398643,op:havoc,rep:3, sha1=d34560b92e5a9d2177e0d18d3af2d1b2aa162e7d, grouped_crashes=1
 * asan_stack:
 *     #0 0x000100be674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000100c71ea8 in flecs_script_stmt flecs/src/addons/script/parser.c:806:5
 *     #2 0x000100c74a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x000100c6f22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x000100c764e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #5 0x000100c7832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x000100b8ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x000100b8ca48 in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_88(void) {
    const char *expr =
    HEAD "using\\\\\\\\\\\\ot {"
    LINE " te:  strina"
    LINE "/*/us[/{\"r\\\\\\\\\\\\\\o\216\216\"root\"}with MsorNode: {iddddddddddddddddddddddddddx: 0th ymonePack {"
    LINE "  prop thrust = flecs."
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "stru\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\~\\\\O\\\\\\oot\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264\264te: {\"root\"}wi   S"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000022,sig:11,src:002011,time:1707292,execs:434265,op:havoc,rep:5, sha1=08ccdc94a07a25a14e6839e7574c8b55aae0ce58, grouped_crashes=1
 * asan_stack:
 *     #0 0x000102b0a74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102b2df64 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #2 0x000102b2e3b0 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #3 0x000102b2e3b0 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #4 0x000102b2e3b0 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #5 0x000102b2e3b0 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #6 0x000102b2e3b0 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #7 0x000102b2e3b0 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #8 0x000102b2d424 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #9 0x000102b2d424 in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:196:15
 *     #10 0x000102b95af4 in flecs_script_stmt flecs/src/addons/script/parser.c:868:5
 *     #11 0x000102b98a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 */
void Fuzzing_89(void) {
    const char *expr =
    HEAD "usi"
    LINE "ct PID {"
    LINE "  p = f!!!/*//d,us[[[X[/*/"
    LINE "/*,us}!!}"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000023,sig:11,src:002011,time:1711178,execs:435335,op:havoc,rep:6, sha1=89620ede803fec6de98933430caf8d59d70b5c51, grouped_crashes=2
 * asan_stack:
 *     #0 0x000104f6674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104ff4914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x000104fedd18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #3 0x000104ff64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #4 0x000104ff832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #5 0x000104f0ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #6 0x000104f0ca48 in main fuzz/flecs_script_harness.c:132:9
 *     #7 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_90(void) {
    const char *expr =
    HEAD "usi/*,us[/*//////"
    LINE "/*,us[p = f!!/"
    LINE "/i/*,us[/*///////////////////;*,us[/*/"
    LINE "ft PI; {"
    LINE "  /*/"
    LINE "ct PI//////////////"
    LINE "/*,us[p = f!!!!!}"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000025,sig:11,src:002107,time:1979756,execs:502400,op:havoc,rep:1, sha1=dc6d3accc1a3648bba9a4e7146689132eaf5d53d, grouped_crashes=2
 * asan_stack:
 *     #0 0x00010088e74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001009143ac in flecs_script_stmt flecs/src/addons/script/parser.c:447:5
 *     #2 0x00010091ca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x00010091722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x00010091ca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x00010091722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #6 0x00010091e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #7 0x00010092032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000100834a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000100834a48 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_91(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "      axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / D.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..ount {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: { attack: $attack + ($i * 0.75)"
    LINE "     $  defense: $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parent"
    LINE "parent_root {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_mix}"
    LINE ""
    LINE " lpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    Engi/*,us[/*/"
    LINE "/*,us[//////efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fa {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenance bay\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_root {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "  Position: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8, orbit_phase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1, py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE ""
    LINE "    @tree Parent"
    LINE "    parent_section {"
    LINE "      nrent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "      nested_parent_b {"
    LINE "Position: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate: {\"mixed/default hierarchy root\"}"
    LINE "  Orbit: {radius: $orbit_radius + $axis_len, phase: $trig_mix + $damping}"
    LINE "  Combat: {"
    LINE "    attack: ($cruise_speed / 3.0) + 1"
    LINE "    defense: (abs(-4.0) + 2.0)"
    LINE "  }"
    LINE "oller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) + ($axis_len / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: `"
    LINE " () \\{"
    LINE "  gl_Position = vec4(0.0);"
    LINE "        }`"
    LINE "      }"
    LINE "  }"
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "          link_a {}"
    LINE "  link_b {}"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  gate_marker {"
    LINE "    Nameplate: {\"fast gate marker\"}"
    LINE "    Flags: {fast: $gate_fast}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000026,sig:11,src:002944,time:2316439,execs:585354,op:havoc,rep:7, sha1=844a1ecf067f61ef7457a22c4a2655739b63bfe2, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010485674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x00010487898c in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:170:13
 *     #2 0x0001048e1af4 in flecs_script_stmt flecs/src/addons/script/parser.c:868:5
 *     #3 0x0001048e4a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x0001048ddd18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #5 0x0001048e4a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #6 0x0001048df22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #7 0x0001048e64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #8 0x0001048e832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #9 0x0001047fca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #10 0x0001047fca48 in main fuzz/flecs_script_harness.c:132:9
 *     #11 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_92(void) {
    const char *expr =
    HEAD "us"
    LINE ""
    LINE "stru$orbit_radte {{"
    LINE "s=------------E----se_s Ch/*,us[/*/"
    LINE "/*,,,,,,,,,,,,,,,,,,,,ildO---"
    LINE "  sed =--------\016---E----se_s > 30.0)"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000027,sig:11,src:002988,time:2358938,execs:596232,op:havoc,rep:2, sha1=977cb43350909f5434f4019d7fcb5d7e60c66dae, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001002c674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001002e9f64 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #2 0x0001002eb748 in flecs_script_parse_rhs flecs/src/addons/script/expr/parser.c:329:9
 *     #3 0x0001002eb748 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:707:12
 *     #4 0x0001002eb474 in flecs_script_parse_rhs flecs/src/addons/script/expr/parser.c:329:9
 *     #5 0x0001002eb474 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:707:12
 *     #6 0x0001002e9c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #7 0x0001002ea6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #8 0x0001002e9c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #9 0x0001002ea6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #10 0x0001002e9c18 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #11 0x0001002ea6a8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 */
void Fuzzing_93(void) {
    const char *expr =
    HEAD "using flecs."
    LINE ""
    LINE "sd Expe {"
    LINE "se = f66"
    LINE "  sed =-------------us[A*"
    LINE "/*,0b0000000E----se_s\024> 3"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer05/crashes/id:000028,sig:11,src:002623,time:2404477,execs:607854,op:havoc,rep:3, sha1=21258ffe20dc3f13d9861a9422196223c1c8f580, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001006ca74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000100752f44 in flecs_script_stmt flecs/src/addons/script/parser.c:833:5
 *     #2 0x000100758a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x000100759608 in flecs_script_if_stmt flecs/src/addons/script/parser.c:245:5
 *     #4 0x0001007503f0 in flecs_script_stmt flecs/src/addons/script/parser.c:613:12
 *     #5 0x000100758a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #6 0x00010075561c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #7 0x00010075a4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #8 0x00010075c32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #9 0x000100670a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #10 0x000100670a48 in main fuzz/flecs_script_harness.c:132:9
 *     #11 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_94(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct ExpionSample {"
    LINE "  be Ch"
    LINE "  if"
    LINE "ch {   l_ase = f64"
    LINE "  shifted = i32"
    LINE "  mixed = f64"
    LINE "  ratio = f64"
    LINE "  g= usi/*,us[/*/"
    LINE "/*,usi/*,us[2"
    LINE "const shifted: (3 <= 4) + (64 >> 3)"
    LINE "const mixed: ((($base + 5.0e1) / 2.5) - ($shifted % 7)) / 3.0"
    LINE "const ratio: ($base / (=$mixed * $mixed) + 1.0))"
    LINE "const gate: (($mixed <= 10.0) && ($mixed < 1.0e2)) || false"
    LINE "r_entity {"
    LINE "  ExpressionSamplllllle: { base: $base shifted: $shifted"
    LINE "    mixed: $mixed ratio: $ratio"
    LINE "    gate: $gate"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000006,sig:11,src:001741,time:1293648,execs:333474,op:havoc,rep:3, sha1=4c1bd5a72b0fcde518cc502161c4923b2cbe1978, grouped_crashes=2
 * asan_stack:
 *     #0 0x00010276a74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001027f8914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x0001027f66bc in flecs_script_stmt flecs/src/addons/script/parser.c:806:5
 *     #3 0x0001027fa4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #4 0x0001027fc32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #5 0x000102710a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #6 0x000102710a48 in main fuzz/flecs_script_harness.c:132:9
 *     #7 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_95(void) {
    const char *expr =
    HEAD "ConwrolPai$: ConwrolP{"
    LINE " /*\200\377\177\377\377(1.4*ase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  a// r"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000012,sig:11,src:001741,time:1308863,execs:337478,op:havoc,rep:4, sha1=e6400771011c97462842e6db96e4624a36b0986d, grouped_crashes=1
 * asan_stack:
 *     #0 0x000100bfe74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000100c20770 in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:149:9
 *     #2 0x000100c22590 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #3 0x000100c21424 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #4 0x000100c21424 in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:196:15
 *     #5 0x000100c89af4 in flecs_script_stmt flecs/src/addons/script/parser.c:868:5
 *     #6 0x000100c8e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #7 0x000100c9032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000100ba4a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000100ba4a48 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_96(void) {
    const char *expr =
    HEAD "CootrolPai=N {"
    LINE " /*\200\377\200"
    LINE "\"(1.4*// r"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000014,sig:11,src:001741,time:1312793,execs:338542,op:havoc,rep:4, sha1=99d828c681efa6aac6da00192efe58fcad41d6a9, grouped_crashes=1
 * asan_stack:
 *     #0 0x000100b8674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000100c14914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x000100c0dd18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #3 0x000100c14a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x000100c0f22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #5 0x000100c164e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #6 0x000100c1832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #7 0x000100b2ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #8 0x000100b2ca48 in main fuzz/flecs_script_harness.c:132:9
 *     #9 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_97(void) {
    const char *expr =
    HEAD "ControlPai$d {"
    LINE " {"
    LINE " /*\200\377\177\377"
    LINE "\001"
    LINE ""
    LINE "\200*/ /*\200\377\177\377\377(1.4*// r"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000020,sig:11,src:001741,time:1329675,execs:343004,op:havoc,rep:4, sha1=0806bb5bb8a7f0355f08a2412c759bca056a0e34, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010447274c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x0001044f8660 in flecs_script_stmt flecs/src/addons/script/parser.c:534:5
 *     #2 0x000104500a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x0001044fb22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x000104500a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x0001044fb22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #6 0x0001045024e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #7 0x00010450432c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000104418a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000104418a48 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_98(void) {
    const char *expr =
    HEAD "Cof"
    LINE "ch {   l_2 { @rree PrntrolPai$:6{"
    LINE " /*"
    LINE ""
    LINE ""
    LINE "@\377(1.4*PrntrolPai$: {"
    LINE " /*"
    LINE ""
    LINE ""
    LINE "@\377// r"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000028,sig:11,src:001773,time:1399078,execs:361059,op:havoc,rep:3, sha1=ea16f0bc2e7844ea8f7f033ce99b01f25e550591, grouped_crashes=1
 * asan_stack:
 *     #0 0x000102c2674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102cb4f1c in flecs_script_comma_expr flecs/src/addons/script/parser.c:67:5
 *     #2 0x000102cb0790 in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #3 0x000102cb64e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #4 0x000102cb832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #5 0x000102bcca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #6 0x000102bcca48 in main fuzz/flecs_script_harness.c:132:9
 *     #7 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_99(void) {
    const char *expr =
    HEAD "Control,/*\200\3771.4*/"
    LINE "  /*//******\026"
    LINE "**********ol /*\200\377\177\377\200\020"
    LINE ""
    LINE ""
    LINE "*/*"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000030,sig:11,src:001701,time:1491092,execs:385070,op:havoc,rep:4, sha1=66269f2ee4eb47c83d0848d14d3da86e992b988b, grouped_crashes=1
 * asan_stack:
 *     #0 0x000102d2e74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102db4660 in flecs_script_stmt flecs/src/addons/script/parser.c:534:5
 *     #2 0x000102dbca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x000102db6144 in flecs_script_stmt flecs/src/addons/script/parser.c:575:5
 *     #4 0x000102dbe4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #5 0x000102dc032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #6 0x000102cd4a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #7 0x000102cd4a48 in main fuzz/flecs_script_harness.c:132:9
 *     #8 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_100(void) {
    const char *expr =
    HEAD "@tr{e Parent"
    LINE ""
    LINE "root {"
    LINE "  pq_a {"
    LINE "  "
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolG_2 { @rree P_it: {radiut_radius + brhild_ntrol /*\200\3771.4*/"
    LINE "  /*//******\026"
    LINE "b {"
    LINE "  a {}"
    LINE " ol_b {}"
    LINE "}"
    LINE ""
    LINE " "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000032,sig:11,src:001570,time:1586566,execs:410157,op:havoc,rep:3, sha1=c904cbc5bf09014bc019f8d340eba9aee7f3fc1e, grouped_crashes=1
 * asan_stack:
 *     #0 0x000104ae674c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104b6c660 in flecs_script_stmt flecs/src/addons/script/parser.c:534:5
 *     #2 0x000104b74a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x000104b6f22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x000104b74a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x000104b7161c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #6 0x000104b74a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x000104b6f22c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #8 0x000104b764e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #9 0x000104b7832c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000104a8ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000104a8ca48 in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_101(void) {
    const char *expr =
    HEAD "@tree Parent"
    LINE ""
    LINE "root {"
    LINE "  pt_a {"
    LINE "    @tree CxildOg"
    LINE "ch {   l_2 { @tree Prens_branch {"
    LINE "  l_pe: i32, a {}"
    LINE " al_b {}"
    LINE "}"
    LINE "  }"
    LINE " ntrolPair: {"
    LINE "left   }"
    LINE "  }g {}ruct er {"
    LINE "hrust: $thru{}"
    LINE "child_b {"
    LINE "  @tree = string"
    LINE "/*//}           {rent"
    LINE "   t_nested {"
    LINE "    l{}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000034,sig:11,src:001722,time:1612953,execs:416873,op:havoc,rep:1, sha1=701a4869cf717bf10196508f49c320747fcdf78b, grouped_crashes=1
 * asan_stack:
 *     #0 0x000102a3e74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102acc914 in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #2 0x000102ac5d18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #3 0x000102acca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #4 0x000102ac722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #5 0x000102acca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #6 0x000102ac722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #7 0x000102acca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #8 0x000102ac961c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #9 0x000102acca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #10 0x000102ac722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #11 0x000102ace4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 */
void Fuzzing_102(void) {
    const char *expr =
    HEAD "@tre"
    LINE ""
    LINE "root {"
    LINE "  pt_a {"
    LINE "    @tree ChildOf"
    LINE "ch {   l_2 { @rree Prent_branch {"
    LINE "  l_a {}"
    LINE " al_b {}"
    LINE "}"
    LINE "  }"
    LINE "    }"
    LINE ""
    LINE "  get_alp{}"
    LINE "        }"
    LINE " ha {"
    LINE "    (Faction, Earth) {}"
    LINE ""
    LINE "struct er {"
    LINE " _sng {ld_a {}"
    LINE "child_b {f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "ler {{"
    LINE " /*\200{   l_26{ @    l{}"
    LINE " $}}}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000036,sig:11,src:001861,time:1682377,execs:435073,op:havoc,rep:4, sha1=e44c3a5156688ede613b4be84382a9d54e75b9e0, grouped_crashes=1
 * asan_stack:
 *     #0 0x000102f9e74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000103024660 in flecs_script_stmt flecs/src/addons/script/parser.c:534:5
 *     #2 0x00010302ca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x00010302722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x00010302ca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x00010302722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #6 0x00010302ca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x00010302722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #8 0x00010302ca3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #9 0x00010302722c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #10 0x00010302e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #11 0x00010303032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 */
void Fuzzing_103(void) {
    const char *expr =
    HEAD "@tre"
    LINE ""
    LINE "root {"
    LINE "  pt_a {"
    LINE "    @tree ChildOf"
    LINE "ch {   l_2 { @r*\200\3771.4*/"
    LINE "  /*//*** /* vector al_b {}"
    LINE "}"
    LINE "  }"
    LINE "    }"
    LINE "et_alp{}"
    LINE "        }"
    LINE " ha {"
    LINE "    (Faction, Earth) {}"
    LINE ""
    LINE "etruct er {"
    LINE " _sng {f32"
    LINE "}"
    LINE ""
    LINE "ld_a {}"
    LINE "child_b {f32"
    LINE "  \212 = f32"
    LINE "}"
    LINE ""
    LINE "enum Te"
    LINE "ch {   l_26{ @    l{}"
    LINE " $}}}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000037,sig:11,src:001600,time:1707153,execs:441533,op:havoc,rep:3, sha1=ea89ccd1570c70723b6e96d0752633e370d72cdf, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010248a74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000102512f44 in flecs_script_stmt flecs/src/addons/script/parser.c:833:5
 *     #2 0x000102518a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x000102511d18 in flecs_script_stmt flecs/src/addons/script/parser.c:432:12
 *     #4 0x000102518a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x00010251561c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #6 0x000102518a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #7 0x00010251322c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #8 0x000102518a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #9 0x00010251322c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #10 0x000102518a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #11 0x00010251322c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 */
void Fuzzing_104(void) {
    const char *expr =
    HEAD "@tree Pa"
    LINE ""
    LINE "root {t_a {"
    LINE "    @tree ChildOf"
    LINE "ch {   l_2 { @tr_bra{"
    LINE "  l_a {}"
    LINE "  final_b {}"
    LINE "}"
    LINE "  }"
    LINE "   @tree Parent"
    LINE ""
    LINE "root {"
    LINE "  pt_a {"
    LINE "    @tree ChildOf"
    LINE "ch {   l_2 { @tree Pren {"
    LINE "  l_a {} "
    LINE " al_b {}"
    LINE "}"
    LINE "}"
    LINE "  }g {}"
    LINE ""
    LINE "struct er {"
    LINE "    {"
    LINE "  Nameplatld_b {"
    LINE "  @tree Paf32"
    LINE "  y = f32"
    LINE "}"
    LINE "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {}"
    LINE "Rel {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f6trig_mix += f64"
    LINE "}"
    LINE ""
    LINE "struct Combf32"
    LINE "  y = f32"
    LINE "/*//}   defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "striency = f64"
    LINE "  axiuct PID {"
    LINE "  y = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Controller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const/ 10"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000039,sig:11,src:001790,time:2135760,execs:551936,op:havoc,rep:3, sha1=585b37923c4f20d72eca9277d61de7de4e761075, grouped_crashes=1
 * asan_stack:
 *     #0 0x000104fce74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104ff32b8 in flecs_script_parse_rhs flecs/src/addons/script/expr/parser.c:329:9
 *     #2 0x000104ff32b8 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:707:12
 *     #3 0x000104ff3748 in flecs_script_parse_rhs flecs/src/addons/script/expr/parser.c:329:9
 *     #4 0x000104ff3748 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:707:12
 *     #5 0x000104ff1424 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #6 0x000104ff1424 in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:196:15
 *     #7 0x000105059af4 in flecs_script_stmt flecs/src/addons/script/parser.c:868:5
 *     #8 0x00010505e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #9 0x00010506032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #10 0x000104f74a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000104f74a48 in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_105(void) {
    const char *expr =
    HEAD "Con = l ack: ($cruiseuise_speed / 3.0) + 1"
    LINE " /*\200\3771.4 ////\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377(1."
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000040,sig:11,src:002937+001986,time:2263386,execs:584192,op:splice,rep:5, sha1=b35338a0c1f624456bf0f71c4aff3656c01c70e3, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001000de74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000100101f64 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #2 0x0001001023b0 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #3 0x0001001023b0 in flecs_script_parse_lhs flecs/src/addons/script/expr/parser.c:453:5
 *     #4 0x000100101424 in flecs_script_parse_expr flecs/src/addons/script/expr/parser.c:725:11
 *     #5 0x000100101424 in flecs_script_parse_initializer flecs/src/addons/script/expr/parser.c:196:15
 *     #6 0x000100169af4 in flecs_script_stmt flecs/src/addons/script/parser.c:868:5
 *     #7 0x00010016e4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #8 0x00010017032c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #9 0x000100084a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #10 0x000100084a48 in main fuzz/flecs_script_harness.c:132:9
 *     #11 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_106(void) {
    const char *expr =
    HEAD "CVn = l!/*\200\377using flecs.meta"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "   right: {"
    LINE "    */"
    LINE "  /*e {"
    LINE "  w = f32"
    LINE " ght = f32"
    LINE "}"
    LINE ""
    LINE "template Marker {"
    LINE "  prop width = flecs.meta.f32: 4"
    LINE "  prap height = flecs"
    LINE "  prop x = flecs.meta.f32: 0"
    LINE "  prop y = flecs.meta.f32: 0"
    LINE ""
    LINE "  marker {"
    LINE "PositionS {$x, $y}"
    LINE "    Size: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "entity_a {"
    LINE "  Marker: {x: 10, y: 20}"
    LINE "}"
    LINE ""
    LINE "entity_b {"
    LINE "  Marker: {x: -5, y: 3, "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000042,sig:11,src:000484+001740,time:2484596,execs:637432,op:splice,rep:1, sha1=950dec7f5f95e141e0872f63fcb55013c5925bd6, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010459a74c in flecs_token flecs/src/addons/parser/tokenizer.c:793:13
 *     #1 0x000104622f44 in flecs_script_stmt flecs/src/addons/script/parser.c:833:5
 *     #2 0x000104628a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #3 0x00010462322c in flecs_script_stmt flecs/src/addons/script/parser.c:451:5
 *     #4 0x000104628a3c in flecs_script_scope flecs/src/addons/script/parser.c:33:5
 *     #5 0x00010462561c in flecs_script_stmt flecs/src/addons/script/parser.c:905:5
 *     #6 0x00010462a4e8 in ecs_script_parse flecs/src/addons/script/parser.c:1050:15
 *     #7 0x00010462c32c in ecs_script_run flecs/src/addons/script/script.c:130:28
 *     #8 0x000104540a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #9 0x000104540a48 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_107(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y =5f32"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE " ader {"
    LINE "  filename = string"
    LINE "/*//}          "
        ;

    fuzz(expr);
}

/* crash=out/fuzzer08/crashes/id:000005,sig:11,src:000008,time:1348094,execs:349650,op:havoc,rep:1, sha1=6942e7438748b415a54141ab2303f4eb94f81235, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001042adbd4 in flecs_script_eval_var_component flecs/src/addons/script/visit_eval.c:1065:55
 *     #1 0x0001042adbd4 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1658:16
 *     #2 0x0001042a5f24 in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:109:13
 *     #3 0x0001042aa6e8 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #4 0x0001042abe10 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 *     #5 0x0001042a649c in ecs_script_visit_node_ flecs/src/addons/script/visit.c:127:9
 *     #6 0x0001042a649c in ecs_script_visit_from_ flecs/src/addons/script/visit.c:150:18
 *     #7 0x0001042a649c in ecs_script_visit_ flecs/src/addons/script/visit.c:172:12
 *     #8 0x0001042af728 in ecs_script_eval flecs/src/addons/script/visit_eval.c:1784:13
 *     #9 0x00010429c354 in ecs_script_run flecs/src/addons/script/script.c:137:9
 *     #10 0x0001041b0a48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x0001041b0a48 in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_108(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct MathSample {"
    LINE "  trig = f64"
    LINE "  power = f64"
    LINE "  root = f64"
    LINE "  dot_value = f64"
    LINE "  len_value = f64"
    LINE "  mixed = Vec3"
    LINE "  unit = Vec3"
    LINE "}"
    LINE ""
    LINE "const a = Vec3: {1.0e1, -2.5e1, 3.1}"
    LINE "const b = Vec3: {2.0, 4.0, -6.0}"
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE ""
    LINE "const trig: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const power: pow(1.0e2, 0.5) + exp2(3.0) - log10(1.0e3)"
    LINE "const root: sqrt(1.6e1) + abs(-2.5)"
    LINE "const mixed: lerp($a, $b, 0.25)"
    LINE "const unit: normalize($axis)$root"
    LINE "    dot_val"
    LINE "const dot_value: dot($a, $b)"
    LINE "const len_value: length($axis)"
    LINE ""
    LINE "mentity {"
    LINE "  MathSample: { trig: $trig"
    LINE "   $power"
    LINE "    root: $root"
    LINE "    dot_value: $dot_value len_value: $len_value"
    LINE "    mixed: $mixed"
    LINE "    unit: $unit"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000000,sig:11,src:003001,time:2328738,execs:569781,op:havoc,rep:1, sha1=9052187fb2925ee67b316584ac68fb9a4e37b3cc, grouped_crashes=5
 * asan_stack:
 *     #0 0x000104406468 in ecs_script_params_free flecs/src/addons/script/function.c:17:9
 *     #1 0x000104406468 in EcsScriptFunction_dtor flecs/src/addons/script/function.c:60:1
 *     #2 0x000104535110 in flecs_table_dtor_all flecs/src/storage/table.c:1138:13
 *     #3 0x000104535110 in flecs_table_fini_data flecs/src/storage/table.c:1187:5
 *     #4 0x000104536200 in flecs_table_fini flecs/src/storage/table.c:1305:5
 *     #5 0x000104557704 in flecs_clean_tables flecs/src/world.c:567:9
 *     #6 0x000104557704 in flecs_fini_store flecs/src/world.c:680:5
 *     #7 0x000104557704 in ecs_fini flecs/src/world.c:1303:5
 *     #8 0x000104330a50 in fuzz_one fuzz/flecs_script_harness.c:108:5
 *     #9 0x000104330a50 in main fuzz/flecs_script_harness.c:132:9
 *     #10 0x00018302eb94  (<unknown module>)
 */
void Fuzzing_109(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "Planet {}"
    LINE "Station {}"
    LINE "Faction {}"
    LINE "Engine {ink_a {}"
    LINE "Tgt {}"
    LINE ""
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Velocity {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Nameplate {"
    LINE "  value = string"
    LINE "}"
    LINE ""
    LINE "struct Orbit {"
    LINE "  radius = f64"
    LINE "  phase = f64"
    LINE "}"
    LINE ""
    LINE "struct Combat {"
    LINE "  attack = f32"
    LINE "  defense = f32"
    LINE "}"
    LINE ""
    LINE "struct Flags {"
    LINE "  fast = bool"
    LINE "}"
    LINE ""
    LINE "struct Vec2 {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct PID {"
    LINE "  p = f64"
    LINE "  i = f64"
    LINE "  d = f64"
    LINE "}"
    LINE ""
    LINE "struct Con: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Matroller {"
    LINE "  gains = PID"
    LINE "  direction = Vec2"
    LINE "  setpoint = f64"
    LINE "}"
    LINE ""
    LINE "struct EngineProfile {"
    LINE "  thrust = f64"
    LINE "  efficiency = f64"
    LINE "  axis = Vec3"
    LINE "}"
    LINE ""
    LINE "struct Shader {"
    LINE "  filename = string"
    LINE "  code = string"
    LINE "}"
    LINE ""
    LINE "enum Team {"
    LINE "  Red,"
    LINE "  Blue"
    LINE "}"
    LINE ""
    LINE "const base_speed: 3.2e1"
    LINE "const shift_factor: (3 << 3) + (64 >> 4)"
    LINE "const orbit_radius: sqrt(9.0e2) + 2.5"
    LINE "const trig_mix: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const damping: (exp2(3.0) - log10(1.0e3)) / 10.0"
    LINE "const cruise_speed: (($base_speed + ($shift_factor % 7)) * (1.0 + $damping))"
    LINE "const gate_fast: (($cruise_speed > 30.0) && ($orbit_radius < 40.0)) || false"
    LINE ""
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE "const unit_axis: normalize($axis)"
    LINE "const probe = Vec3: {1.0e1, -2.5, 3.5}"
    LINE "const mixed_axis: lerp($probe, $unit_axis, 0.35)"
    LINE "const axis_len: length($axis)"
    LINE ""
    LINE "Earth { Planet }"
    LINE "Mars { Planet }"
    LINE ""
    LINE "template EnginePack {"
    LINE "  prop thrust = flecs.meta.f64: 120.0"
    LINE "  prop efficiency = flecs.meta.f64: 0.82"
    LINE "  prop orbit_phase = flecs.meta.f64: 0.0"
    LINE ""
    LINE "  engine {"
    LINE "    Engine"
    LINE "    EngineProfile: {"
    LINE "      thrust: $thrust + ($cruise_speed / 2.0)"
    LINE "      efficiency: $efficiency"
    LINE "      axis: $mixed_axis"
    LINE "    }"
    LINE "    Orbit: {"
    LINE "      radius: $orbit_radius + ($thrust / 100.0)"
    LINE "      phase: $orbit_phase + ($trig_mix / 10.0)"
    LINE "    }"
    LINE "    Velocity: {"
    LINE "      x: $cruise_speed * 0.1"
    LINE "      y: ($cruise_speed / 5.0) - 1.0"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template PatrolGroup {"
    LINE "  prop count = flecs.meta.i32: 3"
    LINE "  prop spacing = flecs.meta.f32: 2"
    LINE "  prop attack = flecs.meta.f32: 5"
    LINE "  prop defense = flecs.meta.f32: 3"
    LINE ""
    LINE "  for i in 0..$count {"
    LINE "    \"unit_{$i}\" {"
    LINE "      Position: {"
    LINE "        x: ($i * $spacing) + ($shift_factor % 5)"
    LINE "        y: ($i * 0.5) - 1"
    LINE "      }"
    LINE "      Combat: { attack: $attack + ($i * 0.75)"
    LINE " $defense + ($i * 0.25)"
    LINE "      }"
    LINE "      Team: {Red}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template SensorNode {"
    LINE "  prop idx = flecs.meta.i32: 0"
    LINE "  prop px = flecs.meta.f32: 0"
    LINE "  prop py = flecs.meta.f32: 0"
    LINE ""
    LINE "  \"sensor_{$idx}\" {"
    LINE "    Position: {$px + ($idx * 1.5), $py - ($idx * 0.5)}"
    LINE "    Nameplate: {\"sensor node\"}"
    LINE "    Team: {Blue}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree Parente: {\"mix"
    LINE "pot {"
    LINE "  Nameplate: {\"parent storage root\"}"
    LINE "  Position: {x: 0, y: 0}"
    LINE "  Orbit: {radius: $orbit_radius, phase: $trig_mix}"
    LINE ""
    LINE " lpha {"
    LINE "    (Faction, Earth)"
    LINE "    Team: {Red}"
    LINE "    EnginePack: {thrust: 180, efficiency: 0.91, orbit_phase: 0.25}"
    LINE "    PatrolGroup: {count: 4, spacing: 2.5, attack: 7, defense: 4}"
    LINE "  }"
    LINE ""
    LINE "  fleet_beta {"
    LINE "    (Faction, Mars)"
    LINE "    Team: {Blue}"
    LINE "    EnginePack: {thrust: 140, efficiency: 0.88, orbit_phase: 0.75}"
    LINE ""
    LINE "    {"
    LINE "      Nameplate: {\"anonymous maintenance bay\"}"
    LINE "      SensorNode: {idx: 2, px: -4, py: 2}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "@tree ChildOf"
    LINE "childof_root {"
    LINE "  Nameplate: {\"childof storage root\"}"
    LINE "  Position: {x: -10, y: 5}"
    LINE ""
    LINE "  dock_a {"
    LINE "    EnginePack: {thrust: 110, efficiency: 0.8, orbit_phase: 1.5}"
    LINE "    SensorNode: {idx: 0, px: 1, py: 1}"
    LINE "  }"
    LINE ""
    LINE "  dock_b {"
    LINE "    PatrolGroup: {count: 2, spacing: 3.0, attack: 6, defense: 5}"
    LINE ""
    LINE "    @tree Parent"
    LINE "    parent_section {"
    LINE "      nested_parent_a {"
    LINE "        Position: {x: 4, y: 8}"
    LINE "      }"
    LINE "      nested_parent_b {"
    LINE "        Position: {x: 5, y: 9}"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "mixed_root {"
    LINE "  Nameplate: {\"mixed/default hierarchy root\"}"
    LINE "  Orbit: {radius: $orbit_radius + $axis_len, phase: $trig_mix + $damping}"
    LINE "  Combat: {"
    LINE "    attack: ($cruise_speed / 3.0) + 1"
    LINE "    defense: (abs(-4.0) + 2.0)"
    LINE "  }"
    LINE ""
    LINE "  controller_node {"
    LINE "    Controller: {"
    LINE "      gains: {p: 1.0 / 1.0e2, i: 2.5 / 1.0e3, d: 5.0 / 1.0e4}"
    LINE "      direction: {x: 1, y: 0}"
    LINE "      setpoint: ($orbit_radius * 2.0) + ($axis_len / 2.0)"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  shader_bank {"
    LINE "    (Shader, Station): {"
    LINE "      filename: \"station.glsl\","
    LINE "      code: `"
    LINE " oid main() \\{"
    LINE "  ion = vec4(0.0);"
    LINE "        }`"
    LINE "      }"
    LINE "  }"
    LINE ""
    LINE "  @tree Parent"
    LINE "  parent_zone {"
    LINE "    (Rel, Tgt) {"
    LINE "      relay {"
    LINE "        @tree ChildOf"
    LINE "        links {"
    LINE "          l}"
    LINE "Rel {}"
    LINE "          link_b {}"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE ""
    LINE "  gate_marker {"
    LINE "    Nameplate: {\"fast gate marker\"}"
    LINE "    Flags: {fast: $gate_fast}"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000001,sig:11,src:003042,time:2424645,execs:593728,op:havoc,rep:3, sha1=7629448a6cb886d7c28b12dec0fbb233a382542c, grouped_crashes=3
 * asan_stack:
 *     #0 0x000104ae9364 in flecs_path_elem flecs/src/entity_name.c
 *     #1 0x000104ae8950 in ecs_lookup_path_w_sep flecs/src/entity_name.c:734:19
 *     #2 0x000104ac9bec in ecs_entity_init flecs/src/entity.c:1334:22
 *     #3 0x000104a6cce8 in flecs_script_create_entity flecs/src/addons/script/visit_eval.c:352:12
 *     #4 0x000104a70cf0 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:762:22
 *     #5 0x000104a70cf0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1696:16
 *     #6 0x000104a69b1c in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:129:13
 *     #7 0x000104a6e410 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #8 0x000104a6fb38 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 *     #9 0x000104a6a200 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:151:9
 *     #10 0x000104a6a200 in ecs_script_visit_from_ flecs/src/addons/script/visit.c:175:18
 *     #11 0x000104a6a200 in ecs_script_visit_ flecs/src/addons/script/visit.c:197:12
 */
void Fuzzing_110(void) {
    const char *expr =
    HEAD ""
    LINE "\"\"\"**\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\:\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/\""
    LINE "@"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000002,sig:11,src:003072,time:2466677,execs:604623,op:havoc,rep:1, sha1=98eca0e7b193c28081f316ffe9de7f03a2c7e94f, grouped_crashes=1
 * asan_stack:
 *     #0 0x000104ca8460 in ecs_meta_set_string flecs/src/addons/meta/cursor.c:1485:9
 *     #1 0x000104d37408 in flecs_expr_initializer_eval_dynamic flecs/src/addons/script/expr/visit_eval.c:246:13
 *     #2 0x000104d37408 in flecs_expr_initializer_eval flecs/src/addons/script/expr/visit_eval.c:271:16
 *     #3 0x000104d33a60 in flecs_expr_initializer_visit_eval flecs/src/addons/script/expr/visit_eval.c:290:12
 *     #4 0x000104d33a60 in flecs_expr_visit_eval_priv flecs/src/addons/script/expr/visit_eval.c:1011:13
 *     #5 0x000104d33088 in flecs_expr_visit_eval flecs/src/addons/script/expr/visit_eval.c:1165:9
 *     #6 0x000104d6e024 in flecs_script_eval_expr flecs/src/addons/script/visit_eval.c:554:9
 *     #7 0x000104d71d5c in flecs_script_eval_component flecs/src/addons/script/visit_eval.c:1006:13
 *     #8 0x000104d71d5c in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1655:16
 *     #9 0x000104d69b1c in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:129:13
 *     #10 0x000104d6e410 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #11 0x000104d6fb38 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 */
void Fuzzing_111(void) {
    const char *expr =
    HEAD "#8 (64 >> 3)"
    LINE "constri"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000005,sig:11,src:003236,time:2793738,execs:692536,op:havoc,rep:4, sha1=3e28483a9de5870b724ae322484227458fb1a422, grouped_crashes=2
 * asan_stack:
 *     #0 0x000102d89364 in flecs_path_elem flecs/src/entity_name.c
 *     #1 0x000102d89cac in ecs_add_path_w_sep flecs/src/entity_name.c:918:23
 *     #2 0x000102d6b03c in flecs_traverse_add flecs/src/entity.c:1027:14
 *     #3 0x000102d6b03c in ecs_entity_init flecs/src/entity.c:1404:13
 *     #4 0x000102d0cce8 in flecs_script_create_entity flecs/src/addons/script/visit_eval.c:352:12
 *     #5 0x000102d10cf0 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:762:22
 *     #6 0x000102d10cf0 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1696:16
 *     #7 0x000102d09b1c in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:129:13
 *     #8 0x000102d0e410 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #9 0x000102d0fb38 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 *     #10 0x000102d0a200 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:151:9
 *     #11 0x000102d0a200 in ecs_script_visit_from_ flecs/src/addons/script/visit.c:175:18
 */
void Fuzzing_112(void) {
    const char *expr =
    HEAD ""
    LINE "\"\"\"**"
    LINE ""
    LINE "cosst axyk = Vec3: x3.0u\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\Y\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\004\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\, 4.0, */\""
    LINE "@"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer01/crashes/id:000007,sig:11,src:003236,time:2815039,execs:697547,op:havoc,rep:6, sha1=faffc5533d637cf09a0d6c86ed04a1dd398d4294, grouped_crashes=1
 * asan_stack:
 *     #0 0x00010293d364 in flecs_path_elem flecs/src/entity_name.c
 *     #1 0x00010293dcac in ecs_add_path_w_sep flecs/src/entity_name.c:918:23
 *     #2 0x00010291f03c in flecs_traverse_add flecs/src/entity.c:1027:14
 *     #3 0x00010291f03c in ecs_entity_init flecs/src/entity.c:1404:13
 *     #4 0x0001028c0ce8 in flecs_script_create_entity flecs/src/addons/script/visit_eval.c:352:12
 *     #5 0x0001028c09b4 in flecs_script_eval_name_expr flecs/src/addons/script/visit_eval.c:214:18
 *     #6 0x0001028c4cb4 in flecs_script_eval_entity flecs/src/addons/script/visit_eval.c:756:22
 *     #7 0x0001028c4cb4 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1696:16
 *     #8 0x0001028bdb1c in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:129:13
 *     #9 0x0001028c2410 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #10 0x0001028c3b38 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 *     #11 0x0001028be200 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:151:9
 */
void Fuzzing_113(void) {
    const char *expr =
    HEAD ""
    LINE "\"\"\"**"
    LINE ""
    LINE "cosst\316\316 axik = Vec3: x3.0u\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\034\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\B** {0}\\, 4.0, *%\""
    LINE "@"
        ;

    fuzz(expr);
}

/* crash=out/fuzzer06/crashes/id:000000,sig:11,src:000008,time:354295,execs:87887,op:havoc,rep:1, sha1=1af6f9c26b52efdb436ac5f52d843da0bdeadd6e, grouped_crashes=1
 * asan_stack:
 *     #0 0x0001050898fc in flecs_script_eval_var_component flecs/src/addons/script/visit_eval.c:1065:55
 *     #1 0x0001050898fc in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1658:16
 *     #2 0x000105081b1c in ecs_script_visit_scope_ flecs/src/addons/script/visit.c:129:13
 *     #3 0x000105086410 in flecs_script_eval_scope flecs/src/addons/script/visit_eval.c:596:18
 *     #4 0x000105087b38 in flecs_script_eval_node flecs/src/addons/script/visit_eval.c:1649:16
 *     #5 0x000105082200 in ecs_script_visit_node_ flecs/src/addons/script/visit.c:151:9
 *     #6 0x000105082200 in ecs_script_visit_from_ flecs/src/addons/script/visit.c:175:18
 *     #7 0x000105082200 in ecs_script_visit_ flecs/src/addons/script/visit.c:197:12
 *     #8 0x00010508b450 in ecs_script_eval flecs/src/addons/script/visit_eval.c:1784:13
 *     #9 0x000105077e94 in ecs_script_run flecs/src/addons/script/script.c:137:9
 *     #10 0x000104f8ca48 in fuzz_one fuzz/flecs_script_harness.c:107:11
 *     #11 0x000104f8ca48 in main fuzz/flecs_script_harness.c:132:9
 */
void Fuzzing_114(void) {
    const char *expr =
    HEAD "using flecs.meta"
    LINE "using flecs.script.math"
    LINE ""
    LINE "struct Vec3 {"
    LINE "  x = f64"
    LINE "  y = f64"
    LINE "  z = f64"
    LINE "}"
    LINE ""
    LINE "struct MathSample {"
    LINE "  trig = f64"
    LINE "  power = f64"
    LINE "  root = f64"
    LINE "  dot_value = f64"
    LINE "  len_value = f64"
    LINE "  mixed = Vec3"
    LINE "  unit = Vec3"
    LINE "}"
    LINE ""
    LINE "const a = Vec3: {1.0e1, -2.5e1, 3.1}"
    LINE "const b = Vec3: {2.0, 4.0, -6.0}"
    LINE "const axis = Vec3: {3.0, 4.0, 0.0}"
    LINE ""
    LINE "const trig: sin(PI / 6.0) + cos(PI / 3.0) + tan(PI / 4.0)"
    LINE "const power: pow(1.0e2, 0.5) + exp2(3.0) - log10(1.0e3)"
    LINE "const root: sqrt(1.6e1) + abs(-2.5)"
    LINE "const mixg: $trig"
    LINE "   $power"
    LINE "   "
    LINE "const unit: normalize($axis)"
    LINE "const dot_value: dot($a, $b)"
    LINE "const len_value: length($axis)"
    LINE ""
    LINE "mentity {"
    LINE "  MathSample: { tried: lerp($a, $b, 0.25) root: $root"
    LINE "    dot_value: $dot_value len_value: $len_value"
    LINE "    mixed: $mixed"
    LINE "    unit: $unit"
    LINE "  }"
    LINE "}"
    LINE ""
        ;

    fuzz(expr);
}
