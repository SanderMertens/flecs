#!/usr/bin/env python3
"""
C++ test generator for Flecs documentation code snippets.

This script extracts C++ code snippets with 'test' attribute from documentation
and generates test files in the test/docs/cpp project.
"""

import os
import sys
import json
from pathlib import Path
from typing import List
from test_snippet_extractor import TestSnippetExtractor, CodeSnippet


class CppTestGenerator(TestSnippetExtractor):
    """Generates C++ test files from documentation snippets."""
    
    def __init__(self, docs_root: str = "docs", test_root: str = "test/docs/cpp"):
        super().__init__(docs_root)
        self.test_root = test_root
        self.src_dir = Path(test_root) / "src"
        self.include_dir = Path(test_root) / "include"
    
    def ensure_directories(self):
        """Ensure test directories exist."""
        self.src_dir.mkdir(parents=True, exist_ok=True)
        self.include_dir.mkdir(parents=True, exist_ok=True)
    
    def generate_test_file_content(self, file_path: str, snippets: List[CodeSnippet]) -> str:
        """Generate a C++ test file content from snippets from the same markdown file."""
        # Get the file name without extension for the test file
        file_stem = Path(file_path).stem.lower()
    # Generate only the prefixed function implementations
        function_implementations = []
        for snippet in snippets:
            if snippet.language != 'cpp':
                continue

            symbol = f"{file_stem}_{snippet.test_name}"
            snippet_code = snippet.get_clean_code()
            # Replace plain asserts with bake test assertions
            snippet_code = snippet_code.replace('assert(', 'test_assert(')

            # If the snippet doesn't contain any bake test macros (test_*),
            # append a dummy assertion so bake doesn't mark the testcase EMPTY.
            if not any(tok in snippet_code for tok in ("test_", "test_assert", "test_int", "test_str", "test_bool")):
                snippet_code = snippet_code.rstrip() + "\n\n    test_assert(true);"

            func_impl = f"""void {symbol}(void) {{
{self._indent_code(snippet_code, "    ")}
}}"""
            function_implementations.append(func_impl)

        if not function_implementations:
            return None

        body = "\n\n".join(function_implementations) + "\n"
        header = "#include <cpp.h>\n\n"
        return header + body
    
    def _indent_code(self, code: str, indent: str) -> str:
        """Indent code block."""
        lines = code.split('\n')
        indented_lines = []
        
        for line in lines:
            if line.strip():  # Don't indent empty lines
                indented_lines.append(indent + line)
            else:
                indented_lines.append("")
        
        return '\n'.join(indented_lines)
    
    def generate_main_test_file(self, test_file_runners: List[str]) -> str:
        """Generate the main test file that calls all test file runners."""
        runner_declarations = []
        runner_calls = []
        
        for runner in test_file_runners:
            runner_declarations.append(f"void {runner}();")
            runner_calls.append(f"    {runner}();")
        
        main_content = f"""/*
 * Generated test runner for C++ documentation snippets
 * Auto-generated from markdown files with 'test' code blocks
 */

#include <flecs.h>
#include <iostream>

// Test runner declarations
{'\n'.join(runner_declarations)}

int main(int argc, char *argv[]) {{
    std::cout << "Running C++ documentation tests..." << std::endl;
    
{'\n'.join(runner_calls)}
    
    std::cout << "All C++ documentation tests passed!" << std::endl;
    return 0;
}}
"""
        return main_content
    
    def update_project_json(self, suites_map):
        """Update project.json with test suites mapping.

        `suites_map` is a dict mapping suite_id -> list of test case names.
        """
        project_file = Path(self.test_root) / "project.json"
        
        # Read existing project.json
        try:
            with open(project_file, 'r') as f:
                project_data = json.load(f)
        except FileNotFoundError:
            # Create new project structure
            project_data = {
                "id": "cpp",
                "type": "application",
                "value": {
                    "author": "Sander Mertens",
                    "description": "tests for the docs C++ code samples of flecs",
                    "public": False,
                    "coverage": False,
                    "language": "c++",
                    "use": [
                        "flecs"
                    ]
                },
                "lang.cpp": {
                    "cpp-standard": "c++17"
                }
            }
        
        # Build testsuites list from suites_map
        testsuites = []
        for suite_id, testcases in suites_map.items():
            testsuites.append({
                "id": suite_id,
                "testcases": testcases
            })

        project_data["test"] = {"testsuites": testsuites}
        
        # Write updated project.json
        with open(project_file, 'w') as f:
            json.dump(project_data, f, indent=4)
    
    def generate_all_tests(self):
        """Generate all C++ test files from documentation snippets."""
        # Extract C++ snippets grouped by file
        snippets_by_file = self.extract_all_snippets(['cpp'])

        if not snippets_by_file:
            print("No C++ test snippets found in documentation")
            return

        # Ensure directories exist
        self.ensure_directories()

        # Generate test files (one per markdown file)
        test_file_runners = []
        suites_map = {}

        for md_file_path, snippets in snippets_by_file.items():
            cpp_snippets = [s for s in snippets if s.language == 'cpp']

            if not cpp_snippets:
                continue

            # Generate test file content
            file_stem = Path(md_file_path).stem.lower()
            test_content = self.generate_test_file_content(md_file_path, cpp_snippets)

            if test_content:
                test_file = self.src_dir / f"{file_stem}.cpp"

                with open(test_file, 'w') as f:
                    f.write(test_content)

                print(f"Generated {test_file} with {len(cpp_snippets)} test(s)")

                # Track the runner function and test names
                test_file_runners.append(f"run_{file_stem}_tests")
                suites_map[file_stem] = [s.test_name for s in cpp_snippets]

        # Generate main test runner (optional)
        if test_file_runners:
            # Do not generate a main runner; bake generates the runnable main. Update project.json instead.
            self.update_project_json(suites_map)

            print(f"\nGenerated {len(test_file_runners)} C++ test file(s) with {sum(len(v) for v in suites_map.values())} total tests in {self.test_root}")
        else:
            print("No C++ test files generated")


def main():
    """Main function."""
    if len(sys.argv) > 1:
        docs_root = sys.argv[1]
    else:
        docs_root = "docs"

    if len(sys.argv) > 2:
        test_root = sys.argv[2]
    else:
        test_root = "test/docs/cpp"

    generator = CppTestGenerator(docs_root, test_root)
    generator.generate_all_tests()


if __name__ == "__main__":
    main()
