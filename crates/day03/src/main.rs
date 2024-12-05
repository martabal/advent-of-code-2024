#[cfg(not(feature = "regex"))]
use std::{iter::Peekable, str::Chars};

use helpers::read_file;
#[cfg(feature = "regex")]
use regex::Regex;

#[cfg(not(feature = "regex"))]
#[must_use]
/// Parses and counts the product of numbers within a "mul(a, b)" format.
/// # Errors
/// Returns an error if input parsing fails.
pub fn count_mul(p: &str) -> i32 {
    let mut parser = ProgramParser::new(p);
    parser.parse()
}

fn main() {
    let message = read_file("crates/day03/input.txt").unwrap();
    // --- Part One ---
    let mut count = count_mul(&message);
    println!("Part One solution: sum is {count}");

    // --- Part Two ---
    count = parse_with_rules(&message);
    println!("Part Two solution: sum is {count}");
}

#[cfg(not(feature = "regex"))]
struct ProgramParser<'a> {
    iterator: Peekable<Chars<'a>>,
}

#[cfg(not(feature = "regex"))]
impl<'a> ProgramParser<'a> {
    fn new(program: &'a str) -> Self {
        ProgramParser {
            iterator: program.chars().peekable(),
        }
    }

    fn parse(&mut self) -> i32 {
        let mut result = 0;

        while self.iterator.peek().is_some() {
            if !self.parse_mul() {
                self.iterator.next();
                continue;
            }
            if self.iterator.next_if_eq(&'(').is_none() {
                continue;
            }
            let Some(a) = self.parse_number() else {
                continue;
            };

            if self.iterator.next_if_eq(&',').is_none() {
                continue;
            }

            let Some(b) = self.parse_number() else {
                continue;
            };

            if self.iterator.next_if_eq(&')').is_some() {
                result += a * b;
            }
        }

        result.try_into().unwrap()
    }

    fn parse_mul(&mut self) -> bool {
        if self.iterator.peek() == Some(&'m') {
            self.iterator.next();
            if self.iterator.peek() == Some(&'u') {
                self.iterator.next();
                if self.iterator.peek() == Some(&'l') {
                    self.iterator.next();

                    return true;
                }
            }
        }

        false
    }

    fn parse_number(&mut self) -> Option<u64> {
        let mut digits = String::new();

        for _ in 0..3 {
            if let Some(c) = self.iterator.peek() {
                if c.is_ascii_digit() {
                    digits.push(*c);

                    self.iterator.next();

                    continue;
                }
            }

            break;
        }

        if !digits.is_empty() {
            return digits.parse::<u64>().ok();
        }

        None
    }
}

fn find_all_indexes(haystack: &str, needle: &str) -> Vec<usize> {
    let needle_len = needle.len();
    let mut indexes = Vec::new();

    for (i, window) in haystack.as_bytes().windows(needle_len).enumerate() {
        if window == needle.as_bytes() {
            indexes.push(i);
        }
    }

    indexes
}

fn closest_greater_than(x: usize, y: &[usize]) -> Option<usize> {
    y.iter().filter(|&&value| value > x).min().copied()
}

fn parse_with_rules(message: &str) -> i32 {
    let mut do_positions = find_all_indexes(message, "do()");
    let dont_positions = find_all_indexes(message, "don't()");

    if do_positions.is_empty() && dont_positions.is_empty() {
        return count_mul(message);
    }

    let first_substring = &message[..=dont_positions[0]];
    let mut count = 0;
    count += count_mul(first_substring);
    let mut next_dont: Option<usize> = None;
    do_positions.retain(|&x| x >= dont_positions[0]);

    for i in &do_positions {
        if next_dont < Some(*i) || next_dont.is_none() {
            if let Some(f) = closest_greater_than(*i, &dont_positions) {
                let substring = &message[*i..f];
                count += count_mul(substring);
                next_dont = Some(f);
            }
        }
    }

    if let (Some(&do_last), Some(&dont_last)) = (do_positions.last(), dont_positions.last()) {
        if do_last > dont_last {
            let last_substring = &message[do_last..];
            println!("{last_substring}");
            count += count_mul(last_substring);
        }
    }

    count
}

#[cfg(feature = "regex")]
fn count_mul(message: &str) -> i32 {
    let re = Regex::new(r"mul\(\s*(\d+)\s*,\s*(\d+)\s*\)").unwrap();

    let result: i32 = re
        .captures_iter(message)
        .map(|cap| {
            let x = cap[1].parse::<i32>().unwrap();
            let y = cap[2].parse::<i32>().unwrap();
            x * y
        })
        .sum();

    result
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message_part_1 = read_file("example_input_part_1.txt").unwrap();
        let message_part_2 = read_file("example_input_part_2.txt").unwrap();

        let response_part_1 = count_mul(&message_part_1);
        let response_part_2 = parse_with_rules(&message_part_2);

        assert!(response_part_1 == 161);
        assert!(response_part_2 == 48);
    }
}
