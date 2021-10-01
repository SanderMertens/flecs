
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
void PrimitiveTypes_struct_w_bool(void);
void PrimitiveTypes_struct_w_byte(void);
void PrimitiveTypes_struct_w_char(void);
void PrimitiveTypes_struct_w_i8(void);
void PrimitiveTypes_struct_w_i16(void);
void PrimitiveTypes_struct_w_i32(void);
void PrimitiveTypes_struct_w_i64(void);
void PrimitiveTypes_struct_w_iptr(void);
void PrimitiveTypes_struct_w_u8(void);
void PrimitiveTypes_struct_w_u16(void);
void PrimitiveTypes_struct_w_u32(void);
void PrimitiveTypes_struct_w_u64(void);
void PrimitiveTypes_struct_w_uptr(void);
void PrimitiveTypes_struct_w_float(void);
void PrimitiveTypes_struct_w_double(void);
void PrimitiveTypes_struct_w_string(void);
void PrimitiveTypes_struct_w_entity(void);

// Testsuite 'EnumTypes'
void EnumTypes_enum_1_constant(void);
void EnumTypes_enum_2_constants(void);
void EnumTypes_enum_3_constants(void);
void EnumTypes_enum_3_constants_manual_values(void);
void EnumTypes_struct_w_enum(void);

// Testsuite 'BitmaskTypes'
void BitmaskTypes_bitmask_1_constant(void);
void BitmaskTypes_bitmask_2_constants(void);
void BitmaskTypes_bitmask_3_constants(void);
void BitmaskTypes_bitmask_4_constants(void);
void BitmaskTypes_bitmask_4_constants_manual_values(void);
void BitmaskTypes_struct_w_bitmask(void);

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
void NestedStructTypes_2_lvls_bool(void);
void NestedStructTypes_2_lvls_bool_bool(void);

// Testsuite 'ArrayTypes'
void ArrayTypes_array_bool_1(void);
void ArrayTypes_array_bool_2(void);
void ArrayTypes_array_bool_3(void);
void ArrayTypes_array_bool_1_before_i32_member(void);
void ArrayTypes_array_bool_2_before_i32_member(void);
void ArrayTypes_array_bool_3_before_i32_member(void);
void ArrayTypes_array_i32_3(void);
void ArrayTypes_array_i32_3_before_i32_member(void);
void ArrayTypes_array_struct_bool_3(void);
void ArrayTypes_array_struct_bool_3_before_i32_member(void);
void ArrayTypes_array_struct_array_bool_3(void);
void ArrayTypes_2_arrays_1_bool_1_i32(void);
void ArrayTypes_2_arrays_1_i32_1_bool(void);
void ArrayTypes_2_arrays_1_i32_1_bool(void);
void ArrayTypes_8_arrays_bool_w_padded_member(void);
void ArrayTypes_standaline_array_bool_1(void);
void ArrayTypes_standaline_array_bool_2(void);
void ArrayTypes_standaline_array_bool_3(void);
void ArrayTypes_array_of_standaline_array_bool_1(void);
void ArrayTypes_array_of_standaline_array_bool_2(void);
void ArrayTypes_array_of_standaline_array_bool_3(void);

// Testsuite 'VectorTypes'
void VectorTypes_vector_bool(void);
void VectorTypes_vector_i32(void);
void VectorTypes_vector_struct(void);

