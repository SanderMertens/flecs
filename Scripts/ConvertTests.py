#!/usr/bin/env python3

import os
import re
import sys
import shutil

# Regex to detect a function signature like:
#   void Some_test_name(void)
FUNCTION_SIGNATURE_REGEX = re.compile(
    r'\bvoid\s+([A-Za-z0-9_]+)\s*\(\s*void\s*\)\s*{?'
)

def main():
    """
    Usage:
        python convert_flecs_tests.py <read_directory> <write_directory>
    """
    if len(sys.argv) < 3:
        print("Usage: python convert_flecs_tests.py <read_directory> <write_directory>")
        sys.exit(1)

    read_directory = sys.argv[1]
    write_directory = sys.argv[2]

    # 1) Wipe the write directory
    if os.path.exists(write_directory):
        shutil.rmtree(write_directory)
    os.makedirs(write_directory, exist_ok=True)

    # 2) Walk through read_directory, find all .c / .cpp files
    for root, dirs, files in os.walk(read_directory):
        for file_name in files:
            if file_name.endswith(".c") or file_name.endswith(".cpp"):
                file_path = os.path.join(root, file_name)

                with open(file_path, "r", encoding="utf-8") as f:
                    content = f.read()

                # 3) Extract all test functions from this file
                test_functions = extract_test_functions(content)
                if not test_functions:
                    # No test functions found; skip
                    continue

                # 4) We generate a single output file for each .c/.cpp,
                #    containing multiple IMPLEMENT_SIMPLE_AUTOMATION_TEST blocks
                base_name, ext = os.path.splitext(file_name)
                output_file_name = f"{base_name}Test.cpp"
                output_path = os.path.join(write_directory, output_file_name)

                file_content = create_unreal_test_file(
                    original_file=file_name,
                    test_functions=test_functions
                )

                with open(output_path, "w", encoding="utf-8") as out_f:
                    out_f.write(file_content)

                print(f"Created Unreal test file: {output_file_name}")


def extract_test_functions(file_content):
    """
    Return a list of dictionaries describing each function found:
      [
        {
          "name": "Doc_get_entity_name",
          "signature": "void Doc_get_entity_name(void)",
          "body": "  // code inside braces\n"
        },
        ...
      ]
    Uses a brace-matching approach to capture the full function body.
    """
    extracted = []
    idx = 0
    length = len(file_content)

    while idx < length:
        match = FUNCTION_SIGNATURE_REGEX.search(file_content, idx)
        if not match:
            break

        func_name = match.group(1)
        start_of_signature = match.start()
        search_start = match.end()

        # Find the '{' that starts the function body
        open_brace_idx = file_content.find('{', search_start)
        if open_brace_idx == -1:
            # No '{' => not a valid definition
            idx = search_start
            continue

        # Use a simple brace counter with a small state machine
        brace_stack = 0
        i = open_brace_idx
        function_end_idx = None

        state = "normal"  # normal, in_single_line_comment, in_multi_line_comment, in_string, in_char

        while i < length:
            c = file_content[i]

            if state == "in_single_line_comment":
                if c == '\n':
                    state = "normal"
            elif state == "in_multi_line_comment":
                if c == '*' and i + 1 < length and file_content[i+1] == '/':
                    state = "normal"
                    i += 2
                    continue
            elif state == "in_string":
                if c == '"' and not is_escaped(file_content, i):
                    state = "normal"
            elif state == "in_char":
                if c == "'" and not is_escaped(file_content, i):
                    state = "normal"
            else:
                # state == "normal"
                if c == '/' and i + 1 < length:
                    nxt = file_content[i + 1]
                    if nxt == '/':
                        state = "in_single_line_comment"
                        i += 2
                        continue
                    elif nxt == '*':
                        state = "in_multi_line_comment"
                        i += 2
                        continue
                elif c == '"' and not is_escaped(file_content, i):
                    state = "in_string"
                elif c == "'" and not is_escaped(file_content, i):
                    state = "in_char"
                elif c == '{':
                    brace_stack += 1
                elif c == '}':
                    brace_stack -= 1
                    if brace_stack == 0:
                        function_end_idx = i + 1  # position after '}'
                        break

            i += 1

        if function_end_idx:
            # Full range is from start_of_signature to function_end_idx
            sig_text = file_content[start_of_signature:open_brace_idx].strip()
            body_text = file_content[open_brace_idx+1:function_end_idx-1]

            extracted.append({
                "name": func_name,
                "signature": sig_text,  # e.g. "void Doc_get_entity_name(void)"
                "body": body_text
            })
            idx = function_end_idx
        else:
            idx = open_brace_idx + 1

    return extracted


def is_escaped(file_content, index):
    """
    Returns True if the character at 'index' is preceded by
    an odd number of backslashes (meaning it's escaped).
    """
    backslash_count = 0
    i = index - 1
    while i >= 0 and file_content[i] == '\\':
        backslash_count += 1
        i -= 1
    return (backslash_count % 2) == 1


def create_unreal_test_file(original_file, test_functions):
    """
    Generate a single .cpp output that contains:
    - #if WITH_AUTOMATION_TESTS
    - The necessary includes
    - For each test function, an IMPLEMENT_SIMPLE_AUTOMATION_TEST block
      that inlines the function code in RunTest().
    """

    # Start with the #if and includes
    content_lines = [
        "#if WITH_AUTOMATION_TESTS",
        "",
        '#include "Bake/FlecsTestUtils.h"',
        '#include "flecs.h"',
        '#include <stdio.h>',
        '#include <stdlib.h>',
        '#include "Misc/AutomationTest.h"  // For IMPLEMENT_SIMPLE_AUTOMATION_TEST',
        ""
    ]

    # Add one automation test per function
    for func in test_functions:
        class_name = f"F{func['name']}Test"
        display_name = f"Flecs.{func['name']}"
        signature_comment = f'// Original function: "{func["signature"]}"'
        file_comment = f'// From file: "{original_file}"'

        # Indent body content
        indented_body = indent_multiline(func["body"], "        ")

        test_block = f"""
IMPLEMENT_SIMPLE_AUTOMATION_TEST({class_name}, "{display_name}", \
EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool {class_name}::RunTest(const FString& Parameters)
{{
    {signature_comment}
    {file_comment}

    // Inline the original test code here:
    {{
{indented_body}
    }}

    return true;
}}
"""
        content_lines.append(test_block)

    # Close out the file with the #endif
    content_lines.append("#endif // WITH_AUTOMATION_TESTS")
    content_lines.append("")

    return "\n".join(content_lines)


def indent_multiline(text, prefix):
    """
    Trim leading/trailing blank lines, then prefix each line with `prefix`.
    """
    text = text.strip('\n\r')
    lines = text.splitlines()
    return "\n".join(prefix + line for line in lines)


if __name__ == "__main__":
    main()
