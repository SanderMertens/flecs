import re

def indent_code(code: str, indent: str = '    ') -> str:
    lines = code.split('\n')
    out = []
    for line in lines:
        if line.strip():
            out.append(indent + line)
        else:
            out.append("")
    return '\n'.join(out)

def replace_asserts(code: str) -> str:
    """Replace plain 'assert(' with 'test_assert(' for test frameworks."""
    return code.replace('assert(', 'test_assert(')
