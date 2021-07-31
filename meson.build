project('flecs', 'c', license : 'mit', default_options : ['c_std=c99'])

flecs_args = []

if get_option('default_library') == 'static'
    flecs_args = '-Dflecs_STATIC'
endif

flecs_inc = include_directories('include')

flecs_deps = dependency('threads')

flecs_src = files(
    'src/addons/bulk.c',
    'src/addons/deprecated.c',
    'src/addons/direct_access.c',
    'src/addons/module.c',
    'src/addons/parser.c',
    'src/addons/queue.c',
    'src/addons/snapshot.c',
    'src/addons/stats.c',
    'src/modules/pipeline/pipeline.c',
    'src/modules/pipeline/worker.c',
    'src/modules/system/system.c',
    'src/modules/timer.c',    
    'src/api_support.c',
    'src/bitset.c',
    'src/bootstrap.c',
    'src/entity.c',
    'src/filter.c',
    'src/hash.c',
    'src/hashmap.c',
    'src/hierarchy.c',
    'src/iter.c',
    'src/log.c',
    'src/map.c',
    'src/misc.c',
    'src/observer.c',
    'src/os_api.c',
    'src/query.c',
    'src/sparse.c',
    'src/stage.c',
    'src/strbuf.c',
    'src/switch_list.c',
    'src/table_graph.c',
    'src/table.c',
    'src/trigger.c',
    'src/type.c',
    'src/vector.c',
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

helloworld_inc = include_directories('examples/c/01_helloworld/include')

helloworld_exe = executable('helloworld',
    'examples/c/01_helloworld/src/main.c',
    include_directories : helloworld_inc,
    implicit_include_directories : false,
    dependencies : flecs_dep
)

if meson.version().version_compare('>= 0.54.0')
    meson.override_dependency('flecs', flecs_dep)
endif

pkg = import('pkgconfig')
pkg.generate(flecs_lib)
