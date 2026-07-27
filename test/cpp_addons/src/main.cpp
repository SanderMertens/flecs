
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <cpp_addons.h>

// Testsuite 'Misc'
void Misc_setup(void);
void Misc_string_compare_w_char_ptr(void);
void Misc_string_compare_w_char_ptr_length_diff(void);
void Misc_string_compare_w_string(void);
void Misc_string_view_compare_w_char_ptr(void);
void Misc_string_view_compare_w_string(void);
void Misc_string_compare_nullptr(void);
void Misc_nullptr_string_compare(void);
void Misc_nullptr_string_compare_nullptr(void);
void Misc_c_macros(void);
void Misc_app_run(void);
void Misc_app_run_target_fps(void);
void Misc_app_run_set_frames(void);
void Misc_app_on_remove_on_fini(void);
void Misc_member_gauge_metric(void);
void Misc_id_gauge_metric(void);
void Misc_oneof_gauge_metric(void);
void Misc_id_counter_metric(void);
void Misc_oneof_counter_metric(void);
void Misc_component_mixin_member_metric(void);
void Misc_component_mixin_member_metric_custom_parent_entity(void);
void Misc_metric_description(void);
void Misc_component_mixin_member_metric_description(void);
void Misc_member_metric_w_value_name(void);
void Misc_member_metric_w_value_name_camel_case_type(void);
void Misc_member_metric_w_custom_name(void);
void Misc_dotmember_metric(void);
void Misc_counter_id_metric(void);
void Misc_counter_target_metric(void);
void Misc_member_metric_w_pair_R_T(void);
void Misc_member_metric_w_pair_R_t(void);
void Misc_member_metric_w_pair_r_t(void);
void Misc_member_metric_w_pair_r_T(void);
void Misc_alert(void);
void Misc_alert_w_message(void);
void Misc_alert_w_brief(void);
void Misc_alert_doc_name(void);
void Misc_alert_severity_info(void);
void Misc_alert_severity_warning(void);
void Misc_alert_severity_error(void);
void Misc_alert_severity_implicit(void);
void Misc_alert_w_retain_period(void);
void Misc_alert_w_severity_filter(void);
void Misc_alert_w_severity_filter_severity_type(void);
void Misc_alert_w_severity_filter_severity_type_id_type(void);
void Misc_alert_w_severity_filter_severity_type_enum_constant(void);
void Misc_alert_w_severity_filter_w_var(void);
void Misc_alert_w_severity_filter_severity_type_w_var(void);
void Misc_alert_w_severity_filter_severity_type_id_type_w_var(void);
void Misc_alert_w_severity_filter_severity_type_enum_constant_w_var(void);
void Misc_alert_for_member_range(void);
void Misc_alert_w_member_range_from_var(void);
void Misc_map_api(void);
void Misc_get_const_var(void);
void Misc_get_const_var_struct(void);
void Misc_get_const_var_as_f32(void);
void Misc_get_const_var_as_i32(void);
void Misc_get_const_var_as_u32(void);
void Misc_get_const_var_w_out(void);
void Misc_get_const_var_struct_w_out(void);
void Misc_get_const_var_struct_w_out_not_found(void);
void Misc_get_const_var_as_f32_w_out(void);
void Misc_get_const_var_as_i32_w_out(void);
void Misc_get_const_var_as_u32_w_out(void);
void Misc_get_const_var_not_found(void);
void Misc_get_const_var_not_a_var(void);
void Misc_get_scoped_const_var(void);
void Misc_get_module_const_var(void);
void Misc_get_module_const_var_from_script_module(void);
void Misc_get_module_const_var_from_nested_script_module(void);
void Misc_get_module_const_var_from_nested_module_no_script_module(void);

// Testsuite 'Doc'
void Doc_set_brief(void);
void Doc_set_name(void);
void Doc_set_link(void);
void Doc_set_color(void);
void Doc_set_uuid(void);
void Doc_get_name_no_doc_name(void);

