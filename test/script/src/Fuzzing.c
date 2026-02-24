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
