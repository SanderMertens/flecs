project('flecs', 'c')

flecs_inc = include_directories('include')

flecs_deps = dependency('threads')

flecs_src = files([
    'src/addon/dbg.c',
    'src/addon/reader.c',
    'src/addon/ringbuf.c',
    'src/addon/snapshot.c',
    'src/addon/writer.c',
    'src/api_support.c',
    'src/bootstrap.c',
    'src/bulk.c',
    'src/entity.c',
    'src/entity_index.c',
    'src/err.c',
    'src/filter.c',
    'src/iter.c',
    'src/map.c',
    'src/misc.c',
    'src/module.c',
    'src/modules/pipeline/pipeline.c',
    'src/modules/pipeline/worker.c',
    'src/modules/stats.c',
    'src/modules/system.c',
    'src/modules/timer.c',
    'src/os_api.c',
    'src/query.c',
    'src/scope.c',
    'src/signature.c',
    'src/sparse.c',
    'src/stage.c',
    'src/strbuf.c',
    'src/table.c',
    'src/table_graph.c',
    'src/type.c',
    'src/vector.c',
    'src/world.c'
])

install_headers('include/flecs.h')
install_subdir('include/flecs', install_dir : get_option('includedir'))

flecs_lib = library('flecs',
    flecs_src,
    install : true,
    dependencies : flecs_deps,
    include_directories : flecs_inc
)

flecs_dep = declare_dependency(
    link_with : flecs_lib,
    dependencies : flecs_deps,
    include_directories : flecs_inc
)

pkg = import('pkgconfig')
pkg.generate(flecs_lib)
