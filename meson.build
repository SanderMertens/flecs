project('flecs', 'c', license : 'mit', default_options : ['c_std=c99'])

flecs_args = []

if get_option('default_library') == 'static'
    flecs_args = '-Dflecs_STATIC'
endif

flecs_inc = include_directories('include')

flecs_deps = dependency('threads')

flecs_src = files(
    'src/addons/coredoc.c',
    'src/addons/deprecated.c',
    'src/addons/doc.c',
    'src/addons/expr/deserialize.c',
    'src/addons/expr/serialize.c',
    'src/addons/expr/strutil.c',
    'src/addons/http.c',
    'src/addons/json/deserialize.c',
    'src/addons/json/serialize.c',
    'src/addons/json/serialize_type_info.c',
    'src/addons/json/json.c',
    'src/addons/log.c',
    'src/addons/meta/api.c',
    'src/addons/meta/meta.c',
    'src/addons/meta/serialized.c',
    'src/addons/meta/cursor.c',
    'src/addons/meta_c.c',
    'src/addons/module.c',
    'src/addons/monitor.c',
    'src/addons/os_api_impl/os_api_impl.c',
    'src/addons/parser.c',
    'src/addons/pipeline/pipeline.c',
    'src/addons/pipeline/worker.c',
    'src/addons/plecs.c',
    'src/addons/rest.c',
    'src/addons/rules.c',
    'src/addons/snapshot.c',
    'src/addons/stats.c',
    'src/addons/system/system.c',
    'src/addons/timer.c',    
    'src/addons/units.c',
    'src/datastructures/bitset.c',
    'src/datastructures/hash.c',
    'src/datastructures/hashmap.c',
    'src/datastructures/map.c',
    'src/datastructures/stack_allocator.c',
    'src/datastructures/name_index.c',
    'src/datastructures/qsort.c',
    'src/datastructures/sparse.c',
    'src/datastructures/strbuf.c',
    'src/datastructures/switch_list.c',
    'src/datastructures/vector.c',
    'src/bootstrap.c',
    'src/entity.c',
    'src/filter.c',
    'src/hierarchy.c',
    'src/id_record.c',
    'src/iter.c',
    'src/misc.c',
    'src/observable.c',
    'src/observer.c',
    'src/os_api.c',
    'src/poly.c',
    'src/query.c',
    'src/stage.c',
    'src/storage.c',
    'src/table_cache.c',
    'src/table_graph.c',
    'src/table.c',
    'src/search.c',
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

helloworld_inc = include_directories('examples/c/hello_world/include')

helloworld_exe = executable('helloworld',
    'examples/c/hello_world/src/main.c',
    include_directories : helloworld_inc,
    implicit_include_directories : false,
    dependencies : flecs_dep
)

if meson.version().version_compare('>= 0.54.0')
    meson.override_dependency('flecs', flecs_dep)
endif

pkg = import('pkgconfig')
pkg.generate(flecs_lib)
