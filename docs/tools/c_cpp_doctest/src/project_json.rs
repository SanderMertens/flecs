use anyhow::Result;
use poly_doctest::CodeSnippet;
use serde_json::{json, Value};
use std::fs;
use std::path::Path;

pub fn update_testsuites_batch(
    project_file: &Path,
    testsuites_data: &[(String, &[CodeSnippet])],
) -> Result<()> {
    if !project_file.exists() {
        return Err(anyhow::anyhow!(
            "Required project.json not found at {}",
            project_file.display()
        ));
    }

    let content = fs::read_to_string(project_file).map_err(|e| {
        anyhow::anyhow!(
            "Failed to read project.json at {}: {}",
            project_file.display(),
            e
        )
    })?;

    let mut data: Value = serde_json::from_str(&content).map_err(|e| {
        anyhow::anyhow!(
            "Invalid JSON in project.json at {}: {}",
            project_file.display(),
            e
        )
    })?;

    let data_obj = data
        .as_object_mut()
        .ok_or_else(|| anyhow::anyhow!("project.json root is not an object"))?;

    let testsuites: Vec<Value> = testsuites_data
        .iter()
        .map(|(suite_id, snippets)| {
            json!({
                "id": suite_id,
                "testcases": snippets.iter().map(|s| &s.name).collect::<Vec<_>>()
            })
        })
        .collect();

    // Replace entire test section with new testsuites
    data_obj.insert(
        "test".to_string(),
        json!({
            "testsuites": testsuites
        }),
    );

    let formatted = serde_json::to_string_pretty(&data)?.replace("  ", "    ");
    fs::write(project_file, formatted)?;
    Ok(())
}
