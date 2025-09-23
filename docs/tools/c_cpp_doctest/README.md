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

## Output Structure

The generator creates the following structure:

```
tmp/doctest/c_cpp/
├── c/
│   ├── src/
│   │   ├── quickstart.c
│   │   └── manual.c
│   └── include/
└── cpp/
    ├── src/
    │   ├── quickstart.cpp
    │   └── manual.cpp
    └── include/
```

## Generated File Format

### C Files
- Include function declarations at the top
- Include `<core.h>` header
- Generate void functions with unique names per snippet

### C++ Files  
- No function declarations needed
- Include `<cpp.h>` header
- Generate void functions with unique names per snippet

## Code Processing

- Replaces `assert()` and `ecs_assert()` with `test_assert()`
- Adds `test_assert(true)` if no test assertions are found
- Preserves original code structure and indentation
- Processes configurable hide prefixes (removes prefix but keeps line content)
