#!/usr/bin/env python3
"""
Doxygen filter to clean markdown for documentation:
- Remove 'test' from code blocks: ```cpp test -> ```cpp  
- Remove HIDE: lines
"""

import sys
import re

# Read input file
with open(sys.argv[1], 'r') as f:
    content = f.read()

# Process each line
filtered_lines = []
for line in content.split('\n'):
    # Remove 'test' attribute from code blocks
    if re.match(r'^```(\w+)\s+test\s*$', line):
        language = re.match(r'^```(\w+)', line).group(1)
        filtered_lines.append(f'```{language}')
    # Skip HIDE: lines  
    elif re.match(r'^\s*HIDE:', line):
        continue
    # Keep all other lines
    else:
        filtered_lines.append(line)

# Output filtered content
print('\n'.join(filtered_lines), end='')
