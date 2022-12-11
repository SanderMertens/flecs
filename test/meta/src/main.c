
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
void PrimitiveTypes_primitive_init(void);
void PrimitiveTypes_primitive_w_short_notation(void);

// Testsuite 'EnumTypes'
void EnumTypes_enum_1_constant(void);
void EnumTypes_enum_2_constants(void);
void EnumTypes_enum_3_constants(void);
void EnumTypes_enum_3_constants_manual_values(void);
void EnumTypes_struct_w_enum(void);
void EnumTypes_zero_initialized(void);
void EnumTypes_enum_relation(void);
void EnumTypes_enum_w_short_notation(void);

// Testsuite 'BitmaskTypes'
void BitmaskTypes_bitmask_1_constant(void);
void BitmaskTypes_bitmask_2_constants(void);
void BitmaskTypes_bitmask_3_constants(void);
void BitmaskTypes_bitmask_4_constants(void);
void BitmaskTypes_bitmask_4_constants_manual_values(void);
void BitmaskTypes_struct_w_bitmask(void);
void BitmaskTypes_bitmask_w_short_notation(void);

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
void StructTypes_incomplete_member(void);
void StructTypes_partial_type(void);
void StructTypes_partial_type_custom_offset(void);
void StructTypes_struct_w_short_notation(void);

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
void ArrayTypes_array_w_short_notation(void);

// Testsuite 'VectorTypes'
void VectorTypes_vector_bool(void);
void VectorTypes_vector_i32(void);
void VectorTypes_vector_struct(void);
void VectorTypes_vector_w_short_notation(void);

// Testsuite 'Units'
void Units_member_w_unit(void);
void Units_member_w_unit_type(void);
void Units_cursor_get_unit(void);
void Units_cursor_get_unit_type(void);
void Units_unit_w_quantity(void);
void Units_unit_w_self_quantity(void);
void Units_unit_w_self_quantity_after_init(void);
void Units_unit_w_derived(void);
void Units_unit_w_over(void);
void Units_unit_w_prefix(void);
void Units_member_w_invalid_unit(void);
void Units_unit_w_invalid_quantity(void);
void Units_unit_w_invalid_derived(void);
void Units_unit_w_invalid_symbol_w_over(void);
void Units_unit_w_invalid_symbol_w_prefix(void);
void Units_unit_w_invalid_over(void);
void Units_unit_w_over_no_derived(void);
void Units_define_twice(void);
void Units_define_twice_different_quantity(void);
void Units_define_twice_remove_quantity(void);
void Units_set_unit(void);
void Units_set_unit_w_derived(void);
void Units_set_unit_w_over(void);
void Units_set_unit_w_prefix(void);
void Units_builtin_units(void);
void Units_unit_w_short_notation(void);
void Units_unit_prefix_w_short_notation(void);
void Units_quantity_w_short_notation(void);

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
void Cursor_set_string_literal(void);
void Cursor_set_string_to_null(void);
void Cursor_set_entity(void);
void Cursor_set_entity_to_number(void);
void Cursor_set_entity_to_0(void);
void Cursor_set_enum(void);
void Cursor_set_bitmask(void);
void Cursor_set_signed_as_unsigned(void);
void Cursor_set_unsigned_as_signed(void);
void Cursor_set_signed_as_unsigned_out_of_range(void);
void Cursor_set_unsigned_as_signed_out_of_range(void);
void Cursor_set_string_to_null_as_signed(void);
void Cursor_set_string_to_null_as_unsigned(void);
void Cursor_set_entity_as_signed(void);
void Cursor_set_entity_as_unsigned(void);
void Cursor_set_entity_as_signed_out_of_range(void);
void Cursor_set_str_to_bool(void);
void Cursor_set_str_to_char(void);
void Cursor_set_str_literal_to_char(void);
void Cursor_set_str_to_i8(void);
void Cursor_set_str_to_i16(void);
void Cursor_set_str_to_i32(void);
void Cursor_set_str_to_i64(void);
void Cursor_set_str_to_f32(void);
void Cursor_set_str_to_f64(void);
void Cursor_set_str_to_entity(void);
void Cursor_set_str_to_invalid_bool(void);
void Cursor_set_str_to_invalid_entity(void);
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
void Cursor_struct_member_i32(void);
void Cursor_struct_member_i32_i32(void);
void Cursor_struct_member_i32_i32_reverse(void);
void Cursor_struct_member_nested_i32_i32(void);
void Cursor_struct_member_2_nested_i32_i32(void);
void Cursor_struct_member_2_nested_i32_i32_reverse(void);
void Cursor_struct_w_array_type_i32_i32(void);
void Cursor_struct_w_array_type_struct(void);
void Cursor_struct_w_2_array_type_i32_i32(void);
void Cursor_struct_w_2_array_type_struct(void);
void Cursor_array_move_primitive(void);
void Cursor_array_move_struct(void);
void Cursor_array_move_out_of_range(void);

// Testsuite 'DeserializeFromExpr'
void DeserializeFromExpr_bool(void);
void DeserializeFromExpr_byte(void);
void DeserializeFromExpr_char(void);
void DeserializeFromExpr_char_literal(void);
void DeserializeFromExpr_i8(void);
void DeserializeFromExpr_i16(void);
void DeserializeFromExpr_i32(void);
void DeserializeFromExpr_i64(void);
void DeserializeFromExpr_iptr(void);
void DeserializeFromExpr_u8(void);
void DeserializeFromExpr_u16(void);
void DeserializeFromExpr_u32(void);
void DeserializeFromExpr_u64(void);
void DeserializeFromExpr_uptr(void);
void DeserializeFromExpr_float(void);
void DeserializeFromExpr_double(void);
void DeserializeFromExpr_negative_int(void);
void DeserializeFromExpr_negative_float(void);
void DeserializeFromExpr_string(void);
void DeserializeFromExpr_entity(void);
void DeserializeFromExpr_enum(void);
void DeserializeFromExpr_bitmask(void);
void DeserializeFromExpr_struct_enum(void);
void DeserializeFromExpr_struct_bitmask(void);
void DeserializeFromExpr_struct_i32(void);
void DeserializeFromExpr_struct_i32_neg(void);
void DeserializeFromExpr_struct_i32_i32(void);
void DeserializeFromExpr_struct_entity(void);
void DeserializeFromExpr_struct_nested_i32(void);
void DeserializeFromExpr_struct_nested_i32_i32(void);
void DeserializeFromExpr_struct_2_nested_i32_i32(void);
void DeserializeFromExpr_struct_member_i32(void);
void DeserializeFromExpr_struct_member_i32_neg(void);
void DeserializeFromExpr_struct_member_i32_i32(void);
void DeserializeFromExpr_struct_member_nested_i32(void);
void DeserializeFromExpr_struct_member_nested_i32_i32(void);
void DeserializeFromExpr_struct_member_2_nested_i32_i32(void);
void DeserializeFromExpr_struct_member_2_nested_i32_i32_reverse(void);
void DeserializeFromExpr_struct_i32_array_3(void);
void DeserializeFromExpr_struct_struct_i32_array_3(void);
void DeserializeFromExpr_struct_struct_i32_i32_array_3(void);
void DeserializeFromExpr_struct_w_array_type_i32_i32(void);
void DeserializeFromExpr_struct_w_array_type_struct(void);
void DeserializeFromExpr_struct_w_2_array_type_i32_i32(void);
void DeserializeFromExpr_struct_w_2_array_type_struct(void);
void DeserializeFromExpr_discover_type_int(void);
void DeserializeFromExpr_discover_type_negative_int(void);
void DeserializeFromExpr_discover_type_float(void);
void DeserializeFromExpr_discover_type_negative_float(void);
void DeserializeFromExpr_discover_type_string(void);
void DeserializeFromExpr_discover_type_multiline_string(void);
void DeserializeFromExpr_discover_type_entity(void);
void DeserializeFromExpr_discover_type_bool(void);
void DeserializeFromExpr_discover_type_unknown(void);
void DeserializeFromExpr_discover_type_invalid(void);

// Testsuite 'SerializeToExpr'
void SerializeToExpr_bool(void);
void SerializeToExpr_byte(void);
void SerializeToExpr_char(void);
void SerializeToExpr_i8(void);
void SerializeToExpr_i16(void);
void SerializeToExpr_i32(void);
void SerializeToExpr_i64(void);
void SerializeToExpr_iptr(void);
void SerializeToExpr_u8(void);
void SerializeToExpr_u16(void);
void SerializeToExpr_u32(void);
void SerializeToExpr_u64(void);
void SerializeToExpr_uptr(void);
void SerializeToExpr_float(void);
void SerializeToExpr_double(void);
void SerializeToExpr_string(void);
void SerializeToExpr_entity(void);
void SerializeToExpr_enum(void);
void SerializeToExpr_bitmask(void);
void SerializeToExpr_float_nan(void);
void SerializeToExpr_float_inf(void);
void SerializeToExpr_double_nan(void);
void SerializeToExpr_double_inf(void);
void SerializeToExpr_struct_enum(void);
void SerializeToExpr_struct_bitmask(void);
void SerializeToExpr_struct_i32(void);
void SerializeToExpr_struct_i32_i32(void);
void SerializeToExpr_struct_entity(void);
void SerializeToExpr_array_i32_3(void);
void SerializeToExpr_array_struct_i32_i32(void);
void SerializeToExpr_array_array_i32_3(void);
void SerializeToExpr_entity_entity_after_float(void);
void SerializeToExpr_struct_nested_i32(void);
void SerializeToExpr_struct_nested_i32_i32(void);
void SerializeToExpr_struct_2_nested_i32_i32(void);
void SerializeToExpr_struct_struct_i32_array_3(void);
void SerializeToExpr_struct_struct_i32_i32_array_3(void);
void SerializeToExpr_struct_w_array_type_i32_i32(void);
void SerializeToExpr_struct_w_array_type_struct(void);
void SerializeToExpr_struct_w_2_array_type_i32_i32(void);
void SerializeToExpr_struct_w_2_array_type_struct(void);
void SerializeToExpr_struct_partial(void);
void SerializeToExpr_escape_simple_string(void);
void SerializeToExpr_escape_newline(void);
void SerializeToExpr_escape_2_newlines(void);
void SerializeToExpr_escape_string_w_trailing_newline(void);
void SerializeToExpr_escape_string_w_2_trailing_newlines(void);
void SerializeToExpr_escape_string_w_delim(void);

