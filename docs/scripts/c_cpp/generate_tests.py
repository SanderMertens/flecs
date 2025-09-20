import json
from pathlib import Path
from typing import List, Optional
import sys

# Add repo root to Python path so we can import docs.scripts modules
repo_root = Path(__file__).resolve().parents[3]  # Go up 3 levels: c_cpp -> scripts -> docs -> repo_root
sys.path.insert(0, str(repo_root))

from docs.scripts.common.snippet_extractor import SnippetExtractor, CodeSnippet
from docs.scripts.c_cpp.code_utils import indent_code, replace_asserts
from docs.scripts.c_cpp.project_json import update_testsuite_with_cases


class UnifiedTestGenerator(SnippetExtractor):
    """Unified generator for both C and C++ tests from documentation snippets."""
    
    def __init__(self, docs_root: str = "docs"):
        super().__init__(docs_root)
        self.language_configs = {
            "c": {
                "test_root": "test/docs/core",
                "header": "core.h",
                "extension": ".c",
                "include_declarations": True
            },
            "cpp": {
                "test_root": "test/docs/cpp", 
                "header": "cpp.h",
                "extension": ".cpp",
                "include_declarations": False
            }
        }
    
    def ensure_directories(self):
        """Ensure all test directories exist."""
        for config in self.language_configs.values():
            src_dir = Path(config["test_root"]) / "src"
            include_dir = Path(config["test_root"]) / "include"
            src_dir.mkdir(parents=True, exist_ok=True)
            include_dir.mkdir(parents=True, exist_ok=True)

    def generate_test_file_content(self, file_path: str, snippets: List[CodeSnippet], language: str) -> Optional[str]:
        """Generate test file content for a specific language."""
        config = self.language_configs[language]
        file_stem = Path(file_path).stem.lower()

        function_declarations = []
        function_implementations = []
        test_names = []

        for snippet in snippets:
            if snippet.language != language:
                continue

            symbol = f"{file_stem}_{snippet.test_name}"
            test_names.append(snippet.test_name)
            function_declarations.append(f"void {symbol}(void);")

            snippet_code = snippet.get_clean_code()
            snippet_code = replace_asserts(snippet_code)

            if not any(tok in snippet_code for tok in ("test_", "test_assert", "test_int", "test_str", "test_bool")):
                # For C++, indent the test_assert more
                indent_prefix = "\n\n    " if language == "cpp" else "\n\n"
                snippet_code = snippet_code.rstrip() + f"{indent_prefix}test_assert(true);"

            func_impl = f"""void {symbol}(void) {{
{indent_code(snippet_code, '    ')}
}}"""
            function_implementations.append(func_impl)

        if not test_names:
            return None

        # Generate content
        header = f"#include <{config['header']}>\n\n"
        
        if config["include_declarations"]:
            # C style with declarations
            declarations = '\n'.join(function_declarations)
            content = f"""/*
 * Tests generated from: {file_path}
 * Contains {len(test_names)} test(s) from documentation code snippets
 */

{header}// Test declarations
{declarations}

// Test implementations

{chr(10).join(function_implementations)}
"""
        else:
            # C++ style (no declarations)
            implementations = "\n\n".join(function_implementations) + "\n"
            content = f"""/*
 * Tests generated from: {file_path}
 * Contains {len(test_names)} test(s) from documentation code snippets  
 */

{header}{implementations}"""

        return content

    def generate_all_tests(self):
        """Generate test files for both C and C++ languages."""
        snippets_by_file = self.extract_all_snippets(['c', 'cpp'])
        if not snippets_by_file:
            print("No C/C++ test snippets found in documentation")
            return

        self.ensure_directories()
        
        # Track results for both languages
        results = {}

        for language in ['c', 'cpp']:
            config = self.language_configs[language]
            suites_map = {}
            file_count = 0

            for md_file_path, snippets in snippets_by_file.items():
                lang_snippets = [s for s in snippets if s.language == language]
                if not lang_snippets:
                    continue

                file_stem = Path(md_file_path).stem.lower()
                test_content = self.generate_test_file_content(md_file_path, lang_snippets, language)
                if test_content:
                    test_file = Path(config["test_root"]) / "src" / f"{file_stem}{config['extension']}"
                    with open(test_file, 'w') as f:
                        f.write(test_content)
                    print(f"Generated {test_file} with {len(lang_snippets)} test(s)")
                    suites_map[file_stem] = [s.test_name for s in lang_snippets]
                    file_count += 1

            # Update project.json per suite
            if suites_map:
                project_file = Path(config["test_root"]) / "project.json"
                for suite_id, testcases in suites_map.items():
                    update_testsuite_with_cases(project_file, suite_id, testcases)

            total_tests = sum(len(test_names) for test_names in suites_map.values())
            results[language] = {"files": file_count, "tests": total_tests}
            print(f"\nGenerated {file_count} {language.upper()} test file(s) with {total_tests} total tests in {config['test_root']}")
        
        # Summary
        total_files = sum(r["files"] for r in results.values())
        total_tests = sum(r["tests"] for r in results.values())
        print(f"\n=== Summary ===")
        print(f"Total: {total_files} test file(s) with {total_tests} total tests generated")


def main():
    import sys
    if len(sys.argv) > 1:
        docs_root = sys.argv[1]
    else:
        docs_root = "docs"

    gen = UnifiedTestGenerator(docs_root)
    gen.generate_all_tests()


if __name__ == '__main__':
    main()
