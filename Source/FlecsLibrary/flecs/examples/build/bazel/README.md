You can include the flecs repository in your `WORKSPACE` with:

```bazel
git_repository(
    name = "flecs",
    remote = "https://github.com/SanderMertens/flecs",
    commit = "f150d96ba9ea8be2b24dbf2217368c231cb17720", # v2.3.2+merge329
    shallow_since = "1615075784 -0800",
)
```

And then add it to your `BUILD` with:

```bazel
    deps = ["@flecs//:flecs"]
```

This directory contains a complete example of this usage. To try it you can run the following from your terminal:

```
bazel run example
```
