project('flecs', 'c', license : 'mit', default_options : ['c_std=c99'])

flecs_args = []

if get_option('default_library') == 'static'
    flecs_args = '-Dflecs_STATIC'
endif

flecs_inc = include_directories('include')

flecs_deps = dependency('threads')

flecs_src = files(
    'src/vector.c',
    'src/log.c',
    'src/misc.c',
    'src/os_api.c',
    'src/type.c',
    'src/sparse.c',
    'src/entity.c',
    'src/entity_index.c',
    'src/strbuf.c',
    'src/iter.c',
    'src/addons/bulk.c'
    'src/addons/dbg.c',
    'src/addons/hierarchy.c',
    'src/addons/module.c',
    'src/addons/os_api_impl.c',
    'src/addons/queue.c',
    'src/addons/reader.c',
    'src/addons/snapshot.c',
    'src/addons/writer.c',
    'src/table.c',
    'src/signature.c',
    'src/world.c',
    'src/stage.c',
    'src/bootstrap.c',
    'src/map.c',
    'src/filter.c',
    'src/query.c',
    'src/api_support.c',
    'src/modules/stats.c',
    'src/modules/pipeline/pipeline.c',
    'src/modules/pipeline/worker.c',
    'src/modules/system/system.c',
    'src/modules/system/system_dbg.c',
    'src/modules/timer.c',
    'src/table_graph.c',
)

install_headers('include/flecs.h')
install_subdir('include/flecs', install_dir : get_option('includedir'))

flecs_lib = library('flecs',
    flecs_src,
    install : true,
    c_args : [ '-Dflecs_EXPORTS', flecs_args ],
    dependencies : flecs_deps,
    include_directories : flecs_inc
)

flecs_dep = declare_dependency(
    link_with : flecs_lib,
    compile_args : flecs_args,
    dependencies : flecs_deps,
    include_directories : flecs_inc
)

helloword_inc = include_directories('examples/c/01_helloworld/include')

helloworld_exe = executable('helloword',
    'examples/c/01_helloworld/src/main.c',
    include_directories : helloword_inc,
    dependencies : flecs_dep
)

if meson.version().version_compare('>= 0.54.0')
    meson.override_dependency('flecs', flecs_dep)
endif

pkg = import('pkgconfig')
pkg.generate(flecs_lib)
