use std::fs::read_to_string;

use regex::Regex;

fn main() {
    // --- Part One ---
    let message = read_file("crates/day03/input.txt").unwrap();
    let mut count = count_mul(&message).unwrap();
    println!("Part One solution: sum is {count}");
    // --- Part Two ---
    count = parse_and_calculate(&message);
    println!("Part Two solution: sum is {count}");
}

fn find_all_indexes(haystack: &str, needle: &str) -> Vec<usize> {
    let mut indexes = Vec::new();
    let mut start = 0;

    while let Some(pos) = haystack[start..].find(needle) {
        indexes.push(start + pos);
        start += pos + 1;
    }

    indexes
}

fn closest_greater_than(x: usize, y: &[usize]) -> Option<usize> {
    let mut closest: Option<usize> = None;

    for &value in y {
        if value > x && (closest.is_none() || value < closest.unwrap()) {
            closest = Some(value);
        }
    }

    closest
}

fn parse_and_calculate(message: &str) -> i32 {
    let do_substring = "do()";
    let do_position = find_all_indexes(message, do_substring);

    let dont_substring = "don't()";
    let dont_position = find_all_indexes(message, dont_substring);

    if do_position.is_empty() && dont_position.is_empty() {
        return count_mul(message).unwrap();
    }

    let first_substring = &message[..=do_position[0]];
    let mut count = 0;
    count += count_mul(first_substring).unwrap();
    let mut next_dont: Option<usize> = None;

    for i in do_position {
        if next_dont < Some(i) || next_dont.is_none() {
            if let Some(f) = closest_greater_than(i, &dont_position) {
                let substring = &message[i..f];

                count += count_mul(substring).unwrap();
                next_dont = Some(f);
            }
        }
    }

    count
}

fn count_mul(message: &str) -> Result<i32, Box<dyn std::error::Error>> {
    let re = Regex::new(r"mul\(\s*(\d+)\s*,\s*(\d+)\s*\)")?;

    let result: i32 = re
        .captures_iter(message)
        .map(|cap| {
            let x = cap[1].parse::<i32>().unwrap();
            let y = cap[2].parse::<i32>().unwrap();
            x * y
        })
        .sum();

    Ok(result)
}

fn read_file(input: &str) -> Result<String, Box<dyn std::error::Error>> {
    let message: String = read_to_string(input)?;
    Ok(message)
}
