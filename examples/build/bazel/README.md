You can include the flecs repository in your `MODULE.bazel` with:

```bazel
bazel_dep(name = "flecs")
git_override(
    module_name = "flecs",
    remote = "https://github.com/SanderMertens/flecs.git",
    commit = "02c8c2666b22ccce5706c8f16efaf813704fe31e",
)
```

And then add it to your `BUILD` with:

```bazel
    deps = ["@flecs"]
```

This directory contains a complete example of this usage. To try it you can run the following from your terminal:

```
bazel run //example
```

To keep your flecs dependency up to date with [Renovate](https://docs.renovatebot.com/) + CI, make sure to declare the version attribute of the `bazel_dep`.

```bazel
bazel_dep(name = "flecs")
git_override(
    module_name = "flecs",
    remote = "https://github.com/SanderMertens/flecs.git",
    commit = "02c8c2666b22ccce5706c8f16efaf813704fe31e",
)
```

If you use a merge queue for managing CI, you can configure Renovate to automatically kick off your CI and automerge for the dependency update with the following in your `.github/renovate.json5`

```json5
{
  "extends": ["config:recommended"],
  "dependencyDashboard": true,
  "automerge": true,
}
```