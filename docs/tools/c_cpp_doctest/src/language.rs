/// Language target configuration for C/C++ code generation
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum LanguageTarget {
    C,
    Cpp,
}

impl LanguageTarget {
    /// File extension for the target language
    pub fn file_extension(self) -> &'static str {
        match self {
            Self::C => ".c",
            Self::Cpp => ".cpp",
        }
    }

    /// Header include for the target language
    pub fn header_include(self) -> &'static str {
        match self {
            Self::C => "core.h",
            Self::Cpp => "cpp.h",
        }
    }
}
