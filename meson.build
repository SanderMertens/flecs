project('flecs', 'c')

flecs_inc = include_directories('include')

flecs_deps = dependency('threads')

flecs_src = []

subdir('src')

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
