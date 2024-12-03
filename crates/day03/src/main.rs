use std::{
    fs::File,
    io::{BufReader, Read},
    str::FromStr,
};

#[cfg(not(feature = "regex"))]
use std::{iter::Peekable, str::Chars};

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
/// # Errors
///
/// Will return `Err` if there's an error in the parsing/regex
pub fn count_mul(p: &str) -> Result<i32, Box<dyn std::error::Error>> {
    let Problem { program } = p.parse()?;
    let mut parser = ProgramParser::new(&program);

    Ok(parser.parse())
}

fn main() {
    // --- Part One ---
    let message = read_file("crates/day03/input.txt").unwrap();
    let mut count = count_mul(&message).unwrap();
    println!("Part One solution: sum is {count}");

    // --- Part Two ---
    count = parse_with_rules(&message);
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
            if self.parse_mul() {
                if self.iterator.peek() == Some(&'(') {
                    let _ = self.iterator.next();

                    if let Some(a) = self.parse_number() {
                        if self.iterator.peek() == Some(&',') {
                            let _ = self.iterator.next();

                            if let Some(b) = self.parse_number() {
                                if self.iterator.peek() == Some(&')') {
                                    let _ = self.iterator.next();

                                    result += a * b;
                                }
                            }
                        }
                    }
                }
            } else {
                let _ = self.iterator.next();
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

fn closest_greater_than(x: usize, y: &[usize]) -> Option<usize> {
    let mut closest: Option<usize> = None;

    for &value in y {
        if value > x && (closest.is_none() || value < closest.unwrap()) {
            closest = Some(value);
        }
    }

    closest
}

fn parse_with_rules(message: &str) -> i32 {
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

fn read_file(file_path: &str) -> Result<String, Box<dyn std::error::Error>> {
    let file = File::open(file_path)?;
    let mut reader = BufReader::new(file);
    let mut content = String::new();
    reader.read_to_string(&mut content)?;
    Ok(content)
}
