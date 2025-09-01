project('flecs', 'c', license : 'mit', default_options : ['c_std=c99'])

flecs_args = []

if get_option('default_library') == 'static'
    flecs_args = '-Dflecs_STATIC'
endif

flecs_inc = include_directories('include')

flecs_deps = [
    dependency('threads')
]

cc = meson.get_compiler('c')

if host_machine.system() == 'windows'
    flecs_deps += cc.find_library('ws2_32') # Required for http addon
endif

flecs_src = files(
    'src/addons/alerts.c',
    'src/addons/app.c',
    'src/addons/doc.c',
    'src/addons/flecs_cpp.c',
    'src/addons/http.c',
    'src/addons/journal.c',
    'src/addons/json/deserialize_value.c',
    'src/addons/json/deserialize.c',
    'src/addons/json/json.c',
    'src/addons/json/serialize_entity.c',
    'src/addons/json/serialize_field_info.c',
    'src/addons/json/serialize_iter_result_query.c',
    'src/addons/json/serialize_iter_result_table.c',
    'src/addons/json/serialize_iter_result.c',
    'src/addons/json/serialize_iter.c',
    'src/addons/json/serialize_query_info.c',
    'src/addons/json/serialize_type_info.c',
    'src/addons/json/serialize_value.c',
    'src/addons/json/serialize_world.c',
    'src/addons/stats/monitor.c',
    'src/addons/stats/pipeline_monitor.c',
    'src/addons/stats/stats.c',
    'src/addons/stats/system_monitor.c',
    'src/addons/stats/world_monitor.c',
    'src/addons/stats/world_summary.c',
    'src/addons/log.c',
    'src/addons/meta/type_support/array_ts.c',
    'src/addons/meta/type_support/enum_ts.c',
    'src/addons/meta/type_support/opaque_ts.c',
    'src/addons/meta/type_support/primitive_ts.c',
    'src/addons/meta/type_support/struct_ts.c',
    'src/addons/meta/type_support/units_ts.c',
    'src/addons/meta/definitions.c',
    'src/addons/meta/meta.c',
    'src/addons/meta/serializer.c',
    'src/addons/meta/cursor.c',
    'src/addons/meta/rtt_lifecycle.c',
    'src/addons/meta/c_utils.c',
    'src/addons/metrics.c',
    'src/addons/module.c',
    'src/addons/os_api_impl/os_api_impl.c',
    'src/addons/parser/tokenizer.c',
    'src/addons/pipeline/frame.c',
    'src/addons/pipeline/pipeline.c',
    'src/addons/pipeline/worker.c',
    'src/addons/query_dsl/parser.c',
    'src/addons/rest.c',
    'src/addons/script/template.c',
    'src/addons/script/ast.c',
    'src/addons/script/function.c',
    'src/addons/script/functions_builtin.c',
    'src/addons/script/functions_math.c',
    'src/addons/script/parser.c',
    'src/addons/script/script.c',
    'src/addons/script/serialize.c',
    'src/addons/script/vars.c',
    'src/addons/script/visit_check.c',
    'src/addons/script/visit_eval.c',
    'src/addons/script/visit_free.c',
    'src/addons/script/visit_to_str.c',
    'src/addons/script/visit.c',
    'src/addons/script/expr/ast.c',
    'src/addons/script/expr/parser.c',
    'src/addons/script/expr/stack.c',
    'src/addons/script/expr/util.c',
    'src/addons/script/expr/visit_eval.c',
    'src/addons/script/expr/visit_fold.c',
    'src/addons/script/expr/visit_free.c',
    'src/addons/script/expr/visit_to_str.c',
    'src/addons/script/expr/visit_type.c',
    'src/addons/system/system.c',
    'src/addons/timer.c',
    'src/addons/units.c',
    'src/datastructures/allocator.c',
    'src/datastructures/bitset.c',
    'src/datastructures/block_allocator.c',
    'src/datastructures/hash.c',
    'src/datastructures/hashmap.c',
    'src/datastructures/map.c',
    'src/datastructures/stack_allocator.c',
    'src/datastructures/name_index.c',
    'src/datastructures/sparse.c',
    'src/datastructures/strbuf.c',
    'src/datastructures/vec.c',
    'src/storage/component_index.c',
    'src/storage/entity_index.c',
    'src/storage/ordered_children.c',
    'src/storage/sparse_storage.c',
    'src/storage/table.c',
    'src/storage/table_cache.c',
    'src/storage/table_graph.c',
    'src/query/compiler/compiler_term.c',
    'src/query/compiler/compiler.c',
    'src/query/cache/cache.c',
    'src/query/cache/cache_iter.c',
    'src/query/cache/change_detection.c',
    'src/query/cache/group.c',
    'src/query/cache/match.c',
    'src/query/cache/order_by.c',
    'src/query/engine/eval_iter.c',
    'src/query/engine/eval_member.c',
    'src/query/engine/eval_pred.c',
    'src/query/engine/eval_toggle.c',
    'src/query/engine/eval_trav.c',
    'src/query/engine/eval_up.c',
    'src/query/engine/eval_sparse.c',
    'src/query/engine/eval_utils.c',
    'src/query/engine/eval.c',
    'src/query/engine/trav_cache.c',
    'src/query/engine/trav_down_cache.c',
    'src/query/engine/trav_up_cache.c',
    'src/query/engine/trivial_iter.c',
    'src/query/api.c',
    'src/query/util.c',
    'src/query/validator.c',
    'src/bootstrap.c',
    'src/component_actions.c',
    'src/commands.c',
    'src/each.c',
    'src/entity_name.c',
    'src/entity.c',
    'src/id.c',
    'src/instantiate.c',
    'src/iter.c',
    'src/misc.c',
    'src/observable.c',
    'src/observer.c',
    'src/on_delete.c',
    'src/os_api.c',
    'src/poly.c',
    'src/ref.c',
    'src/stage.c',
    'src/search.c',
    'src/type_info.c',
    'src/value.c',
    'src/world.c',
)

install_headers('include/flecs.h')
install_subdir('include/flecs', install_dir : get_option('includedir'))

flecs_lib = library('flecs',
    flecs_src,
    install : true,
    c_args : [ '-Dflecs_EXPORTS', flecs_args ],
    dependencies : flecs_deps,
    include_directories : flecs_inc,
    implicit_include_directories : false
)

flecs_dep = declare_dependency(
    link_with : flecs_lib,
    compile_args : flecs_args,
    dependencies : flecs_deps,
    include_directories : flecs_inc
)

opt_helloworld = get_option('build_example').disable_auto_if(meson.is_subproject())

if opt_helloworld.allowed()
    helloworld_inc = include_directories('examples/c/hello_world/include')

    helloworld_exe = executable('helloworld',
        'examples/c/hello_world/src/main.c',
        include_directories : helloworld_inc,
        implicit_include_directories : false,
        dependencies : flecs_dep
)
endif

if meson.version().version_compare('>= 0.54.0')
    meson.override_dependency('flecs', flecs_dep)
endif

pkg = import('pkgconfig')
pkg.generate(flecs_lib)