// Testsuite 'Serialized'
void Serialized_primitive_constants(void);
void Serialized_ops_bool(void);
void Serialized_ops_byte(void);
void Serialized_ops_char(void);
void Serialized_ops_i8(void);
void Serialized_ops_i16(void);
void Serialized_ops_i32(void);
void Serialized_ops_i64(void);
void Serialized_ops_iptr(void);
void Serialized_ops_u8(void);
void Serialized_ops_u16(void);
void Serialized_ops_u32(void);
void Serialized_ops_u64(void);
void Serialized_ops_uptr(void);
void Serialized_ops_float(void);
void Serialized_ops_double(void);
void Serialized_ops_string(void);
void Serialized_ops_entity(void);
void Serialized_ops_struct_bool(void);
void Serialized_ops_struct_bool_bool(void);
void Serialized_ops_struct_i32(void);
void Serialized_ops_struct_i32_i32(void);
void Serialized_ops_struct_i32_bool(void);
void Serialized_ops_struct_bool_i32(void);
void Serialized_ops_nested_struct_1_bool(void);
void Serialized_ops_nested_struct_1_i32(void);
void Serialized_ops_nested_struct_1_bool_w_bool_member(void);
void Serialized_ops_nested_struct_1_bool_w_i32_member(void);
void Serialized_ops_nested_struct_1_bool_2_bool(void);
void Serialized_ops_nested_struct_1_i32_2_bool(void);
void Serialized_ops_nested_struct_1_i32_i32_2_bool_bool(void);
void Serialized_ops_nested_struct_1_bool_bool_2_i32_i32(void);
void Serialized_ops_nested_struct_1_i32_bool_2_bool_i32(void);
void Serialized_ops_nested_struct_1_bool_i32_2_i32_bool(void);
void Serialized_ops_nested_2_lvls_bool(void);
void Serialized_ops_nested_2_lvls_bool_bool(void);
void Serialized_ops_nested_2_lvls_i32_i32_w_member_before(void);
void Serialized_ops_nested_2_lvls_1_bool_i32_2_i32_bool_w_member_before(void);
void Serialized_ops_nested_2_lvls_1_i32_bool_2_bool_i32_w_member_before(void);
void Serialized_ops_struct_array_bool_1(void);
void Serialized_ops_struct_array_bool_2(void);
void Serialized_ops_struct_array_bool_3(void);
void Serialized_ops_struct_array_bool_1_w_i32_after(void);
void Serialized_ops_struct_array_bool_2_w_i32_after(void);
void Serialized_ops_struct_array_bool_3_w_i32_after(void);
void Serialized_ops_struct_array_struct_bool_3_w_i32_after(void);
void Serialized_ops_standalone_array_bool_1(void);
void Serialized_ops_standalone_array_bool_2(void);
void Serialized_ops_standalone_array_bool_3(void);
void Serialized_ops_standalone_array_bool_1_w_bool_after(void);
void Serialized_ops_standalone_array_bool_2_w_bool_after(void);
void Serialized_ops_standalone_array_bool_3_w_bool_after(void);
void Serialized_ops_vector(void);
void Serialized_ops_struct_w_vector(void);
void Serialized_ops_struct_w_vector_w_bool_before(void);
void Serialized_ops_struct_w_vector_w_bool_after(void);
void Serialized_ops_bitmask(void);
void Serialized_ops_struct_w_bitmask(void);
void Serialized_ops_enum(void);
void Serialized_ops_struct_w_enum(void);

