
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <meta.h>

// Testsuite 'PrimitiveTypes'
void PrimitiveTypes_bool(void);
void PrimitiveTypes_byte(void);
void PrimitiveTypes_char(void);
void PrimitiveTypes_i8(void);
void PrimitiveTypes_i16(void);
void PrimitiveTypes_i32(void);
void PrimitiveTypes_i64(void);
void PrimitiveTypes_iptr(void);
void PrimitiveTypes_u8(void);
void PrimitiveTypes_u16(void);
void PrimitiveTypes_u32(void);
void PrimitiveTypes_u64(void);
void PrimitiveTypes_uptr(void);
void PrimitiveTypes_float(void);
void PrimitiveTypes_double(void);
void PrimitiveTypes_string(void);
void PrimitiveTypes_entity(void);
void PrimitiveTypes_builtin_bool(void);
void PrimitiveTypes_builtin_byte(void);
void PrimitiveTypes_builtin_char(void);
void PrimitiveTypes_builtin_i8(void);
void PrimitiveTypes_builtin_i16(void);
void PrimitiveTypes_builtin_i32(void);
void PrimitiveTypes_builtin_i64(void);
void PrimitiveTypes_builtin_iptr(void);
void PrimitiveTypes_builtin_u8(void);
void PrimitiveTypes_builtin_u16(void);
void PrimitiveTypes_builtin_u32(void);
void PrimitiveTypes_builtin_u64(void);
void PrimitiveTypes_builtin_uptr(void);
void PrimitiveTypes_builtin_float(void);
void PrimitiveTypes_builtin_double(void);
void PrimitiveTypes_builtin_string(void);
void PrimitiveTypes_builtin_entity(void);

// Testsuite 'StructTypes'
void StructTypes_i32(void);
void StructTypes_i32_i32(void);
void StructTypes_i32_i32_i32(void);
void StructTypes_bool_i32(void);
void StructTypes_bool_bool_i32(void);
void StructTypes_bool_i32_bool_i32(void);
void StructTypes_i32_bool(void);
void StructTypes_i32_bool_bool(void);
void StructTypes_i32_bool_bool_bool(void);
void StructTypes_i32_i64(void);
void StructTypes_i32_i64_i32(void);

// Testsuite 'NestedStructTypes'
void NestedStructTypes_1_bool(void);
void NestedStructTypes_1_int32(void);
void NestedStructTypes_1_bool_w_bool_member(void);
void NestedStructTypes_1_bool_w_int32_member(void);
void NestedStructTypes_1_int32_w_bool_member(void);
void NestedStructTypes_1_int32_w_int32_member(void);
void NestedStructTypes_1_bool_2_same(void);
void NestedStructTypes_1_bool_2_bool(void);
void NestedStructTypes_1_bool_2_i32(void);
void NestedStructTypes_1_i32_2_bool(void);
void NestedStructTypes_1_bool_2_bool_bool(void);
void NestedStructTypes_1_bool_bool_2_bool(void);
void NestedStructTypes_1_bool_bool_2_same(void);
void NestedStructTypes_1_bool_bool_2_bool_bool(void);
void NestedStructTypes_1_i32_2_same(void);
void NestedStructTypes_1_i32_2_i32(void);
void NestedStructTypes_1_i32_2_i32_i32(void);
void NestedStructTypes_1_i32_i32_2_i32(void);
void NestedStructTypes_1_i32_i32_2_same(void);
void NestedStructTypes_1_i32_i32_2_i32_i32(void);

bake_test_case PrimitiveTypes_testcases[] = {
    {
        "bool",
        PrimitiveTypes_bool
    },
    {
        "byte",
        PrimitiveTypes_byte
    },
    {
        "char",
        PrimitiveTypes_char
    },
    {
        "i8",
        PrimitiveTypes_i8
    },
    {
        "i16",
        PrimitiveTypes_i16
    },
    {
        "i32",
        PrimitiveTypes_i32
    },
    {
        "i64",
        PrimitiveTypes_i64
    },
    {
        "iptr",
        PrimitiveTypes_iptr
    },
    {
        "u8",
        PrimitiveTypes_u8
    },
    {
        "u16",
        PrimitiveTypes_u16
    },
    {
        "u32",
        PrimitiveTypes_u32
    },
    {
        "u64",
        PrimitiveTypes_u64
    },
    {
        "uptr",
        PrimitiveTypes_uptr
    },
    {
        "float",
        PrimitiveTypes_float
    },
    {
        "double",
        PrimitiveTypes_double
    },
    {
        "string",
        PrimitiveTypes_string
    },
    {
        "entity",
        PrimitiveTypes_entity
    },
    {
        "builtin_bool",
        PrimitiveTypes_builtin_bool
    },
    {
        "builtin_byte",
        PrimitiveTypes_builtin_byte
    },
    {
        "builtin_char",
        PrimitiveTypes_builtin_char
    },
    {
        "builtin_i8",
        PrimitiveTypes_builtin_i8
    },
    {
        "builtin_i16",
        PrimitiveTypes_builtin_i16
    },
    {
        "builtin_i32",
        PrimitiveTypes_builtin_i32
    },
    {
        "builtin_i64",
        PrimitiveTypes_builtin_i64
    },
    {
        "builtin_iptr",
        PrimitiveTypes_builtin_iptr
    },
    {
        "builtin_u8",
        PrimitiveTypes_builtin_u8
    },
    {
        "builtin_u16",
        PrimitiveTypes_builtin_u16
    },
    {
        "builtin_u32",
        PrimitiveTypes_builtin_u32
    },
    {
        "builtin_u64",
        PrimitiveTypes_builtin_u64
    },
    {
        "builtin_uptr",
        PrimitiveTypes_builtin_uptr
    },
    {
        "builtin_float",
        PrimitiveTypes_builtin_float
    },
    {
        "builtin_double",
        PrimitiveTypes_builtin_double
    },
    {
        "builtin_string",
        PrimitiveTypes_builtin_string
    },
    {
        "builtin_entity",
        PrimitiveTypes_builtin_entity
    }
};

