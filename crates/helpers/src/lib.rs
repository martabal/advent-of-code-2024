use std::{
    fs::File,
    io::{BufReader, Read},
};

/// # Errors
/// Returns an error if reading file fails.
pub fn read_file(file_path: &str) -> Result<String, Box<dyn std::error::Error>> {
    let file = File::open(file_path)?;
    let mut reader = BufReader::new(file);
    let mut content = String::new();
    reader.read_to_string(&mut content)?;
    Ok(content)
}
