
def persuite_bake_tests(name, deps, suites, visibility=None):
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
