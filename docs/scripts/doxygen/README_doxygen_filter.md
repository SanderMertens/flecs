# Doxygen Filter Scripts

This directory contains Python scripts for filtering Flecs documentation before processing with Doxygen.

## Files

- `doxygen_filter.py` - Main filter script that processes markdown files
- `test_doxygen_filter.py` - Comprehensive test suite for the filter script

## Usage

### Running the Filter

```bash
python3 docs/scripts/doxygen_filter.py input_file.md
```

The filtered output will be printed to stdout.

### Filter Behavior

The script performs two main transformations:

1. **Removes 'test' attribute from code blocks**
   - `````cpp test` → `````cpp`
   - `````c test` → `````c` 
   - Works with any language identifier

2. **Removes lines starting with 'HIDE:'**
   - Removes entire lines that start with `HIDE:` (with optional leading whitespace)
   - Useful for hiding setup code that shouldn't appear in documentation

### Example

Input:
```markdown
```cpp test
HIDE: flecs::world world;
world.component<Position>().add(flecs::CanToggle);

flecs::entity e = world.entity().set(Position{10, 20});
```
```

Output:
```markdown
```cpp
world.component<Position>().add(flecs::CanToggle);

flecs::entity e = world.entity().set(Position{10, 20});
```
```

### Testing

Run the test suite to verify the filter works correctly:

```bash
cd docs/scripts
python3 test_doxygen_filter.py
```

The test suite includes:
- Basic functionality tests
- Multiple programming languages
- Edge cases and mixed content
- HIDE line handling with various indentation

### Integration with Doxygen

To use this filter with Doxygen, add the following to your Doxyfile:

```
FILTER_PATTERNS        = *.md="python3 docs/scripts/doxygen_filter.py"
```