// Testsuite 'DeserializeFromJson'
void DeserializeFromJson_struct_bool(void);
void DeserializeFromJson_struct_byte(void);
void DeserializeFromJson_struct_char(void);
void DeserializeFromJson_struct_char_literal(void);
void DeserializeFromJson_struct_i8(void);
void DeserializeFromJson_struct_i16(void);
void DeserializeFromJson_struct_i32(void);
void DeserializeFromJson_struct_i64(void);
void DeserializeFromJson_struct_iptr(void);
void DeserializeFromJson_struct_u8(void);
void DeserializeFromJson_struct_u16(void);
void DeserializeFromJson_struct_u32(void);
void DeserializeFromJson_struct_u64(void);
void DeserializeFromJson_struct_uptr(void);
void DeserializeFromJson_struct_float(void);
void DeserializeFromJson_struct_double(void);
void DeserializeFromJson_struct_negative_int(void);
void DeserializeFromJson_struct_negative_float(void);
void DeserializeFromJson_struct_string(void);
void DeserializeFromJson_struct_entity(void);
void DeserializeFromJson_struct_enum(void);
void DeserializeFromJson_struct_bitmask(void);
void DeserializeFromJson_struct_i32_i32(void);
void DeserializeFromJson_struct_nested_i32(void);
void DeserializeFromJson_struct_nested_i32_i32(void);
void DeserializeFromJson_struct_2_nested_i32_i32(void);
void DeserializeFromJson_struct_i32_array_3(void);
void DeserializeFromJson_struct_struct_i32_array_3(void);
void DeserializeFromJson_struct_struct_i32_i32_array_3(void);
void DeserializeFromJson_struct_w_array_type_i32_i32(void);
void DeserializeFromJson_struct_w_2_array_type_i32_i32(void);
void DeserializeFromJson_struct_w_nested_member_i32(void);
void DeserializeFromJson_struct_w_2_nested_members_i32(void);
void DeserializeFromJson_struct_w_nested_members_struct(void);
void DeserializeFromJson_struct_w_2_nested_members_struct(void);
void DeserializeFromJson_deser_entity_1_component_1_member(void);
void DeserializeFromJson_deser_entity_1_component_1_member_w_spaces(void);
void DeserializeFromJson_deser_entity_1_component_2_members(void);
void DeserializeFromJson_deser_entity_2_components(void);
void DeserializeFromJson_deser_entity_1_component_composite_member(void);
void DeserializeFromJson_deser_entity_1_component_nested_member(void);
void DeserializeFromJson_deser_entity_1_pair(void);
void DeserializeFromJson_deser_entity_2_pairs(void);

// Testsuite 'SerializeToJson'
void SerializeToJson_struct_bool(void);
void SerializeToJson_struct_byte(void);
void SerializeToJson_struct_char(void);
void SerializeToJson_struct_i8(void);
void SerializeToJson_struct_i16(void);
void SerializeToJson_struct_i32(void);
void SerializeToJson_struct_i64(void);
void SerializeToJson_struct_iptr(void);
void SerializeToJson_struct_u8(void);
void SerializeToJson_struct_u16(void);
void SerializeToJson_struct_u32(void);
void SerializeToJson_struct_u64(void);
void SerializeToJson_struct_uptr(void);
void SerializeToJson_struct_float(void);
void SerializeToJson_struct_double(void);
void SerializeToJson_struct_string(void);
void SerializeToJson_struct_entity(void);
void SerializeToJson_struct_entity_after_float(void);
void SerializeToJson_struct_float_nan(void);
void SerializeToJson_struct_float_inf(void);
void SerializeToJson_struct_double_nan(void);
void SerializeToJson_struct_double_inf(void);
void SerializeToJson_struct_double_large(void);
void SerializeToJson_struct_double_large_e(void);
void SerializeToJson_struct_enum(void);
void SerializeToJson_struct_bitmask(void);
void SerializeToJson_struct_i32_i32(void);
void SerializeToJson_struct_nested_i32(void);
void SerializeToJson_struct_nested_i32_i32(void);
void SerializeToJson_struct_2_nested_i32_i32(void);
void SerializeToJson_struct_i32_array_3(void);
void SerializeToJson_struct_struct_i32_array_3(void);
void SerializeToJson_struct_struct_i32_i32_array_3(void);
void SerializeToJson_struct_w_array_type_i32_i32(void);
void SerializeToJson_struct_w_2_array_type_i32_i32(void);
void SerializeToJson_struct_partial(void);
void SerializeToJson_array_i32_3(void);
void SerializeToJson_array_struct_i32_i32(void);
void SerializeToJson_array_array_i32_3(void);
void SerializeToJson_serialize_entity_empty(void);
void SerializeToJson_serialize_entity_w_name(void);
void SerializeToJson_serialize_entity_w_name_1_tag(void);
void SerializeToJson_serialize_entity_w_name_2_tags(void);
void SerializeToJson_serialize_entity_w_name_1_pair(void);
void SerializeToJson_serialize_entity_w_base(void);
void SerializeToJson_serialize_entity_w_base_override(void);
void SerializeToJson_serialize_entity_w_2_base(void);
void SerializeToJson_serialize_entity_w_nested_base(void);
void SerializeToJson_serialize_entity_w_1_component(void);
void SerializeToJson_serialize_entity_w_2_components(void);
void SerializeToJson_serialize_entity_w_primitive_component(void);
void SerializeToJson_serialize_entity_w_enum_component(void);
void SerializeToJson_serialize_entity_w_struct_and_enum_component(void);
void SerializeToJson_serialize_entity_w_invalid_enum_component(void);
void SerializeToJson_serialize_entity_w_type_info(void);
void SerializeToJson_serialize_entity_w_type_info_unit(void);
void SerializeToJson_serialize_entity_w_type_info_unit_quantity(void);
void SerializeToJson_serialize_entity_w_type_info_unit_over(void);
void SerializeToJson_serialize_entity_wo_private(void);
void SerializeToJson_serialize_entity_w_private(void);
void SerializeToJson_serialize_entity_w_label(void);
void SerializeToJson_serialize_entity_w_label_no_name(void);
void SerializeToJson_serialize_entity_w_id_labels(void);
void SerializeToJson_serialize_entity_w_brief(void);
void SerializeToJson_serialize_entity_w_brief_no_brief(void);
void SerializeToJson_serialize_entity_w_link(void);
void SerializeToJson_serialize_entity_w_link_no_link(void);
void SerializeToJson_serialize_entity_w_meta_ids(void);
void SerializeToJson_serialize_entity_color(void);
void SerializeToJson_serialize_entity_w_doc_w_quotes(void);
void SerializeToJson_serialize_entity_union_relationship(void);
void SerializeToJson_serialize_entity_union_relationship_w_labels(void);
void SerializeToJson_serialize_entity_union_relationship_invalid_entity(void);
void SerializeToJson_serialize_entity_union_relationship_invalid_entity_w_labels(void);
void SerializeToJson_serialize_entity_w_union_property(void);
void SerializeToJson_serialize_entity_w_union_property(void);
void SerializeToJson_serialize_entity_from_core(void);
void SerializeToJson_serialize_iterator_1_comps_empty(void);
void SerializeToJson_serialize_iterator_1_comps_2_ents_same_table(void);
void SerializeToJson_serialize_iterator_1_tag_2_ents_same_table(void);
void SerializeToJson_serialize_iterator_2_comps_2_ents_same_table(void);
void SerializeToJson_serialize_iterator_1_tag_1_comp_2_ents_same_table(void);
void SerializeToJson_serialize_iterator_1_tag_1_comp_4_ents_two_tables(void);
void SerializeToJson_serialize_iterator_2_comps_1_owned_2_ents(void);
void SerializeToJson_serialize_iterator_w_pair_wildcard(void);
void SerializeToJson_serialize_iterator_w_var(void);
void SerializeToJson_serialize_iterator_w_2_vars(void);
void SerializeToJson_serialize_iterator_type_info_1_tags(void);
void SerializeToJson_serialize_iterator_type_info_2_tags(void);
void SerializeToJson_serialize_iterator_type_info_1_component(void);
void SerializeToJson_serialize_iterator_type_info_2_components(void);
void SerializeToJson_serialize_iterator_type_info_1_struct(void);
void SerializeToJson_serialize_iterator_type_info_1_component_1_struct(void);
void SerializeToJson_serialize_iterator_type_info_2_structs(void);
void SerializeToJson_serialize_iterator_type_info_w_unit(void);
void SerializeToJson_serialize_iterator_type_info_w_unit_quantity(void);
void SerializeToJson_serialize_iterator_type_info_w_unit_over(void);
void SerializeToJson_serialize_iterator_w_entity_label(void);
void SerializeToJson_serialize_iterator_w_var_labels(void);
void SerializeToJson_serialize_iterator_w_var_component(void);
void SerializeToJson_serialize_iterator_w_optional_tag(void);
void SerializeToJson_serialize_iterator_w_optional_component(void);
void SerializeToJson_serialize_iterator_w_optional_reflected_component(void);
void SerializeToJson_serialize_iterator_w_inout_filter_tag(void);
void SerializeToJson_serialize_iterator_w_inout_filter_component(void);
void SerializeToJson_serialize_iterator_w_inout_filter_reflected_component(void);
void SerializeToJson_serialize_iterator_w_inout_out_tag(void);
void SerializeToJson_serialize_iterator_w_inout_out_component(void);
void SerializeToJson_serialize_iterator_w_inout_out_reflected_component(void);
void SerializeToJson_serialize_iterator_component_from_var(void);
void SerializeToJson_serialize_iterator_color(void);
void SerializeToJson_serialize_iterator_ids(void);
void SerializeToJson_serialize_iterator_ids_2_entities(void);
void SerializeToJson_serialize_iterator_variable_ids(void);
void SerializeToJson_serialize_iterator_variable_ids_2_entities(void);
void SerializeToJson_serialize_paged_iterator(void);
void SerializeToJson_serialize_paged_iterator_w_optional_component(void);
void SerializeToJson_serialize_paged_iterator_w_optional_tag(void);
void SerializeToJson_serialize_paged_iterator_w_vars(void);