bake_test_case StructTypes_testcases[] = {
    {
        "i32",
        StructTypes_i32
    },
    {
        "i32_i32",
        StructTypes_i32_i32
    },
    {
        "i32_i32_i32",
        StructTypes_i32_i32_i32
    },
    {
        "bool_i32",
        StructTypes_bool_i32
    },
    {
        "bool_bool_i32",
        StructTypes_bool_bool_i32
    },
    {
        "bool_i32_bool_i32",
        StructTypes_bool_i32_bool_i32
    },
    {
        "i32_bool",
        StructTypes_i32_bool
    },
    {
        "i32_bool_bool",
        StructTypes_i32_bool_bool
    },
    {
        "i32_bool_bool_bool",
        StructTypes_i32_bool_bool_bool
    },
    {
        "i32_i64",
        StructTypes_i32_i64
    },
    {
        "i32_i64_i32",
        StructTypes_i32_i64_i32
    }
};

bake_test_case NestedStructTypes_testcases[] = {
    {
        "1_bool",
        NestedStructTypes_1_bool
    },
    {
        "1_int32",
        NestedStructTypes_1_int32
    },
    {
        "1_bool_w_bool_member",
        NestedStructTypes_1_bool_w_bool_member
    },
    {
        "1_bool_w_int32_member",
        NestedStructTypes_1_bool_w_int32_member
    },
    {
        "1_int32_w_bool_member",
        NestedStructTypes_1_int32_w_bool_member
    },
    {
        "1_int32_w_int32_member",
        NestedStructTypes_1_int32_w_int32_member
    },
    {
        "1_bool_2_same",
        NestedStructTypes_1_bool_2_same
    },
    {
        "1_bool_2_bool",
        NestedStructTypes_1_bool_2_bool
    },
    {
        "1_bool_2_i32",
        NestedStructTypes_1_bool_2_i32
    },
    {
        "1_i32_2_bool",
        NestedStructTypes_1_i32_2_bool
    },
    {
        "1_bool_2_bool_bool",
        NestedStructTypes_1_bool_2_bool_bool
    },
    {
        "1_bool_bool_2_bool",
        NestedStructTypes_1_bool_bool_2_bool
    },
    {
        "1_bool_bool_2_same",
        NestedStructTypes_1_bool_bool_2_same
    },
    {
        "1_bool_bool_2_bool_bool",
        NestedStructTypes_1_bool_bool_2_bool_bool
    },
    {
        "1_i32_2_same",
        NestedStructTypes_1_i32_2_same
    },
    {
        "1_i32_2_i32",
        NestedStructTypes_1_i32_2_i32
    },
    {
        "1_i32_2_i32_i32",
        NestedStructTypes_1_i32_2_i32_i32
    },
    {
        "1_i32_i32_2_i32",
        NestedStructTypes_1_i32_i32_2_i32
    },
    {
        "1_i32_i32_2_same",
        NestedStructTypes_1_i32_i32_2_same
    },
    {
        "1_i32_i32_2_i32_i32",
        NestedStructTypes_1_i32_i32_2_i32_i32
    }
};

static bake_test_suite suites[] = {
    {
        "PrimitiveTypes",
        NULL,
        NULL,
        34,
        PrimitiveTypes_testcases
    },
    {
        "StructTypes",
        NULL,
        NULL,
        11,
        StructTypes_testcases
    },
    {
        "NestedStructTypes",
        NULL,
        NULL,
        20,
        NestedStructTypes_testcases
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("meta", argc, argv, suites, 3);
}
