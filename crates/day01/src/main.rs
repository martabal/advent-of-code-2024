use std::fs::File;
use std::io::{self, BufRead};

fn main() {
    // --- Part One ---
    let (mut left_column, mut right_column) = read_file("crates/day01/input.txt").unwrap();
    let mut distances: Vec<u32> = Vec::new();

    assert!(left_column.len() == right_column.len());
    #[cfg(feature = "unstable")]
    {
        left_column.sort_unstable();
        right_column.sort_unstable();
    }
    #[cfg(not(feature = "unstable"))]
    {
        #[allow(clippy::stable_sort_primitive)]
        left_column.sort();
        #[allow(clippy::stable_sort_primitive)]
        right_column.sort();
    }

    for (i, _) in left_column.iter().enumerate() {
        distances.push((left_column[i] - right_column[i]).unsigned_abs());
    }
    let mut sum: u32 = distances.iter().copied().sum();
    println!("Part One solution: sum is {sum}");

    // --- Part Two ---
    let mut appear: Vec<u32> = Vec::new();
    for e in &left_column {
        let count: i32 = right_column
            .iter()
            .filter(|&n| *n == *e)
            .count()
            .try_into()
            .unwrap();
        appear.push((*e * count).try_into().unwrap());
    }
    sum = appear.iter().copied().sum();
    println!("Part Two solution: sum is {sum}");
}

fn read_file(input: &str) -> Result<(Vec<i32>, Vec<i32>), Box<dyn std::error::Error>> {
    let mut left_column: Vec<i32> = Vec::new();
    let mut right_column: Vec<i32> = Vec::new();
    let file = File::open(input)?;
    let reader = io::BufReader::new(file);

    for line in reader.lines() {
        let line = line?;
        if let Some((left, right)) = line.split_once(char::is_whitespace) {
            if let (Ok(left_num), Ok(right_num)) =
                (left.trim().parse::<i32>(), right.trim().parse::<i32>())
            {
                left_column.push(left_num);
                right_column.push(right_num);
            }
        }
    }

    Ok((left_column, right_column))
}
