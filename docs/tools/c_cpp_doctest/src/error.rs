/// Errors that can occur during code generation
#[derive(Debug, thiserror::Error)]
pub enum CodeGenError {
    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),

    #[error("JSON error: {0}")]
    Json(#[from] serde_json::Error),

    #[error("Poly doctest error: {0}")]
    PolyDoctest(#[from] poly_doctest::DocGenError),
}

pub type Result<T> = std::result::Result<T, CodeGenError>;
