use std::collections::HashSet;

use helpers::read_file;

struct Lab {
    pos_init: (usize, usize),
    grid: Vec<Vec<char>>,
    right_bottom_position: (usize, usize),
}

// bool = is out of the grid
// Option<(usize, usize)> = next position
// Option<(usize, usize)> = position of the blocking object
type FindPath = (bool, Option<(usize, usize)>, Option<(usize, usize)>);

const OBJECT: char = '#';

#[derive(Debug)]
enum Direction {
    Up,
    Down,
    Left,
    Right,
}

fn main() {
    let message = read_file("crates/day06/input.txt").unwrap();
    // --- Part One ---
    let mut grid = Lab::new(&message).unwrap();
    println!("Part One solution: {:?}", grid.find_path());
    // --- Part Two ---
    println!("Part Two solution: {:?}", grid.find_if_stuck());
}

impl Lab {
    fn new(content: &str) -> Result<Self, String> {
        let grid: Vec<Vec<char>> = content
            .lines() // Split into lines
            .map(|line| line.replace('^', ".").chars().collect()) // Convert each line into Vec<char>
            .collect();

        let pos_init = content
            .lines()
            .enumerate()
            .find_map(|(row_idx, row)| row.find('^').map(|col_idx| (row_idx, col_idx)))
            .ok_or_else(|| "initial position not detected".to_string())?;

        assert!(!grid.is_empty());
        let right_bottom_position = (grid.len() - 1, grid[0].len() - 1);
        Ok(Self {
            pos_init,
            grid,
            right_bottom_position,
        })
    }

    fn find_path(&self) -> usize {
        let mut direction = Direction::Up;
        let mut current_position = self.pos_init;
        let mut already_explored_pos: HashSet<(usize, usize)> = HashSet::new();
        already_explored_pos.insert(self.pos_init);
        if self.pos_init.1 == 0
            || (self.pos_init.1 > 0 && self.grid[self.pos_init.1][self.pos_init.0] == OBJECT)
        {
            direction = Self::change_direction(&direction);
        }

        let mut second_try = false;

        loop {
            let (is_out, pos, _) = self.change_position(&direction, current_position);
            {
                if is_out {
                    break;
                }
                if let Some(new_pos) = pos {
                    current_position = new_pos;
                    already_explored_pos.insert(current_position);
                    if second_try {
                        second_try = false;
                    }
                } else {
                    let new_direction = Self::change_direction(&direction);
                    if second_try {
                        break;
                    }
                    second_try = true;

                    direction = new_direction;
                }
            }
        }

        already_explored_pos.len()
    }

    fn find_if_stuck(&mut self) -> usize {
        let grid = self.grid.clone();
        let mut count = 0;

        for (row, cols) in grid.iter().enumerate() {
            for (col, char) in cols.iter().enumerate() {
                if OBJECT != *char {
                    self.grid[row][col] = OBJECT;

                    let mut direction = Direction::Up;
                    let mut current_position = self.pos_init;
                    let mut detect_loop: HashSet<((usize, usize), (usize, usize))> = HashSet::new();

                    if self.pos_init.1 == 0
                        || (self.pos_init.1 > 0
                            && self.grid[self.pos_init.1][self.pos_init.0] == OBJECT)
                    {
                        direction = Self::change_direction(&direction);
                    }

                    let mut second_try = false;

                    loop {
                        let (is_out, pos, block) =
                            self.change_position(&direction, current_position);
                        {
                            if is_out {
                                break;
                            }
                            if let Some(new_pos) = pos {
                                current_position = new_pos;

                                if second_try {
                                    second_try = false;
                                }
                            } else if let Some(new_block) = block {
                                let blocking = (current_position, new_block);
                                if detect_loop.contains(&blocking) {
                                    count += 1;
                                    break;
                                }
                                let new_direction = Self::change_direction(&direction);
                                detect_loop.insert(blocking);
                                second_try = true;
                                direction = new_direction;
                            }
                        }
                    }

                    self.grid[row][col] = grid[row][col];
                }
            }
        }

        count
    }

    fn change_position(&self, direction: &Direction, position: (usize, usize)) -> FindPath {
        let (y, x) = position;

        match direction {
            Direction::Up => {
                if i32::try_from(y).unwrap() - 1 < 0 {
                    return (true, None, None);
                }
                if self.grid[y - 1][x] != OBJECT {
                    return (false, Some((y - 1, x)), None);
                }
                (false, None, Some((y - 1, x)))
            }
            Direction::Left => {
                if i32::try_from(x).unwrap() - 1 < 0 {
                    return (true, None, None);
                }
                if self.grid[y][x - 1] != OBJECT {
                    return (false, Some((y, x - 1)), None);
                }
                (false, None, Some((y, x - 1)))
            }
            Direction::Down => {
                if i32::try_from(y).unwrap() + 1 > self.right_bottom_position.0.try_into().unwrap()
                {
                    return (true, None, None);
                }
                if self.grid[y + 1][x] != OBJECT {
                    return (false, Some((y + 1, x)), None);
                }
                (false, None, Some((y + 1, x)))
            }
            Direction::Right => {
                if i32::try_from(x).unwrap() + 1 > self.right_bottom_position.1.try_into().unwrap()
                {
                    return (true, None, None);
                }
                if self.grid[y][x + 1] != OBJECT {
                    return (false, Some((y, x + 1)), None);
                }
                (false, None, Some((y, x + 1)))
            }
        }
    }

    const fn change_direction(direction: &Direction) -> Direction {
        match direction {
            Direction::Up => Direction::Right,
            Direction::Right => Direction::Down,
            Direction::Down => Direction::Left,
            Direction::Left => Direction::Up,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let mut checker = Lab::new(&message).unwrap();
        let response_part_1 = checker.find_path();
        let response_part_2 = checker.find_if_stuck();

        assert!(response_part_1 == 41);
        assert!(response_part_2 == 6);
    }

    #[test]
    fn check_result() {
        let message = read_file("input.txt").unwrap();

        let mut checker = Lab::new(&message).unwrap();
        let response_part_1 = checker.find_path();
        let response_part_2 = checker.find_if_stuck();

        assert!(response_part_1 == 4374);
        assert!(response_part_2 == 1705);
    }
}
