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

#[must_use]
pub const fn floored_division(a: i32, b: i32) -> i32 {
    let quotient = a / b;
    if (a % b != 0) && ((a < 0) != (b < 0)) {
        quotient - 1
    } else {
        quotient
    }
}