// Testsuite 'Cursor'
void Cursor_set_bool(void);
void Cursor_set_byte(void);
void Cursor_set_char(void);
void Cursor_set_i8(void);
void Cursor_set_i16(void);
void Cursor_set_i32(void);
void Cursor_set_i64(void);
void Cursor_set_iptr(void);
void Cursor_set_u8(void);
void Cursor_set_u16(void);
void Cursor_set_u32(void);
void Cursor_set_u64(void);
void Cursor_set_uptr(void);
void Cursor_set_float(void);
void Cursor_set_double(void);
void Cursor_set_string(void);
void Cursor_set_entity(void);
void Cursor_struct_set_i32(void);
void Cursor_struct_set_i32_i32(void);
void Cursor_struct_set_i32_i32_i32(void);
void Cursor_struct_set_bool(void);
void Cursor_struct_set_bool_bool(void);
void Cursor_struct_set_bool_bool_bool(void);
void Cursor_struct_set_bool_bool_i32_bool(void);
void Cursor_struct_nested_i32(void);
void Cursor_struct_2_nested_1_i32_2_i32(void);
void Cursor_struct_2_nested_1_i32_i32_2_i32_i32(void);
void Cursor_struct_2_nested_1_i32_i32_2_i32_i32_w_padding_members(void);
void Cursor_struct_set_array_i32_3(void);
void Cursor_struct_set_2_array_i32_3(void);
void Cursor_struct_set_2_array_i32_3_w_padding_members(void);
void Cursor_struct_set_array_w_struct_i32(void);
void Cursor_struct_set_2_array_w_struct_i32(void);
void Cursor_struct_set_array_w_struct_w_array_i32(void);
void Cursor_struct_set_2_array_w_struct_w_array_i32(void);

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
    },
    {
        "struct_w_bool",
        PrimitiveTypes_struct_w_bool
    },
    {
        "struct_w_byte",
        PrimitiveTypes_struct_w_byte
    },
    {
        "struct_w_char",
        PrimitiveTypes_struct_w_char
    },
    {
        "struct_w_i8",
        PrimitiveTypes_struct_w_i8
    },
    {
        "struct_w_i16",
        PrimitiveTypes_struct_w_i16
    },
    {
        "struct_w_i32",
        PrimitiveTypes_struct_w_i32
    },
    {
        "struct_w_i64",
        PrimitiveTypes_struct_w_i64
    },
    {
        "struct_w_iptr",
        PrimitiveTypes_struct_w_iptr
    },
    {
        "struct_w_u8",
        PrimitiveTypes_struct_w_u8
    },
    {
        "struct_w_u16",
        PrimitiveTypes_struct_w_u16
    },
    {
        "struct_w_u32",
        PrimitiveTypes_struct_w_u32
    },
    {
        "struct_w_u64",
        PrimitiveTypes_struct_w_u64
    },
    {
        "struct_w_uptr",
        PrimitiveTypes_struct_w_uptr
    },
    {
        "struct_w_float",
        PrimitiveTypes_struct_w_float
    },
    {
        "struct_w_double",
        PrimitiveTypes_struct_w_double
    },
    {
        "struct_w_string",
        PrimitiveTypes_struct_w_string
    },
    {
        "struct_w_entity",
        PrimitiveTypes_struct_w_entity
    }
};

bake_test_case EnumTypes_testcases[] = {
    {
        "enum_1_constant",
        EnumTypes_enum_1_constant
    },
    {
        "enum_2_constants",
        EnumTypes_enum_2_constants
    },
    {
        "enum_3_constants",
        EnumTypes_enum_3_constants
    },
    {
        "enum_3_constants_manual_values",
        EnumTypes_enum_3_constants_manual_values
    },
    {
        "struct_w_enum",
        EnumTypes_struct_w_enum
    }
};

bake_test_case BitmaskTypes_testcases[] = {
    {
        "bitmask_1_constant",
        BitmaskTypes_bitmask_1_constant
    },
    {
        "bitmask_2_constants",
        BitmaskTypes_bitmask_2_constants
    },
    {
        "bitmask_3_constants",
        BitmaskTypes_bitmask_3_constants
    },
    {
        "bitmask_4_constants",
        BitmaskTypes_bitmask_4_constants
    },
    {
        "bitmask_4_constants_manual_values",
        BitmaskTypes_bitmask_4_constants_manual_values
    },
    {
        "struct_w_bitmask",
        BitmaskTypes_struct_w_bitmask
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
    },
    {
        "2_lvls_bool",
        NestedStructTypes_2_lvls_bool
    },
    {
        "2_lvls_bool_bool",
        NestedStructTypes_2_lvls_bool_bool
    }
};