// Testsuite 'SerializeTypeInfoToJson'
void SerializeTypeInfoToJson_bool(void);
void SerializeTypeInfoToJson_byte(void);
void SerializeTypeInfoToJson_char(void);
void SerializeTypeInfoToJson_i8(void);
void SerializeTypeInfoToJson_i16(void);
void SerializeTypeInfoToJson_i32(void);
void SerializeTypeInfoToJson_i64(void);
void SerializeTypeInfoToJson_iptr(void);
void SerializeTypeInfoToJson_u8(void);
void SerializeTypeInfoToJson_u16(void);
void SerializeTypeInfoToJson_u32(void);
void SerializeTypeInfoToJson_u64(void);
void SerializeTypeInfoToJson_uptr(void);
void SerializeTypeInfoToJson_float(void);
void SerializeTypeInfoToJson_double(void);
void SerializeTypeInfoToJson_string(void);
void SerializeTypeInfoToJson_entity(void);
void SerializeTypeInfoToJson_enum(void);
void SerializeTypeInfoToJson_bitmask(void);
void SerializeTypeInfoToJson_struct(void);
void SerializeTypeInfoToJson_nested_struct(void);
void SerializeTypeInfoToJson_struct_array_i32_2(void);
void SerializeTypeInfoToJson_struct_array_struct_2(void);
void SerializeTypeInfoToJson_struct_array_type(void);

// Testsuite 'MetaUtils'
void MetaUtils_struct_w_2_i32(void);
void MetaUtils_struct_w_2_bool(void);
void MetaUtils_struct_w_2_char(void);
void MetaUtils_struct_w_2_string(void);
void MetaUtils_struct_w_2_f32(void);
void MetaUtils_struct_w_2_f64(void);
void MetaUtils_struct_w_3_enum(void);
void MetaUtils_struct_w_3_enum_multiline(void);
void MetaUtils_struct_w_3_enum_w_assignment(void);
void MetaUtils_struct_w_4_bitmask(void);
void MetaUtils_struct_w_3_entities(void);
void MetaUtils_struct_w_2_array_3_i32(void);
void MetaUtils_struct_w_nested(void);
void MetaUtils_enum_nospace(void);
void MetaUtils_struct_nospace(void);
void MetaUtils_identifier_w_underscore(void);
void MetaUtils_struct_w_ptr(void);

// Testsuite 'Vars'
void Vars_declare_1_var(void);
void Vars_declare_2_vars(void);
void Vars_declare_vars_nested_scope(void);
void Vars_declare_vars_2_scopes(void);
void Vars_redeclare_var(void);
void Vars_i32_expr_w_i32_var(void);
void Vars_i32_expr_w_f32_var(void);
void Vars_i32_expr_w_string_var(void);
void Vars_string_expr_w_string_var(void);
void Vars_struct_expr_w_i32_vars(void);
void Vars_struct_expr_w_struct_var(void);
void Vars_nested_struct_expr_w_struct_var(void);
void Vars_declare_w_value(void);
void Vars_redeclare_in_scope(void);

// Testsuite 'DeserExprOperators'
void DeserExprOperators_add_2_int_literals(void);
void DeserExprOperators_add_2_int_literals_twice(void);
void DeserExprOperators_sub_2_int_literals(void);
void DeserExprOperators_mul_2_int_literals(void);
void DeserExprOperators_div_2_int_literals(void);
void DeserExprOperators_add_3_int_literals(void);
void DeserExprOperators_add_3_int_literals_twice(void);
void DeserExprOperators_sub_3_int_literals(void);
void DeserExprOperators_mul_3_int_literals(void);
void DeserExprOperators_div_3_int_literals(void);
void DeserExprOperators_int_to_bool(void);
void DeserExprOperators_add_mul_3_int_literals(void);
void DeserExprOperators_sub_mul_3_int_literals(void);
void DeserExprOperators_div_mul_3_int_literals(void);
void DeserExprOperators_add_div_3_int_literals(void);
void DeserExprOperators_sub_div_3_int_literals(void);
void DeserExprOperators_mul_div_3_int_literals(void);
void DeserExprOperators_mul_add_mul_add_int_literals(void);
void DeserExprOperators_mul_sub_mul_sub_int_literals(void);
void DeserExprOperators_mul_div_mul_div_int_literals(void);
void DeserExprOperators_div_add_div_add_int_literals(void);
void DeserExprOperators_div_sub_div_sub_int_literals(void);
void DeserExprOperators_div_sub_div_mul_int_literals(void);
void DeserExprOperators_div_mul_div_mul_int_literals(void);
void DeserExprOperators_add_2_flt_literals(void);
void DeserExprOperators_sub_2_flt_literals(void);
void DeserExprOperators_mul_2_flt_literals(void);
void DeserExprOperators_div_2_flt_literals(void);
void DeserExprOperators_add_2_int_neg_literals(void);
void DeserExprOperators_sub_2_int_neg_literals(void);
void DeserExprOperators_mul_2_int_neg_literals(void);
void DeserExprOperators_div_2_int_neg_literals(void);
void DeserExprOperators_mul_lparen_add_add_rparen_int_literals(void);
void DeserExprOperators_mul_lparen_add_add_add_rparen_int_literals(void);
void DeserExprOperators_mul_lparen_add_add_rparen_add_int_literals(void);
void DeserExprOperators_lparen_add_add_rparen_mul_int_literals(void);
void DeserExprOperators_lparen_add_add_add_rparen_mul_int_literals(void);
void DeserExprOperators_double_paren_add_add(void);
void DeserExprOperators_double_paren_literal(void);
void DeserExprOperators_lparen_add_add_rparen_mul_lparen_add_add_rparen(void);
void DeserExprOperators_float_result_add_2_int_literals(void);
void DeserExprOperators_struct_result_add_2_int_literals(void);
void DeserExprOperators_struct_result_add_2_2_fields_int_literals(void);
void DeserExprOperators_struct_result_add_3_int_literals(void);
void DeserExprOperators_struct_result_lparen_int_rparen(void);
void DeserExprOperators_add_to_var(void);
void DeserExprOperators_add_var_to(void);
void DeserExprOperators_bool_cond_and_bool(void);
void DeserExprOperators_bool_cond_or_bool(void);
void DeserExprOperators_int_cond_and_int(void);
void DeserExprOperators_int_cond_or_int(void);
void DeserExprOperators_bool_cond_and_int(void);
void DeserExprOperators_int_cond_and_bool(void);
void DeserExprOperators_bool_cond_or_int(void);
void DeserExprOperators_int_cond_or_bool(void);
void DeserExprOperators_cond_eq_bool(void);
void DeserExprOperators_cond_eq_int(void);
void DeserExprOperators_cond_neq_bool(void);
void DeserExprOperators_cond_neq_int(void);
void DeserExprOperators_cond_eq_bool_int(void);
void DeserExprOperators_cond_eq_int_flt(void);
void DeserExprOperators_cond_eq_cond_and(void);
void DeserExprOperators_cond_eq_cond_or(void);
void DeserExprOperators_cond_gt_bool(void);
void DeserExprOperators_cond_gt_int(void);
void DeserExprOperators_cond_gt_flt(void);
void DeserExprOperators_cond_gteq_bool(void);
void DeserExprOperators_cond_gteq_int(void);
void DeserExprOperators_cond_gteq_flt(void);
void DeserExprOperators_cond_lt_bool(void);
void DeserExprOperators_cond_lt_int(void);
void DeserExprOperators_cond_lt_flt(void);
void DeserExprOperators_cond_lteq_bool(void);
void DeserExprOperators_cond_lteq_int(void);
void DeserExprOperators_cond_lteq_flt(void);
void DeserExprOperators_min_lparen_int_rparen(void);
void DeserExprOperators_min_lparen_int_add_int_rparen(void);
void DeserExprOperators_min_var(void);
void DeserExprOperators_min_lparen_int_rparen_to_i64(void);
void DeserExprOperators_min_lparen_int_rparen_to_i32(void);
void DeserExprOperators_struct_w_min_var(void);
void DeserExprOperators_struct_w_min_lparen_int_rparen(void);
void DeserExprOperators_struct_w_min_lparen_var_rparen(void);
void DeserExprOperators_shift_left_int(void);
void DeserExprOperators_shift_right_int(void);
void DeserExprOperators_shift_left_int_add_int(void);
void DeserExprOperators_shift_left_int_mul_int(void);
void DeserExprOperators_add_int_shift_left_int(void);
void DeserExprOperators_mul_int_shift_left_int(void);
void DeserExprOperators_add_int_shift_left_int_add_int(void);
void DeserExprOperators_mul_int_shift_left_int_mul_int(void);

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
    },
    {
        "primitive_init",
        PrimitiveTypes_primitive_init
    },
    {
        "primitive_w_short_notation",
        PrimitiveTypes_primitive_w_short_notation
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
    },
    {
        "zero_initialized",
        EnumTypes_zero_initialized
    },
    {
        "enum_relation",
        EnumTypes_enum_relation
    },
    {
        "enum_w_short_notation",
        EnumTypes_enum_w_short_notation
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
    },
    {
        "bitmask_w_short_notation",
        BitmaskTypes_bitmask_w_short_notation
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
    },
    {
        "incomplete_member",
        StructTypes_incomplete_member
    },
    {
        "partial_type",
        StructTypes_partial_type
    },
    {
        "partial_type_custom_offset",
        StructTypes_partial_type_custom_offset
    },
    {
        "struct_w_short_notation",
        StructTypes_struct_w_short_notation
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
    },
    {
        "array_w_short_notation",
        ArrayTypes_array_w_short_notation
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
    },
    {
        "vector_w_short_notation",
        VectorTypes_vector_w_short_notation
    }
};

