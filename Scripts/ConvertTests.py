#!/usr/bin/env python3

import os
import re
import sys
import shutil
import logging

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)

# Regex for both static and non-static void functions
STATIC_FUNCTION_REGEX = re.compile(
    r'\bstatic\s+void\s+'  # static void
    r'([A-Za-z_][A-Za-z0-9_]*)'  # Function name
    r'\s*\(\s*(?:void)?\s*\)'  # (void) or ()
    r'\s*{',  # Opening brace
    re.MULTILINE
)

NON_STATIC_FUNCTION_REGEX = re.compile(
    r'(?<!static\s)'  # Not preceded by "static"
    r'\bvoid\s+'  # Return type "void"
    r'([A-Za-z_][A-Za-z0-9_]*)'  # Function name
    r'\s*\(\s*(?:void)?\s*\)'  # (void) or ()
    r'\s*{',  # Opening brace
    re.MULTILINE
)


def extract_function_body(content, start_pos):
    """Find the matching closing brace and extract function body."""
    brace_count = 1
    pos = start_pos

    while pos < len(content):
        if content[pos] == '{':
            brace_count += 1
        elif content[pos] == '}':
            brace_count -= 1
            if brace_count == 0:
                return content[start_pos:pos].strip()
        pos += 1
    return None


def extract_functions(file_content):
    """Find all static and non-static void functions and extract their bodies."""
    static_functions = []
    test_functions = []

    # Find static functions
    for match in STATIC_FUNCTION_REGEX.finditer(file_content):
        name = match.group(1)
        body = extract_function_body(file_content, match.end())
        if body:
            static_functions.append({
                'name': name,
                'body': body
            })
            logger.info(f"Found static function: {name}")

    # Find non-static functions
    for match in NON_STATIC_FUNCTION_REGEX.finditer(file_content):
        name = match.group(1)
        body = extract_function_body(file_content, match.end())
        if body:
            test_functions.append({
                'name': name,
                'body': body
            })
            logger.info(f"Found test function: {name}")

    return static_functions, test_functions


def create_test_file(origin_file, static_functions, test_functions):
    """Create a single .cpp file containing static helper functions and test functions."""
    lines = []
    lines.append("#if WITH_AUTOMATION_TESTS")
    lines.append("")
    lines.append('#include "Bake/FlecsTestUtils.h"')
    lines.append('#include "flecs.h"')
    lines.append('#include <stdio.h>')
    lines.append('#include <stdlib.h>')
    lines.append('#include "Misc/AutomationTest.h"')
    lines.append("")

    # Add static functions first
    if static_functions:
        lines.append("// Static helper functions")
        for func in static_functions:
            name = func['name']
            body = func['body']
            lines.append(f"static void {name}(void) {{")
            lines.append(body)
            lines.append("}")
            lines.append("")
        lines.append("// Test implementations")
        lines.append("")

    # Add test functions
    for func in test_functions:
        name = func['name']
        body = func['body']

        class_name = f"F{name}Test"
        display_name = f"Flecs.{name}"

        # Indent the body
        indented_body = '\n'.join(f"        {line}" for line in body.splitlines())

        test_block = f"""
IMPLEMENT_SIMPLE_AUTOMATION_TEST({class_name}, "{display_name}", \
EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool {class_name}::RunTest(const FString& Parameters)
{{
    // Original source: {origin_file}
{indented_body}
    return true;
}}
"""
        lines.append(test_block)

    lines.append("")
    lines.append("#endif // WITH_AUTOMATION_TESTS")
    return '\n'.join(lines)


def main():
    if len(sys.argv) < 3:
        print("Usage: python convert_tests.py <read_directory> <write_directory>")
        sys.exit(1)

    read_dir = sys.argv[1]
    write_dir = sys.argv[2]

    # Clean write directory
    if os.path.exists(write_dir):
        shutil.rmtree(write_dir)
    os.makedirs(write_dir)

    # Process each .c file
    for root, dirs, files in os.walk(read_dir):
        # Skip 'include' directories
        if 'include' in dirs:
            dirs.remove('include')

        # Skip if we're currently in an include directory
        if 'include' in root.lower():
            continue

        for fname in files:
            if not fname.endswith('.c'):
                continue

            file_path = os.path.join(root, fname)
            logger.info(f"Processing {file_path}...")

            try:
                # Read file content
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()

                # Extract function bodies
                static_functions, test_functions = extract_functions(content)
                if not test_functions:
                    logger.warning(f"No test functions found in {fname}")
                    continue

                # Generate test file
                base_name = os.path.splitext(fname)[0]
                out_fname = f"{base_name}Test.cpp"
                out_path = os.path.join(write_dir, out_fname)

                test_content = create_test_file(fname, static_functions, test_functions)

                with open(out_path, 'w', encoding='utf-8') as f:
                    f.write(test_content)

                logger.info(
                    f"Created {out_fname} with {len(static_functions)} static and {len(test_functions)} test functions")

            except Exception as e:
                logger.error(f"Error processing {fname}: {e}")


if __name__ == "__main__":
    main()