bake_test_case ArrayTypes_testcases[] = {
    {
        "array_bool_1",
        ArrayTypes_array_bool_1
    },
    {
        "array_bool_2",
        ArrayTypes_array_bool_2
    },
    {
        "array_bool_3",
        ArrayTypes_array_bool_3
    },
    {
        "array_bool_1_before_i32_member",
        ArrayTypes_array_bool_1_before_i32_member
    },
    {
        "array_bool_2_before_i32_member",
        ArrayTypes_array_bool_2_before_i32_member
    },
    {
        "array_bool_3_before_i32_member",
        ArrayTypes_array_bool_3_before_i32_member
    },
    {
        "array_i32_3",
        ArrayTypes_array_i32_3
    },
    {
        "array_i32_3_before_i32_member",
        ArrayTypes_array_i32_3_before_i32_member
    },
    {
        "array_struct_bool_3",
        ArrayTypes_array_struct_bool_3
    },
    {
        "array_struct_bool_3_before_i32_member",
        ArrayTypes_array_struct_bool_3_before_i32_member
    },
    {
        "array_struct_array_bool_3",
        ArrayTypes_array_struct_array_bool_3
    },
    {
        "2_arrays_1_bool_1_i32",
        ArrayTypes_2_arrays_1_bool_1_i32
    },
    {
        "2_arrays_1_i32_1_bool",
        ArrayTypes_2_arrays_1_i32_1_bool
    },
    {
        "2_arrays_1_i32_1_bool",
        ArrayTypes_2_arrays_1_i32_1_bool
    },
    {
        "8_arrays_bool_w_padded_member",
        ArrayTypes_8_arrays_bool_w_padded_member
    },
    {
        "standaline_array_bool_1",
        ArrayTypes_standaline_array_bool_1
    },
    {
        "standaline_array_bool_2",
        ArrayTypes_standaline_array_bool_2
    },
    {
        "standaline_array_bool_3",
        ArrayTypes_standaline_array_bool_3
    },
    {
        "array_of_standaline_array_bool_1",
        ArrayTypes_array_of_standaline_array_bool_1
    },
    {
        "array_of_standaline_array_bool_2",
        ArrayTypes_array_of_standaline_array_bool_2
    },
    {
        "array_of_standaline_array_bool_3",
        ArrayTypes_array_of_standaline_array_bool_3
    }
};

bake_test_case VectorTypes_testcases[] = {
    {
        "vector_bool",
        VectorTypes_vector_bool
    },
    {
        "vector_i32",
        VectorTypes_vector_i32
    },
    {
        "vector_struct",
        VectorTypes_vector_struct
    }
};

