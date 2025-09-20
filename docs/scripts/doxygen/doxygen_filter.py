#!/usr/bin/env python3
"""
Doxygen filter script for Flecs documentation.

This script processes markdown files to:
1. Remove 'test' attribute from code blocks (e.g., ```cpp test -> ```cpp)
2. Remove lines that start with 'HIDE: '

Usage:
    python3 doxygen_filter.py input_file
"""

import sys
import re


def filter_content(content):
    """
    Filter the content by removing test attributes and HIDE lines.
    
    Args:
        content (str): The input content to filter
        
    Returns:
        str: The filtered content
    """
    lines = content.split('\n')
    filtered_lines = []
    in_code_block = False
    
    for line in lines:
        # Check if this is a code block start with test attribute
        # Pattern matches: ```language test (with optional whitespace)
        code_block_match = re.match(r'^```(\w+)\s+test\s*$', line)
        if code_block_match:
            # Replace with just the language, no test attribute
            language = code_block_match.group(1)
            filtered_lines.append(f'```{language}')
            in_code_block = True
            continue
        
        # Check if this is a code block end
        if line.startswith('```') and in_code_block:
            in_code_block = False
            filtered_lines.append(line)
            continue
        
        # Check if line starts with HIDE: (with optional whitespace before HIDE)
        if re.match(r'^\s*HIDE:\s', line):
            # Skip this line entirely
            continue
        
        # Keep all other lines
        filtered_lines.append(line)
    
    return '\n'.join(filtered_lines)


def main():
    """Main function to process input file or stdin."""
    if len(sys.argv) != 2:
        print("Usage: python3 doxygen_filter.py <input_file>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    
    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        filtered_content = filter_content(content)
        print(filtered_content, end='')
        
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error processing file: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
