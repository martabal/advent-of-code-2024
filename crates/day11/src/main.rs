use std::collections::HashMap;

use helpers::read_file;

const BLINK_NUMBER_PART_1: u8 = 25;
const BLINK_NUMBER_PART_2: u8 = 75;

fn main() {
    let message = read_file("crates/day11/input.txt").unwrap();

    // // --- Part One ---
    let stones = Stones::new(&message);

    println!("Part One solution: {}", stones.blink(BLINK_NUMBER_PART_1));

    // // --- Part Two ---
    println!("Part Two solution: {}", stones.blink(BLINK_NUMBER_PART_2));
}

struct Stones {
    stones: HashMap<u64, usize>,
}

impl Stones {
    fn new(content: &str) -> Self {
        let mut stones = HashMap::new();
        content.split_whitespace().for_each(|stone| {
            let stone = stone.parse::<u64>().unwrap();

            *stones.entry(stone).or_default() += 1;
        });

        Self { stones }
    }

    fn rules(stones: &HashMap<u64, usize>) -> HashMap<u64, usize> {
        let mut new_stones: HashMap<u64, usize> = HashMap::new();

        for (stone, amount) in stones {
            let stone_str = stone.to_string();

            if *stone == 0 {
                *new_stones.entry(1).or_default() += amount;
            } else if stone_str.len() % 2 == 0 {
                *new_stones
                    .entry(stone_str[..stone_str.len() / 2].parse().unwrap())
                    .or_default() += amount;
                *new_stones
                    .entry(stone_str[stone_str.len() / 2..].parse().unwrap())
                    .or_default() += amount;
            } else {
                *new_stones.entry(*stone * 2024).or_default() += amount;
            }
        }

        new_stones
    }

    fn blink(&self, blink_number: u8) -> usize {
        let mut new_stones = self.stones.clone();

        for _ in 0..blink_number {
            new_stones = Self::rules(&new_stones);
        }

        new_stones.values().sum()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let checker = Stones::new(&message);
        let response_part_1 = checker.blink(BLINK_NUMBER_PART_1);
        let response_part_2 = checker.blink(BLINK_NUMBER_PART_2);

        assert!(response_part_1 == 55312);
        assert!(response_part_2 == 65601038650482);
    }

    #[test]
    fn check_result() {
        let message = read_file("input.txt").unwrap();

        let checker = Stones::new(&message);
        let response_part_1 = checker.blink(BLINK_NUMBER_PART_1);
        let response_part_2 = checker.blink(BLINK_NUMBER_PART_2);

        assert!(response_part_1 == 183248);
        assert!(response_part_2 == 218811774248729);
    }
}
