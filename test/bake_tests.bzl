
def _impl(ctx):
    name = ctx.attr.name
    deps = ctx.attr.deps
    suites = ctx.attr.suites
    visibility = None # ctx.attr.visibility

    suites_mangled = [s.partition(".")[0].rpartition("/")[2] for s in suites]

    for s in suites_mangled:
        native.cc_test(
            name = "{}-{}".format(name, s),
            deps = deps,
            visibility = visibility,
            args = [s]
        )

    native.test_suite(
        name = name,
        tests = [":{}-{}".format(name, s) for s in suites_mangled]
    )

persuite_bake_tests = rule(
    implementation = _impl,
    attrs = {
        "deps": attr.label_list(),
        "suites": attr.string_list(),
    },
)
