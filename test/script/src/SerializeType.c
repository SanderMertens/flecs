#include <script.h>

void SerializeType_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            { "x", ecs_id(ecs_f32_t) },
            { "y", ecs_id(ecs_f32_t) }
        }
    });

    char *str = ecs_type_to_script(world, t);
    test_str(str, "struct Position(x: f32, y: f32)\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_struct_w_deps(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "enum Color(Red, Green)"
        LINE "struct Line(start: Point, stop: Point, color: Color)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Line"));
    test_str(str,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "enum Color(Red: 0, Green: 1)"
        LINE "struct Line(start: Point, stop: Point, color: Color)"
        LINE);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_struct_w_array_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Points" }),
        .members = {
            { "values", ecs_id(ecs_f32_t), 3 }
        }
    });

    char *str = ecs_type_to_script(world, t);
    test_str(str, "struct Points(values: {f32, count: 3})\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_struct_w_unit(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t unit = ecs_unit(world, {
        .entity = ecs_entity(world, { .name = "Meters" }),
        .symbol = "m"
    });

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Car" }),
        .members = {
            { "length", ecs_id(ecs_f32_t), .unit = unit }
        }
    });

    char *str = ecs_type_to_script(world, t);
    test_str(str, "struct Car(length: {f32, unit: Meters})\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_struct_inheritance(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "struct Point3D : Point(z: f32)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Point3D"));
    test_str(str,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "struct Point3D : Point(z: f32)"
        LINE);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_enum(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "enum Mix(A, B: 10, C)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Mix"));
    test_str(str, "enum Mix(A: 0, B: 10, C: 11)\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_enum_underlying_type(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "enum Color(Red, Green, {underlying_type: u8})", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Color"));
    test_str(str, "enum Color(Red: 0, Green: 1, {underlying_type: u8})\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_bitmask(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "bitmask Toppings(Bacon, Lettuce, Both: 3)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Toppings"));
    test_str(str, "bitmask Toppings(Bacon: 1, Lettuce: 2, Both: 3)\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_vector(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "vector Points(type: Point)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Points"));
    test_str(str,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "vector Points(type: Point)"
        LINE);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_array(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "array Points(type: f32, count: 3)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Points"));
    test_str(str, "array Points(type: f32, count: 3)\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_map(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        "map Lookup(key_type: i32, type: f32)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Lookup"));
    test_str(str, "map Lookup(key_type: i32, type: f32)\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_scoped(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "game {"
        LINE "  math {"
        LINE "    struct Point(x: f32, y: f32)"
        LINE "  }"
        LINE "  struct Line(start: math.Point, stop: math.Point)"
        LINE "}", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "game.Line"));
    test_str(str,
        HEAD "game {"
        LINE "  math {"
        LINE "    struct Point(x: f32, y: f32)"
        LINE "  }"
        LINE "}"
        LINE "game {"
        LINE "  struct Line(start: game.math.Point, stop: game.math.Point)"
        LINE "}"
        LINE);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_shared_dep(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "struct Line(start: Point, stop: Point)"
        LINE "struct Shape(line: Line, center: Point)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Shape"));
    test_str(str,
        HEAD "struct Point(x: f32, y: f32)"
        LINE "struct Line(start: Point, stop: Point)"
        LINE "struct Shape(line: Line, center: Point)"
        LINE);
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_builtin_dep(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ref" }),
        .members = {
            { "target", ecs_id(ecs_entity_t) },
            { "member", ecs_id(EcsMember) }
        }
    });

    char *str = ecs_type_to_script(world, t);
    test_str(str, "struct Ref(target: entity, member: member)\n");
    ecs_os_free(str);

    ecs_fini(world);
}

void SerializeType_roundtrip(void) {
    ecs_world_t *world = ecs_init();

    test_int(ecs_script_run(world, NULL,
        HEAD "game {"
        LINE "  struct Point(x: f32, y: f32)"
        LINE "}"
        LINE "enum Color(Red, Green: 5, Blue, {underlying_type: u16})"
        LINE "bitmask Flags(A, B)"
        LINE "struct Point3D : game.Point(z: f32)"
        LINE "struct Shape(points: {game.Point, count: 2}, color: Color, "
            "flags: Flags, p: Point3D)", NULL), 0);

    char *str = ecs_type_to_script(world, ecs_lookup(world, "Shape"));
    test_assert(str != NULL);

    ecs_world_t *world2 = ecs_init();
    test_int(ecs_script_run(world2, NULL, str, NULL), 0);
    char *str2 = ecs_type_to_script(world2, ecs_lookup(world2, "Shape"));
    test_str(str, str2);

    ecs_entity_t shape = ecs_lookup(world, "Shape");
    ecs_entity_t shape2 = ecs_lookup(world2, "Shape");
    test_int(ecs_get(world, shape, EcsComponent)->size,
        ecs_get(world2, shape2, EcsComponent)->size);

    ecs_os_free(str);
    ecs_os_free(str2);
    ecs_fini(world2);
    ecs_fini(world);
}

void SerializeType_primitive(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_type_to_script(world, ecs_id(ecs_f32_t)) == NULL);

    ecs_fini(world);
}

void SerializeType_not_a_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_entity(world, { .name = "Tag" });
    ecs_log_set_level(-4);
    test_assert(ecs_type_to_script(world, e) == NULL);

    ecs_fini(world);
}

void SerializeType_opaque_dep(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t t = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Named" }),
        .members = {
            { "name", ecs_id(ecs_string_t) }
        }
    });

    char *str = ecs_type_to_script(world, t);
    test_str(str, "struct Named(name: string)\n");
    ecs_os_free(str);

    ecs_fini(world);
}