bake_test_case Serialized_testcases[] = {
    {
        "primitive_constants",
        Serialized_primitive_constants
    },
    {
        "ops_bool",
        Serialized_ops_bool
    },
    {
        "ops_byte",
        Serialized_ops_byte
    },
    {
        "ops_char",
        Serialized_ops_char
    },
    {
        "ops_i8",
        Serialized_ops_i8
    },
    {
        "ops_i16",
        Serialized_ops_i16
    },
    {
        "ops_i32",
        Serialized_ops_i32
    },
    {
        "ops_i64",
        Serialized_ops_i64
    },
    {
        "ops_iptr",
        Serialized_ops_iptr
    },
    {
        "ops_u8",
        Serialized_ops_u8
    },
    {
        "ops_u16",
        Serialized_ops_u16
    },
    {
        "ops_u32",
        Serialized_ops_u32
    },
    {
        "ops_u64",
        Serialized_ops_u64
    },
    {
        "ops_uptr",
        Serialized_ops_uptr
    },
    {
        "ops_float",
        Serialized_ops_float
    },
    {
        "ops_double",
        Serialized_ops_double
    },
    {
        "ops_string",
        Serialized_ops_string
    },
    {
        "ops_entity",
        Serialized_ops_entity
    },
    {
        "ops_struct_bool",
        Serialized_ops_struct_bool
    },
    {
        "ops_struct_bool_bool",
        Serialized_ops_struct_bool_bool
    },
    {
        "ops_struct_i32",
        Serialized_ops_struct_i32
    },
    {
        "ops_struct_i32_i32",
        Serialized_ops_struct_i32_i32
    },
    {
        "ops_struct_i32_bool",
        Serialized_ops_struct_i32_bool
    },
    {
        "ops_struct_bool_i32",
        Serialized_ops_struct_bool_i32
    },
    {
        "ops_nested_struct_1_bool",
        Serialized_ops_nested_struct_1_bool
    },
    {
        "ops_nested_struct_1_i32",
        Serialized_ops_nested_struct_1_i32
    },
    {
        "ops_nested_struct_1_bool_w_bool_member",
        Serialized_ops_nested_struct_1_bool_w_bool_member
    },
    {
        "ops_nested_struct_1_bool_w_i32_member",
        Serialized_ops_nested_struct_1_bool_w_i32_member
    },
    {
        "ops_nested_struct_1_bool_2_bool",
        Serialized_ops_nested_struct_1_bool_2_bool
    },
    {
        "ops_nested_struct_1_i32_2_bool",
        Serialized_ops_nested_struct_1_i32_2_bool
    },
    {
        "ops_nested_struct_1_i32_i32_2_bool_bool",
        Serialized_ops_nested_struct_1_i32_i32_2_bool_bool
    },
    {
        "ops_nested_struct_1_bool_bool_2_i32_i32",
        Serialized_ops_nested_struct_1_bool_bool_2_i32_i32
    },
    {
        "ops_nested_struct_1_i32_bool_2_bool_i32",
        Serialized_ops_nested_struct_1_i32_bool_2_bool_i32
    },
    {
        "ops_nested_struct_1_bool_i32_2_i32_bool",
        Serialized_ops_nested_struct_1_bool_i32_2_i32_bool
    },
    {
        "ops_nested_2_lvls_bool",
        Serialized_ops_nested_2_lvls_bool
    },
    {
        "ops_nested_2_lvls_bool_bool",
        Serialized_ops_nested_2_lvls_bool_bool
    },
    {
        "ops_nested_2_lvls_i32_i32_w_member_before",
        Serialized_ops_nested_2_lvls_i32_i32_w_member_before
    },
    {
        "ops_nested_2_lvls_1_bool_i32_2_i32_bool_w_member_before",
        Serialized_ops_nested_2_lvls_1_bool_i32_2_i32_bool_w_member_before
    },
    {
        "ops_nested_2_lvls_1_i32_bool_2_bool_i32_w_member_before",
        Serialized_ops_nested_2_lvls_1_i32_bool_2_bool_i32_w_member_before
    },
    {
        "ops_struct_array_bool_1",
        Serialized_ops_struct_array_bool_1
    },
    {
        "ops_struct_array_bool_2",
        Serialized_ops_struct_array_bool_2
    },
    {
        "ops_struct_array_bool_3",
        Serialized_ops_struct_array_bool_3
    },
    {
        "ops_struct_array_bool_1_w_i32_after",
        Serialized_ops_struct_array_bool_1_w_i32_after
    },
    {
        "ops_struct_array_bool_2_w_i32_after",
        Serialized_ops_struct_array_bool_2_w_i32_after
    },
    {
        "ops_struct_array_bool_3_w_i32_after",
        Serialized_ops_struct_array_bool_3_w_i32_after
    },
    {
        "ops_struct_array_struct_bool_3_w_i32_after",
        Serialized_ops_struct_array_struct_bool_3_w_i32_after
    },
    {
        "ops_standalone_array_bool_1",
        Serialized_ops_standalone_array_bool_1
    },
    {
        "ops_standalone_array_bool_2",
        Serialized_ops_standalone_array_bool_2
    },
    {
        "ops_standalone_array_bool_3",
        Serialized_ops_standalone_array_bool_3
    },
    {
        "ops_standalone_array_bool_1_w_bool_after",
        Serialized_ops_standalone_array_bool_1_w_bool_after
    },
    {
        "ops_standalone_array_bool_2_w_bool_after",
        Serialized_ops_standalone_array_bool_2_w_bool_after
    },
    {
        "ops_standalone_array_bool_3_w_bool_after",
        Serialized_ops_standalone_array_bool_3_w_bool_after
    },
    {
        "ops_vector",
        Serialized_ops_vector
    },
    {
        "ops_struct_w_vector",
        Serialized_ops_struct_w_vector
    },
    {
        "ops_struct_w_vector_w_bool_before",
        Serialized_ops_struct_w_vector_w_bool_before
    },
    {
        "ops_struct_w_vector_w_bool_after",
        Serialized_ops_struct_w_vector_w_bool_after
    },
    {
        "ops_bitmask",
        Serialized_ops_bitmask
    },
    {
        "ops_struct_w_bitmask",
        Serialized_ops_struct_w_bitmask
    },
    {
        "ops_enum",
        Serialized_ops_enum
    },
    {
        "ops_struct_w_enum",
        Serialized_ops_struct_w_enum
    }
};

