project('flecs', 'c')

flecs_inc = include_directories('include')

flecs_deps = dependency('threads')

flecs_src = files([
    'api_support.c',
    'bootstrap.c',
    'bulk.c',
    'entity_index.c',
    'entity.c',
    'err.c',
    'filter.c',
    'iter.c',
    'map.c',
    'misc.c',
    'module.c',
    'os_api.c',
    'query.c',
    'scope.c',
    'signature.c',
    'sparse.c',
    'strbuf.c',
    'table_graph.c',
    'table.c',
    'type.c',
    'vector.c',
    'world.c',
    'addon/dbg.c',
    'addon/reader.c',
    'addon/ringbug.c',
    'addon/snapshot.c',
    'addon/writer.c'
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
