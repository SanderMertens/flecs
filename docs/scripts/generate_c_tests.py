#!/usr/bin/env python3
"""
C test generator for Flecs documentation code snippets.

This script extracts C code snippets with 'test' attribute from documentation
and generates test files in the test/docs/core project.
"""

import os
import sys
import json
from pathlib import Path
from typing import List
from test_snippet_extractor import TestSnippetExtractor, CodeSnippet


class CTestGenerator(TestSnippetExtractor):
    """Generates C test files from documentation snippets."""
    
    def __init__(self, docs_root: str = "docs", test_root: str = "test/docs/core"):
        super().__init__(docs_root)
        self.test_root = test_root
        self.src_dir = Path(test_root) / "src"
        self.include_dir = Path(test_root) / "include"
    
    def ensure_directories(self):
        """Ensure test directories exist."""
        self.src_dir.mkdir(parents=True, exist_ok=True)
        self.include_dir.mkdir(parents=True, exist_ok=True)
    
    def generate_test_file_content(self, file_path: str, snippets: List[CodeSnippet]) -> str:
        """Generate a C test file content from snippets from the same markdown file."""
        # Get the file name without extension for the test file
        file_stem = Path(file_path).stem.lower()
        
        # Generate function declarations and implementations
        function_declarations = []
        function_implementations = []
        test_names = []
        
        for snippet in snippets:
            if snippet.language == 'c':
                test_names.append(snippet.test_name)
                function_declarations.append(f"void {snippet.test_name}(void);")
                
                # Generate function implementation containing only the snippet
                func_impl = f"""void {snippet.test_name}(void) {{
{self._indent_code(snippet.get_clean_code(), "    ")}
}}"""
                function_implementations.append(func_impl)
        
        if not test_names:
            return None
        
        # Generate complete file content
        test_content = f"""/* 
 * Tests generated from: {file_path}
 * Contains {len(test_names)} test(s) from documentation code snippets
 */

#include <flecs.h>
#include <test.h>

// (No hardcoded component or world setup is injected)

// Test function declarations
{'\n'.join(function_declarations)}

// Test function implementations

{'\n\n'.join(function_implementations)}

// Test runner for this file
void run_{file_stem}_tests(void) {{
    printf("Running {file_stem} tests...\\n");
    
{'\n'.join(f"    {name}();" for name in test_names)}
    
    printf("All {file_stem} tests passed!\\n");
}}
"""
        return test_content
    
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
            runner_declarations.append(f"void {runner}(void);")
            runner_calls.append(f"    {runner}();")
        
        main_content = f"""/*
 * Generated test runner for C documentation snippets
 * Auto-generated from markdown files with 'test' code blocks
 */

#include <flecs.h>
#include <stdio.h>

// Test runner declarations
{'\n'.join(runner_declarations)}

int main(int argc, char *argv[]) {{
    printf("Running C documentation tests...\\n");
    
{'\n'.join(runner_calls)}
    
    printf("All C documentation tests passed!\\n");
    return 0;
}}
"""
        return main_content
    
    def update_project_json(self, all_test_names: List[str]):
        """Update project.json with test cases."""
        project_file = Path(self.test_root) / "project.json"
        
        # Read existing project.json
        try:
            with open(project_file, 'r') as f:
                project_data = json.load(f)
        except FileNotFoundError:
            # Create new project structure
            project_data = {
                "id": "core",
                "type": "application",
                "value": {
                    "author": "Sander Mertens",
                    "description": "tests for the docs C code samples of flecs",
                    "public": False,
                    "coverage": False,
                    "use": [
                        "flecs"
                    ]
                }
            }
        
        # Add test section if it doesn't exist
        if "test" not in project_data:
            project_data["test"] = {"testsuites": []}
        
        # Find or create documentation test suite
        doc_suite = None
        for suite in project_data["test"]["testsuites"]:
            if suite["id"] == "Documentation":
                doc_suite = suite
                break
        
        if doc_suite is None:
            doc_suite = {
                "id": "Documentation", 
                "testcases": []
            }
            project_data["test"]["testsuites"].append(doc_suite)
        
        # Update test cases (replace existing ones)
        doc_suite["testcases"] = all_test_names
        
        # Write updated project.json
        with open(project_file, 'w') as f:
            json.dump(project_data, f, indent=4)
        
        print(f"Updated {project_file} with {len(all_test_names)} test cases")
    
    def generate_all_tests(self):
        """Generate all C test files from documentation snippets."""
        # Extract C snippets grouped by file
        snippets_by_file = self.extract_all_snippets(['c'])
        
        if not snippets_by_file:
            print("No C test snippets found in documentation")
            return
        
        # Ensure directories exist
        self.ensure_directories()
        
        # Generate test files (one per markdown file)
        test_file_runners = []
        all_test_names = []
        
        for md_file_path, snippets in snippets_by_file.items():
            c_snippets = [s for s in snippets if s.language == 'c']
            
            if not c_snippets:
                continue
            
            # Generate test file content
            file_stem = Path(md_file_path).stem.lower()
            test_content = self.generate_test_file_content(md_file_path, c_snippets)
            
            if test_content:
                test_file = self.src_dir / f"{file_stem}.c"
                
                with open(test_file, 'w') as f:
                    f.write(test_content)
                
                print(f"Generated {test_file} with {len(c_snippets)} test(s)")
                
                # Track the runner function and test names
                test_file_runners.append(f"run_{file_stem}_tests")
                all_test_names.extend([s.test_name for s in c_snippets])
        
        # Generate main test runner
        if test_file_runners:
            main_content = self.generate_main_test_file(test_file_runners)
            main_file = self.src_dir / "main.c"
            
            with open(main_file, 'w') as f:
                f.write(main_content)
            
            print(f"Generated {main_file}")
            
            # Update project.json
            self.update_project_json(all_test_names)
            
            print(f"\\nGenerated {len(test_file_runners)} C test file(s) with {len(all_test_names)} total tests in {self.test_root}")
        else:
            print("No C test files generated")


def main():
    """Main function."""
    if len(sys.argv) > 1:
        docs_root = sys.argv[1]
    else:
        docs_root = "docs"
    
    if len(sys.argv) > 2:
        test_root = sys.argv[2]
    else:
        test_root = "test/docs/core"
    
    generator = CTestGenerator(docs_root, test_root)
    generator.generate_all_tests()


if __name__ == "__main__":
    main()