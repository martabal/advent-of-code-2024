use std::collections::HashMap;

use helpers::{read_file, GridSize};

struct Antenna {
    frequencies: HashMap<char, Vec<(i32, i32)>>,
    grid: GridSize,
}

impl Antenna {
    fn new(content: &str) -> Self {
        let mut frequencies: HashMap<char, Vec<(i32, i32)>> = HashMap::new();
        let rows = i32::try_from(content.lines().count()).unwrap() - 1;
        let cols = i32::try_from(content.lines().next().map_or(0, str::len)).unwrap() - 1;
        for (row_idx, line) in content.lines().enumerate() {
            for (col_idx, ch) in line.chars().enumerate() {
                if ch.is_ascii_alphanumeric() {
                    frequencies
                        .entry(ch)
                        .or_default()
                        .push((row_idx.try_into().unwrap(), col_idx.try_into().unwrap()));
                }
            }
        }
        Self {
            frequencies,
            grid: GridSize { rows, cols },
        }
    }

    const fn new_antenna(
        current_antenna: (i32, i32),
        other_antenna: (i32, i32),
        distance: (i32, i32),
    ) -> ((i32, i32), (i32, i32)) {
        let y = if current_antenna.0 < other_antenna.0 {
            current_antenna.0 - distance.0
        } else {
            current_antenna.0 + distance.0
        };
        let x = if current_antenna.1 < other_antenna.1 {
            current_antenna.1 - distance.1
        } else {
            current_antenna.1 + distance.1
        };

        let dy = y - current_antenna.0;
        let dx = x - current_antenna.1;

        ((y, x), (dy, dx))
    }

    fn can_add_antenna(
        &self,
        antenna: (i32, i32),
        antenna_positions: &[(i32, i32)],
    ) -> (bool, bool) {
        if antenna.0 >= 0
            && antenna.1 >= 0
            && antenna.1 <= self.grid.cols
            && antenna.0 <= self.grid.rows
        {
            if antenna_positions.contains(&antenna) {
                return (false, false);
            }
            return (false, true);
        }

        (true, false)
    }

    fn count_antennas(&self, harmony_frequencies: bool) -> usize {
        let mut antenna_positions = Vec::new();

        for positions in self.frequencies.values() {
            for (i, &pos_i) in positions.iter().enumerate() {
                for &pos_j in &positions[i + 1..] {
                    let distance = ((pos_i.0 - pos_j.0).abs(), (pos_i.1 - pos_j.1).abs());

                    if harmony_frequencies {
                        for pos in &[pos_i, pos_j] {
                            let (is_out, can_add) = self.can_add_antenna(*pos, &antenna_positions);
                            if is_out {
                                break;
                            }
                            if can_add {
                                antenna_positions.push(*pos);
                            }
                        }
                    }

                    let (mut first_antenna, distance_1) = Self::new_antenna(pos_i, pos_j, distance);
                    let (mut second_antenna, distance_2) =
                        Self::new_antenna(pos_j, pos_i, distance);

                    while let (false, can_add) =
                        self.can_add_antenna(first_antenna, &antenna_positions)
                    {
                        if can_add {
                            antenna_positions.push(first_antenna);
                        }
                        if !harmony_frequencies {
                            break;
                        }
                        first_antenna.0 += distance_1.0;
                        first_antenna.1 += distance_1.1;
                    }

                    while let (false, can_add) =
                        self.can_add_antenna(second_antenna, &antenna_positions)
                    {
                        if can_add {
                            antenna_positions.push(second_antenna);
                        }
                        if !harmony_frequencies {
                            break;
                        }
                        second_antenna.0 += distance_2.0;
                        second_antenna.1 += distance_2.1;
                    }
                }
            }
        }

        antenna_positions.len()
    }
}

fn main() {
    let message = read_file("crates/day08/input.txt").unwrap();

    // --- Part One ---
    let checker = Antenna::new(&message);
    println!("Part One solution: {:?}", checker.count_antennas(false));
    // --- Part Two ---
    println!("Part Two solution: {:?}", checker.count_antennas(true));
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let checker = Antenna::new(&message);
        let response_part_1 = checker.count_antennas(false);
        let response_part_2 = checker.count_antennas(true);

        assert!(response_part_1 == 14);
        assert!(response_part_2 == 34);
    }

    #[test]
    fn check_result() {
        let message = read_file("input.txt").unwrap();

        let checker = Antenna::new(&message);
        let response_part_1 = checker.count_antennas(false);
        let response_part_2 = checker.count_antennas(true);

        assert!(response_part_1 == 323);
        assert!(response_part_2 == 1077);
    }
}
