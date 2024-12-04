use std::{
    fs::File,
    io::{BufRead, BufReader},
};

fn main() {
    let message = read_file("crates/day04/input.txt").unwrap();
    // --- Part One ---
    let mut count = count_word(&message, "XMAS");
    println!("{count}");

    // --- Part Two ---
    count = count_x_pattern(&message, "MAS").unwrap();
    println!("{count}");
}

fn read_file(input: &str) -> Result<Vec<Vec<char>>, Box<dyn std::error::Error>> {
    let file = File::open(input)?;
    let reader = BufReader::new(file);

    let vec_of_vecs: Vec<Vec<char>> = reader
        .lines() // Split into lines
        .map(|line| line.unwrap().chars().collect()) // Convert each line into Vec<char>
        .collect();

    Ok(vec_of_vecs)
}

const fn floored_division(a: i32, b: i32) -> i32 {
    let quotient = a / b;
    if (a % b != 0) && ((a < 0) != (b < 0)) {
        quotient - 1
    } else {
        quotient
    }
}

fn count_x_pattern(grid: &[Vec<char>], word: &str) -> Result<usize, String> {
    let rows = grid.len();
    let cols = if rows > 0 {
        grid[0].len()
    } else {
        return Err("Grid must be a valid grid".to_string());
    };
    let word_len = word.len();
    let mut count = 0;

    let bounded: usize = floored_division(word_len.try_into().unwrap(), 2)
        .try_into()
        .unwrap();
    let middle_char = word.chars().nth(bounded).unwrap();

    let reverse = word.chars().rev().collect::<String>();

    for r in bounded..(rows - bounded) {
        for c in bounded..(cols - bounded) {
            if grid[r][c] == middle_char {
                let mut first_diagonal = false;
                let mut second_diagonal = false;

                // Check forward diagonal \
                if (0..word_len)
                    .all(|i| grid[r - bounded + i][c - bounded + i] == word.chars().nth(i).unwrap())
                {
                    first_diagonal = true;
                }
                // Check backward diagonal \
                if (0..word_len).all(|i| {
                    grid[r - bounded + i][c - bounded + i] == reverse.chars().nth(i).unwrap()
                }) {
                    first_diagonal = true;
                }

                if first_diagonal {
                    // Check forward diagonal /
                    if (0..word_len).all(|i| {
                        grid[r + bounded - i][c - bounded + i] == word.chars().nth(i).unwrap()
                    }) {
                        second_diagonal = true;
                    }
                    // Check backward diagonal /
                    if (0..word_len).all(|i| {
                        grid[r + bounded - i][c - bounded + i] == reverse.chars().nth(i).unwrap()
                    }) {
                        second_diagonal = true;
                    }

                    if second_diagonal {
                        count += 1;
                    }
                }
            }
        }
    }

    Ok(count)
}

fn count_word(grid: &[Vec<char>], word: &str) -> usize {
    let word_chars: Vec<char> = word.chars().collect();
    let word_len = word_chars.len();
    let rows = grid.len();
    let cols = grid[0].len();
    let mut count = 0;

    let directions = vec![
        (0, 1),   // Horizontal right
        (0, -1),  // Horizontal left
        (1, 0),   // Vertical down
        (-1, 0),  // Vertical up
        (1, 1),   // Diagonal down-right
        (1, -1),  // Diagonal down-left
        (-1, 1),  // Diagonal up-right
        (-1, -1), // Diagonal up-left
    ];

    for row in 0..rows {
        for col in 0..cols {
            for (dx, dy) in &directions {
                let mut found = true;

                for (i, _) in word_chars.iter().enumerate().take(word_len) {
                    let x = isize::try_from(row).unwrap() + isize::try_from(i).unwrap() * dx;
                    let y = isize::try_from(col).unwrap() + isize::try_from(i).unwrap() * dy;

                    if x < 0
                        || y < 0
                        || x >= rows.try_into().unwrap()
                        || y >= cols.try_into().unwrap()
                    {
                        found = false;
                        break;
                    }

                    let u_x: usize = usize::try_from(x).unwrap();
                    let u_y: usize = usize::try_from(y).unwrap();

                    if grid[u_x][u_y] != word_chars[i] {
                        found = false;
                        break;
                    }
                }

                if found {
                    count += 1;
                }
            }
        }
    }

    count
}
