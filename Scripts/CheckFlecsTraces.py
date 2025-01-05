#!/usr/bin/env python3
import os
import sys
import clang.cindex

# ---------------------------------------------------------------------------
# Adjust to point clang.cindex to your libclang.dll
# ---------------------------------------------------------------------------
clang.cindex.Config.set_library_file(r"C:\Program Files\LLVM\bin\libclang.dll")

# ---------------------------------------------------------------------------
# Utility: which files to parse
# ---------------------------------------------------------------------------
def is_source_file(path):
    _, ext = os.path.splitext(path.lower())
    return ext in (".c", ".cpp", ".cc", ".cxx", ".h", ".hpp", ".hxx")

def find_source_files(root_dir):
    for dirpath, _dirs, files in os.walk(root_dir):
        for f in files:
            full_path = os.path.join(dirpath, f)
            if is_source_file(full_path):
                yield full_path

# ---------------------------------------------------------------------------
# Determine function name
# ---------------------------------------------------------------------------
def get_function_name(node):
    if node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
        return node.spelling
    elif node.kind == clang.cindex.CursorKind.CXX_METHOD:
        return node.spelling
    elif node.kind == clang.cindex.CursorKind.CONSTRUCTOR:
        return node.spelling
    return None

# ---------------------------------------------------------------------------
# Visit function: gather push/pop calls
# ---------------------------------------------------------------------------
def visit_function_and_check(node, filename):
    func_name = get_function_name(node)
    if not func_name:
        return []

    calls = []
    # We also track return lines if we want further checks
    return_locations = []

    def _visitor(child, depth=0):
        # Debug logging of every AST node
        kind = child.kind
        spelling = child.spelling
        line = child.extent.start.line

        if kind == clang.cindex.CursorKind.CALL_EXPR:
            called_name = child.spelling or "<no_spelling>"
            if called_name in ("ecs_os_perf_trace_push", "ecs_os_perf_trace_pop"):
                # Attempt to get string literal
                arg_literal = None
                for arg in child.get_children():
                    if arg.kind == clang.cindex.CursorKind.STRING_LITERAL:
                        arg_literal = arg.displayname
                        break
                if not arg_literal:
                    arg_literal = "UNKNOWN_STRING"

                # -- Skip if unknown to avoid spurious mismatch
                if called_name == "ecs_os_perf_trace_push":
                    calls.append(("push", arg_literal, line))
                else:
                    calls.append(("pop", arg_literal, line))

        elif kind == clang.cindex.CursorKind.RETURN_STMT:
            return_locations.append(line)

        # Recurse
        for c in child.get_children():
            _visitor(c, depth+1)

    # Traverse
    _visitor(node)

    # If no push calls remain, skip checks
    push_calls = [c for c in calls if c[0] == "push"]
    if not push_calls:
        return []

    # Tally up push/pop calls
    push_map = {}
    pop_map = {}

    for kind, literal, line in calls:
        if kind == "push":
            push_map.setdefault(literal, 0)
            push_map[literal] += 1
        else:  # pop
            pop_map.setdefault(literal, 0)
            pop_map[literal] += 1

    # Compare counts
    issues = []
    all_literals = set(push_map.keys()) | set(pop_map.keys())
    for literal in all_literals:
        push_count = push_map.get(literal, 0)
        pop_count = pop_map.get(literal, 0)
        if pop_count < push_count:
            issues.append(
                f"In function '{func_name}' at {filename}: "
                f"Missing pop(s) for push {literal} (pushed {push_count}, popped {pop_count})."
            )
        elif pop_count > push_count:
            issues.append(
                f"In function '{func_name}' at {filename}: "
                f"Too many pop(s) for push {literal} (pushed {push_count}, popped {pop_count})."
            )

    return issues

# ---------------------------------------------------------------------------
# Parse each file, gather issues
# ---------------------------------------------------------------------------
def process_file(filename):
    index = clang.cindex.Index.create()

    translation_unit = index.parse(
        filename,
        args=["-x", "c", "-std=c11"]
    )

    if not translation_unit:
        print(f"Could not parse {filename}")
        return []

    all_issues = []

    def traverse(node):
        # If this is a function definition
        if (
                node.kind == clang.cindex.CursorKind.FUNCTION_DECL and node.is_definition()
        ) or (
                node.kind == clang.cindex.CursorKind.CXX_METHOD and node.is_definition()
        ):
            issues = visit_function_and_check(node, filename)
            all_issues.extend(issues)

        for c in node.get_children():
            traverse(c)

    print(f"\n[INFO] Parsing: {filename}")
    traverse(translation_unit.cursor)
    return all_issues

# ---------------------------------------------------------------------------
# Main routine
# ---------------------------------------------------------------------------
def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <directory>")
        sys.exit(1)

    root_dir = sys.argv[1]
    source_files = list(find_source_files(root_dir))
    if not source_files:
        print(f"No source files found under {root_dir}")
        sys.exit(0)

    total_issues = []
    for f in source_files:
        issues = process_file(f)
        total_issues.extend(issues)

    if total_issues:
        print("\nFound the following issues with ecs_os_perf_trace_push/pop usage:")
        for issue in total_issues:
            print(f"  - {issue}")
    else:
        print("\nNo push/pop issues found.")

if __name__ == "__main__":
    main()