bake_test_case Units_testcases[] = {
    {
        "member_w_unit",
        Units_member_w_unit
    },
    {
        "member_w_unit_type",
        Units_member_w_unit_type
    },
    {
        "cursor_get_unit",
        Units_cursor_get_unit
    },
    {
        "cursor_get_unit_type",
        Units_cursor_get_unit_type
    },
    {
        "unit_w_quantity",
        Units_unit_w_quantity
    },
    {
        "unit_w_self_quantity",
        Units_unit_w_self_quantity
    },
    {
        "unit_w_self_quantity_after_init",
        Units_unit_w_self_quantity_after_init
    },
    {
        "unit_w_derived",
        Units_unit_w_derived
    },
    {
        "unit_w_over",
        Units_unit_w_over
    },
    {
        "unit_w_prefix",
        Units_unit_w_prefix
    },
    {
        "member_w_invalid_unit",
        Units_member_w_invalid_unit
    },
    {
        "unit_w_invalid_quantity",
        Units_unit_w_invalid_quantity
    },
    {
        "unit_w_invalid_derived",
        Units_unit_w_invalid_derived
    },
    {
        "unit_w_invalid_symbol_w_over",
        Units_unit_w_invalid_symbol_w_over
    },
    {
        "unit_w_invalid_symbol_w_prefix",
        Units_unit_w_invalid_symbol_w_prefix
    },
    {
        "unit_w_invalid_over",
        Units_unit_w_invalid_over
    },
    {
        "unit_w_over_no_derived",
        Units_unit_w_over_no_derived
    },
    {
        "define_twice",
        Units_define_twice
    },
    {
        "define_twice_different_quantity",
        Units_define_twice_different_quantity
    },
    {
        "define_twice_remove_quantity",
        Units_define_twice_remove_quantity
    },
    {
        "set_unit",
        Units_set_unit
    },
    {
        "set_unit_w_derived",
        Units_set_unit_w_derived
    },
    {
        "set_unit_w_over",
        Units_set_unit_w_over
    },
    {
        "set_unit_w_prefix",
        Units_set_unit_w_prefix
    },
    {
        "builtin_units",
        Units_builtin_units
    },
    {
        "unit_w_short_notation",
        Units_unit_w_short_notation
    },
    {
        "unit_prefix_w_short_notation",
        Units_unit_prefix_w_short_notation
    },
    {
        "quantity_w_short_notation",
        Units_quantity_w_short_notation
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
        "set_string_literal",
        Cursor_set_string_literal
    },
    {
        "set_string_to_null",
        Cursor_set_string_to_null
    },
    {
        "set_entity",
        Cursor_set_entity
    },
    {
        "set_entity_to_number",
        Cursor_set_entity_to_number
    },
    {
        "set_entity_to_0",
        Cursor_set_entity_to_0
    },
    {
        "set_enum",
        Cursor_set_enum
    },
    {
        "set_bitmask",
        Cursor_set_bitmask
    },
    {
        "set_signed_as_unsigned",
        Cursor_set_signed_as_unsigned
    },
    {
        "set_unsigned_as_signed",
        Cursor_set_unsigned_as_signed
    },
    {
        "set_signed_as_unsigned_out_of_range",
        Cursor_set_signed_as_unsigned_out_of_range
    },
    {
        "set_unsigned_as_signed_out_of_range",
        Cursor_set_unsigned_as_signed_out_of_range
    },
    {
        "set_string_to_null_as_signed",
        Cursor_set_string_to_null_as_signed
    },
    {
        "set_string_to_null_as_unsigned",
        Cursor_set_string_to_null_as_unsigned
    },
    {
        "set_entity_as_signed",
        Cursor_set_entity_as_signed
    },
    {
        "set_entity_as_unsigned",
        Cursor_set_entity_as_unsigned
    },
    {
        "set_entity_as_signed_out_of_range",
        Cursor_set_entity_as_signed_out_of_range
    },
    {
        "set_str_to_bool",
        Cursor_set_str_to_bool
    },
    {
        "set_str_to_char",
        Cursor_set_str_to_char
    },
    {
        "set_str_literal_to_char",
        Cursor_set_str_literal_to_char
    },
    {
        "set_str_to_i8",
        Cursor_set_str_to_i8
    },
    {
        "set_str_to_i16",
        Cursor_set_str_to_i16
    },
    {
        "set_str_to_i32",
        Cursor_set_str_to_i32
    },
    {
        "set_str_to_i64",
        Cursor_set_str_to_i64
    },
    {
        "set_str_to_f32",
        Cursor_set_str_to_f32
    },
    {
        "set_str_to_f64",
        Cursor_set_str_to_f64
    },
    {
        "set_str_to_entity",
        Cursor_set_str_to_entity
    },
    {
        "set_str_to_invalid_bool",
        Cursor_set_str_to_invalid_bool
    },
    {
        "set_str_to_invalid_entity",
        Cursor_set_str_to_invalid_entity
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
    },
    {
        "struct_member_i32",
        Cursor_struct_member_i32
    },
    {
        "struct_member_i32_i32",
        Cursor_struct_member_i32_i32
    },
    {
        "struct_member_i32_i32_reverse",
        Cursor_struct_member_i32_i32_reverse
    },
    {
        "struct_member_nested_i32_i32",
        Cursor_struct_member_nested_i32_i32
    },
    {
        "struct_member_2_nested_i32_i32",
        Cursor_struct_member_2_nested_i32_i32
    },
    {
        "struct_member_2_nested_i32_i32_reverse",
        Cursor_struct_member_2_nested_i32_i32_reverse
    },
    {
        "struct_w_array_type_i32_i32",
        Cursor_struct_w_array_type_i32_i32
    },
    {
        "struct_w_array_type_struct",
        Cursor_struct_w_array_type_struct
    },
    {
        "struct_w_2_array_type_i32_i32",
        Cursor_struct_w_2_array_type_i32_i32
    },
    {
        "struct_w_2_array_type_struct",
        Cursor_struct_w_2_array_type_struct
    },
    {
        "array_move_primitive",
        Cursor_array_move_primitive
    },
    {
        "array_move_struct",
        Cursor_array_move_struct
    },
    {
        "array_move_out_of_range",
        Cursor_array_move_out_of_range
    }
};

bake_test_case DeserializeFromExpr_testcases[] = {
    {
        "bool",
        DeserializeFromExpr_bool
    },
    {
        "byte",
        DeserializeFromExpr_byte
    },
    {
        "char",
        DeserializeFromExpr_char
    },
    {
        "char_literal",
        DeserializeFromExpr_char_literal
    },
    {
        "i8",
        DeserializeFromExpr_i8
    },
    {
        "i16",
        DeserializeFromExpr_i16
    },
    {
        "i32",
        DeserializeFromExpr_i32
    },
    {
        "i64",
        DeserializeFromExpr_i64
    },
    {
        "iptr",
        DeserializeFromExpr_iptr
    },
    {
        "u8",
        DeserializeFromExpr_u8
    },
    {
        "u16",
        DeserializeFromExpr_u16
    },
    {
        "u32",
        DeserializeFromExpr_u32
    },
    {
        "u64",
        DeserializeFromExpr_u64
    },
    {
        "uptr",
        DeserializeFromExpr_uptr
    },
    {
        "float",
        DeserializeFromExpr_float
    },
    {
        "double",
        DeserializeFromExpr_double
    },
    {
        "negative_int",
        DeserializeFromExpr_negative_int
    },
    {
        "negative_float",
        DeserializeFromExpr_negative_float
    },
    {
        "string",
        DeserializeFromExpr_string
    },
    {
        "entity",
        DeserializeFromExpr_entity
    },
    {
        "enum",
        DeserializeFromExpr_enum
    },
    {
        "bitmask",
        DeserializeFromExpr_bitmask
    },
    {
        "struct_enum",
        DeserializeFromExpr_struct_enum
    },
    {
        "struct_bitmask",
        DeserializeFromExpr_struct_bitmask
    },
    {
        "struct_i32",
        DeserializeFromExpr_struct_i32
    },
    {
        "struct_i32_neg",
        DeserializeFromExpr_struct_i32_neg
    },
    {
        "struct_i32_i32",
        DeserializeFromExpr_struct_i32_i32
    },
    {
        "struct_entity",
        DeserializeFromExpr_struct_entity
    },
    {
        "struct_nested_i32",
        DeserializeFromExpr_struct_nested_i32
    },
    {
        "struct_nested_i32_i32",
        DeserializeFromExpr_struct_nested_i32_i32
    },
    {
        "struct_2_nested_i32_i32",
        DeserializeFromExpr_struct_2_nested_i32_i32
    },
    {
        "struct_member_i32",
        DeserializeFromExpr_struct_member_i32
    },
    {
        "struct_member_i32_neg",
        DeserializeFromExpr_struct_member_i32_neg
    },
    {
        "struct_member_i32_i32",
        DeserializeFromExpr_struct_member_i32_i32
    },
    {
        "struct_member_nested_i32",
        DeserializeFromExpr_struct_member_nested_i32
    },
    {
        "struct_member_nested_i32_i32",
        DeserializeFromExpr_struct_member_nested_i32_i32
    },
    {
        "struct_member_2_nested_i32_i32",
        DeserializeFromExpr_struct_member_2_nested_i32_i32
    },
    {
        "struct_member_2_nested_i32_i32_reverse",
        DeserializeFromExpr_struct_member_2_nested_i32_i32_reverse
    },
    {
        "struct_i32_array_3",
        DeserializeFromExpr_struct_i32_array_3
    },
    {
        "struct_struct_i32_array_3",
        DeserializeFromExpr_struct_struct_i32_array_3
    },
    {
        "struct_struct_i32_i32_array_3",
        DeserializeFromExpr_struct_struct_i32_i32_array_3
    },
    {
        "struct_w_array_type_i32_i32",
        DeserializeFromExpr_struct_w_array_type_i32_i32
    },
    {
        "struct_w_array_type_struct",
        DeserializeFromExpr_struct_w_array_type_struct
    },
    {
        "struct_w_2_array_type_i32_i32",
        DeserializeFromExpr_struct_w_2_array_type_i32_i32
    },
    {
        "struct_w_2_array_type_struct",
        DeserializeFromExpr_struct_w_2_array_type_struct
    },
    {
        "discover_type_int",
        DeserializeFromExpr_discover_type_int
    },
    {
        "discover_type_negative_int",
        DeserializeFromExpr_discover_type_negative_int
    },
    {
        "discover_type_float",
        DeserializeFromExpr_discover_type_float
    },
    {
        "discover_type_negative_float",
        DeserializeFromExpr_discover_type_negative_float
    },
    {
        "discover_type_string",
        DeserializeFromExpr_discover_type_string
    },
    {
        "discover_type_multiline_string",
        DeserializeFromExpr_discover_type_multiline_string
    },
    {
        "discover_type_entity",
        DeserializeFromExpr_discover_type_entity
    },
    {
        "discover_type_bool",
        DeserializeFromExpr_discover_type_bool
    },
    {
        "discover_type_unknown",
        DeserializeFromExpr_discover_type_unknown
    },
    {
        "discover_type_invalid",
        DeserializeFromExpr_discover_type_invalid
    }
};

