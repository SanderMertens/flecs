import os
import sys
import clang.cindex
from tqdm import tqdm

clang.cindex.Config.set_library_file(r"C:\\Program Files\\LLVM\\bin\\libclang.dll")

def find_h_files(directory):
    h_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".h"):
                h_files.append(os.path.join(root, file))
    return h_files

def analyze_function_for_restrict(node, file_path):
    if node.kind == clang.cindex.CursorKind.FUNCTION_DECL and node.location.file and node.location.file.name == file_path:
        parameters = list(node.get_arguments())
        candidates = []
        for param in parameters:
            if param.type.kind == clang.cindex.TypeKind.POINTER:
                pointee = param.type.get_pointee()
                if pointee.kind in {
                    clang.cindex.TypeKind.SCHAR,
                    clang.cindex.TypeKind.UCHAR,
                    clang.cindex.TypeKind.VOID,
                    clang.cindex.TypeKind.INT,
                    clang.cindex.TypeKind.FLOAT,
                    clang.cindex.TypeKind.DOUBLE,
                }:
                    candidates.append(param.spelling)
        if candidates:
            return node.spelling, candidates
    return None

def analyze_file(file_path):
    index = clang.cindex.Index.create()
    translation_unit = index.parse(file_path, args=['-std=c11'])
    restrict_candidates = []
    for cursor in translation_unit.cursor.get_children():
        result = analyze_function_for_restrict(cursor, file_path)
        if result:
            restrict_candidates.append(result)
    return restrict_candidates

def main():
    if len(sys.argv) != 2:
        print("Usage: python analyze_restrict.py <directory>")
        sys.exit(1)

    directory = sys.argv[1]
    if not os.path.isdir(directory):
        print(f"Error: {directory} is not a valid directory.")
        sys.exit(1)

    h_files = find_h_files(directory)
    if not h_files:
        print("No .h files found in the directory.")
        sys.exit(0)

    print(f"Found {len(h_files)} .h files. Analyzing...")

    results = {}
    for file_path in tqdm(h_files, desc="Analyzing files"):
        restrict_candidates = analyze_file(file_path)
        if restrict_candidates:
            results[file_path] = restrict_candidates

    if results:
        print("\nAnalysis Results:")
        for file, functions in results.items():
            print(f"\nFile: {file}")
            for func_name, params in functions:
                print(f"  Function: {func_name}")
                print(f"    Candidates for 'restrict': {', '.join(params)}")
    else:
        print("No candidates for 'restrict' found.")

if __name__ == "__main__":
    main()
