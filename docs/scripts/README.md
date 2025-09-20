# Docs scripts

This folder contains utilities and generators for producing test files from
documentation snippets. It is organized to make it easy to add new language
bindings.

## Layout
- `common/` - Language-agnostic utilities used by all generators:
  - `snippet_extractor.py` - Extracts test-marked code blocks from markdown
  - `name_utils.py` - Generates test function names from headings
  - `test/` - Tests for common utilities
- `c_cpp/` - C and C++ test generators and language-specific helpers:
  - `generate_cpp_tests.py` - C++ test generator 
  - `generate_c_tests.py` - C test generator
  - `project_json.py` - Project.json management helpers
  - `code_utils.py` - Code formatting utilities
- `doxygen/` - Documentation processing scripts:
  - `doxygen_filter.py` - Removes test attributes and HIDE lines for Doxygen
  - `test/` - Tests for doxygen utilities

## Usage

### Extract snippets (language-agnostic)
```bash
# Extract C++, C and Rust snippets
python3 docs/scripts/common/snippet_extractor.py docs output.json cpp c rust

# Extract just Rust snippets  
python3 docs/scripts/common/snippet_extractor.py docs rust_snippets.json rust
```

Test names are automatically generated from markdown headings using `name_utils.py`.

### Generate tests
```bash
# Generate C++ tests
python3 docs/scripts/c_cpp/generate_cpp_tests.py

# Generate C tests  
python3 docs/scripts/c_cpp/generate_c_tests.py
```

## Adding a new language generator
1. Create your generator script (base your implementation on
`docs/scripts/c_cpp/generate_cpp_tests.py`)
2. Import and use `docs.scripts.common.snippet_extractor.SnippetExtractor` 
3. Call `extractor.extract_all_snippets(['your_language'])` to get snippets
4. Generate your test files from the `CodeSnippet` instances

## Testing
Each package has its own test directory:
```bash
# Test snippet extractor
python3 docs/scripts/common/test/test_snippet_extractor.py

# Test doxygen filter
python3 docs/scripts/doxygen/test/test_doxygen_filter.py
```