bake_test_case SerializeToExpr_testcases[] = {
    {
        "bool",
        SerializeToExpr_bool
    },
    {
        "byte",
        SerializeToExpr_byte
    },
    {
        "char",
        SerializeToExpr_char
    },
    {
        "i8",
        SerializeToExpr_i8
    },
    {
        "i16",
        SerializeToExpr_i16
    },
    {
        "i32",
        SerializeToExpr_i32
    },
    {
        "i64",
        SerializeToExpr_i64
    },
    {
        "iptr",
        SerializeToExpr_iptr
    },
    {
        "u8",
        SerializeToExpr_u8
    },
    {
        "u16",
        SerializeToExpr_u16
    },
    {
        "u32",
        SerializeToExpr_u32
    },
    {
        "u64",
        SerializeToExpr_u64
    },
    {
        "uptr",
        SerializeToExpr_uptr
    },
    {
        "float",
        SerializeToExpr_float
    },
    {
        "double",
        SerializeToExpr_double
    },
    {
        "string",
        SerializeToExpr_string
    },
    {
        "entity",
        SerializeToExpr_entity
    },
    {
        "enum",
        SerializeToExpr_enum
    },
    {
        "bitmask",
        SerializeToExpr_bitmask
    },
    {
        "float_nan",
        SerializeToExpr_float_nan
    },
    {
        "float_inf",
        SerializeToExpr_float_inf
    },
    {
        "double_nan",
        SerializeToExpr_double_nan
    },
    {
        "double_inf",
        SerializeToExpr_double_inf
    },
    {
        "struct_enum",
        SerializeToExpr_struct_enum
    },
    {
        "struct_bitmask",
        SerializeToExpr_struct_bitmask
    },
    {
        "struct_i32",
        SerializeToExpr_struct_i32
    },
    {
        "struct_i32_i32",
        SerializeToExpr_struct_i32_i32
    },
    {
        "struct_entity",
        SerializeToExpr_struct_entity
    },
    {
        "array_i32_3",
        SerializeToExpr_array_i32_3
    },
    {
        "array_struct_i32_i32",
        SerializeToExpr_array_struct_i32_i32
    },
    {
        "array_array_i32_3",
        SerializeToExpr_array_array_i32_3
    },
    {
        "entity_entity_after_float",
        SerializeToExpr_entity_entity_after_float
    },
    {
        "struct_nested_i32",
        SerializeToExpr_struct_nested_i32
    },
    {
        "struct_nested_i32_i32",
        SerializeToExpr_struct_nested_i32_i32
    },
    {
        "struct_2_nested_i32_i32",
        SerializeToExpr_struct_2_nested_i32_i32
    },
    {
        "struct_struct_i32_array_3",
        SerializeToExpr_struct_struct_i32_array_3
    },
    {
        "struct_struct_i32_i32_array_3",
        SerializeToExpr_struct_struct_i32_i32_array_3
    },
    {
        "struct_w_array_type_i32_i32",
        SerializeToExpr_struct_w_array_type_i32_i32
    },
    {
        "struct_w_array_type_struct",
        SerializeToExpr_struct_w_array_type_struct
    },
    {
        "struct_w_2_array_type_i32_i32",
        SerializeToExpr_struct_w_2_array_type_i32_i32
    },
    {
        "struct_w_2_array_type_struct",
        SerializeToExpr_struct_w_2_array_type_struct
    },
    {
        "struct_partial",
        SerializeToExpr_struct_partial
    },
    {
        "escape_simple_string",
        SerializeToExpr_escape_simple_string
    },
    {
        "escape_newline",
        SerializeToExpr_escape_newline
    },
    {
        "escape_2_newlines",
        SerializeToExpr_escape_2_newlines
    },
    {
        "escape_string_w_trailing_newline",
        SerializeToExpr_escape_string_w_trailing_newline
    },
    {
        "escape_string_w_2_trailing_newlines",
        SerializeToExpr_escape_string_w_2_trailing_newlines
    },
    {
        "escape_string_w_delim",
        SerializeToExpr_escape_string_w_delim
    }
};

bake_test_case DeserializeFromJson_testcases[] = {
    {
        "struct_bool",
        DeserializeFromJson_struct_bool
    },
    {
        "struct_byte",
        DeserializeFromJson_struct_byte
    },
    {
        "struct_char",
        DeserializeFromJson_struct_char
    },
    {
        "struct_char_literal",
        DeserializeFromJson_struct_char_literal
    },
    {
        "struct_i8",
        DeserializeFromJson_struct_i8
    },
    {
        "struct_i16",
        DeserializeFromJson_struct_i16
    },
    {
        "struct_i32",
        DeserializeFromJson_struct_i32
    },
    {
        "struct_i64",
        DeserializeFromJson_struct_i64
    },
    {
        "struct_iptr",
        DeserializeFromJson_struct_iptr
    },
    {
        "struct_u8",
        DeserializeFromJson_struct_u8
    },
    {
        "struct_u16",
        DeserializeFromJson_struct_u16
    },
    {
        "struct_u32",
        DeserializeFromJson_struct_u32
    },
    {
        "struct_u64",
        DeserializeFromJson_struct_u64
    },
    {
        "struct_uptr",
        DeserializeFromJson_struct_uptr
    },
    {
        "struct_float",
        DeserializeFromJson_struct_float
    },
    {
        "struct_double",
        DeserializeFromJson_struct_double
    },
    {
        "struct_negative_int",
        DeserializeFromJson_struct_negative_int
    },
    {
        "struct_negative_float",
        DeserializeFromJson_struct_negative_float
    },
    {
        "struct_string",
        DeserializeFromJson_struct_string
    },
    {
        "struct_entity",
        DeserializeFromJson_struct_entity
    },
    {
        "struct_enum",
        DeserializeFromJson_struct_enum
    },
    {
        "struct_bitmask",
        DeserializeFromJson_struct_bitmask
    },
    {
        "struct_i32_i32",
        DeserializeFromJson_struct_i32_i32
    },
    {
        "struct_nested_i32",
        DeserializeFromJson_struct_nested_i32
    },
    {
        "struct_nested_i32_i32",
        DeserializeFromJson_struct_nested_i32_i32
    },
    {
        "struct_2_nested_i32_i32",
        DeserializeFromJson_struct_2_nested_i32_i32
    },
    {
        "struct_i32_array_3",
        DeserializeFromJson_struct_i32_array_3
    },
    {
        "struct_struct_i32_array_3",
        DeserializeFromJson_struct_struct_i32_array_3
    },
    {
        "struct_struct_i32_i32_array_3",
        DeserializeFromJson_struct_struct_i32_i32_array_3
    },
    {
        "struct_w_array_type_i32_i32",
        DeserializeFromJson_struct_w_array_type_i32_i32
    },
    {
        "struct_w_2_array_type_i32_i32",
        DeserializeFromJson_struct_w_2_array_type_i32_i32
    },
    {
        "struct_w_nested_member_i32",
        DeserializeFromJson_struct_w_nested_member_i32
    },
    {
        "struct_w_2_nested_members_i32",
        DeserializeFromJson_struct_w_2_nested_members_i32
    },
    {
        "struct_w_nested_members_struct",
        DeserializeFromJson_struct_w_nested_members_struct
    },
    {
        "struct_w_2_nested_members_struct",
        DeserializeFromJson_struct_w_2_nested_members_struct
    },
    {
        "deser_entity_1_component_1_member",
        DeserializeFromJson_deser_entity_1_component_1_member
    },
    {
        "deser_entity_1_component_1_member_w_spaces",
        DeserializeFromJson_deser_entity_1_component_1_member_w_spaces
    },
    {
        "deser_entity_1_component_2_members",
        DeserializeFromJson_deser_entity_1_component_2_members
    },
    {
        "deser_entity_2_components",
        DeserializeFromJson_deser_entity_2_components
    },
    {
        "deser_entity_1_component_composite_member",
        DeserializeFromJson_deser_entity_1_component_composite_member
    },
    {
        "deser_entity_1_component_nested_member",
        DeserializeFromJson_deser_entity_1_component_nested_member
    },
    {
        "deser_entity_1_pair",
        DeserializeFromJson_deser_entity_1_pair
    },
    {
        "deser_entity_2_pairs",
        DeserializeFromJson_deser_entity_2_pairs
    }
};

