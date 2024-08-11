
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
void PrimitiveTypes_id(void);
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
void PrimitiveTypes_builtin_id(void);
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
void PrimitiveTypes_struct_w_id(void);
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
void StructTypes_value_range(void);
void StructTypes_error_range(void);
void StructTypes_warning_range(void);
void StructTypes_error_and_warning_range(void);
void StructTypes_error_range_invalid_type(void);
void StructTypes_warning_range_invalid_type(void);
void StructTypes_invalid_value_range(void);
void StructTypes_invalid_error_range(void);
void StructTypes_invalid_warning_range(void);
void StructTypes_overlapping_error_warning_range(void);
void StructTypes_overlapping_value_error_range(void);
void StructTypes_overlapping_value_warning_range(void);
void StructTypes_struct_w_16_alignment(void);
void StructTypes_struct_w_use_offset(void);

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
void Units_import_units_after_mini(void);

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
void Serialized_ops_id(void);
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
void Serialized_ops_missing_metatype(void);

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
void Cursor_set_id(void);
void Cursor_set_id_to_number(void);
void Cursor_set_id_to_0(void);
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
void Cursor_set_id_as_signed(void);
void Cursor_set_id_as_unsigned(void);
void Cursor_set_id_as_signed_out_of_range(void);
void Cursor_set_str_to_bool(void);
void Cursor_set_str_to_char(void);
void Cursor_set_str_literal_to_char(void);
void Cursor_set_str_to_i8(void);
void Cursor_set_str_to_i16(void);
void Cursor_set_str_to_i32(void);
void Cursor_set_str_to_i64(void);
void Cursor_set_str_to_u64(void);
void Cursor_set_str_to_f32(void);
void Cursor_set_str_to_f64(void);
void Cursor_set_str_to_entity(void);
void Cursor_set_str_to_id(void);
void Cursor_set_str_to_invalid_bool(void);
void Cursor_set_str_to_invalid_entity(void);
void Cursor_set_str_to_invalid_id(void);
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
void Cursor_struct_dotmember(void);
void Cursor_struct_dotmember_assign_twice(void);
void Cursor_struct_member_after_dotmember(void);
void Cursor_struct_next_after_dotmember(void);
void Cursor_struct_pop_after_dotmember(void);
void Cursor_struct_w_array_type_i32_i32(void);
void Cursor_struct_w_array_type_struct(void);
void Cursor_struct_w_2_array_type_i32_i32(void);
void Cursor_struct_w_2_array_type_struct(void);
void Cursor_array_i32_3(void);
void Cursor_array_struct_3(void);
void Cursor_array_move_primitive(void);
void Cursor_array_move_struct(void);
void Cursor_array_move_out_of_range(void);
void Cursor_opaque_set_bool(void);
void Cursor_opaque_set_char(void);
void Cursor_opaque_set_int(void);
void Cursor_opaque_set_uint(void);
void Cursor_opaque_set_float(void);
void Cursor_opaque_set_string(void);
void Cursor_opaque_set_entity(void);
void Cursor_opaque_set_id(void);
void Cursor_opaque_set_int_vec(void);
void Cursor_opaque_set_int_vec_empty(void);
void Cursor_opaque_set_int_vec_resize_smaller(void);
void Cursor_opaque_set_int_vec_resize_larger(void);
void Cursor_opaque_set_int_vec_resize_empty(void);
void Cursor_opaque_set_opaque_int_vec(void);
void Cursor_opaque_set_struct(void);
void Cursor_opaque_set_nested_struct(void);
void Cursor_opaque_set_nested_opaque_struct(void);
void Cursor_opaque_set_array(void);
void Cursor_opaque_set_array_2_of_3_elems(void);
void Cursor_opaque_set_array_empty(void);
void Cursor_opaque_set_int_to_uint(void);
void Cursor_opaque_set_uint_to_int(void);
void Cursor_opaque_set_float_to_int(void);
void Cursor_opaque_set_float_to_uint(void);
void Cursor_opaque_set_int_to_float(void);
void Cursor_opaque_set_uint_to_float(void);
void Cursor_opaque_set_int_to_uint(void);
void Cursor_opaque_set_uint_to_int(void);
void Cursor_opaque_set_string_to_char(void);
void Cursor_opaque_set_int_to_char(void);
void Cursor_opaque_set_uint_to_char(void);
void Cursor_opaque_set_char_to_int(void);
void Cursor_opaque_set_char_to_uint(void);
void Cursor_struct_w_2_opaque_structs(void);
void Cursor_struct_w_2_opaque_vectors(void);
void Cursor_struct_w_2_opaque_arrays(void);
void Cursor_struct_w_3_opaque_structs(void);
void Cursor_struct_w_3_opaque_vectors(void);
void Cursor_struct_w_3_opaque_arrays(void);
void Cursor_opaque_struct_w_opaque_vec(void);
void Cursor_opaque_vec_w_opaque_elem(void);
void Cursor_next_out_of_bounds(void);
void Cursor_set_out_of_bounds(void);
void Cursor_get_member_id(void);

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
void DeserializeFromJson_struct_id(void);
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
void DeserializeFromJson_ser_deser_entity_named(void);
void DeserializeFromJson_ser_deser_entity_named_child(void);
void DeserializeFromJson_ser_deser_entity_namespaced_component(void);
void DeserializeFromJson_deser_entity_1_component_1_member(void);
void DeserializeFromJson_deser_entity_1_component_1_member_w_spaces(void);
void DeserializeFromJson_deser_entity_1_component_2_members(void);
void DeserializeFromJson_deser_entity_2_components(void);
void DeserializeFromJson_deser_entity_2_components_missing_object_close(void);
void DeserializeFromJson_deser_entity_1_component_composite_member(void);
void DeserializeFromJson_deser_entity_1_component_nested_member(void);
void DeserializeFromJson_deser_entity_1_pair(void);
void DeserializeFromJson_deser_entity_2_pairs(void);
void DeserializeFromJson_deser_entity_1_pair_2_targets(void);
void DeserializeFromJson_deser_entity_empty(void);
void DeserializeFromJson_deser_entity_w_path(void);
void DeserializeFromJson_deser_entity_w_path_and_ids(void);
void DeserializeFromJson_deser_entity_w_path_and_ids_and_values(void);
void DeserializeFromJson_deser_entity_w_ids(void);
void DeserializeFromJson_ser_deser_mini(void);
void DeserializeFromJson_ser_deser_init(void);
void DeserializeFromJson_ser_deser_mini_serialize_builtin(void);
void DeserializeFromJson_ser_deser_mini_serialize_modules(void);
void DeserializeFromJson_ser_deser_mini_serialize_builtin_modules(void);
void DeserializeFromJson_ser_deser_init_serialize_builtin(void);
void DeserializeFromJson_ser_deser_init_serialize_modules(void);
void DeserializeFromJson_ser_deser_init_serialize_builtin_modules(void);
void DeserializeFromJson_ser_deser_new_world(void);
void DeserializeFromJson_ser_deser_new_world_1_entity_w_tag(void);
void DeserializeFromJson_ser_deser_new_world_1_entity_w_component(void);
void DeserializeFromJson_ser_deser_new_world_1_entity_w_component_meta(void);
void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_tag(void);
void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component(void);
void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_meta(void);
void DeserializeFromJson_ser_deser_new_world_1_entity_w_tag_serialize_all(void);
void DeserializeFromJson_ser_deser_new_world_1_entity_w_component_serialize_all(void);
void DeserializeFromJson_ser_deser_new_world_1_entity_w_component_meta_serialize_all(void);
void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_tag_serialize_all(void);
void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_serialize_all(void);
void DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_meta_serialize_all(void);
void DeserializeFromJson_ser_deser_new_world_3_entities_w_component_meta(void);
void DeserializeFromJson_ser_deser_new_world_3_entities_w_2_components_meta(void);
void DeserializeFromJson_ser_deser_new_world_4_entities_2_tables_w_component_meta(void);
void DeserializeFromJson_ser_deser_new_world_component_w_anon_entity_member(void);
void DeserializeFromJson_ser_deser_new_world_component_w_named_entity_member(void);
void DeserializeFromJson_ser_deser_new_world_component_w_anon_and_named_entity_member(void);
void DeserializeFromJson_ser_deser_new_world_component_w_anon_entity_with_self(void);
void DeserializeFromJson_ser_deser_new_world_component_w_named_entity_with_self(void);
void DeserializeFromJson_ser_deser_new_world_2_entities_w_anon_parent(void);
void DeserializeFromJson_ser_deser_new_world_2_entities_w_named_parent(void);
void DeserializeFromJson_ser_deser_new_world_2_entities_w_anon_parent_w_cycle(void);
void DeserializeFromJson_ser_deser_new_world_2_entities_w_named_parent_w_cycle(void);
void DeserializeFromJson_ser_deser_new_world_w_prefab(void);
void DeserializeFromJson_ser_deser_new_world_w_disabled(void);
void DeserializeFromJson_ser_deser_restore_1_entity_to_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_entity_to_non_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_anon_entity_to_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_anon_entity_to_non_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_entity_to_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_entity_to_non_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_anon_entity_to_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_anon_entity_to_non_empty_table(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_w_ref(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_ref(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_w_cycle_ref(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_cycle_ref(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_w_recycled(void);
void DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_recycled(void);
void DeserializeFromJson_ser_deser_on_set_3_entities(void);
void DeserializeFromJson_ser_deser_on_set_3_entities_2_restored(void);
void DeserializeFromJson_ser_deser_on_set_3_entities_1_restored(void);
void DeserializeFromJson_ser_deser_3_entities_after_remove_all(void);
void DeserializeFromJson_ser_deser_3_entities_after_delete_with(void);
void DeserializeFromJson_ser_deser_w_hooks(void);
void DeserializeFromJson_ser_deser_large_data(void);
void DeserializeFromJson_ser_deser_different_component_order(void);
void DeserializeFromJson_ser_deser_no_reflection_data(void);
void DeserializeFromJson_ser_deser_no_reflection_data_strict(void);
void DeserializeFromJson_ser_deser_value_for_tag(void);
void DeserializeFromJson_ser_deser_value_for_tag_strict(void);
void DeserializeFromJson_ser_deser_value_for_non_existing(void);
void DeserializeFromJson_ser_deser_value_for_non_existing_strict(void);
void DeserializeFromJson_ser_deser_cpp_typename(void);
void DeserializeFromJson_ser_deser_cpp_template(void);
void DeserializeFromJson_ser_deser_cpp_template_1_param(void);
void DeserializeFromJson_ser_deser_cpp_template_n_params(void);
void DeserializeFromJson_ser_deser_cpp_template_nested(void);
void DeserializeFromJson_ser_deser_cpp_template_n_params_nested(void);
void DeserializeFromJson_ser_deser_long_name(void);
void DeserializeFromJson_ser_deser_long_name_256_chars(void);
void DeserializeFromJson_ser_deser_w_alerts(void);
void DeserializeFromJson_ser_deser_w_alerts_w_progress(void);
void DeserializeFromJson_ser_deser_struct(void);
void DeserializeFromJson_ser_deser_anon_w_same_id_as_existing_named(void);
void DeserializeFromJson_ser_deser_named_to_different_table(void);
void DeserializeFromJson_ser_deser_named_child_to_different_table(void);
void DeserializeFromJson_ser_deser_with_child_tgt(void);
void DeserializeFromJson_ser_deser_with_child_tgt_no_child(void);
void DeserializeFromJson_deser_invalid_entity_name(void);

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
void SerializeToJson_struct_entity_0(void);
void SerializeToJson_struct_entity_10k(void);
void SerializeToJson_struct_entity_after_float(void);
void SerializeToJson_struct_id(void);
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
void SerializeToJson_vector_i32_3(void);
void SerializeToJson_vector_struct_i32_i32(void);
void SerializeToJson_vector_array_i32_3(void);
void SerializeToJson_serialize_from_stage(void);

// Testsuite 'SerializeEntityToJson'
void SerializeEntityToJson_serialize_empty(void);
void SerializeEntityToJson_serialize_w_name(void);
void SerializeEntityToJson_serialize_w_name_1_tag(void);
void SerializeEntityToJson_serialize_w_name_2_tags(void);
void SerializeEntityToJson_serialize_w_name_1_pair(void);
void SerializeEntityToJson_serialize_w_base(void);
void SerializeEntityToJson_serialize_w_2_base(void);
void SerializeEntityToJson_serialize_component_w_base(void);
void SerializeEntityToJson_serialize_component_w_base_no_reflection_data(void);
void SerializeEntityToJson_serialize_component_w_base_w_owned(void);
void SerializeEntityToJson_serialize_component_w_base_w_owned_no_reflection_data(void);
void SerializeEntityToJson_serialize_component_w_base_w_owned_override(void);
void SerializeEntityToJson_serialize_component_w_base_w_owned_no_reflection_data_override(void);
void SerializeEntityToJson_serialize_w_nested_base(void);
void SerializeEntityToJson_serialize_w_1_component(void);
void SerializeEntityToJson_serialize_w_2_components(void);
void SerializeEntityToJson_serialize_w_primitive_component(void);
void SerializeEntityToJson_serialize_w_enum_component(void);
void SerializeEntityToJson_serialize_w_struct_and_enum_component(void);
void SerializeEntityToJson_serialize_w_invalid_enum_component(void);
void SerializeEntityToJson_serialize_w_type_info(void);
void SerializeEntityToJson_serialize_w_type_info_unit(void);
void SerializeEntityToJson_serialize_w_type_info_unit_quantity(void);
void SerializeEntityToJson_serialize_w_type_info_unit_over(void);
void SerializeEntityToJson_serialize_w_type_info_no_types(void);
void SerializeEntityToJson_serialize_w_type_info_no_components(void);
void SerializeEntityToJson_serialize_w_label(void);
void SerializeEntityToJson_serialize_w_label_no_name(void);
void SerializeEntityToJson_serialize_w_brief(void);
void SerializeEntityToJson_serialize_w_brief_no_brief(void);
void SerializeEntityToJson_serialize_w_link(void);
void SerializeEntityToJson_serialize_w_link_no_link(void);
void SerializeEntityToJson_serialize_color(void);
void SerializeEntityToJson_serialize_w_doc_w_quotes(void);
void SerializeEntityToJson_serialize_from_core(void);
void SerializeEntityToJson_serialize_w_1_alert(void);
void SerializeEntityToJson_serialize_w_2_alerts(void);
void SerializeEntityToJson_serialize_w_child_alerts(void);
void SerializeEntityToJson_serialize_w_severity_filter_alert(void);
void SerializeEntityToJson_serialize_w_alerts_not_imported(void);
void SerializeEntityToJson_serialize_w_alerts_no_message(void);
void SerializeEntityToJson_serialize_refs_childof(void);
void SerializeEntityToJson_serialize_refs_custom(void);
void SerializeEntityToJson_serialize_refs_wildcard(void);
void SerializeEntityToJson_serialize_matches_filter(void);
void SerializeEntityToJson_serialize_matches_query(void);
void SerializeEntityToJson_serialize_matches_rule(void);
void SerializeEntityToJson_serialize_no_matches(void);
void SerializeEntityToJson_serialize_id_recycled(void);
void SerializeEntityToJson_serialize_union_target(void);
void SerializeEntityToJson_serialize_union_target_recycled(void);
void SerializeEntityToJson_serialize_anonymous_w_builtin(void);
void SerializeEntityToJson_serialize_named_w_builtin(void);
void SerializeEntityToJson_serialize_named_child_w_builtin(void);
void SerializeEntityToJson_serialize_named_child_w_builtin_w_type_info(void);
void SerializeEntityToJson_serialize_from_stage(void);
void SerializeEntityToJson_serialize_sparse(void);
void SerializeEntityToJson_serialize_sparse_pair(void);
void SerializeEntityToJson_serialize_sparse_mixed(void);
void SerializeEntityToJson_serialize_sparse_inherited(void);
void SerializeEntityToJson_serialize_sparse_inherited_pair(void);
void SerializeEntityToJson_serialize_sparse_inherited_mixed(void);

// Testsuite 'SerializeIterToJson'
void SerializeIterToJson_serialize_1_comps_empty(void);
void SerializeIterToJson_serialize_1_comps_2_ents_same_table(void);
void SerializeIterToJson_serialize_1_tag_2_ents_same_table(void);
void SerializeIterToJson_serialize_2_comps_2_ents_same_table(void);
void SerializeIterToJson_serialize_1_tag_1_comp_2_ents_same_table(void);
void SerializeIterToJson_serialize_1_tag_1_comp_4_ents_two_tables(void);
void SerializeIterToJson_serialize_2_comps_1_owned_2_ents(void);
void SerializeIterToJson_serialize_w_pair_wildcard(void);
void SerializeIterToJson_serialize_w_var(void);
void SerializeIterToJson_serialize_w_2_vars(void);
void SerializeIterToJson_serialize_type_info_1_tags(void);
void SerializeIterToJson_serialize_type_info_2_tags(void);
void SerializeIterToJson_serialize_type_info_1_component(void);
void SerializeIterToJson_serialize_type_info_2_components(void);
void SerializeIterToJson_serialize_type_info_1_struct(void);
void SerializeIterToJson_serialize_type_info_1_component_1_struct(void);
void SerializeIterToJson_serialize_type_info_2_structs(void);
void SerializeIterToJson_serialize_type_info_w_unit(void);
void SerializeIterToJson_serialize_type_info_w_unit_quantity(void);
void SerializeIterToJson_serialize_type_info_w_unit_over(void);
void SerializeIterToJson_serialize_w_entity_label(void);
void SerializeIterToJson_serialize_w_entity_label_w_str(void);
void SerializeIterToJson_serialize_entity_label_w_newline(void);
void SerializeIterToJson_serialize_w_var_labels(void);
void SerializeIterToJson_serialize_w_var_component(void);
void SerializeIterToJson_serialize_w_optional_tag(void);
void SerializeIterToJson_serialize_w_optional_component(void);
void SerializeIterToJson_serialize_w_optional_reflected_component(void);
void SerializeIterToJson_serialize_w_inout_filter_tag(void);
void SerializeIterToJson_serialize_w_inout_filter_component(void);
void SerializeIterToJson_serialize_w_inout_filter_reflected_component(void);
void SerializeIterToJson_serialize_w_inout_out_tag(void);
void SerializeIterToJson_serialize_w_inout_out_component(void);
void SerializeIterToJson_serialize_w_inout_out_reflected_component(void);
void SerializeIterToJson_serialize_component_from_var(void);
void SerializeIterToJson_serialize_color(void);
void SerializeIterToJson_serialize_ids(void);
void SerializeIterToJson_serialize_ids_2_entities(void);
void SerializeIterToJson_serialize_anonymous(void);
void SerializeIterToJson_serialize_anonymous_ids(void);
void SerializeIterToJson_serialize_variable_anonymous_no_full_path(void);
void SerializeIterToJson_serialize_variable_anonymous(void);
void SerializeIterToJson_serialize_anonymous_tag(void);
void SerializeIterToJson_serialize_anonymous_component(void);
void SerializeIterToJson_serialize_anonymous_pair(void);
void SerializeIterToJson_serialize_invalid_value(void);
void SerializeIterToJson_serialize_recycled_pair_id(void);
void SerializeIterToJson_serialize_w_alert(void);
void SerializeIterToJson_serialize_no_this_alert_imported(void);
void SerializeIterToJson_serialize_paged_iterator(void);
void SerializeIterToJson_serialize_paged_iterator_w_optional_component(void);
void SerializeIterToJson_serialize_paged_iterator_w_optional_tag(void);
void SerializeIterToJson_serialize_paged_iterator_w_vars(void);
void SerializeIterToJson_serialize_w_offset(void);
void SerializeIterToJson_serialize_labels_w_offset(void);
void SerializeIterToJson_serialize_colors_w_offset(void);
void SerializeIterToJson_serialize_anonymous_entities_w_offset(void);
void SerializeIterToJson_serialize_table(void);
void SerializeIterToJson_serialize_table_w_id_labels(void);
void SerializeIterToJson_serialize_table_w_var_labels(void);
void SerializeIterToJson_serialize_world(void);
void SerializeIterToJson_serialize_vars_for_query(void);
void SerializeIterToJson_serialize_var_labels_for_query(void);
void SerializeIterToJson_serialize_null_doc_name(void);
void SerializeIterToJson_serialize_rule_w_optional(void);
void SerializeIterToJson_serialize_rule_w_optional_component(void);
void SerializeIterToJson_serialize_entity_w_flecs_core_parent(void);
void SerializeIterToJson_no_fields(void);
void SerializeIterToJson_no_fields_w_vars(void);
void SerializeIterToJson_serialize_from_stage(void);
void SerializeIterToJson_serialize_sparse(void);

// Testsuite 'SerializeIterToRowJson'
void SerializeIterToRowJson_serialize_this_w_1_tag(void);
void SerializeIterToRowJson_serialize_this_w_1_tag_w_parent(void);
void SerializeIterToRowJson_serialize_this_w_1_tag_no_name(void);
void SerializeIterToRowJson_serialize_this_w_2_tag(void);
void SerializeIterToRowJson_serialize_this_w_1_component(void);
void SerializeIterToRowJson_serialize_this_w_2_component(void);
void SerializeIterToRowJson_serialize_this_w_2_component_1_shared(void);
void SerializeIterToRowJson_serialize_this_w_1_pair(void);
void SerializeIterToRowJson_serialize_this_w_2_pair(void);
void SerializeIterToRowJson_serialize_this_w_1_pair_component(void);
void SerializeIterToRowJson_serialize_this_w_1_var(void);
void SerializeIterToRowJson_serialize_this_w_2_var(void);
void SerializeIterToRowJson_serialize_this_w_2_var_doc_name(void);
void SerializeIterToRowJson_serialize_this_w_1_tag_component_pair_var(void);
void SerializeIterToRowJson_serialize_this_w_2_tag_component_pair_var(void);
void SerializeIterToRowJson_serialize_var_w_1_tag(void);
void SerializeIterToRowJson_serialize_var_w_1_component(void);
void SerializeIterToRowJson_serialize_var_w_1_pair(void);
void SerializeIterToRowJson_serialize_var_w_1_var(void);
void SerializeIterToRowJson_serialize_var_w_2_component_1_shared(void);
void SerializeIterToRowJson_serialize_var_w_1_tag_component_pair_var(void);
void SerializeIterToRowJson_serialize_var_w_2_tag_component_pair_var(void);
void SerializeIterToRowJson_serialize_fixed_w_1_tag(void);
void SerializeIterToRowJson_serialize_fixed_w_1_component(void);
void SerializeIterToRowJson_serialize_fixed_w_1_pair(void);
void SerializeIterToRowJson_serialize_fixed_w_1_var(void);
void SerializeIterToRowJson_serialize_fixed_w_2_component_1_shared(void);
void SerializeIterToRowJson_serialize_fixed_w_1_tag_component_pair_var(void);
void SerializeIterToRowJson_serialize_fixed_w_2_tag_component_pair_var(void);
void SerializeIterToRowJson_serialize_not(void);
void SerializeIterToRowJson_serialize_not_pair_wildcard(void);
void SerializeIterToRowJson_serialize_not_pair_var(void);
void SerializeIterToRowJson_serialize_not_pair_var_constrained(void);
void SerializeIterToRowJson_serialize_optional(void);
void SerializeIterToRowJson_serialize_optional_pair_wildcard(void);
void SerializeIterToRowJson_serialize_optional_pair_var(void);
void SerializeIterToRowJson_serialize_optional_pair_var_constrained(void);
void SerializeIterToRowJson_serialize_or(void);
void SerializeIterToRowJson_serialize_scope(void);
void SerializeIterToRowJson_serialize_eq(void);
void SerializeIterToRowJson_serialize_neq(void);
void SerializeIterToRowJson_serialize_eq_m(void);
void SerializeIterToRowJson_serialize_table(void);
void SerializeIterToRowJson_serialize_table_w_eq(void);
void SerializeIterToRowJson_serialize_table_w_neq(void);
void SerializeIterToRowJson_serialize_table_w_2_pair_targets(void);
void SerializeIterToRowJson_serialize_table_w_2_pair_targets_2_rel(void);
void SerializeIterToRowJson_serialize_table_w_3_pair_targets(void);
void SerializeIterToRowJson_serialize_table_w_3_pair_targets_2_rel(void);
void SerializeIterToRowJson_serialize_everything(void);
void SerializeIterToRowJson_serialize_everything_table(void);
void SerializeIterToRowJson_serialize_w_type_info(void);
void SerializeIterToRowJson_serialize_w_field_info(void);
void SerializeIterToRowJson_serialize_w_field_info_pair_w_0_target(void);
void SerializeIterToRowJson_serialize_w_field_info_pair_w_not_tag(void);
void SerializeIterToRowJson_serialize_w_field_info_pair_w_not_pair(void);
void SerializeIterToRowJson_serialize_w_field_info_pair_w_not_component(void);
void SerializeIterToRowJson_serialize_w_field_info_w_or(void);
void SerializeIterToRowJson_serialize_recycled_id(void);
void SerializeIterToRowJson_serialize_entity_w_flecs_core_parent(void);

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
void SerializeTypeInfoToJson_id(void);
void SerializeTypeInfoToJson_enum(void);
void SerializeTypeInfoToJson_bitmask(void);
void SerializeTypeInfoToJson_struct(void);
void SerializeTypeInfoToJson_nested_struct(void);
void SerializeTypeInfoToJson_array_type(void);
void SerializeTypeInfoToJson_vector_type(void);
void SerializeTypeInfoToJson_struct_array_i32_2(void);
void SerializeTypeInfoToJson_struct_array_struct_2(void);
void SerializeTypeInfoToJson_struct_array_type(void);
void SerializeTypeInfoToJson_struct_vector_type(void);
void SerializeTypeInfoToJson_custom_primitive_type(void);
void SerializeTypeInfoToJson_custom_array_type(void);
void SerializeTypeInfoToJson_custom_vector_type(void);
void SerializeTypeInfoToJson_custom_struct_type(void);
void SerializeTypeInfoToJson_struct_w_value_range(void);
void SerializeTypeInfoToJson_struct_w_error_range(void);
void SerializeTypeInfoToJson_struct_w_warning_range(void);
void SerializeTypeInfoToJson_struct_w_error_and_warning_range(void);
void SerializeTypeInfoToJson_struct_nested(void);
void SerializeTypeInfoToJson_struct_nested_2_lvls(void);
void SerializeTypeInfoToJson_struct_nested_2_members(void);
void SerializeTypeInfoToJson_struct_nested_3_members(void);

// Testsuite 'SerializeQueryInfoToJson'
void SerializeQueryInfoToJson_1_tag(void);
void SerializeQueryInfoToJson_1_component(void);
void SerializeQueryInfoToJson_1_pair(void);
void SerializeQueryInfoToJson_1_pair_w_wildcard(void);
void SerializeQueryInfoToJson_1_pair_w_any(void);
void SerializeQueryInfoToJson_1_tag_fixed_src(void);
void SerializeQueryInfoToJson_1_tag_var_src(void);
void SerializeQueryInfoToJson_1_component_in(void);
void SerializeQueryInfoToJson_1_component_inout(void);
void SerializeQueryInfoToJson_1_component_out(void);
void SerializeQueryInfoToJson_1_component_none(void);
void SerializeQueryInfoToJson_1_tag_not(void);
void SerializeQueryInfoToJson_2_tags_or(void);
void SerializeQueryInfoToJson_1_tag_optional(void);
void SerializeQueryInfoToJson_1_tag_self(void);
void SerializeQueryInfoToJson_1_tag_self_dont_inherit(void);
void SerializeQueryInfoToJson_1_tag_up(void);
void SerializeQueryInfoToJson_1_tag_cascade(void);
void SerializeQueryInfoToJson_0_term(void);
void SerializeQueryInfoToJson_anonymous_tag(void);
void SerializeQueryInfoToJson_anonymous_pair(void);
void SerializeQueryInfoToJson_anonymous_component(void);
void SerializeQueryInfoToJson_anonymous_tag_recycled(void);
void SerializeQueryInfoToJson_anonymous_pair_recycled(void);
void SerializeQueryInfoToJson_anonymous_component_recycled(void);

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
void MetaUtils_struct_w_2_nested(void);
void MetaUtils_enum_nospace(void);
void MetaUtils_struct_nospace(void);
void MetaUtils_identifier_w_underscore(void);
void MetaUtils_struct_w_ptr(void);
void MetaUtils_private_members(void);
void MetaUtils_enum_constant_w_name_prefix(void);
void MetaUtils_enum_constant_w_type_prefix(void);
void MetaUtils_enum_constant_w_name_type_prefix(void);

// Testsuite 'OpaqueTypes'
void OpaqueTypes_ser_i32_type_to_json(void);
void OpaqueTypes_ser_string_type_to_json(void);
void OpaqueTypes_ser_vec_i32_type_to_json(void);
void OpaqueTypes_ser_vec_string_type_to_json(void);
void OpaqueTypes_ser_struct_1_member(void);
void OpaqueTypes_ser_struct_2_members(void);
void OpaqueTypes_ser_struct_3_members(void);
void OpaqueTypes_deser_bool_from_json(void);
void OpaqueTypes_deser_char_from_json(void);
void OpaqueTypes_deser_int_from_json(void);
void OpaqueTypes_deser_uint_from_json(void);
void OpaqueTypes_deser_float_from_json(void);
void OpaqueTypes_deser_string_from_json(void);
void OpaqueTypes_deser_entity_from_json(void);
void OpaqueTypes_ser_deser_world_w_ser_opaque(void);
void OpaqueTypes_ser_deser_entity(void);
void OpaqueTypes_ser_deser_0_entity(void);

// Testsuite 'Misc'
void Misc_primitive_from_stage(void);
void Misc_enum_from_stage(void);
void Misc_bitmask_from_stage(void);
void Misc_struct_from_stage(void);
void Misc_array_from_stage(void);
void Misc_vector_from_stage(void);
void Misc_opaque_from_stage(void);
void Misc_unit_from_stage(void);
void Misc_unit_prefix_from_stage(void);
void Misc_quantity_from_stage(void);
void Misc_primitive_from_defer(void);
void Misc_enum_from_defer(void);
void Misc_bitmask_from_defer(void);
void Misc_array_from_defer(void);
void Misc_vector_from_defer(void);
void Misc_struct_from_defer(void);
void Misc_opaque_from_defer(void);
void Misc_unit_from_defer(void);
void Misc_unit_prefix_from_defer(void);
void Misc_quantity_from_defer(void);
void Misc_primitive_from_readonly(void);
void Misc_enum_from_readonly(void);
void Misc_bitmask_from_readonly(void);
void Misc_array_from_readonly(void);
void Misc_vector_from_readonly(void);
void Misc_struct_from_readonly(void);
void Misc_opaque_from_readonly(void);
void Misc_unit_from_readonly(void);
void Misc_unit_prefix_from_readonly(void);
void Misc_quantity_from_readonly(void);
void Misc_primitive_from_suspend_defer(void);
void Misc_enum_from_suspend_defer(void);
void Misc_bitmask_from_suspend_defer(void);
void Misc_array_from_suspend_defer(void);
void Misc_vector_from_suspend_defer(void);
void Misc_struct_from_suspend_defer(void);
void Misc_opaque_from_suspend_defer(void);
void Misc_unit_from_suspend_defer(void);
void Misc_unit_prefix_from_suspend_defer(void);
void Misc_quantity_from_suspend_defer(void);

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
        "id",
        PrimitiveTypes_id
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
        "builtin_id",
        PrimitiveTypes_builtin_id
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
        "struct_w_id",
        PrimitiveTypes_struct_w_id
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
    },
    {
        "value_range",
        StructTypes_value_range
    },
    {
        "error_range",
        StructTypes_error_range
    },
    {
        "warning_range",
        StructTypes_warning_range
    },
    {
        "error_and_warning_range",
        StructTypes_error_and_warning_range
    },
    {
        "error_range_invalid_type",
        StructTypes_error_range_invalid_type
    },
    {
        "warning_range_invalid_type",
        StructTypes_warning_range_invalid_type
    },
    {
        "invalid_value_range",
        StructTypes_invalid_value_range
    },
    {
        "invalid_error_range",
        StructTypes_invalid_error_range
    },
    {
        "invalid_warning_range",
        StructTypes_invalid_warning_range
    },
    {
        "overlapping_error_warning_range",
        StructTypes_overlapping_error_warning_range
    },
    {
        "overlapping_value_error_range",
        StructTypes_overlapping_value_error_range
    },
    {
        "overlapping_value_warning_range",
        StructTypes_overlapping_value_warning_range
    },
    {
        "struct_w_16_alignment",
        StructTypes_struct_w_16_alignment
    },
    {
        "struct_w_use_offset",
        StructTypes_struct_w_use_offset
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
    },
    {
        "import_units_after_mini",
        Units_import_units_after_mini
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
        "ops_id",
        Serialized_ops_id
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
    },
    {
        "ops_missing_metatype",
        Serialized_ops_missing_metatype
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
        "set_id",
        Cursor_set_id
    },
    {
        "set_id_to_number",
        Cursor_set_id_to_number
    },
    {
        "set_id_to_0",
        Cursor_set_id_to_0
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
        "set_id_as_signed",
        Cursor_set_id_as_signed
    },
    {
        "set_id_as_unsigned",
        Cursor_set_id_as_unsigned
    },
    {
        "set_id_as_signed_out_of_range",
        Cursor_set_id_as_signed_out_of_range
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
        "set_str_to_u64",
        Cursor_set_str_to_u64
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
        "set_str_to_id",
        Cursor_set_str_to_id
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
        "set_str_to_invalid_id",
        Cursor_set_str_to_invalid_id
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
        "struct_dotmember",
        Cursor_struct_dotmember
    },
    {
        "struct_dotmember_assign_twice",
        Cursor_struct_dotmember_assign_twice
    },
    {
        "struct_member_after_dotmember",
        Cursor_struct_member_after_dotmember
    },
    {
        "struct_next_after_dotmember",
        Cursor_struct_next_after_dotmember
    },
    {
        "struct_pop_after_dotmember",
        Cursor_struct_pop_after_dotmember
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
        "array_i32_3",
        Cursor_array_i32_3
    },
    {
        "array_struct_3",
        Cursor_array_struct_3
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
    },
    {
        "opaque_set_bool",
        Cursor_opaque_set_bool
    },
    {
        "opaque_set_char",
        Cursor_opaque_set_char
    },
    {
        "opaque_set_int",
        Cursor_opaque_set_int
    },
    {
        "opaque_set_uint",
        Cursor_opaque_set_uint
    },
    {
        "opaque_set_float",
        Cursor_opaque_set_float
    },
    {
        "opaque_set_string",
        Cursor_opaque_set_string
    },
    {
        "opaque_set_entity",
        Cursor_opaque_set_entity
    },
    {
        "opaque_set_id",
        Cursor_opaque_set_id
    },
    {
        "opaque_set_int_vec",
        Cursor_opaque_set_int_vec
    },
    {
        "opaque_set_int_vec_empty",
        Cursor_opaque_set_int_vec_empty
    },
    {
        "opaque_set_int_vec_resize_smaller",
        Cursor_opaque_set_int_vec_resize_smaller
    },
    {
        "opaque_set_int_vec_resize_larger",
        Cursor_opaque_set_int_vec_resize_larger
    },
    {
        "opaque_set_int_vec_resize_empty",
        Cursor_opaque_set_int_vec_resize_empty
    },
    {
        "opaque_set_opaque_int_vec",
        Cursor_opaque_set_opaque_int_vec
    },
    {
        "opaque_set_struct",
        Cursor_opaque_set_struct
    },
    {
        "opaque_set_nested_struct",
        Cursor_opaque_set_nested_struct
    },
    {
        "opaque_set_nested_opaque_struct",
        Cursor_opaque_set_nested_opaque_struct
    },
    {
        "opaque_set_array",
        Cursor_opaque_set_array
    },
    {
        "opaque_set_array_2_of_3_elems",
        Cursor_opaque_set_array_2_of_3_elems
    },
    {
        "opaque_set_array_empty",
        Cursor_opaque_set_array_empty
    },
    {
        "opaque_set_int_to_uint",
        Cursor_opaque_set_int_to_uint
    },
    {
        "opaque_set_uint_to_int",
        Cursor_opaque_set_uint_to_int
    },
    {
        "opaque_set_float_to_int",
        Cursor_opaque_set_float_to_int
    },
    {
        "opaque_set_float_to_uint",
        Cursor_opaque_set_float_to_uint
    },
    {
        "opaque_set_int_to_float",
        Cursor_opaque_set_int_to_float
    },
    {
        "opaque_set_uint_to_float",
        Cursor_opaque_set_uint_to_float
    },
    {
        "opaque_set_int_to_uint",
        Cursor_opaque_set_int_to_uint
    },
    {
        "opaque_set_uint_to_int",
        Cursor_opaque_set_uint_to_int
    },
    {
        "opaque_set_string_to_char",
        Cursor_opaque_set_string_to_char
    },
    {
        "opaque_set_int_to_char",
        Cursor_opaque_set_int_to_char
    },
    {
        "opaque_set_uint_to_char",
        Cursor_opaque_set_uint_to_char
    },
    {
        "opaque_set_char_to_int",
        Cursor_opaque_set_char_to_int
    },
    {
        "opaque_set_char_to_uint",
        Cursor_opaque_set_char_to_uint
    },
    {
        "struct_w_2_opaque_structs",
        Cursor_struct_w_2_opaque_structs
    },
    {
        "struct_w_2_opaque_vectors",
        Cursor_struct_w_2_opaque_vectors
    },
    {
        "struct_w_2_opaque_arrays",
        Cursor_struct_w_2_opaque_arrays
    },
    {
        "struct_w_3_opaque_structs",
        Cursor_struct_w_3_opaque_structs
    },
    {
        "struct_w_3_opaque_vectors",
        Cursor_struct_w_3_opaque_vectors
    },
    {
        "struct_w_3_opaque_arrays",
        Cursor_struct_w_3_opaque_arrays
    },
    {
        "opaque_struct_w_opaque_vec",
        Cursor_opaque_struct_w_opaque_vec
    },
    {
        "opaque_vec_w_opaque_elem",
        Cursor_opaque_vec_w_opaque_elem
    },
    {
        "next_out_of_bounds",
        Cursor_next_out_of_bounds
    },
    {
        "set_out_of_bounds",
        Cursor_set_out_of_bounds
    },
    {
        "get_member_id",
        Cursor_get_member_id
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
        "struct_id",
        DeserializeFromJson_struct_id
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
        "ser_deser_entity_named",
        DeserializeFromJson_ser_deser_entity_named
    },
    {
        "ser_deser_entity_named_child",
        DeserializeFromJson_ser_deser_entity_named_child
    },
    {
        "ser_deser_entity_namespaced_component",
        DeserializeFromJson_ser_deser_entity_namespaced_component
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
        "deser_entity_2_components_missing_object_close",
        DeserializeFromJson_deser_entity_2_components_missing_object_close
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
    },
    {
        "deser_entity_1_pair_2_targets",
        DeserializeFromJson_deser_entity_1_pair_2_targets
    },
    {
        "deser_entity_empty",
        DeserializeFromJson_deser_entity_empty
    },
    {
        "deser_entity_w_path",
        DeserializeFromJson_deser_entity_w_path
    },
    {
        "deser_entity_w_path_and_ids",
        DeserializeFromJson_deser_entity_w_path_and_ids
    },
    {
        "deser_entity_w_path_and_ids_and_values",
        DeserializeFromJson_deser_entity_w_path_and_ids_and_values
    },
    {
        "deser_entity_w_ids",
        DeserializeFromJson_deser_entity_w_ids
    },
    {
        "ser_deser_mini",
        DeserializeFromJson_ser_deser_mini
    },
    {
        "ser_deser_init",
        DeserializeFromJson_ser_deser_init
    },
    {
        "ser_deser_mini_serialize_builtin",
        DeserializeFromJson_ser_deser_mini_serialize_builtin
    },
    {
        "ser_deser_mini_serialize_modules",
        DeserializeFromJson_ser_deser_mini_serialize_modules
    },
    {
        "ser_deser_mini_serialize_builtin_modules",
        DeserializeFromJson_ser_deser_mini_serialize_builtin_modules
    },
    {
        "ser_deser_init_serialize_builtin",
        DeserializeFromJson_ser_deser_init_serialize_builtin
    },
    {
        "ser_deser_init_serialize_modules",
        DeserializeFromJson_ser_deser_init_serialize_modules
    },
    {
        "ser_deser_init_serialize_builtin_modules",
        DeserializeFromJson_ser_deser_init_serialize_builtin_modules
    },
    {
        "ser_deser_new_world",
        DeserializeFromJson_ser_deser_new_world
    },
    {
        "ser_deser_new_world_1_entity_w_tag",
        DeserializeFromJson_ser_deser_new_world_1_entity_w_tag
    },
    {
        "ser_deser_new_world_1_entity_w_component",
        DeserializeFromJson_ser_deser_new_world_1_entity_w_component
    },
    {
        "ser_deser_new_world_1_entity_w_component_meta",
        DeserializeFromJson_ser_deser_new_world_1_entity_w_component_meta
    },
    {
        "ser_deser_new_world_1_named_entity_w_tag",
        DeserializeFromJson_ser_deser_new_world_1_named_entity_w_tag
    },
    {
        "ser_deser_new_world_1_named_entity_w_component",
        DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component
    },
    {
        "ser_deser_new_world_1_named_entity_w_component_meta",
        DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_meta
    },
    {
        "ser_deser_new_world_1_entity_w_tag_serialize_all",
        DeserializeFromJson_ser_deser_new_world_1_entity_w_tag_serialize_all
    },
    {
        "ser_deser_new_world_1_entity_w_component_serialize_all",
        DeserializeFromJson_ser_deser_new_world_1_entity_w_component_serialize_all
    },
    {
        "ser_deser_new_world_1_entity_w_component_meta_serialize_all",
        DeserializeFromJson_ser_deser_new_world_1_entity_w_component_meta_serialize_all
    },
    {
        "ser_deser_new_world_1_named_entity_w_tag_serialize_all",
        DeserializeFromJson_ser_deser_new_world_1_named_entity_w_tag_serialize_all
    },
    {
        "ser_deser_new_world_1_named_entity_w_component_serialize_all",
        DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_serialize_all
    },
    {
        "ser_deser_new_world_1_named_entity_w_component_meta_serialize_all",
        DeserializeFromJson_ser_deser_new_world_1_named_entity_w_component_meta_serialize_all
    },
    {
        "ser_deser_new_world_3_entities_w_component_meta",
        DeserializeFromJson_ser_deser_new_world_3_entities_w_component_meta
    },
    {
        "ser_deser_new_world_3_entities_w_2_components_meta",
        DeserializeFromJson_ser_deser_new_world_3_entities_w_2_components_meta
    },
    {
        "ser_deser_new_world_4_entities_2_tables_w_component_meta",
        DeserializeFromJson_ser_deser_new_world_4_entities_2_tables_w_component_meta
    },
    {
        "ser_deser_new_world_component_w_anon_entity_member",
        DeserializeFromJson_ser_deser_new_world_component_w_anon_entity_member
    },
    {
        "ser_deser_new_world_component_w_named_entity_member",
        DeserializeFromJson_ser_deser_new_world_component_w_named_entity_member
    },
    {
        "ser_deser_new_world_component_w_anon_and_named_entity_member",
        DeserializeFromJson_ser_deser_new_world_component_w_anon_and_named_entity_member
    },
    {
        "ser_deser_new_world_component_w_anon_entity_with_self",
        DeserializeFromJson_ser_deser_new_world_component_w_anon_entity_with_self
    },
    {
        "ser_deser_new_world_component_w_named_entity_with_self",
        DeserializeFromJson_ser_deser_new_world_component_w_named_entity_with_self
    },
    {
        "ser_deser_new_world_2_entities_w_anon_parent",
        DeserializeFromJson_ser_deser_new_world_2_entities_w_anon_parent
    },
    {
        "ser_deser_new_world_2_entities_w_named_parent",
        DeserializeFromJson_ser_deser_new_world_2_entities_w_named_parent
    },
    {
        "ser_deser_new_world_2_entities_w_anon_parent_w_cycle",
        DeserializeFromJson_ser_deser_new_world_2_entities_w_anon_parent_w_cycle
    },
    {
        "ser_deser_new_world_2_entities_w_named_parent_w_cycle",
        DeserializeFromJson_ser_deser_new_world_2_entities_w_named_parent_w_cycle
    },
    {
        "ser_deser_new_world_w_prefab",
        DeserializeFromJson_ser_deser_new_world_w_prefab
    },
    {
        "ser_deser_new_world_w_disabled",
        DeserializeFromJson_ser_deser_new_world_w_disabled
    },
    {
        "ser_deser_restore_1_entity_to_empty_table",
        DeserializeFromJson_ser_deser_restore_1_entity_to_empty_table
    },
    {
        "ser_deser_restore_1_entity_to_non_empty_table",
        DeserializeFromJson_ser_deser_restore_1_entity_to_non_empty_table
    },
    {
        "ser_deser_restore_1_anon_entity_to_empty_table",
        DeserializeFromJson_ser_deser_restore_1_anon_entity_to_empty_table
    },
    {
        "ser_deser_restore_1_anon_entity_to_non_empty_table",
        DeserializeFromJson_ser_deser_restore_1_anon_entity_to_non_empty_table
    },
    {
        "ser_deser_restore_1_deleted_entity_to_empty_table",
        DeserializeFromJson_ser_deser_restore_1_deleted_entity_to_empty_table
    },
    {
        "ser_deser_restore_1_deleted_entity_to_non_empty_table",
        DeserializeFromJson_ser_deser_restore_1_deleted_entity_to_non_empty_table
    },
    {
        "ser_deser_restore_1_deleted_anon_entity_to_empty_table",
        DeserializeFromJson_ser_deser_restore_1_deleted_anon_entity_to_empty_table
    },
    {
        "ser_deser_restore_1_deleted_anon_entity_to_non_empty_table",
        DeserializeFromJson_ser_deser_restore_1_deleted_anon_entity_to_non_empty_table
    },
    {
        "ser_deser_restore_1_deleted_w_ref",
        DeserializeFromJson_ser_deser_restore_1_deleted_w_ref
    },
    {
        "ser_deser_restore_1_deleted_anon_w_ref",
        DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_ref
    },
    {
        "ser_deser_restore_1_deleted_w_cycle_ref",
        DeserializeFromJson_ser_deser_restore_1_deleted_w_cycle_ref
    },
    {
        "ser_deser_restore_1_deleted_anon_w_cycle_ref",
        DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_cycle_ref
    },
    {
        "ser_deser_restore_1_deleted_w_recycled",
        DeserializeFromJson_ser_deser_restore_1_deleted_w_recycled
    },
    {
        "ser_deser_restore_1_deleted_anon_w_recycled",
        DeserializeFromJson_ser_deser_restore_1_deleted_anon_w_recycled
    },
    {
        "ser_deser_on_set_3_entities",
        DeserializeFromJson_ser_deser_on_set_3_entities
    },
    {
        "ser_deser_on_set_3_entities_2_restored",
        DeserializeFromJson_ser_deser_on_set_3_entities_2_restored
    },
    {
        "ser_deser_on_set_3_entities_1_restored",
        DeserializeFromJson_ser_deser_on_set_3_entities_1_restored
    },
    {
        "ser_deser_3_entities_after_remove_all",
        DeserializeFromJson_ser_deser_3_entities_after_remove_all
    },
    {
        "ser_deser_3_entities_after_delete_with",
        DeserializeFromJson_ser_deser_3_entities_after_delete_with
    },
    {
        "ser_deser_w_hooks",
        DeserializeFromJson_ser_deser_w_hooks
    },
    {
        "ser_deser_large_data",
        DeserializeFromJson_ser_deser_large_data
    },
    {
        "ser_deser_different_component_order",
        DeserializeFromJson_ser_deser_different_component_order
    },
    {
        "ser_deser_no_reflection_data",
        DeserializeFromJson_ser_deser_no_reflection_data
    },
    {
        "ser_deser_no_reflection_data_strict",
        DeserializeFromJson_ser_deser_no_reflection_data_strict
    },
    {
        "ser_deser_value_for_tag",
        DeserializeFromJson_ser_deser_value_for_tag
    },
    {
        "ser_deser_value_for_tag_strict",
        DeserializeFromJson_ser_deser_value_for_tag_strict
    },
    {
        "ser_deser_value_for_non_existing",
        DeserializeFromJson_ser_deser_value_for_non_existing
    },
    {
        "ser_deser_value_for_non_existing_strict",
        DeserializeFromJson_ser_deser_value_for_non_existing_strict
    },
    {
        "ser_deser_cpp_typename",
        DeserializeFromJson_ser_deser_cpp_typename
    },
    {
        "ser_deser_cpp_template",
        DeserializeFromJson_ser_deser_cpp_template
    },
    {
        "ser_deser_cpp_template_1_param",
        DeserializeFromJson_ser_deser_cpp_template_1_param
    },
    {
        "ser_deser_cpp_template_n_params",
        DeserializeFromJson_ser_deser_cpp_template_n_params
    },
    {
        "ser_deser_cpp_template_nested",
        DeserializeFromJson_ser_deser_cpp_template_nested
    },
    {
        "ser_deser_cpp_template_n_params_nested",
        DeserializeFromJson_ser_deser_cpp_template_n_params_nested
    },
    {
        "ser_deser_long_name",
        DeserializeFromJson_ser_deser_long_name
    },
    {
        "ser_deser_long_name_256_chars",
        DeserializeFromJson_ser_deser_long_name_256_chars
    },
    {
        "ser_deser_w_alerts",
        DeserializeFromJson_ser_deser_w_alerts
    },
    {
        "ser_deser_w_alerts_w_progress",
        DeserializeFromJson_ser_deser_w_alerts_w_progress
    },
    {
        "ser_deser_struct",
        DeserializeFromJson_ser_deser_struct
    },
    {
        "ser_deser_anon_w_same_id_as_existing_named",
        DeserializeFromJson_ser_deser_anon_w_same_id_as_existing_named
    },
    {
        "ser_deser_named_to_different_table",
        DeserializeFromJson_ser_deser_named_to_different_table
    },
    {
        "ser_deser_named_child_to_different_table",
        DeserializeFromJson_ser_deser_named_child_to_different_table
    },
    {
        "ser_deser_with_child_tgt",
        DeserializeFromJson_ser_deser_with_child_tgt
    },
    {
        "ser_deser_with_child_tgt_no_child",
        DeserializeFromJson_ser_deser_with_child_tgt_no_child
    },
    {
        "deser_invalid_entity_name",
        DeserializeFromJson_deser_invalid_entity_name
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
        "struct_entity_0",
        SerializeToJson_struct_entity_0
    },
    {
        "struct_entity_10k",
        SerializeToJson_struct_entity_10k
    },
    {
        "struct_entity_after_float",
        SerializeToJson_struct_entity_after_float
    },
    {
        "struct_id",
        SerializeToJson_struct_id
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
        "vector_i32_3",
        SerializeToJson_vector_i32_3
    },
    {
        "vector_struct_i32_i32",
        SerializeToJson_vector_struct_i32_i32
    },
    {
        "vector_array_i32_3",
        SerializeToJson_vector_array_i32_3
    },
    {
        "serialize_from_stage",
        SerializeToJson_serialize_from_stage
    }
};

bake_test_case SerializeEntityToJson_testcases[] = {
    {
        "serialize_empty",
        SerializeEntityToJson_serialize_empty
    },
    {
        "serialize_w_name",
        SerializeEntityToJson_serialize_w_name
    },
    {
        "serialize_w_name_1_tag",
        SerializeEntityToJson_serialize_w_name_1_tag
    },
    {
        "serialize_w_name_2_tags",
        SerializeEntityToJson_serialize_w_name_2_tags
    },
    {
        "serialize_w_name_1_pair",
        SerializeEntityToJson_serialize_w_name_1_pair
    },
    {
        "serialize_w_base",
        SerializeEntityToJson_serialize_w_base
    },
    {
        "serialize_w_2_base",
        SerializeEntityToJson_serialize_w_2_base
    },
    {
        "serialize_component_w_base",
        SerializeEntityToJson_serialize_component_w_base
    },
    {
        "serialize_component_w_base_no_reflection_data",
        SerializeEntityToJson_serialize_component_w_base_no_reflection_data
    },
    {
        "serialize_component_w_base_w_owned",
        SerializeEntityToJson_serialize_component_w_base_w_owned
    },
    {
        "serialize_component_w_base_w_owned_no_reflection_data",
        SerializeEntityToJson_serialize_component_w_base_w_owned_no_reflection_data
    },
    {
        "serialize_component_w_base_w_owned_override",
        SerializeEntityToJson_serialize_component_w_base_w_owned_override
    },
    {
        "serialize_component_w_base_w_owned_no_reflection_data_override",
        SerializeEntityToJson_serialize_component_w_base_w_owned_no_reflection_data_override
    },
    {
        "serialize_w_nested_base",
        SerializeEntityToJson_serialize_w_nested_base
    },
    {
        "serialize_w_1_component",
        SerializeEntityToJson_serialize_w_1_component
    },
    {
        "serialize_w_2_components",
        SerializeEntityToJson_serialize_w_2_components
    },
    {
        "serialize_w_primitive_component",
        SerializeEntityToJson_serialize_w_primitive_component
    },
    {
        "serialize_w_enum_component",
        SerializeEntityToJson_serialize_w_enum_component
    },
    {
        "serialize_w_struct_and_enum_component",
        SerializeEntityToJson_serialize_w_struct_and_enum_component
    },
    {
        "serialize_w_invalid_enum_component",
        SerializeEntityToJson_serialize_w_invalid_enum_component
    },
    {
        "serialize_w_type_info",
        SerializeEntityToJson_serialize_w_type_info
    },
    {
        "serialize_w_type_info_unit",
        SerializeEntityToJson_serialize_w_type_info_unit
    },
    {
        "serialize_w_type_info_unit_quantity",
        SerializeEntityToJson_serialize_w_type_info_unit_quantity
    },
    {
        "serialize_w_type_info_unit_over",
        SerializeEntityToJson_serialize_w_type_info_unit_over
    },
    {
        "serialize_w_type_info_no_types",
        SerializeEntityToJson_serialize_w_type_info_no_types
    },
    {
        "serialize_w_type_info_no_components",
        SerializeEntityToJson_serialize_w_type_info_no_components
    },
    {
        "serialize_w_label",
        SerializeEntityToJson_serialize_w_label
    },
    {
        "serialize_w_label_no_name",
        SerializeEntityToJson_serialize_w_label_no_name
    },
    {
        "serialize_w_brief",
        SerializeEntityToJson_serialize_w_brief
    },
    {
        "serialize_w_brief_no_brief",
        SerializeEntityToJson_serialize_w_brief_no_brief
    },
    {
        "serialize_w_link",
        SerializeEntityToJson_serialize_w_link
    },
    {
        "serialize_w_link_no_link",
        SerializeEntityToJson_serialize_w_link_no_link
    },
    {
        "serialize_color",
        SerializeEntityToJson_serialize_color
    },
    {
        "serialize_w_doc_w_quotes",
        SerializeEntityToJson_serialize_w_doc_w_quotes
    },
    {
        "serialize_from_core",
        SerializeEntityToJson_serialize_from_core
    },
    {
        "serialize_w_1_alert",
        SerializeEntityToJson_serialize_w_1_alert
    },
    {
        "serialize_w_2_alerts",
        SerializeEntityToJson_serialize_w_2_alerts
    },
    {
        "serialize_w_child_alerts",
        SerializeEntityToJson_serialize_w_child_alerts
    },
    {
        "serialize_w_severity_filter_alert",
        SerializeEntityToJson_serialize_w_severity_filter_alert
    },
    {
        "serialize_w_alerts_not_imported",
        SerializeEntityToJson_serialize_w_alerts_not_imported
    },
    {
        "serialize_w_alerts_no_message",
        SerializeEntityToJson_serialize_w_alerts_no_message
    },
    {
        "serialize_refs_childof",
        SerializeEntityToJson_serialize_refs_childof
    },
    {
        "serialize_refs_custom",
        SerializeEntityToJson_serialize_refs_custom
    },
    {
        "serialize_refs_wildcard",
        SerializeEntityToJson_serialize_refs_wildcard
    },
    {
        "serialize_matches_filter",
        SerializeEntityToJson_serialize_matches_filter
    },
    {
        "serialize_matches_query",
        SerializeEntityToJson_serialize_matches_query
    },
    {
        "serialize_matches_rule",
        SerializeEntityToJson_serialize_matches_rule
    },
    {
        "serialize_no_matches",
        SerializeEntityToJson_serialize_no_matches
    },
    {
        "serialize_id_recycled",
        SerializeEntityToJson_serialize_id_recycled
    },
    {
        "serialize_union_target",
        SerializeEntityToJson_serialize_union_target
    },
    {
        "serialize_union_target_recycled",
        SerializeEntityToJson_serialize_union_target_recycled
    },
    {
        "serialize_anonymous_w_builtin",
        SerializeEntityToJson_serialize_anonymous_w_builtin
    },
    {
        "serialize_named_w_builtin",
        SerializeEntityToJson_serialize_named_w_builtin
    },
    {
        "serialize_named_child_w_builtin",
        SerializeEntityToJson_serialize_named_child_w_builtin
    },
    {
        "serialize_named_child_w_builtin_w_type_info",
        SerializeEntityToJson_serialize_named_child_w_builtin_w_type_info
    },
    {
        "serialize_from_stage",
        SerializeEntityToJson_serialize_from_stage
    },
    {
        "serialize_sparse",
        SerializeEntityToJson_serialize_sparse
    },
    {
        "serialize_sparse_pair",
        SerializeEntityToJson_serialize_sparse_pair
    },
    {
        "serialize_sparse_mixed",
        SerializeEntityToJson_serialize_sparse_mixed
    },
    {
        "serialize_sparse_inherited",
        SerializeEntityToJson_serialize_sparse_inherited
    },
    {
        "serialize_sparse_inherited_pair",
        SerializeEntityToJson_serialize_sparse_inherited_pair
    },
    {
        "serialize_sparse_inherited_mixed",
        SerializeEntityToJson_serialize_sparse_inherited_mixed
    }
};

bake_test_case SerializeIterToJson_testcases[] = {
    {
        "serialize_1_comps_empty",
        SerializeIterToJson_serialize_1_comps_empty
    },
    {
        "serialize_1_comps_2_ents_same_table",
        SerializeIterToJson_serialize_1_comps_2_ents_same_table
    },
    {
        "serialize_1_tag_2_ents_same_table",
        SerializeIterToJson_serialize_1_tag_2_ents_same_table
    },
    {
        "serialize_2_comps_2_ents_same_table",
        SerializeIterToJson_serialize_2_comps_2_ents_same_table
    },
    {
        "serialize_1_tag_1_comp_2_ents_same_table",
        SerializeIterToJson_serialize_1_tag_1_comp_2_ents_same_table
    },
    {
        "serialize_1_tag_1_comp_4_ents_two_tables",
        SerializeIterToJson_serialize_1_tag_1_comp_4_ents_two_tables
    },
    {
        "serialize_2_comps_1_owned_2_ents",
        SerializeIterToJson_serialize_2_comps_1_owned_2_ents
    },
    {
        "serialize_w_pair_wildcard",
        SerializeIterToJson_serialize_w_pair_wildcard
    },
    {
        "serialize_w_var",
        SerializeIterToJson_serialize_w_var
    },
    {
        "serialize_w_2_vars",
        SerializeIterToJson_serialize_w_2_vars
    },
    {
        "serialize_type_info_1_tags",
        SerializeIterToJson_serialize_type_info_1_tags
    },
    {
        "serialize_type_info_2_tags",
        SerializeIterToJson_serialize_type_info_2_tags
    },
    {
        "serialize_type_info_1_component",
        SerializeIterToJson_serialize_type_info_1_component
    },
    {
        "serialize_type_info_2_components",
        SerializeIterToJson_serialize_type_info_2_components
    },
    {
        "serialize_type_info_1_struct",
        SerializeIterToJson_serialize_type_info_1_struct
    },
    {
        "serialize_type_info_1_component_1_struct",
        SerializeIterToJson_serialize_type_info_1_component_1_struct
    },
    {
        "serialize_type_info_2_structs",
        SerializeIterToJson_serialize_type_info_2_structs
    },
    {
        "serialize_type_info_w_unit",
        SerializeIterToJson_serialize_type_info_w_unit
    },
    {
        "serialize_type_info_w_unit_quantity",
        SerializeIterToJson_serialize_type_info_w_unit_quantity
    },
    {
        "serialize_type_info_w_unit_over",
        SerializeIterToJson_serialize_type_info_w_unit_over
    },
    {
        "serialize_w_entity_label",
        SerializeIterToJson_serialize_w_entity_label
    },
    {
        "serialize_w_entity_label_w_str",
        SerializeIterToJson_serialize_w_entity_label_w_str
    },
    {
        "serialize_entity_label_w_newline",
        SerializeIterToJson_serialize_entity_label_w_newline
    },
    {
        "serialize_w_var_labels",
        SerializeIterToJson_serialize_w_var_labels
    },
    {
        "serialize_w_var_component",
        SerializeIterToJson_serialize_w_var_component
    },
    {
        "serialize_w_optional_tag",
        SerializeIterToJson_serialize_w_optional_tag
    },
    {
        "serialize_w_optional_component",
        SerializeIterToJson_serialize_w_optional_component
    },
    {
        "serialize_w_optional_reflected_component",
        SerializeIterToJson_serialize_w_optional_reflected_component
    },
    {
        "serialize_w_inout_filter_tag",
        SerializeIterToJson_serialize_w_inout_filter_tag
    },
    {
        "serialize_w_inout_filter_component",
        SerializeIterToJson_serialize_w_inout_filter_component
    },
    {
        "serialize_w_inout_filter_reflected_component",
        SerializeIterToJson_serialize_w_inout_filter_reflected_component
    },
    {
        "serialize_w_inout_out_tag",
        SerializeIterToJson_serialize_w_inout_out_tag
    },
    {
        "serialize_w_inout_out_component",
        SerializeIterToJson_serialize_w_inout_out_component
    },
    {
        "serialize_w_inout_out_reflected_component",
        SerializeIterToJson_serialize_w_inout_out_reflected_component
    },
    {
        "serialize_component_from_var",
        SerializeIterToJson_serialize_component_from_var
    },
    {
        "serialize_color",
        SerializeIterToJson_serialize_color
    },
    {
        "serialize_ids",
        SerializeIterToJson_serialize_ids
    },
    {
        "serialize_ids_2_entities",
        SerializeIterToJson_serialize_ids_2_entities
    },
    {
        "serialize_anonymous",
        SerializeIterToJson_serialize_anonymous
    },
    {
        "serialize_anonymous_ids",
        SerializeIterToJson_serialize_anonymous_ids
    },
    {
        "serialize_variable_anonymous_no_full_path",
        SerializeIterToJson_serialize_variable_anonymous_no_full_path
    },
    {
        "serialize_variable_anonymous",
        SerializeIterToJson_serialize_variable_anonymous
    },
    {
        "serialize_anonymous_tag",
        SerializeIterToJson_serialize_anonymous_tag
    },
    {
        "serialize_anonymous_component",
        SerializeIterToJson_serialize_anonymous_component
    },
    {
        "serialize_anonymous_pair",
        SerializeIterToJson_serialize_anonymous_pair
    },
    {
        "serialize_invalid_value",
        SerializeIterToJson_serialize_invalid_value
    },
    {
        "serialize_recycled_pair_id",
        SerializeIterToJson_serialize_recycled_pair_id
    },
    {
        "serialize_w_alert",
        SerializeIterToJson_serialize_w_alert
    },
    {
        "serialize_no_this_alert_imported",
        SerializeIterToJson_serialize_no_this_alert_imported
    },
    {
        "serialize_paged_iterator",
        SerializeIterToJson_serialize_paged_iterator
    },
    {
        "serialize_paged_iterator_w_optional_component",
        SerializeIterToJson_serialize_paged_iterator_w_optional_component
    },
    {
        "serialize_paged_iterator_w_optional_tag",
        SerializeIterToJson_serialize_paged_iterator_w_optional_tag
    },
    {
        "serialize_paged_iterator_w_vars",
        SerializeIterToJson_serialize_paged_iterator_w_vars
    },
    {
        "serialize_w_offset",
        SerializeIterToJson_serialize_w_offset
    },
    {
        "serialize_labels_w_offset",
        SerializeIterToJson_serialize_labels_w_offset
    },
    {
        "serialize_colors_w_offset",
        SerializeIterToJson_serialize_colors_w_offset
    },
    {
        "serialize_anonymous_entities_w_offset",
        SerializeIterToJson_serialize_anonymous_entities_w_offset
    },
    {
        "serialize_table",
        SerializeIterToJson_serialize_table
    },
    {
        "serialize_table_w_id_labels",
        SerializeIterToJson_serialize_table_w_id_labels
    },
    {
        "serialize_table_w_var_labels",
        SerializeIterToJson_serialize_table_w_var_labels
    },
    {
        "serialize_world",
        SerializeIterToJson_serialize_world
    },
    {
        "serialize_vars_for_query",
        SerializeIterToJson_serialize_vars_for_query
    },
    {
        "serialize_var_labels_for_query",
        SerializeIterToJson_serialize_var_labels_for_query
    },
    {
        "serialize_null_doc_name",
        SerializeIterToJson_serialize_null_doc_name
    },
    {
        "serialize_rule_w_optional",
        SerializeIterToJson_serialize_rule_w_optional
    },
    {
        "serialize_rule_w_optional_component",
        SerializeIterToJson_serialize_rule_w_optional_component
    },
    {
        "serialize_entity_w_flecs_core_parent",
        SerializeIterToJson_serialize_entity_w_flecs_core_parent
    },
    {
        "no_fields",
        SerializeIterToJson_no_fields
    },
    {
        "no_fields_w_vars",
        SerializeIterToJson_no_fields_w_vars
    },
    {
        "serialize_from_stage",
        SerializeIterToJson_serialize_from_stage
    },
    {
        "serialize_sparse",
        SerializeIterToJson_serialize_sparse
    }
};

bake_test_case SerializeIterToRowJson_testcases[] = {
    {
        "serialize_this_w_1_tag",
        SerializeIterToRowJson_serialize_this_w_1_tag
    },
    {
        "serialize_this_w_1_tag_w_parent",
        SerializeIterToRowJson_serialize_this_w_1_tag_w_parent
    },
    {
        "serialize_this_w_1_tag_no_name",
        SerializeIterToRowJson_serialize_this_w_1_tag_no_name
    },
    {
        "serialize_this_w_2_tag",
        SerializeIterToRowJson_serialize_this_w_2_tag
    },
    {
        "serialize_this_w_1_component",
        SerializeIterToRowJson_serialize_this_w_1_component
    },
    {
        "serialize_this_w_2_component",
        SerializeIterToRowJson_serialize_this_w_2_component
    },
    {
        "serialize_this_w_2_component_1_shared",
        SerializeIterToRowJson_serialize_this_w_2_component_1_shared
    },
    {
        "serialize_this_w_1_pair",
        SerializeIterToRowJson_serialize_this_w_1_pair
    },
    {
        "serialize_this_w_2_pair",
        SerializeIterToRowJson_serialize_this_w_2_pair
    },
    {
        "serialize_this_w_1_pair_component",
        SerializeIterToRowJson_serialize_this_w_1_pair_component
    },
    {
        "serialize_this_w_1_var",
        SerializeIterToRowJson_serialize_this_w_1_var
    },
    {
        "serialize_this_w_2_var",
        SerializeIterToRowJson_serialize_this_w_2_var
    },
    {
        "serialize_this_w_2_var_doc_name",
        SerializeIterToRowJson_serialize_this_w_2_var_doc_name
    },
    {
        "serialize_this_w_1_tag_component_pair_var",
        SerializeIterToRowJson_serialize_this_w_1_tag_component_pair_var
    },
    {
        "serialize_this_w_2_tag_component_pair_var",
        SerializeIterToRowJson_serialize_this_w_2_tag_component_pair_var
    },
    {
        "serialize_var_w_1_tag",
        SerializeIterToRowJson_serialize_var_w_1_tag
    },
    {
        "serialize_var_w_1_component",
        SerializeIterToRowJson_serialize_var_w_1_component
    },
    {
        "serialize_var_w_1_pair",
        SerializeIterToRowJson_serialize_var_w_1_pair
    },
    {
        "serialize_var_w_1_var",
        SerializeIterToRowJson_serialize_var_w_1_var
    },
    {
        "serialize_var_w_2_component_1_shared",
        SerializeIterToRowJson_serialize_var_w_2_component_1_shared
    },
    {
        "serialize_var_w_1_tag_component_pair_var",
        SerializeIterToRowJson_serialize_var_w_1_tag_component_pair_var
    },
    {
        "serialize_var_w_2_tag_component_pair_var",
        SerializeIterToRowJson_serialize_var_w_2_tag_component_pair_var
    },
    {
        "serialize_fixed_w_1_tag",
        SerializeIterToRowJson_serialize_fixed_w_1_tag
    },
    {
        "serialize_fixed_w_1_component",
        SerializeIterToRowJson_serialize_fixed_w_1_component
    },
    {
        "serialize_fixed_w_1_pair",
        SerializeIterToRowJson_serialize_fixed_w_1_pair
    },
    {
        "serialize_fixed_w_1_var",
        SerializeIterToRowJson_serialize_fixed_w_1_var
    },
    {
        "serialize_fixed_w_2_component_1_shared",
        SerializeIterToRowJson_serialize_fixed_w_2_component_1_shared
    },
    {
        "serialize_fixed_w_1_tag_component_pair_var",
        SerializeIterToRowJson_serialize_fixed_w_1_tag_component_pair_var
    },
    {
        "serialize_fixed_w_2_tag_component_pair_var",
        SerializeIterToRowJson_serialize_fixed_w_2_tag_component_pair_var
    },
    {
        "serialize_not",
        SerializeIterToRowJson_serialize_not
    },
    {
        "serialize_not_pair_wildcard",
        SerializeIterToRowJson_serialize_not_pair_wildcard
    },
    {
        "serialize_not_pair_var",
        SerializeIterToRowJson_serialize_not_pair_var
    },
    {
        "serialize_not_pair_var_constrained",
        SerializeIterToRowJson_serialize_not_pair_var_constrained
    },
    {
        "serialize_optional",
        SerializeIterToRowJson_serialize_optional
    },
    {
        "serialize_optional_pair_wildcard",
        SerializeIterToRowJson_serialize_optional_pair_wildcard
    },
    {
        "serialize_optional_pair_var",
        SerializeIterToRowJson_serialize_optional_pair_var
    },
    {
        "serialize_optional_pair_var_constrained",
        SerializeIterToRowJson_serialize_optional_pair_var_constrained
    },
    {
        "serialize_or",
        SerializeIterToRowJson_serialize_or
    },
    {
        "serialize_scope",
        SerializeIterToRowJson_serialize_scope
    },
    {
        "serialize_eq",
        SerializeIterToRowJson_serialize_eq
    },
    {
        "serialize_neq",
        SerializeIterToRowJson_serialize_neq
    },
    {
        "serialize_eq_m",
        SerializeIterToRowJson_serialize_eq_m
    },
    {
        "serialize_table",
        SerializeIterToRowJson_serialize_table
    },
    {
        "serialize_table_w_eq",
        SerializeIterToRowJson_serialize_table_w_eq
    },
    {
        "serialize_table_w_neq",
        SerializeIterToRowJson_serialize_table_w_neq
    },
    {
        "serialize_table_w_2_pair_targets",
        SerializeIterToRowJson_serialize_table_w_2_pair_targets
    },
    {
        "serialize_table_w_2_pair_targets_2_rel",
        SerializeIterToRowJson_serialize_table_w_2_pair_targets_2_rel
    },
    {
        "serialize_table_w_3_pair_targets",
        SerializeIterToRowJson_serialize_table_w_3_pair_targets
    },
    {
        "serialize_table_w_3_pair_targets_2_rel",
        SerializeIterToRowJson_serialize_table_w_3_pair_targets_2_rel
    },
    {
        "serialize_everything",
        SerializeIterToRowJson_serialize_everything
    },
    {
        "serialize_everything_table",
        SerializeIterToRowJson_serialize_everything_table
    },
    {
        "serialize_w_type_info",
        SerializeIterToRowJson_serialize_w_type_info
    },
    {
        "serialize_w_field_info",
        SerializeIterToRowJson_serialize_w_field_info
    },
    {
        "serialize_w_field_info_pair_w_0_target",
        SerializeIterToRowJson_serialize_w_field_info_pair_w_0_target
    },
    {
        "serialize_w_field_info_pair_w_not_tag",
        SerializeIterToRowJson_serialize_w_field_info_pair_w_not_tag
    },
    {
        "serialize_w_field_info_pair_w_not_pair",
        SerializeIterToRowJson_serialize_w_field_info_pair_w_not_pair
    },
    {
        "serialize_w_field_info_pair_w_not_component",
        SerializeIterToRowJson_serialize_w_field_info_pair_w_not_component
    },
    {
        "serialize_w_field_info_w_or",
        SerializeIterToRowJson_serialize_w_field_info_w_or
    },
    {
        "serialize_recycled_id",
        SerializeIterToRowJson_serialize_recycled_id
    },
    {
        "serialize_entity_w_flecs_core_parent",
        SerializeIterToRowJson_serialize_entity_w_flecs_core_parent
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
        "id",
        SerializeTypeInfoToJson_id
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
        "array_type",
        SerializeTypeInfoToJson_array_type
    },
    {
        "vector_type",
        SerializeTypeInfoToJson_vector_type
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
    },
    {
        "struct_vector_type",
        SerializeTypeInfoToJson_struct_vector_type
    },
    {
        "custom_primitive_type",
        SerializeTypeInfoToJson_custom_primitive_type
    },
    {
        "custom_array_type",
        SerializeTypeInfoToJson_custom_array_type
    },
    {
        "custom_vector_type",
        SerializeTypeInfoToJson_custom_vector_type
    },
    {
        "custom_struct_type",
        SerializeTypeInfoToJson_custom_struct_type
    },
    {
        "struct_w_value_range",
        SerializeTypeInfoToJson_struct_w_value_range
    },
    {
        "struct_w_error_range",
        SerializeTypeInfoToJson_struct_w_error_range
    },
    {
        "struct_w_warning_range",
        SerializeTypeInfoToJson_struct_w_warning_range
    },
    {
        "struct_w_error_and_warning_range",
        SerializeTypeInfoToJson_struct_w_error_and_warning_range
    },
    {
        "struct_nested",
        SerializeTypeInfoToJson_struct_nested
    },
    {
        "struct_nested_2_lvls",
        SerializeTypeInfoToJson_struct_nested_2_lvls
    },
    {
        "struct_nested_2_members",
        SerializeTypeInfoToJson_struct_nested_2_members
    },
    {
        "struct_nested_3_members",
        SerializeTypeInfoToJson_struct_nested_3_members
    }
};

bake_test_case SerializeQueryInfoToJson_testcases[] = {
    {
        "1_tag",
        SerializeQueryInfoToJson_1_tag
    },
    {
        "1_component",
        SerializeQueryInfoToJson_1_component
    },
    {
        "1_pair",
        SerializeQueryInfoToJson_1_pair
    },
    {
        "1_pair_w_wildcard",
        SerializeQueryInfoToJson_1_pair_w_wildcard
    },
    {
        "1_pair_w_any",
        SerializeQueryInfoToJson_1_pair_w_any
    },
    {
        "1_tag_fixed_src",
        SerializeQueryInfoToJson_1_tag_fixed_src
    },
    {
        "1_tag_var_src",
        SerializeQueryInfoToJson_1_tag_var_src
    },
    {
        "1_component_in",
        SerializeQueryInfoToJson_1_component_in
    },
    {
        "1_component_inout",
        SerializeQueryInfoToJson_1_component_inout
    },
    {
        "1_component_out",
        SerializeQueryInfoToJson_1_component_out
    },
    {
        "1_component_none",
        SerializeQueryInfoToJson_1_component_none
    },
    {
        "1_tag_not",
        SerializeQueryInfoToJson_1_tag_not
    },
    {
        "2_tags_or",
        SerializeQueryInfoToJson_2_tags_or
    },
    {
        "1_tag_optional",
        SerializeQueryInfoToJson_1_tag_optional
    },
    {
        "1_tag_self",
        SerializeQueryInfoToJson_1_tag_self
    },
    {
        "1_tag_self_dont_inherit",
        SerializeQueryInfoToJson_1_tag_self_dont_inherit
    },
    {
        "1_tag_up",
        SerializeQueryInfoToJson_1_tag_up
    },
    {
        "1_tag_cascade",
        SerializeQueryInfoToJson_1_tag_cascade
    },
    {
        "0_term",
        SerializeQueryInfoToJson_0_term
    },
    {
        "anonymous_tag",
        SerializeQueryInfoToJson_anonymous_tag
    },
    {
        "anonymous_pair",
        SerializeQueryInfoToJson_anonymous_pair
    },
    {
        "anonymous_component",
        SerializeQueryInfoToJson_anonymous_component
    },
    {
        "anonymous_tag_recycled",
        SerializeQueryInfoToJson_anonymous_tag_recycled
    },
    {
        "anonymous_pair_recycled",
        SerializeQueryInfoToJson_anonymous_pair_recycled
    },
    {
        "anonymous_component_recycled",
        SerializeQueryInfoToJson_anonymous_component_recycled
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
        "struct_w_2_nested",
        MetaUtils_struct_w_2_nested
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
    },
    {
        "private_members",
        MetaUtils_private_members
    },
    {
        "enum_constant_w_name_prefix",
        MetaUtils_enum_constant_w_name_prefix
    },
    {
        "enum_constant_w_type_prefix",
        MetaUtils_enum_constant_w_type_prefix
    },
    {
        "enum_constant_w_name_type_prefix",
        MetaUtils_enum_constant_w_name_type_prefix
    }
};

bake_test_case OpaqueTypes_testcases[] = {
    {
        "ser_i32_type_to_json",
        OpaqueTypes_ser_i32_type_to_json
    },
    {
        "ser_string_type_to_json",
        OpaqueTypes_ser_string_type_to_json
    },
    {
        "ser_vec_i32_type_to_json",
        OpaqueTypes_ser_vec_i32_type_to_json
    },
    {
        "ser_vec_string_type_to_json",
        OpaqueTypes_ser_vec_string_type_to_json
    },
    {
        "ser_struct_1_member",
        OpaqueTypes_ser_struct_1_member
    },
    {
        "ser_struct_2_members",
        OpaqueTypes_ser_struct_2_members
    },
    {
        "ser_struct_3_members",
        OpaqueTypes_ser_struct_3_members
    },
    {
        "deser_bool_from_json",
        OpaqueTypes_deser_bool_from_json
    },
    {
        "deser_char_from_json",
        OpaqueTypes_deser_char_from_json
    },
    {
        "deser_int_from_json",
        OpaqueTypes_deser_int_from_json
    },
    {
        "deser_uint_from_json",
        OpaqueTypes_deser_uint_from_json
    },
    {
        "deser_float_from_json",
        OpaqueTypes_deser_float_from_json
    },
    {
        "deser_string_from_json",
        OpaqueTypes_deser_string_from_json
    },
    {
        "deser_entity_from_json",
        OpaqueTypes_deser_entity_from_json
    },
    {
        "ser_deser_world_w_ser_opaque",
        OpaqueTypes_ser_deser_world_w_ser_opaque
    },
    {
        "ser_deser_entity",
        OpaqueTypes_ser_deser_entity
    },
    {
        "ser_deser_0_entity",
        OpaqueTypes_ser_deser_0_entity
    }
};

bake_test_case Misc_testcases[] = {
    {
        "primitive_from_stage",
        Misc_primitive_from_stage
    },
    {
        "enum_from_stage",
        Misc_enum_from_stage
    },
    {
        "bitmask_from_stage",
        Misc_bitmask_from_stage
    },
    {
        "struct_from_stage",
        Misc_struct_from_stage
    },
    {
        "array_from_stage",
        Misc_array_from_stage
    },
    {
        "vector_from_stage",
        Misc_vector_from_stage
    },
    {
        "opaque_from_stage",
        Misc_opaque_from_stage
    },
    {
        "unit_from_stage",
        Misc_unit_from_stage
    },
    {
        "unit_prefix_from_stage",
        Misc_unit_prefix_from_stage
    },
    {
        "quantity_from_stage",
        Misc_quantity_from_stage
    },
    {
        "primitive_from_defer",
        Misc_primitive_from_defer
    },
    {
        "enum_from_defer",
        Misc_enum_from_defer
    },
    {
        "bitmask_from_defer",
        Misc_bitmask_from_defer
    },
    {
        "array_from_defer",
        Misc_array_from_defer
    },
    {
        "vector_from_defer",
        Misc_vector_from_defer
    },
    {
        "struct_from_defer",
        Misc_struct_from_defer
    },
    {
        "opaque_from_defer",
        Misc_opaque_from_defer
    },
    {
        "unit_from_defer",
        Misc_unit_from_defer
    },
    {
        "unit_prefix_from_defer",
        Misc_unit_prefix_from_defer
    },
    {
        "quantity_from_defer",
        Misc_quantity_from_defer
    },
    {
        "primitive_from_readonly",
        Misc_primitive_from_readonly
    },
    {
        "enum_from_readonly",
        Misc_enum_from_readonly
    },
    {
        "bitmask_from_readonly",
        Misc_bitmask_from_readonly
    },
    {
        "array_from_readonly",
        Misc_array_from_readonly
    },
    {
        "vector_from_readonly",
        Misc_vector_from_readonly
    },
    {
        "struct_from_readonly",
        Misc_struct_from_readonly
    },
    {
        "opaque_from_readonly",
        Misc_opaque_from_readonly
    },
    {
        "unit_from_readonly",
        Misc_unit_from_readonly
    },
    {
        "unit_prefix_from_readonly",
        Misc_unit_prefix_from_readonly
    },
    {
        "quantity_from_readonly",
        Misc_quantity_from_readonly
    },
    {
        "primitive_from_suspend_defer",
        Misc_primitive_from_suspend_defer
    },
    {
        "enum_from_suspend_defer",
        Misc_enum_from_suspend_defer
    },
    {
        "bitmask_from_suspend_defer",
        Misc_bitmask_from_suspend_defer
    },
    {
        "array_from_suspend_defer",
        Misc_array_from_suspend_defer
    },
    {
        "vector_from_suspend_defer",
        Misc_vector_from_suspend_defer
    },
    {
        "struct_from_suspend_defer",
        Misc_struct_from_suspend_defer
    },
    {
        "opaque_from_suspend_defer",
        Misc_opaque_from_suspend_defer
    },
    {
        "unit_from_suspend_defer",
        Misc_unit_from_suspend_defer
    },
    {
        "unit_prefix_from_suspend_defer",
        Misc_unit_prefix_from_suspend_defer
    },
    {
        "quantity_from_suspend_defer",
        Misc_quantity_from_suspend_defer
    }
};


static bake_test_suite suites[] = {
    {
        "PrimitiveTypes",
        NULL,
        NULL,
        56,
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
        29,
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
        29,
        Units_testcases
    },
    {
        "Serialized",
        NULL,
        NULL,
        62,
        Serialized_testcases
    },
    {
        "Cursor",
        NULL,
        NULL,
        135,
        Cursor_testcases
    },
    {
        "DeserializeFromJson",
        NULL,
        NULL,
        134,
        DeserializeFromJson_testcases
    },
    {
        "SerializeToJson",
        NULL,
        NULL,
        46,
        SerializeToJson_testcases
    },
    {
        "SerializeEntityToJson",
        NULL,
        NULL,
        62,
        SerializeEntityToJson_testcases
    },
    {
        "SerializeIterToJson",
        NULL,
        NULL,
        71,
        SerializeIterToJson_testcases
    },
    {
        "SerializeIterToRowJson",
        NULL,
        NULL,
        60,
        SerializeIterToRowJson_testcases
    },
    {
        "SerializeTypeInfoToJson",
        NULL,
        NULL,
        40,
        SerializeTypeInfoToJson_testcases
    },
    {
        "SerializeQueryInfoToJson",
        NULL,
        NULL,
        25,
        SerializeQueryInfoToJson_testcases
    },
    {
        "MetaUtils",
        NULL,
        NULL,
        22,
        MetaUtils_testcases
    },
    {
        "OpaqueTypes",
        NULL,
        NULL,
        17,
        OpaqueTypes_testcases
    },
    {
        "Misc",
        NULL,
        NULL,
        40,
        Misc_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("meta", argc, argv, suites, 20);
}
