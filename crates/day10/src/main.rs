use std::collections::{HashMap, HashSet};

use helpers::read_file;

const DIRECTIONS: [(isize, isize); 4] = [
    (0, 1),  // Right
    (1, 0),  // Down
    (0, -1), // Left
    (-1, 0), // Up
];

struct TrailHeads {
    trailheads: Vec<(isize, isize)>,
    grid: HashMap<(isize, isize), u32>,
}

impl TrailHeads {
    fn new(content: &str) -> Self {
        let mut trailheads = Vec::new();
        let mut grid = HashMap::new();

        for (row, line) in content.lines().enumerate() {
            for (col, ch) in line.chars().enumerate() {
                if let Some(height) = ch.to_digit(10) {
                    let coords = (isize::try_from(row).unwrap(), isize::try_from(col).unwrap());
                    if height == 0 {
                        trailheads.push(coords);
                    }
                    grid.insert(coords, height);
                }
            }
        }

        Self { trailheads, grid }
    }

    fn count_trailheads(&self) -> usize {
        self.trailheads
            .clone()
            .into_iter()
            .map(|head| self.count_trail_unique(head, 0).len())
            .sum()
    }

    fn count_distinct_trailheads(&self) -> usize {
        self.trailheads
            .clone()
            .into_iter()
            .map(|head| self.count_trail_all(head, 0))
            .sum()
    }
    fn count_trail_unique(&self, position: (isize, isize), height: u32) -> HashSet<(isize, isize)> {
        let mut res = HashSet::new();

        for &(row_dir, col_dir) in &DIRECTIONS {
            let new_dir = (position.0 + row_dir, position.1 + col_dir);
            if let Some(&new_pos) = self.grid.get(&new_dir) {
                if height + 1 != new_pos {
                    continue;
                }

                if new_pos == 9 {
                    res.insert(new_dir);
                } else {
                    res.extend(&self.count_trail_unique(new_dir, new_pos));
                }
            }
        }
        res
    }

    fn count_trail_all(&self, position: (isize, isize), height: u32) -> usize {
        let mut res = 0;
        for &(row_dir, col_dir) in &DIRECTIONS {
            let new_dir = (position.0 + row_dir, position.1 + col_dir);
            if let Some(&new_pos) = self.grid.get(&new_dir) {
                if height + 1 != new_pos {
                    continue;
                }

                if new_pos == 9 {
                    res += 1;
                } else {
                    res += &self.count_trail_all(new_dir, new_pos);
                }
            }
        }
        res
    }
}

fn main() {
    let message = read_file("crates/day10/input.txt").unwrap();
    let checker = TrailHeads::new(&message);

    // --- Part One ---
    println!("Part One solution: {}", checker.count_trailheads());

    // --- Part Two ---
    println!("Part Two solution: {}", checker.count_distinct_trailheads());
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let checker = TrailHeads::new(&message);
        let response_part_1 = checker.count_trailheads();
        let response_part_2 = checker.count_distinct_trailheads();

        assert!(response_part_1 == 36);
        assert!(response_part_2 == 81);
    }

    #[test]
    fn check_result() {
        let message = read_file("input.txt").unwrap();

        let checker = TrailHeads::new(&message);
        let response_part_1 = checker.count_trailheads();
        let response_part_2 = checker.count_distinct_trailheads();

        assert!(response_part_1 == 688);
        assert!(response_part_2 == 1459);
    }
}
