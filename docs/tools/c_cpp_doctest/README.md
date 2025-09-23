# C/C++ Documentation Test Generator

This tool generates C and C++ test files from documentation code snippets using the `poly_doctest` library. c_cpp_doctest is specialized to work with bake.

## Usage

from the `docs/tools/c_cpp_doctest` directory, run:

```bash
# Generate tests from docs directory with default settings
cargo run -- --local ../../../docs/

# Generate tests with custom output directory
cargo run -- --local ../../../docs/ --output custom_output/

# Use custom hide prefix
cargo run -- --local ../../../docs/ --hide-prefix "SKIP:"

# Show help
cargo run -- --help

# generate docs from remote url
cargo run -- --remote https://github.com/SanderMertens/flecs/tree/master/docs
```

## Code Processing

- Replaces `assert()` with `test_assert()`
- Adds `test_assert(true)` if no test assertions are found
- Preserves original code structure and indentation
- Processes configurable hide prefixes (removes prefix but keeps line content)