bake_test_case SerializeToJson_testcases[] = {
    {
        "struct_bool",
        SerializeToJson_struct_bool
    },
    {
        "struct_byte",
        SerializeToJson_struct_byte
    },
    {
        "struct_char",
        SerializeToJson_struct_char
    },
    {
        "struct_i8",
        SerializeToJson_struct_i8
    },
    {
        "struct_i16",
        SerializeToJson_struct_i16
    },
    {
        "struct_i32",
        SerializeToJson_struct_i32
    },
    {
        "struct_i64",
        SerializeToJson_struct_i64
    },
    {
        "struct_iptr",
        SerializeToJson_struct_iptr
    },
    {
        "struct_u8",
        SerializeToJson_struct_u8
    },
    {
        "struct_u16",
        SerializeToJson_struct_u16
    },
    {
        "struct_u32",
        SerializeToJson_struct_u32
    },
    {
        "struct_u64",
        SerializeToJson_struct_u64
    },
    {
        "struct_uptr",
        SerializeToJson_struct_uptr
    },
    {
        "struct_float",
        SerializeToJson_struct_float
    },
    {
        "struct_double",
        SerializeToJson_struct_double
    },
    {
        "struct_string",
        SerializeToJson_struct_string
    },
    {
        "struct_entity",
        SerializeToJson_struct_entity
    },
    {
        "struct_entity_after_float",
        SerializeToJson_struct_entity_after_float
    },
    {
        "struct_float_nan",
        SerializeToJson_struct_float_nan
    },
    {
        "struct_float_inf",
        SerializeToJson_struct_float_inf
    },
    {
        "struct_double_nan",
        SerializeToJson_struct_double_nan
    },
    {
        "struct_double_inf",
        SerializeToJson_struct_double_inf
    },
    {
        "struct_double_large",
        SerializeToJson_struct_double_large
    },
    {
        "struct_double_large_e",
        SerializeToJson_struct_double_large_e
    },
    {
        "struct_enum",
        SerializeToJson_struct_enum
    },
    {
        "struct_bitmask",
        SerializeToJson_struct_bitmask
    },
    {
        "struct_i32_i32",
        SerializeToJson_struct_i32_i32
    },
    {
        "struct_nested_i32",
        SerializeToJson_struct_nested_i32
    },
    {
        "struct_nested_i32_i32",
        SerializeToJson_struct_nested_i32_i32
    },
    {
        "struct_2_nested_i32_i32",
        SerializeToJson_struct_2_nested_i32_i32
    },
    {
        "struct_i32_array_3",
        SerializeToJson_struct_i32_array_3
    },
    {
        "struct_struct_i32_array_3",
        SerializeToJson_struct_struct_i32_array_3
    },
    {
        "struct_struct_i32_i32_array_3",
        SerializeToJson_struct_struct_i32_i32_array_3
    },
    {
        "struct_w_array_type_i32_i32",
        SerializeToJson_struct_w_array_type_i32_i32
    },
    {
        "struct_w_2_array_type_i32_i32",
        SerializeToJson_struct_w_2_array_type_i32_i32
    },
    {
        "struct_partial",
        SerializeToJson_struct_partial
    },
    {
        "array_i32_3",
        SerializeToJson_array_i32_3
    },
    {
        "array_struct_i32_i32",
        SerializeToJson_array_struct_i32_i32
    },
    {
        "array_array_i32_3",
        SerializeToJson_array_array_i32_3
    },
    {
        "serialize_entity_empty",
        SerializeToJson_serialize_entity_empty
    },
    {
        "serialize_entity_w_name",
        SerializeToJson_serialize_entity_w_name
    },
    {
        "serialize_entity_w_name_1_tag",
        SerializeToJson_serialize_entity_w_name_1_tag
    },
    {
        "serialize_entity_w_name_2_tags",
        SerializeToJson_serialize_entity_w_name_2_tags
    },
    {
        "serialize_entity_w_name_1_pair",
        SerializeToJson_serialize_entity_w_name_1_pair
    },
    {
        "serialize_entity_w_base",
        SerializeToJson_serialize_entity_w_base
    },
    {
        "serialize_entity_w_base_override",
        SerializeToJson_serialize_entity_w_base_override
    },
    {
        "serialize_entity_w_2_base",
        SerializeToJson_serialize_entity_w_2_base
    },
    {
        "serialize_entity_w_nested_base",
        SerializeToJson_serialize_entity_w_nested_base
    },
    {
        "serialize_entity_w_1_component",
        SerializeToJson_serialize_entity_w_1_component
    },
    {
        "serialize_entity_w_2_components",
        SerializeToJson_serialize_entity_w_2_components
    },
    {
        "serialize_entity_w_primitive_component",
        SerializeToJson_serialize_entity_w_primitive_component
    },
    {
        "serialize_entity_w_enum_component",
        SerializeToJson_serialize_entity_w_enum_component
    },
    {
        "serialize_entity_w_struct_and_enum_component",
        SerializeToJson_serialize_entity_w_struct_and_enum_component
    },
    {
        "serialize_entity_w_invalid_enum_component",
        SerializeToJson_serialize_entity_w_invalid_enum_component
    },
    {
        "serialize_entity_w_type_info",
        SerializeToJson_serialize_entity_w_type_info
    },
    {
        "serialize_entity_w_type_info_unit",
        SerializeToJson_serialize_entity_w_type_info_unit
    },
    {
        "serialize_entity_w_type_info_unit_quantity",
        SerializeToJson_serialize_entity_w_type_info_unit_quantity
    },
    {
        "serialize_entity_w_type_info_unit_over",
        SerializeToJson_serialize_entity_w_type_info_unit_over
    },
    {
        "serialize_entity_wo_private",
        SerializeToJson_serialize_entity_wo_private
    },
    {
        "serialize_entity_w_private",
        SerializeToJson_serialize_entity_w_private
    },
    {
        "serialize_entity_w_label",
        SerializeToJson_serialize_entity_w_label
    },
    {
        "serialize_entity_w_label_no_name",
        SerializeToJson_serialize_entity_w_label_no_name
    },
    {
        "serialize_entity_w_id_labels",
        SerializeToJson_serialize_entity_w_id_labels
    },
    {
        "serialize_entity_w_brief",
        SerializeToJson_serialize_entity_w_brief
    },
    {
        "serialize_entity_w_brief_no_brief",
        SerializeToJson_serialize_entity_w_brief_no_brief
    },
    {
        "serialize_entity_w_link",
        SerializeToJson_serialize_entity_w_link
    },
    {
        "serialize_entity_w_link_no_link",
        SerializeToJson_serialize_entity_w_link_no_link
    },
    {
        "serialize_entity_w_meta_ids",
        SerializeToJson_serialize_entity_w_meta_ids
    },
    {
        "serialize_entity_color",
        SerializeToJson_serialize_entity_color
    },
    {
        "serialize_entity_w_doc_w_quotes",
        SerializeToJson_serialize_entity_w_doc_w_quotes
    },
    {
        "serialize_entity_union_relationship",
        SerializeToJson_serialize_entity_union_relationship
    },
    {
        "serialize_entity_union_relationship_w_labels",
        SerializeToJson_serialize_entity_union_relationship_w_labels
    },
    {
        "serialize_entity_union_relationship_invalid_entity",
        SerializeToJson_serialize_entity_union_relationship_invalid_entity
    },
    {
        "serialize_entity_union_relationship_invalid_entity_w_labels",
        SerializeToJson_serialize_entity_union_relationship_invalid_entity_w_labels
    },
    {
        "serialize_entity_w_union_property",
        SerializeToJson_serialize_entity_w_union_property
    },
    {
        "serialize_entity_w_union_property",
        SerializeToJson_serialize_entity_w_union_property
    },
    {
        "serialize_entity_from_core",
        SerializeToJson_serialize_entity_from_core
    },
    {
        "serialize_iterator_1_comps_empty",
        SerializeToJson_serialize_iterator_1_comps_empty
    },
    {
        "serialize_iterator_1_comps_2_ents_same_table",
        SerializeToJson_serialize_iterator_1_comps_2_ents_same_table
    },
    {
        "serialize_iterator_1_tag_2_ents_same_table",
        SerializeToJson_serialize_iterator_1_tag_2_ents_same_table
    },
    {
        "serialize_iterator_2_comps_2_ents_same_table",
        SerializeToJson_serialize_iterator_2_comps_2_ents_same_table
    },
    {
        "serialize_iterator_1_tag_1_comp_2_ents_same_table",
        SerializeToJson_serialize_iterator_1_tag_1_comp_2_ents_same_table
    },
    {
        "serialize_iterator_1_tag_1_comp_4_ents_two_tables",
        SerializeToJson_serialize_iterator_1_tag_1_comp_4_ents_two_tables
    },
    {
        "serialize_iterator_2_comps_1_owned_2_ents",
        SerializeToJson_serialize_iterator_2_comps_1_owned_2_ents
    },
    {
        "serialize_iterator_w_pair_wildcard",
        SerializeToJson_serialize_iterator_w_pair_wildcard
    },
    {
        "serialize_iterator_w_var",
        SerializeToJson_serialize_iterator_w_var
    },
    {
        "serialize_iterator_w_2_vars",
        SerializeToJson_serialize_iterator_w_2_vars
    },
    {
        "serialize_iterator_type_info_1_tags",
        SerializeToJson_serialize_iterator_type_info_1_tags
    },
    {
        "serialize_iterator_type_info_2_tags",
        SerializeToJson_serialize_iterator_type_info_2_tags
    },
    {
        "serialize_iterator_type_info_1_component",
        SerializeToJson_serialize_iterator_type_info_1_component
    },
    {
        "serialize_iterator_type_info_2_components",
        SerializeToJson_serialize_iterator_type_info_2_components
    },
    {
        "serialize_iterator_type_info_1_struct",
        SerializeToJson_serialize_iterator_type_info_1_struct
    },
    {
        "serialize_iterator_type_info_1_component_1_struct",
        SerializeToJson_serialize_iterator_type_info_1_component_1_struct
    },
    {
        "serialize_iterator_type_info_2_structs",
        SerializeToJson_serialize_iterator_type_info_2_structs
    },
    {
        "serialize_iterator_type_info_w_unit",
        SerializeToJson_serialize_iterator_type_info_w_unit
    },
    {
        "serialize_iterator_type_info_w_unit_quantity",
        SerializeToJson_serialize_iterator_type_info_w_unit_quantity
    },
    {
        "serialize_iterator_type_info_w_unit_over",
        SerializeToJson_serialize_iterator_type_info_w_unit_over
    },
    {
        "serialize_iterator_w_entity_label",
        SerializeToJson_serialize_iterator_w_entity_label
    },
    {
        "serialize_iterator_w_var_labels",
        SerializeToJson_serialize_iterator_w_var_labels
    },
    {
        "serialize_iterator_w_var_component",
        SerializeToJson_serialize_iterator_w_var_component
    },
    {
        "serialize_iterator_w_optional_tag",
        SerializeToJson_serialize_iterator_w_optional_tag
    },
    {
        "serialize_iterator_w_optional_component",
        SerializeToJson_serialize_iterator_w_optional_component
    },
    {
        "serialize_iterator_w_optional_reflected_component",
        SerializeToJson_serialize_iterator_w_optional_reflected_component
    },
    {
        "serialize_iterator_w_inout_filter_tag",
        SerializeToJson_serialize_iterator_w_inout_filter_tag
    },
    {
        "serialize_iterator_w_inout_filter_component",
        SerializeToJson_serialize_iterator_w_inout_filter_component
    },
    {
        "serialize_iterator_w_inout_filter_reflected_component",
        SerializeToJson_serialize_iterator_w_inout_filter_reflected_component
    },
    {
        "serialize_iterator_w_inout_out_tag",
        SerializeToJson_serialize_iterator_w_inout_out_tag
    },
    {
        "serialize_iterator_w_inout_out_component",
        SerializeToJson_serialize_iterator_w_inout_out_component
    },
    {
        "serialize_iterator_w_inout_out_reflected_component",
        SerializeToJson_serialize_iterator_w_inout_out_reflected_component
    },
    {
        "serialize_iterator_component_from_var",
        SerializeToJson_serialize_iterator_component_from_var
    },
    {
        "serialize_iterator_color",
        SerializeToJson_serialize_iterator_color
    },
    {
        "serialize_iterator_ids",
        SerializeToJson_serialize_iterator_ids
    },
    {
        "serialize_iterator_ids_2_entities",
        SerializeToJson_serialize_iterator_ids_2_entities
    },
    {
        "serialize_iterator_variable_ids",
        SerializeToJson_serialize_iterator_variable_ids
    },
    {
        "serialize_iterator_variable_ids_2_entities",
        SerializeToJson_serialize_iterator_variable_ids_2_entities
    },
    {
        "serialize_paged_iterator",
        SerializeToJson_serialize_paged_iterator
    },
    {
        "serialize_paged_iterator_w_optional_component",
        SerializeToJson_serialize_paged_iterator_w_optional_component
    },
    {
        "serialize_paged_iterator_w_optional_tag",
        SerializeToJson_serialize_paged_iterator_w_optional_tag
    },
    {
        "serialize_paged_iterator_w_vars",
        SerializeToJson_serialize_paged_iterator_w_vars
    }
};

