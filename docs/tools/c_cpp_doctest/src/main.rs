mod c_gen;
mod code_generator;
mod cpp_gen;
mod error;
mod language;
mod project_json;

use c_gen::CGenerator;
use clap::Parser;
use cpp_gen::CppGenerator;
use poly_doctest::{run_with_args, CliArgs};

fn main() -> anyhow::Result<()> {
    let args: CliArgs = CliArgs::parse();

    // Get the output path - use provided or default
    let output_base = args
        .output
        .clone()
        .unwrap_or_else(|| std::path::PathBuf::from("tmp/doctest/c_cpp"));

    // Create args for C generator
    let c_args = CliArgs {
        local: args.local.clone(),
        remote: args.remote.clone(),
        recursive: args.recursive,
        output: Some(output_base.join("c")),
        hide_prefix: args.hide_prefix.clone(),
    };

    // Create args for C++ generator
    let mut cpp_args = c_args.clone();
    cpp_args.output = Some(output_base.join("cpp"));

    // Generate C tests
    let c_generator = CGenerator;
    println!("\nGenerating C tests...");
    run_with_args(c_generator, c_args)?;
    println!("C test generation complete!\n");

    // Generate C++ tests
    let cpp_generator = CppGenerator;
    println!("Generating C++ tests...");
    run_with_args(cpp_generator, cpp_args)?;
    println!("C++ test generation complete!\n");

    Ok(())
}
