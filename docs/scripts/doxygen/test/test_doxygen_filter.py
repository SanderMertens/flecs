#!/usr/bin/env python3
"""
Test script for the Doxygen filter.

This script tests the doxygen_filter.py script with various input scenarios
to ensure it correctly strips test attributes and HIDE lines.
"""

import tempfile
import subprocess
import os
import sys


def run_filter(input_content):
    """
    Run the doxygen filter on the given input content.
    
    Args:
        input_content (str): The content to filter
        
    Returns:
        str: The filtered output
    """
    # Create a temporary file with the input content
    with tempfile.NamedTemporaryFile(mode='w', suffix='.md', delete=False) as temp_file:
        temp_file.write(input_content)
        temp_file_path = temp_file.name

    try:
        # Get the directory containing this test script
        script_dir = os.path.dirname(os.path.abspath(__file__))
        # filter script is in the parent directory
        filter_script = os.path.join(script_dir, '..', 'doxygen_filter.py')

        # Run the filter script
        result = subprocess.run([
            sys.executable, filter_script, temp_file_path
        ], capture_output=True, text=True)

        if result.returncode != 0:
            raise Exception(f"Filter script failed: {result.stderr}")

        return result.stdout
    finally:
        # Clean up the temporary file
        os.unlink(temp_file_path)


def test_basic_cpp_test_removal():
    """Test basic cpp test attribute removal."""
    input_text = """# Example

```cpp test
HIDE: flecs::world world;
world.component<Position>().add(flecs::CanToggle);

flecs::entity e = world.entity().set(Position{10, 20});

e.disable<Position>(); // Disable component
assert(!e.enabled<Position>());

e.enable<Position>(); // Enable component
assert(e.enabled<Position>());
```

Some other text."""

    expected_output = """# Example

```cpp
world.component<Position>().add(flecs::CanToggle);

flecs::entity e = world.entity().set(Position{10, 20});

e.disable<Position>(); // Disable component
assert(!e.enabled<Position>());

e.enable<Position>(); // Enable component
assert(e.enabled<Position>());
```

Some other text."""

    result = run_filter(input_text)
    assert result == expected_output, f"Basic cpp test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ Basic cpp test removal passed")


def test_c_test_removal():
    """Test C test attribute removal."""
    input_text = """```c test
HIDE: ecs_world_t *world = ecs_init();
ecs_entity_t pos = ecs_component_init(world, &(ecs_component_desc_t){
    .entity = ecs_entity(world, {.name = "Position"}),
    .type.size = sizeof(Position),
    .type.alignment = ECS_ALIGNOF(Position)
});
```"""

    expected_output = """```c
ecs_entity_t pos = ecs_component_init(world, &(ecs_component_desc_t){
    .entity = ecs_entity(world, {.name = "Position"}),
    .type.size = sizeof(Position),
    .type.alignment = ECS_ALIGNOF(Position)
});
```"""

    result = run_filter(input_text)
    assert result == expected_output, f"C test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ C test removal passed")


def test_multiple_languages():
    """Test multiple languages with test attributes."""
    input_text = """```javascript test
HIDE: const world = new flecs.World();
const position = world.component("Position");
```

```python test  
HIDE: world = flecs.World()
pos = world.component("Position")
print("Hello")
```

```rust test
HIDE: let world = World::new();
let position = world.component::<Position>();
```"""

    expected_output = """```javascript
const position = world.component("Position");
```

```python
pos = world.component("Position")
print("Hello")
```

```rust
let position = world.component::<Position>();
```"""

    result = run_filter(input_text)
    assert result == expected_output, f"Multiple languages test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ Multiple languages test passed")


def test_hide_lines_only():
    """Test removal of HIDE lines without test attribute."""
    input_text = """```cpp
HIDE: flecs::world world;
HIDE: auto pos = world.component<Position>();
world.entity().set(Position{10, 20});
HIDE: // This should be hidden
std::cout << "Hello World" << std::endl;
```"""

    expected_output = """```cpp
world.entity().set(Position{10, 20});
std::cout << "Hello World" << std::endl;
```"""

    result = run_filter(input_text)
    assert result == expected_output, f"HIDE lines only test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ HIDE lines only test passed")


def test_normal_code_blocks():
    """Test that normal code blocks are left unchanged."""
    input_text = """```cpp
flecs::world world;
world.component<Position>();
```

```c
ecs_world_t *world = ecs_init();
```

Some text here.

```python
import flecs
world = flecs.World()
```"""

    expected_output = input_text  # Should be unchanged

    result = run_filter(input_text)
    assert result == expected_output, f"Normal code blocks test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ Normal code blocks test passed")


def test_indented_hide_lines():
    """Test HIDE lines with indentation."""
    input_text = """```cpp test
    HIDE: flecs::world world;
    world.component<Position>();
  HIDE:   auto entity = world.entity();
    entity.set(Position{10, 20});
```"""

    expected_output = """```cpp
    world.component<Position>();
    entity.set(Position{10, 20});
```"""

    result = run_filter(input_text)
    assert result == expected_output, f"Indented HIDE lines test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ Indented HIDE lines test passed")


def test_mixed_content():
    """Test mixed content with text, code blocks, and edge cases."""
    input_text = """# Documentation

This is some text.

```cpp test
HIDE: #include <flecs.h>
HIDE: using namespace flecs;

world.component<Position>().add(CanToggle);
```

More text here.

```javascript
// This is a normal JS block
console.log("Hello");
```

```c test  
HIDE: ecs_world_t *world = ecs_init();
ecs_entity_t e = ecs_new(world, Position);
```

Final text."""

    expected_output = """# Documentation

This is some text.

```cpp

world.component<Position>().add(CanToggle);
```

More text here.

```javascript
// This is a normal JS block
console.log("Hello");
```

```c
ecs_entity_t e = ecs_new(world, Position);
```

Final text."""

    result = run_filter(input_text)
    assert result == expected_output, f"Mixed content test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ Mixed content test passed")


def test_edge_cases():
    """Test edge cases."""
    input_text = """```cpp test    
HIDE: // Extra spaces after test
world.entity();
```

```cpp
HIDE: // HIDE in normal block should be removed too
normal_code();
```

Text with HIDE: in it should not be affected.

```cpptest
// This should not be affected (no space before test)
code();
```"""

    expected_output = """```cpp
world.entity();
```

```cpp
normal_code();
```

Text with HIDE: in it should not be affected.

```cpptest
// This should not be affected (no space before test)
code();
```"""

    result = run_filter(input_text)
    assert result == expected_output, f"Edge cases test failed.\nExpected:\n{expected_output}\nGot:\n{result}"
    print("✓ Edge cases test passed")


def main():
    """Run all tests."""
    print("Running Doxygen filter tests...")
    
    try:
        test_basic_cpp_test_removal()
        test_c_test_removal()
        test_multiple_languages()
        test_hide_lines_only()
        test_normal_code_blocks()
        test_indented_hide_lines()
        test_mixed_content()
        test_edge_cases()
        
        print("\n🎉 All tests passed!")
        
    except AssertionError as e:
        print(f"\n❌ Test failed: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Error running tests: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