bake_test_case Misc_testcases[] = {
    {
        "string_compare_w_char_ptr",
        Misc_string_compare_w_char_ptr
    },
    {
        "string_compare_w_char_ptr_length_diff",
        Misc_string_compare_w_char_ptr_length_diff
    },
    {
        "string_compare_w_string",
        Misc_string_compare_w_string
    },
    {
        "string_view_compare_w_char_ptr",
        Misc_string_view_compare_w_char_ptr
    },
    {
        "string_view_compare_w_string",
        Misc_string_view_compare_w_string
    },
    {
        "string_compare_nullptr",
        Misc_string_compare_nullptr
    },
    {
        "nullptr_string_compare",
        Misc_nullptr_string_compare
    },
    {
        "nullptr_string_compare_nullptr",
        Misc_nullptr_string_compare_nullptr
    },
    {
        "c_macros",
        Misc_c_macros
    },
    {
        "app_run",
        Misc_app_run
    },
    {
        "app_run_target_fps",
        Misc_app_run_target_fps
    },
    {
        "app_run_set_frames",
        Misc_app_run_set_frames
    },
    {
        "app_on_remove_on_fini",
        Misc_app_on_remove_on_fini
    },
    {
        "member_gauge_metric",
        Misc_member_gauge_metric
    },
    {
        "id_gauge_metric",
        Misc_id_gauge_metric
    },
    {
        "oneof_gauge_metric",
        Misc_oneof_gauge_metric
    },
    {
        "id_counter_metric",
        Misc_id_counter_metric
    },
    {
        "oneof_counter_metric",
        Misc_oneof_counter_metric
    },
    {
        "component_mixin_member_metric",
        Misc_component_mixin_member_metric
    },
    {
        "component_mixin_member_metric_custom_parent_entity",
        Misc_component_mixin_member_metric_custom_parent_entity
    },
    {
        "metric_description",
        Misc_metric_description
    },
    {
        "component_mixin_member_metric_description",
        Misc_component_mixin_member_metric_description
    },
    {
        "member_metric_w_value_name",
        Misc_member_metric_w_value_name
    },
    {
        "member_metric_w_value_name_camel_case_type",
        Misc_member_metric_w_value_name_camel_case_type
    },
    {
        "member_metric_w_custom_name",
        Misc_member_metric_w_custom_name
    },
    {
        "dotmember_metric",
        Misc_dotmember_metric
    },
    {
        "counter_id_metric",
        Misc_counter_id_metric
    },
    {
        "counter_target_metric",
        Misc_counter_target_metric
    },
    {
        "member_metric_w_pair_R_T",
        Misc_member_metric_w_pair_R_T
    },
    {
        "member_metric_w_pair_R_t",
        Misc_member_metric_w_pair_R_t
    },
    {
        "member_metric_w_pair_r_t",
        Misc_member_metric_w_pair_r_t
    },
    {
        "member_metric_w_pair_r_T",
        Misc_member_metric_w_pair_r_T
    },
    {
        "alert",
        Misc_alert
    },
    {
        "alert_w_message",
        Misc_alert_w_message
    },
    {
        "alert_w_brief",
        Misc_alert_w_brief
    },
    {
        "alert_doc_name",
        Misc_alert_doc_name
    },
    {
        "alert_severity_info",
        Misc_alert_severity_info
    },
    {
        "alert_severity_warning",
        Misc_alert_severity_warning
    },
    {
        "alert_severity_error",
        Misc_alert_severity_error
    },
    {
        "alert_severity_implicit",
        Misc_alert_severity_implicit
    },
    {
        "alert_w_retain_period",
        Misc_alert_w_retain_period
    },
    {
        "alert_w_severity_filter",
        Misc_alert_w_severity_filter
    },
    {
        "alert_w_severity_filter_severity_type",
        Misc_alert_w_severity_filter_severity_type
    },
    {
        "alert_w_severity_filter_severity_type_id_type",
        Misc_alert_w_severity_filter_severity_type_id_type
    },
    {
        "alert_w_severity_filter_severity_type_enum_constant",
        Misc_alert_w_severity_filter_severity_type_enum_constant
    },
    {
        "alert_w_severity_filter_w_var",
        Misc_alert_w_severity_filter_w_var
    },
    {
        "alert_w_severity_filter_severity_type_w_var",
        Misc_alert_w_severity_filter_severity_type_w_var
    },
    {
        "alert_w_severity_filter_severity_type_id_type_w_var",
        Misc_alert_w_severity_filter_severity_type_id_type_w_var
    },
    {
        "alert_w_severity_filter_severity_type_enum_constant_w_var",
        Misc_alert_w_severity_filter_severity_type_enum_constant_w_var
    },
    {
        "alert_for_member_range",
        Misc_alert_for_member_range
    },
    {
        "alert_w_member_range_from_var",
        Misc_alert_w_member_range_from_var
    },
    {
        "map_api",
        Misc_map_api
    },
    {
        "get_const_var",
        Misc_get_const_var
    },
    {
        "get_const_var_struct",
        Misc_get_const_var_struct
    },
    {
        "get_const_var_as_f32",
        Misc_get_const_var_as_f32
    },
    {
        "get_const_var_as_i32",
        Misc_get_const_var_as_i32
    },
    {
        "get_const_var_as_u32",
        Misc_get_const_var_as_u32
    },
    {
        "get_const_var_w_out",
        Misc_get_const_var_w_out
    },
    {
        "get_const_var_struct_w_out",
        Misc_get_const_var_struct_w_out
    },
    {
        "get_const_var_struct_w_out_not_found",
        Misc_get_const_var_struct_w_out_not_found
    },
    {
        "get_const_var_as_f32_w_out",
        Misc_get_const_var_as_f32_w_out
    },
    {
        "get_const_var_as_i32_w_out",
        Misc_get_const_var_as_i32_w_out
    },
    {
        "get_const_var_as_u32_w_out",
        Misc_get_const_var_as_u32_w_out
    },
    {
        "get_const_var_not_found",
        Misc_get_const_var_not_found
    },
    {
        "get_const_var_not_a_var",
        Misc_get_const_var_not_a_var
    },
    {
        "get_scoped_const_var",
        Misc_get_scoped_const_var
    },
    {
        "get_module_const_var",
        Misc_get_module_const_var
    },
    {
        "get_module_const_var_from_script_module",
        Misc_get_module_const_var_from_script_module
    },
    {
        "get_module_const_var_from_nested_script_module",
        Misc_get_module_const_var_from_nested_script_module
    },
    {
        "get_module_const_var_from_nested_module_no_script_module",
        Misc_get_module_const_var_from_nested_module_no_script_module
    }
};

bake_test_case Doc_testcases[] = {
    {
        "set_brief",
        Doc_set_brief
    },
    {
        "set_name",
        Doc_set_name
    },
    {
        "set_link",
        Doc_set_link
    },
    {
        "set_color",
        Doc_set_color
    },
    {
        "set_uuid",
        Doc_set_uuid
    },
    {
        "get_name_no_doc_name",
        Doc_get_name_no_doc_name
    }
};


static bake_test_suite suites[] = {
    {
        "Misc",
        Misc_setup,
        NULL,
        70,
        Misc_testcases
    },
    {
        "Doc",
        NULL,
        NULL,
        6,
        Doc_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("cpp_addons", argc, argv, suites, 2);
}
