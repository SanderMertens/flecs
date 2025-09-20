import json
from pathlib import Path
from typing import List
import sys

# Add repo root to Python path so we can import docs.scripts modules
repo_root = Path(__file__).resolve().parents[3]  # Go up 3 levels: c_cpp -> scripts -> docs -> repo_root
sys.path.insert(0, str(repo_root))

from docs.scripts.common.snippet_extractor import SnippetExtractor, CodeSnippet
from docs.scripts.c_cpp.code_utils import indent_code, replace_asserts
from docs.scripts.c_cpp.project_json import update_testsuite_with_cases


class CppTestGenerator(SnippetExtractor):
    def __init__(self, docs_root: str = "docs", test_root: str = "test/docs/cpp"):
        super().__init__(docs_root)
        self.test_root = test_root
        self.src_dir = Path(test_root) / "src"
        self.include_dir = Path(test_root) / "include"

    def ensure_directories(self):
        self.src_dir.mkdir(parents=True, exist_ok=True)
        self.include_dir.mkdir(parents=True, exist_ok=True)

    def generate_test_file_content(self, file_path: str, snippets: List[CodeSnippet]) -> str:
        file_stem = Path(file_path).stem.lower()
        function_implementations = []

        for snippet in snippets:
            if snippet.language != 'cpp':
                continue

            symbol = f"{file_stem}_{snippet.test_name}"
            snippet_code = snippet.get_clean_code()
            snippet_code = replace_asserts(snippet_code)

            if not any(tok in snippet_code for tok in ("test_", "test_assert", "test_int", "test_str", "test_bool")):
                snippet_code = snippet_code.rstrip() + "\n\n    test_assert(true);"

            func_impl = f"""void {symbol}(void) {{
{indent_code(snippet_code, '    ')}
}}"""
            function_implementations.append(func_impl)

        if not function_implementations:
            return None

        body = "\n\n".join(function_implementations) + "\n"
        header = "#include <cpp.h>\n\n"
        return header + body

    def generate_all_tests(self):
        snippets_by_file = self.extract_all_snippets(['cpp'])
        if not snippets_by_file:
            print("No C++ test snippets found in documentation")
            return

        self.ensure_directories()

        suites_map = {}

        for md_file_path, snippets in snippets_by_file.items():
            cpp_snippets = [s for s in snippets if s.language == 'cpp']
            if not cpp_snippets:
                continue

            file_stem = Path(md_file_path).stem.lower()
            test_content = self.generate_test_file_content(md_file_path, cpp_snippets)
            if test_content:
                test_file = self.src_dir / f"{file_stem}.cpp"
                with open(test_file, 'w') as f:
                    f.write(test_content)
                print(f"Generated {test_file} with {len(cpp_snippets)} test(s)")
                suites_map[file_stem] = [s.test_name for s in cpp_snippets]

        # update project.json per suite
        if suites_map:
            project_file = Path(self.test_root) / "project.json"
            for suite_id, testcases in suites_map.items():
                update_testsuite_with_cases(project_file, suite_id, testcases)
            print(f"\nGenerated {len(suites_map)} C++ test file(s) with {sum(len(v) for v in suites_map.values())} total tests in {self.test_root}")
        else:
            print("No C++ test files generated")


def main():
    import sys
    if len(sys.argv) > 1:
        docs_root = sys.argv[1]
    else:
        docs_root = "docs"

    if len(sys.argv) > 2:
        test_root = sys.argv[2]
    else:
        test_root = "test/docs/cpp"

    gen = CppTestGenerator(docs_root, test_root)
    gen.generate_all_tests()


if __name__ == '__main__':
    main()
