use std::str::FromStr;

#[cfg(not(feature = "regex"))]
use std::{iter::Peekable, str::Chars};

use helpers::read_file;
#[cfg(feature = "regex")]
use regex::Regex;

#[derive(Debug, Eq, PartialEq)]
pub struct Problem {
    program: String,
}

impl FromStr for Problem {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        Ok(Self { program: s.into() })
    }
}

#[cfg(not(feature = "regex"))]
/// Parses and counts the product of numbers within a "mul(a, b)" format.
/// # Errors
/// Returns an error if input parsing fails.
pub fn count_mul(p: &str) -> Result<i32, Box<dyn std::error::Error>> {
    let program: Problem = p.parse()?;
    let mut parser = ProgramParser::new(&program.program);
    Ok(parser.parse())
}

fn main() {
    let message = read_file("crates/day03/input.txt").unwrap();
    // --- Part One ---
    let mut count = count_mul(&message).unwrap();
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
            let _ = self.iterator.next();
            if self.iterator.peek() == Some(&'u') {
                let _ = self.iterator.next();
                if self.iterator.peek() == Some(&'l') {
                    let _ = self.iterator.next();

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

                    let _ = self.iterator.next();

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
    let do_positions = find_all_indexes(message, "do()");
    let dont_positions = find_all_indexes(message, "don't()");

    if do_positions.is_empty() && dont_positions.is_empty() {
        return count_mul(message).unwrap();
    }

    let first_substring = &message[..=do_positions[0]];
    let mut count = 0;
    count += count_mul(first_substring).unwrap();
    let mut next_dont: Option<usize> = None;

    for i in do_positions {
        if next_dont < Some(i) || next_dont.is_none() {
            if let Some(f) = closest_greater_than(i, &dont_positions) {
                let substring = &message[i..f];

                count += count_mul(substring).unwrap();
                next_dont = Some(f);
            }
        }
    }

    count
}

#[cfg(feature = "regex")]
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