bake_test_case SerializeTypeInfoToJson_testcases[] = {
    {
        "bool",
        SerializeTypeInfoToJson_bool
    },
    {
        "byte",
        SerializeTypeInfoToJson_byte
    },
    {
        "char",
        SerializeTypeInfoToJson_char
    },
    {
        "i8",
        SerializeTypeInfoToJson_i8
    },
    {
        "i16",
        SerializeTypeInfoToJson_i16
    },
    {
        "i32",
        SerializeTypeInfoToJson_i32
    },
    {
        "i64",
        SerializeTypeInfoToJson_i64
    },
    {
        "iptr",
        SerializeTypeInfoToJson_iptr
    },
    {
        "u8",
        SerializeTypeInfoToJson_u8
    },
    {
        "u16",
        SerializeTypeInfoToJson_u16
    },
    {
        "u32",
        SerializeTypeInfoToJson_u32
    },
    {
        "u64",
        SerializeTypeInfoToJson_u64
    },
    {
        "uptr",
        SerializeTypeInfoToJson_uptr
    },
    {
        "float",
        SerializeTypeInfoToJson_float
    },
    {
        "double",
        SerializeTypeInfoToJson_double
    },
    {
        "string",
        SerializeTypeInfoToJson_string
    },
    {
        "entity",
        SerializeTypeInfoToJson_entity
    },
    {
        "enum",
        SerializeTypeInfoToJson_enum
    },
    {
        "bitmask",
        SerializeTypeInfoToJson_bitmask
    },
    {
        "struct",
        SerializeTypeInfoToJson_struct
    },
    {
        "nested_struct",
        SerializeTypeInfoToJson_nested_struct
    },
    {
        "struct_array_i32_2",
        SerializeTypeInfoToJson_struct_array_i32_2
    },
    {
        "struct_array_struct_2",
        SerializeTypeInfoToJson_struct_array_struct_2
    },
    {
        "struct_array_type",
        SerializeTypeInfoToJson_struct_array_type
    }
};

bake_test_case MetaUtils_testcases[] = {
    {
        "struct_w_2_i32",
        MetaUtils_struct_w_2_i32
    },
    {
        "struct_w_2_bool",
        MetaUtils_struct_w_2_bool
    },
    {
        "struct_w_2_char",
        MetaUtils_struct_w_2_char
    },
    {
        "struct_w_2_string",
        MetaUtils_struct_w_2_string
    },
    {
        "struct_w_2_f32",
        MetaUtils_struct_w_2_f32
    },
    {
        "struct_w_2_f64",
        MetaUtils_struct_w_2_f64
    },
    {
        "struct_w_3_enum",
        MetaUtils_struct_w_3_enum
    },
    {
        "struct_w_3_enum_multiline",
        MetaUtils_struct_w_3_enum_multiline
    },
    {
        "struct_w_3_enum_w_assignment",
        MetaUtils_struct_w_3_enum_w_assignment
    },
    {
        "struct_w_4_bitmask",
        MetaUtils_struct_w_4_bitmask
    },
    {
        "struct_w_3_entities",
        MetaUtils_struct_w_3_entities
    },
    {
        "struct_w_2_array_3_i32",
        MetaUtils_struct_w_2_array_3_i32
    },
    {
        "struct_w_nested",
        MetaUtils_struct_w_nested
    },
    {
        "enum_nospace",
        MetaUtils_enum_nospace
    },
    {
        "struct_nospace",
        MetaUtils_struct_nospace
    },
    {
        "identifier_w_underscore",
        MetaUtils_identifier_w_underscore
    },
    {
        "struct_w_ptr",
        MetaUtils_struct_w_ptr
    }
};

bake_test_case Vars_testcases[] = {
    {
        "declare_1_var",
        Vars_declare_1_var
    },
    {
        "declare_2_vars",
        Vars_declare_2_vars
    },
    {
        "declare_vars_nested_scope",
        Vars_declare_vars_nested_scope
    },
    {
        "declare_vars_2_scopes",
        Vars_declare_vars_2_scopes
    },
    {
        "redeclare_var",
        Vars_redeclare_var
    },
    {
        "i32_expr_w_i32_var",
        Vars_i32_expr_w_i32_var
    },
    {
        "i32_expr_w_f32_var",
        Vars_i32_expr_w_f32_var
    },
    {
        "i32_expr_w_string_var",
        Vars_i32_expr_w_string_var
    },
    {
        "string_expr_w_string_var",
        Vars_string_expr_w_string_var
    },
    {
        "struct_expr_w_i32_vars",
        Vars_struct_expr_w_i32_vars
    },
    {
        "struct_expr_w_struct_var",
        Vars_struct_expr_w_struct_var
    },
    {
        "nested_struct_expr_w_struct_var",
        Vars_nested_struct_expr_w_struct_var
    },
    {
        "declare_w_value",
        Vars_declare_w_value
    },
    {
        "redeclare_in_scope",
        Vars_redeclare_in_scope
    }
};

