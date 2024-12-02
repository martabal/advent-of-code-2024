use std::{
    fs::File,
    io::{self, BufRead},
};

fn has_small_difference(vec: &[i32]) -> bool {
    vec.windows(2).all(|pair| {
        let diff = (pair[1] - pair[0]).abs();
        (1..=3).contains(&diff)
    })
}
fn is_safe(e: &[i32]) -> bool {
    let mut sorted = e.to_vec();
    #[cfg(feature = "unstable")]
    {
        sorted.sort_unstable();
    }
    #[cfg(not(feature = "unstable"))]
    {
        #[allow(clippy::stable_sort_primitive)]
        sorted.sort();
    }

    let test = has_small_difference(e);
    (sorted == e || sorted.iter().rev().eq(e.iter())) && test
}
fn main() {
    // --- Part One ---
    let report = read_file("crates/day02/input.txt").unwrap();
    let mut safe: u32 = 0;

    for e in &report {
        if is_safe(e) {
            safe += 1;
        }
    }
    println!("Part One solution: sum is {safe}");

    // --- Part Two ---
    safe = 0;
    for e in &report {
        if is_safe(e) {
            safe += 1;
        } else {
            for (pos, _) in e.iter().enumerate() {
                let mut new = e.clone();
                new.remove(pos);

                if is_safe(&new) {
                    safe += 1;
                    break;
                }
            }
        }
    }
    println!("Part Two solution: sum is {safe}");
}

fn read_file(input: &str) -> Result<Vec<Vec<i32>>, Box<dyn std::error::Error>> {
    let mut report: Vec<Vec<i32>> = Vec::new();
    let file = File::open(input)?;
    let reader = io::BufReader::new(file);

    for line in reader.lines() {
        let line = line?;
        let row: Vec<i32> = line
            .split_whitespace()
            .filter_map(|num| num.parse::<i32>().ok())
            .collect();
        report.push(row);
    }

    Ok(report)
}