bake_test_case Cursor_testcases[] = {
    {
        "set_bool",
        Cursor_set_bool
    },
    {
        "set_byte",
        Cursor_set_byte
    },
    {
        "set_char",
        Cursor_set_char
    },
    {
        "set_i8",
        Cursor_set_i8
    },
    {
        "set_i16",
        Cursor_set_i16
    },
    {
        "set_i32",
        Cursor_set_i32
    },
    {
        "set_i64",
        Cursor_set_i64
    },
    {
        "set_iptr",
        Cursor_set_iptr
    },
    {
        "set_u8",
        Cursor_set_u8
    },
    {
        "set_u16",
        Cursor_set_u16
    },
    {
        "set_u32",
        Cursor_set_u32
    },
    {
        "set_u64",
        Cursor_set_u64
    },
    {
        "set_uptr",
        Cursor_set_uptr
    },
    {
        "set_float",
        Cursor_set_float
    },
    {
        "set_double",
        Cursor_set_double
    },
    {
        "set_string",
        Cursor_set_string
    },
    {
        "set_entity",
        Cursor_set_entity
    },
    {
        "struct_set_i32",
        Cursor_struct_set_i32
    },
    {
        "struct_set_i32_i32",
        Cursor_struct_set_i32_i32
    },
    {
        "struct_set_i32_i32_i32",
        Cursor_struct_set_i32_i32_i32
    },
    {
        "struct_set_bool",
        Cursor_struct_set_bool
    },
    {
        "struct_set_bool_bool",
        Cursor_struct_set_bool_bool
    },
    {
        "struct_set_bool_bool_bool",
        Cursor_struct_set_bool_bool_bool
    },
    {
        "struct_set_bool_bool_i32_bool",
        Cursor_struct_set_bool_bool_i32_bool
    },
    {
        "struct_nested_i32",
        Cursor_struct_nested_i32
    },
    {
        "struct_2_nested_1_i32_2_i32",
        Cursor_struct_2_nested_1_i32_2_i32
    },
    {
        "struct_2_nested_1_i32_i32_2_i32_i32",
        Cursor_struct_2_nested_1_i32_i32_2_i32_i32
    },
    {
        "struct_2_nested_1_i32_i32_2_i32_i32_w_padding_members",
        Cursor_struct_2_nested_1_i32_i32_2_i32_i32_w_padding_members
    },
    {
        "struct_set_array_i32_3",
        Cursor_struct_set_array_i32_3
    },
    {
        "struct_set_2_array_i32_3",
        Cursor_struct_set_2_array_i32_3
    },
    {
        "struct_set_2_array_i32_3_w_padding_members",
        Cursor_struct_set_2_array_i32_3_w_padding_members
    },
    {
        "struct_set_array_w_struct_i32",
        Cursor_struct_set_array_w_struct_i32
    },
    {
        "struct_set_2_array_w_struct_i32",
        Cursor_struct_set_2_array_w_struct_i32
    },
    {
        "struct_set_array_w_struct_w_array_i32",
        Cursor_struct_set_array_w_struct_w_array_i32
    },
    {
        "struct_set_2_array_w_struct_w_array_i32",
        Cursor_struct_set_2_array_w_struct_w_array_i32
    }
};

static bake_test_suite suites[] = {
    {
        "PrimitiveTypes",
        NULL,
        NULL,
        51,
        PrimitiveTypes_testcases
    },
    {
        "EnumTypes",
        NULL,
        NULL,
        5,
        EnumTypes_testcases
    },
    {
        "BitmaskTypes",
        NULL,
        NULL,
        6,
        BitmaskTypes_testcases
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
        22,
        NestedStructTypes_testcases
    },
    {
        "ArrayTypes",
        NULL,
        NULL,
        21,
        ArrayTypes_testcases
    },
    {
        "VectorTypes",
        NULL,
        NULL,
        3,
        VectorTypes_testcases
    },
    {
        "Serialized",
        NULL,
        NULL,
        60,
        Serialized_testcases
    },
    {
        "Cursor",
        NULL,
        NULL,
        35,
        Cursor_testcases
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("meta", argc, argv, suites, 9);
}