bake_test_case DeserExprOperators_testcases[] = {
    {
        "add_2_int_literals",
        DeserExprOperators_add_2_int_literals
    },
    {
        "add_2_int_literals_twice",
        DeserExprOperators_add_2_int_literals_twice
    },
    {
        "sub_2_int_literals",
        DeserExprOperators_sub_2_int_literals
    },
    {
        "mul_2_int_literals",
        DeserExprOperators_mul_2_int_literals
    },
    {
        "div_2_int_literals",
        DeserExprOperators_div_2_int_literals
    },
    {
        "add_3_int_literals",
        DeserExprOperators_add_3_int_literals
    },
    {
        "add_3_int_literals_twice",
        DeserExprOperators_add_3_int_literals_twice
    },
    {
        "sub_3_int_literals",
        DeserExprOperators_sub_3_int_literals
    },
    {
        "mul_3_int_literals",
        DeserExprOperators_mul_3_int_literals
    },
    {
        "div_3_int_literals",
        DeserExprOperators_div_3_int_literals
    },
    {
        "int_to_bool",
        DeserExprOperators_int_to_bool
    },
    {
        "add_mul_3_int_literals",
        DeserExprOperators_add_mul_3_int_literals
    },
    {
        "sub_mul_3_int_literals",
        DeserExprOperators_sub_mul_3_int_literals
    },
    {
        "div_mul_3_int_literals",
        DeserExprOperators_div_mul_3_int_literals
    },
    {
        "add_div_3_int_literals",
        DeserExprOperators_add_div_3_int_literals
    },
    {
        "sub_div_3_int_literals",
        DeserExprOperators_sub_div_3_int_literals
    },
    {
        "mul_div_3_int_literals",
        DeserExprOperators_mul_div_3_int_literals
    },
    {
        "mul_add_mul_add_int_literals",
        DeserExprOperators_mul_add_mul_add_int_literals
    },
    {
        "mul_sub_mul_sub_int_literals",
        DeserExprOperators_mul_sub_mul_sub_int_literals
    },
    {
        "mul_div_mul_div_int_literals",
        DeserExprOperators_mul_div_mul_div_int_literals
    },
    {
        "div_add_div_add_int_literals",
        DeserExprOperators_div_add_div_add_int_literals
    },
    {
        "div_sub_div_sub_int_literals",
        DeserExprOperators_div_sub_div_sub_int_literals
    },
    {
        "div_sub_div_mul_int_literals",
        DeserExprOperators_div_sub_div_mul_int_literals
    },
    {
        "div_mul_div_mul_int_literals",
        DeserExprOperators_div_mul_div_mul_int_literals
    },
    {
        "add_2_flt_literals",
        DeserExprOperators_add_2_flt_literals
    },
    {
        "sub_2_flt_literals",
        DeserExprOperators_sub_2_flt_literals
    },
    {
        "mul_2_flt_literals",
        DeserExprOperators_mul_2_flt_literals
    },
    {
        "div_2_flt_literals",
        DeserExprOperators_div_2_flt_literals
    },
    {
        "add_2_int_neg_literals",
        DeserExprOperators_add_2_int_neg_literals
    },
    {
        "sub_2_int_neg_literals",
        DeserExprOperators_sub_2_int_neg_literals
    },
    {
        "mul_2_int_neg_literals",
        DeserExprOperators_mul_2_int_neg_literals
    },
    {
        "div_2_int_neg_literals",
        DeserExprOperators_div_2_int_neg_literals
    },
    {
        "mul_lparen_add_add_rparen_int_literals",
        DeserExprOperators_mul_lparen_add_add_rparen_int_literals
    },
    {
        "mul_lparen_add_add_add_rparen_int_literals",
        DeserExprOperators_mul_lparen_add_add_add_rparen_int_literals
    },
    {
        "mul_lparen_add_add_rparen_add_int_literals",
        DeserExprOperators_mul_lparen_add_add_rparen_add_int_literals
    },
    {
        "lparen_add_add_rparen_mul_int_literals",
        DeserExprOperators_lparen_add_add_rparen_mul_int_literals
    },
    {
        "lparen_add_add_add_rparen_mul_int_literals",
        DeserExprOperators_lparen_add_add_add_rparen_mul_int_literals
    },
    {
        "double_paren_add_add",
        DeserExprOperators_double_paren_add_add
    },
    {
        "double_paren_literal",
        DeserExprOperators_double_paren_literal
    },
    {
        "lparen_add_add_rparen_mul_lparen_add_add_rparen",
        DeserExprOperators_lparen_add_add_rparen_mul_lparen_add_add_rparen
    },
    {
        "float_result_add_2_int_literals",
        DeserExprOperators_float_result_add_2_int_literals
    },
    {
        "struct_result_add_2_int_literals",
        DeserExprOperators_struct_result_add_2_int_literals
    },
    {
        "struct_result_add_2_2_fields_int_literals",
        DeserExprOperators_struct_result_add_2_2_fields_int_literals
    },
    {
        "struct_result_add_3_int_literals",
        DeserExprOperators_struct_result_add_3_int_literals
    },
    {
        "struct_result_lparen_int_rparen",
        DeserExprOperators_struct_result_lparen_int_rparen
    },
    {
        "add_to_var",
        DeserExprOperators_add_to_var
    },
    {
        "add_var_to",
        DeserExprOperators_add_var_to
    },
    {
        "bool_cond_and_bool",
        DeserExprOperators_bool_cond_and_bool
    },
    {
        "bool_cond_or_bool",
        DeserExprOperators_bool_cond_or_bool
    },
    {
        "int_cond_and_int",
        DeserExprOperators_int_cond_and_int
    },
    {
        "int_cond_or_int",
        DeserExprOperators_int_cond_or_int
    },
    {
        "bool_cond_and_int",
        DeserExprOperators_bool_cond_and_int
    },
    {
        "int_cond_and_bool",
        DeserExprOperators_int_cond_and_bool
    },
    {
        "bool_cond_or_int",
        DeserExprOperators_bool_cond_or_int
    },
    {
        "int_cond_or_bool",
        DeserExprOperators_int_cond_or_bool
    },
    {
        "cond_eq_bool",
        DeserExprOperators_cond_eq_bool
    },
    {
        "cond_eq_int",
        DeserExprOperators_cond_eq_int
    },
    {
        "cond_neq_bool",
        DeserExprOperators_cond_neq_bool
    },
    {
        "cond_neq_int",
        DeserExprOperators_cond_neq_int
    },
    {
        "cond_eq_bool_int",
        DeserExprOperators_cond_eq_bool_int
    },
    {
        "cond_eq_int_flt",
        DeserExprOperators_cond_eq_int_flt
    },
    {
        "cond_eq_cond_and",
        DeserExprOperators_cond_eq_cond_and
    },
    {
        "cond_eq_cond_or",
        DeserExprOperators_cond_eq_cond_or
    },
    {
        "cond_gt_bool",
        DeserExprOperators_cond_gt_bool
    },
    {
        "cond_gt_int",
        DeserExprOperators_cond_gt_int
    },
    {
        "cond_gt_flt",
        DeserExprOperators_cond_gt_flt
    },
    {
        "cond_gteq_bool",
        DeserExprOperators_cond_gteq_bool
    },
    {
        "cond_gteq_int",
        DeserExprOperators_cond_gteq_int
    },
    {
        "cond_gteq_flt",
        DeserExprOperators_cond_gteq_flt
    },
    {
        "cond_lt_bool",
        DeserExprOperators_cond_lt_bool
    },
    {
        "cond_lt_int",
        DeserExprOperators_cond_lt_int
    },
    {
        "cond_lt_flt",
        DeserExprOperators_cond_lt_flt
    },
    {
        "cond_lteq_bool",
        DeserExprOperators_cond_lteq_bool
    },
    {
        "cond_lteq_int",
        DeserExprOperators_cond_lteq_int
    },
    {
        "cond_lteq_flt",
        DeserExprOperators_cond_lteq_flt
    },
    {
        "min_lparen_int_rparen",
        DeserExprOperators_min_lparen_int_rparen
    },
    {
        "min_lparen_int_add_int_rparen",
        DeserExprOperators_min_lparen_int_add_int_rparen
    },
    {
        "min_var",
        DeserExprOperators_min_var
    },
    {
        "min_lparen_int_rparen_to_i64",
        DeserExprOperators_min_lparen_int_rparen_to_i64
    },
    {
        "min_lparen_int_rparen_to_i32",
        DeserExprOperators_min_lparen_int_rparen_to_i32
    },
    {
        "struct_w_min_var",
        DeserExprOperators_struct_w_min_var
    },
    {
        "struct_w_min_lparen_int_rparen",
        DeserExprOperators_struct_w_min_lparen_int_rparen
    },
    {
        "struct_w_min_lparen_var_rparen",
        DeserExprOperators_struct_w_min_lparen_var_rparen
    },
    {
        "shift_left_int",
        DeserExprOperators_shift_left_int
    },
    {
        "shift_right_int",
        DeserExprOperators_shift_right_int
    },
    {
        "shift_left_int_add_int",
        DeserExprOperators_shift_left_int_add_int
    },
    {
        "shift_left_int_mul_int",
        DeserExprOperators_shift_left_int_mul_int
    },
    {
        "add_int_shift_left_int",
        DeserExprOperators_add_int_shift_left_int
    },
    {
        "mul_int_shift_left_int",
        DeserExprOperators_mul_int_shift_left_int
    },
    {
        "add_int_shift_left_int_add_int",
        DeserExprOperators_add_int_shift_left_int_add_int
    },
    {
        "mul_int_shift_left_int_mul_int",
        DeserExprOperators_mul_int_shift_left_int_mul_int
    }
};

static bake_test_suite suites[] = {
    {
        "PrimitiveTypes",
        NULL,
        NULL,
        53,
        PrimitiveTypes_testcases
    },
    {
        "EnumTypes",
        NULL,
        NULL,
        8,
        EnumTypes_testcases
    },
    {
        "BitmaskTypes",
        NULL,
        NULL,
        7,
        BitmaskTypes_testcases
    },
    {
        "StructTypes",
        NULL,
        NULL,
        15,
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
        22,
        ArrayTypes_testcases
    },
    {
        "VectorTypes",
        NULL,
        NULL,
        4,
        VectorTypes_testcases
    },
    {
        "Units",
        NULL,
        NULL,
        28,
        Units_testcases
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
        75,
        Cursor_testcases
    },
    {
        "DeserializeFromExpr",
        NULL,
        NULL,
        55,
        DeserializeFromExpr_testcases
    },
    {
        "SerializeToExpr",
        NULL,
        NULL,
        48,
        SerializeToExpr_testcases
    },
    {
        "DeserializeFromJson",
        NULL,
        NULL,
        43,
        DeserializeFromJson_testcases
    },
    {
        "SerializeToJson",
        NULL,
        NULL,
        119,
        SerializeToJson_testcases
    },
    {
        "SerializeTypeInfoToJson",
        NULL,
        NULL,
        24,
        SerializeTypeInfoToJson_testcases
    },
    {
        "MetaUtils",
        NULL,
        NULL,
        17,
        MetaUtils_testcases
    },
    {
        "Vars",
        NULL,
        NULL,
        14,
        Vars_testcases
    },
    {
        "DeserExprOperators",
        NULL,
        NULL,
        91,
        DeserExprOperators_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("meta", argc, argv, suites, 18);
}
