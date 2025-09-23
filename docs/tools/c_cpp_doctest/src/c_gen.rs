use crate::code_generator::generate_test_files;
use crate::language::LanguageTarget;
use poly_doctest::{LangGenerator, Result, SourceFileSnippets};
use std::path::{Path, PathBuf};

#[derive(Default)]
pub struct CGenerator;

impl LangGenerator for CGenerator {
    fn code_fence_languages(&self) -> &[&str] {
        &["c"]
    }
    fn default_output(&self) -> PathBuf {
        PathBuf::from("test/docs/core")
    }

    fn generate(&self, source_files: &[SourceFileSnippets], output_path: &Path) -> Result<()> {
        generate_test_files(source_files, output_path, LanguageTarget::C)
            .map_err(|e| poly_doctest::DocGenError::Processing(e.to_string()))?;
        Ok(())
    }
}
