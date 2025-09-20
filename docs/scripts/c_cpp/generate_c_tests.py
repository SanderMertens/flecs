import json
from pathlib import Path
from typing import List
import sys

# Add repo root to Python path so we can import docs.scripts modules
repo_root = Path(__file__).resolve().parents[3]  # Go up 3 levels: c_cpp -> scripts -> docs -> repo_root
sys.path.insert(0, str(repo_root))

from docs.scripts.common.snippet_extractor import SnippetExtractor, CodeSnippet
from docs.scripts.c_cpp.code_utils import indent_code
from docs.scripts.c_cpp.project_json import update_testsuite_with_cases


class CTestGenerator(SnippetExtractor):
    def __init__(self, docs_root: str = "docs", test_root: str = "test/docs/core"):
        super().__init__(docs_root)
        self.test_root = test_root
        self.src_dir = Path(test_root) / "src"
        self.include_dir = Path(test_root) / "include"

    def ensure_directories(self):
        self.src_dir.mkdir(parents=True, exist_ok=True)
        self.include_dir.mkdir(parents=True, exist_ok=True)

    def generate_test_file_content(self, file_path: str, snippets: List[CodeSnippet]) -> str:
        file_stem = Path(file_path).stem.lower()

        function_declarations = []
        function_implementations = []
        test_names = []

        for snippet in snippets:
            if snippet.language != 'c':
                continue

            test_names.append(snippet.test_name)
            function_declarations.append(f"void {snippet.test_name}(void);")

            func_impl = f"""void {snippet.test_name}(void) {{
{indent_code(snippet.get_clean_code(), '    ')}
}}"""
            function_implementations.append(func_impl)

        if not test_names:
            return None

        content = f"""/*
 * Tests generated from: {file_path}
 * Contains {len(test_names)} test(s) from documentation code snippets
 */

#include <core.h>

// Test declarations
{('\n'.join(function_declarations))}

// Test implementations

{('\n\n'.join(function_implementations))}

void run_{file_stem}_tests(void) {{
    printf("Running {file_stem} tests...\n");
{('\n'.join(f'    {name}();' for name in test_names))}
    printf("All {file_stem} tests passed!\n");
}}
"""

        return content

    def generate_all_tests(self):
        snippets_by_file = self.extract_all_snippets(['c'])
        if not snippets_by_file:
            print("No C test snippets found in documentation")
            return

        self.ensure_directories()

        all_test_names = []

        for md_file_path, snippets in snippets_by_file.items():
            c_snippets = [s for s in snippets if s.language == 'c']
            if not c_snippets:
                continue

            file_stem = Path(md_file_path).stem.lower()
            test_content = self.generate_test_file_content(md_file_path, c_snippets)
            if test_content:
                test_file = self.src_dir / f"{file_stem}.c"
                with open(test_file, 'w') as f:
                    f.write(test_content)
                print(f"Generated {test_file} with {len(c_snippets)} test(s)")
                all_test_names.extend([s.test_name for s in c_snippets])

        if all_test_names:
            project_file = Path(self.test_root) / "project.json"
            update_testsuite_with_cases(project_file, "Documentation", all_test_names)
            print(f"\nGenerated {len(all_test_names)} C test(s) in {self.test_root}")
        else:
            print("No C test files generated")


def main():
    import sys
    if len(sys.argv) > 1:
        docs_root = sys.argv[1]
    else:
        docs_root = "docs"

    if len(sys.argv) > 2:
        test_root = sys.argv[2]
    else:
        test_root = "test/docs/core"

    gen = CTestGenerator(docs_root, test_root)
    gen.generate_all_tests()


if __name__ == '__main__':
    main()
