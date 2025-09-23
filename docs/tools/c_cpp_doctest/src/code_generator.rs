use poly_doctest::{extract_file_stem, indent_code, SourceFileSnippets};
use std::fs;
use std::path::Path;

use crate::error::{CodeGenError, Result};
use crate::language::LanguageTarget;
use crate::project_json::update_testsuites_batch;

/// Generate C/C++ test files from source snippets
pub fn generate_test_files(
    source_files: &[SourceFileSnippets],
    output_path: &Path,
    target: LanguageTarget,
) -> Result<()> {
    let src_dir = output_path.join("src");
    fs::create_dir_all(&src_dir)?;

    let mut testsuites = Vec::new();

    for source_file in source_files {
        if source_file.snippets.is_empty() {
            continue;
        }

        let file_stem = extract_file_stem(&source_file.source_file)?;

        let content = generate_file_content(&file_stem, source_file, target)?;

        let filename = format!("{}{}", file_stem, target.file_extension());
        fs::write(src_dir.join(filename), content)?;

        testsuites.push((file_stem, source_file.snippets.as_slice()));
    }

    update_project_json_batch(output_path, &testsuites)?;
    Ok(())
}

/// Generate the content for a single test file
fn generate_file_content(
    file_stem: &str,
    source_file: &SourceFileSnippets,
    target: LanguageTarget,
) -> Result<String> {
    let mut content = format!(
        "/*\n * Tests generated from: {}\n * Contains {} test(s) from documentation code snippets\n */\n\n#include <{}>\n\n",
        source_file.source_file,
        source_file.snippets.len(),
        target.header_include()
    );

    // Generate function implementations
    content.push_str(&generate_function_implementations(
        file_stem,
        &source_file.snippets,
    )?);

    Ok(content)
}

/// Generate function implementations for both C and C++
fn generate_function_implementations(
    file_stem: &str,
    snippets: &[poly_doctest::CodeSnippet],
) -> Result<String> {
    let mut implementations = Vec::new();

    for snippet in snippets {
        let processed_code = process_snippet_code(&snippet.code);
        let indented = indent_code(&processed_code, 4);

        let function = format!(
            "void {}_{}(void) {{\n{}\n}}",
            file_stem, snippet.name, indented
        );

        implementations.push(function);
    }

    Ok(implementations.join("\n\n"))
}

/// Process snippet code (replace assert calls, add fallback test_assert)
fn process_snippet_code(code: &str) -> String {
    let code = code.replace("assert(", "test_assert(");

    if code.contains("test_") {
        code
    } else {
        format!("{}{}", code.trim_end(), "\n\ntest_assert(true);")
    }
}

fn update_project_json_batch(
    output_path: &Path,
    testsuites: &[(String, &[poly_doctest::CodeSnippet])],
) -> Result<()> {
    let project_file = output_path.join("project.json");

    update_testsuites_batch(&project_file, testsuites).map_err(|e| {
        CodeGenError::PolyDoctest(poly_doctest::DocGenError::Processing(e.to_string()))
    })
}
