#!/usr/bin/env python3
"""
Base script for extracting test code snippets from Flecs documentation.

This script finds all code blocks with the 'test' attribute in markdown files
and provides infrastructure for derived scripts to generate test files.
"""

import os
import re
import sys
import json
from pathlib import Path
from typing import List, Dict, Optional, Tuple


class CodeSnippet:
    """Represents a code snippet found in documentation."""
    
    def __init__(self, language: str, code: str, file_path: str, line_number: int, 
                 section_title: str = "", test_number: int = 1):
        self.language = language
        self.code = code
        self.file_path = file_path
        self.line_number = line_number
        self.section_title = section_title
        self.test_number = test_number
        self.test_name = self._generate_test_name()
    
    def _generate_test_name(self) -> str:
        """Generate test name based on section title and test number."""
        # Clean up section title to make it a valid identifier
        clean_title = re.sub(r'[^a-zA-Z0-9_]', '_', self.section_title)
        clean_title = re.sub(r'_+', '_', clean_title)  # Remove multiple underscores
        clean_title = clean_title.strip('_')  # Remove leading/trailing underscores
        
        if not clean_title:
            # Fallback to file name if no section title
            file_stem = Path(self.file_path).stem
            clean_title = re.sub(r'[^a-zA-Z0-9_]', '_', file_stem)
        
        return f"{clean_title}_{self.test_number:02d}"
    
    def get_clean_code(self) -> str:
        """Get code with HIDE: prefix removed but keeping the rest of the line."""
        lines = self.code.split('\n')
        clean_lines = []
        
        for line in lines:
            # Remove HIDE: prefix but keep the rest of the line
            if re.match(r'^\s*HIDE:\s*', line):
                # Remove the HIDE: prefix and any leading whitespace after it
                clean_line = re.sub(r'^\s*HIDE:\s*', '', line)
                if clean_line.strip():  # Only add non-empty lines
                    clean_lines.append(clean_line)
            else:
                clean_lines.append(line)
        
        return '\n'.join(clean_lines)


class TestSnippetExtractor:
    """Base class for extracting test snippets from documentation."""
    
    def __init__(self, docs_root: str = "docs"):
        self.docs_root = docs_root
        self.snippets_by_file: Dict[str, List[CodeSnippet]] = {}
    
    def find_documentation_files(self) -> List[str]:
        """Find all markdown files in the documentation directory."""
        md_files = []
        docs_path = Path(self.docs_root)
        
        if not docs_path.exists():
            raise FileNotFoundError(f"Documentation directory '{self.docs_root}' not found")
        
        for md_file in docs_path.rglob("*.md"):
            # Skip the scripts directory README
            if "scripts" not in str(md_file):
                md_files.append(str(md_file))
        
        return md_files
    
    def extract_code_snippets(self, file_path: str, target_languages: List[str]) -> List[CodeSnippet]:
        """Extract code snippets with 'test' attribute from a markdown file."""
        snippets = []
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except Exception as e:
            print(f"Warning: Could not read {file_path}: {e}")
            return snippets
        
        lines = content.split('\n')
        current_section = ""
        test_counters = {lang: 0 for lang in target_languages}
        
        i = 0
        while i < len(lines):
            line = lines[i]
            
            # Track section headings
            if line.startswith('#'):
                current_section = line.lstrip('#').strip()
                # Reset test counters for new section
                test_counters = {lang: 0 for lang in target_languages}
            
            # Look for code block start with test attribute
            for lang in target_languages:
                pattern = rf'^```{re.escape(lang)}\s+test\s*$'
                if re.match(pattern, line):
                    # Increment counter for this language
                    test_counters[lang] += 1
                    
                    # Found a test code block, extract the code
                    code_lines = []
                    i += 1
                    
                    # Collect lines until we find the closing ```
                    while i < len(lines) and not lines[i].startswith('```'):
                        code_lines.append(lines[i])
                        i += 1
                    
                    if code_lines:
                        code = '\n'.join(code_lines)
                        line_number = i - len(code_lines)
                        
                        snippet = CodeSnippet(
                            language=lang,
                            code=code,
                            file_path=file_path,
                            line_number=line_number,
                            section_title=current_section,
                            test_number=test_counters[lang]
                        )
                        snippets.append(snippet)
                    
                    break
            
            i += 1
        
        return snippets
    
    def extract_all_snippets(self, target_languages: List[str]) -> Dict[str, List[CodeSnippet]]:
        """Extract all test snippets from documentation for target languages."""
        self.snippets_by_file = {}
        
        md_files = self.find_documentation_files()
        print(f"Scanning {len(md_files)} documentation files...")
        
        total_snippets = 0
        for file_path in md_files:
            file_snippets = self.extract_code_snippets(file_path, target_languages)
            
            if file_snippets:
                self.snippets_by_file[file_path] = file_snippets
                total_snippets += len(file_snippets)
                print(f"Found {len(file_snippets)} test snippets in {file_path}")
        
        print(f"Total: {total_snippets} test snippets found in {len(self.snippets_by_file)} files")
        return self.snippets_by_file
    
    def save_snippets_json(self, output_file: str):
        """Save extracted snippets to JSON file for debugging."""
        all_snippets = []
        
        for file_path, snippets in self.snippets_by_file.items():
            for snippet in snippets:
                all_snippets.append({
                    'test_name': snippet.test_name,
                    'language': snippet.language,
                    'file_path': snippet.file_path,
                    'line_number': snippet.line_number,
                    'section_title': snippet.section_title,
                    'test_number': snippet.test_number,
                    'code': snippet.code,
                    'clean_code': snippet.get_clean_code()
                })
        
        with open(output_file, 'w') as f:
            json.dump(all_snippets, f, indent=2)
        
        print(f"Snippets saved to {output_file}")


def main():
    """Main function for testing the base extractor."""
    if len(sys.argv) != 3:
        print("Usage: python3 test_snippet_extractor.py <docs_root> <output_json>")
        sys.exit(1)
    
    docs_root = sys.argv[1]
    output_file = sys.argv[2]
    
    # Test with both C and C++
    extractor = TestSnippetExtractor(docs_root)
    snippets_by_file = extractor.extract_all_snippets(['c', 'cpp'])
    extractor.save_snippets_json(output_file)
    
    # Print summary
    c_count = 0
    cpp_count = 0
    for snippets in snippets_by_file.values():
        for snippet in snippets:
            if snippet.language == 'c':
                c_count += 1
            elif snippet.language == 'cpp':
                cpp_count += 1
    
    print(f"\nSummary:")
    print(f"C snippets: {c_count}")
    print(f"C++ snippets: {cpp_count}")
    print(f"Total: {c_count + cpp_count}")


if __name__ == "__main__":
    main()