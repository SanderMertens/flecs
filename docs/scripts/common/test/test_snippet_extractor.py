#!/usr/bin/env python3
"""
Test script for the snippet extractor functionality.
"""

import sys
import tempfile
import os
from pathlib import Path

# Add repo root to Python path
repo_root = Path(__file__).resolve().parents[4]  # Go up 4 levels: test -> common -> scripts -> docs -> repo_root
sys.path.insert(0, str(repo_root))

from docs.scripts.common.snippet_extractor import SnippetExtractor


def create_test_markdown_file(content: str) -> str:
    """Create a temporary markdown file with the given content."""
    with tempfile.NamedTemporaryFile(mode='w', suffix='.md', delete=False) as f:
        f.write(content)
        return f.name


def test_basic_cpp_extraction():
    """Test basic C++ snippet extraction."""
    md_content = """# Test Document

## Component System

```cpp test
flecs::world world;
world.component<Position>();
```

Some other text.

```cpp test
flecs::entity e = world.entity();
e.set(Position{10, 20});
```
"""
    
    md_file = create_test_markdown_file(md_content)
    temp_dir = tempfile.mkdtemp()
    
    try:
        # Move the file to a docs-like structure
        docs_dir = Path(temp_dir) / "docs"
        docs_dir.mkdir()
        test_md = docs_dir / "test.md"
        Path(md_file).rename(test_md)
        
        extractor = SnippetExtractor(str(docs_dir))
        snippets_by_file = extractor.extract_all_snippets(['cpp'])
        
        assert len(snippets_by_file) == 1, f"Expected 1 file, got {len(snippets_by_file)}"
        
        snippets = list(snippets_by_file.values())[0]
        assert len(snippets) == 2, f"Expected 2 snippets, got {len(snippets)}"
        
        # Check first snippet
        assert snippets[0].language == 'cpp'
        assert 'world.component<Position>' in snippets[0].code
        assert snippets[0].h2_title == 'Component System'
        
        # Check second snippet
        assert snippets[1].language == 'cpp'
        assert 'e.set(Position{10, 20})' in snippets[1].code
        assert snippets[1].test_number == 2
        
        print("✓ Basic C++ extraction test passed")
        
    finally:
        # Cleanup
        import shutil
        shutil.rmtree(temp_dir)


def test_multiple_languages():
    """Test extraction of multiple languages."""
    md_content = """# Multi-language Test

```cpp test
flecs::world world;
```

```c test
ecs_world_t *world = ecs_init();
```

```rust test
let world = World::new();
```
"""
    
    md_file = create_test_markdown_file(md_content)
    temp_dir = tempfile.mkdtemp()
    
    try:
        docs_dir = Path(temp_dir) / "docs"
        docs_dir.mkdir()
        test_md = docs_dir / "test.md"
        Path(md_file).rename(test_md)
        
        extractor = SnippetExtractor(str(docs_dir))
        
        # Test extracting specific languages
        cpp_snippets = extractor.extract_all_snippets(['cpp'])
        c_snippets = extractor.extract_all_snippets(['c'])
        rust_snippets = extractor.extract_all_snippets(['rust'])
        
        assert len(list(cpp_snippets.values())[0]) == 1
        assert len(list(c_snippets.values())[0]) == 1
        assert len(list(rust_snippets.values())[0]) == 1
        
        # Verify language specificity
        assert list(cpp_snippets.values())[0][0].language == 'cpp'
        assert list(c_snippets.values())[0][0].language == 'c'
        assert list(rust_snippets.values())[0][0].language == 'rust'
        
        print("✓ Multiple languages test passed")
        
    finally:
        import shutil
        shutil.rmtree(temp_dir)


def main():
    """Run all tests."""
    print("Running snippet extractor tests...")
    
    try:
        test_basic_cpp_extraction()
        test_multiple_languages()
        
        print("\n🎉 All snippet extractor tests passed!")
        
    except AssertionError as e:
        print(f"\n❌ Test failed: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Error running tests: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
