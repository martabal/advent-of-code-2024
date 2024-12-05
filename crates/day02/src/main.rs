use std::{
    fs::File,
    io::{BufRead, BufReader},
};

fn has_small_difference(vec: &[i32]) -> bool {
    vec.windows(2).all(|pair| {
        let diff = (pair[1] - pair[0]).abs();
        (1..=3).contains(&diff)
    })
}
fn is_safe(e: &[i32]) -> bool {
    let test = has_small_difference(e);
    (e.windows(2).all(|w| w[0] >= w[1]) || e.windows(2).all(|w| w[0] <= w[1])) && test
}
fn main() {
    // --- Part One ---
    let reports = read_file("crates/day02/input.txt").unwrap();
    let mut count: u32 = find_reports(&reports);
    println!("Part One solution: sum is {count}");

    // --- Part Two ---
    count = find_safe_report(&reports);
    println!("Part Two solution: sum is {count}");
}

fn find_reports(reports: &Vec<Vec<i32>>) -> u32 {
    let mut count: u32 = 0;
    for e in reports {
        if is_safe(e) {
            count += 1;
        }
    }

    count
}

fn find_safe_report(reports: &Vec<Vec<i32>>) -> u32 {
    let mut count = 0;

    // not really optimal, ideally the first pass give hints on what element to remove
    for e in reports {
        if is_safe(e) {
            count += 1;
        } else {
            for (pos, _) in e.iter().enumerate() {
                let mut new = e.clone();
                new.remove(pos);

                if is_safe(&new) {
                    count += 1;
                    break;
                }
            }
        }
    }

    count
}

fn read_file(input: &str) -> Result<Vec<Vec<i32>>, Box<dyn std::error::Error>> {
    let mut report: Vec<Vec<i32>> = Vec::new();
    let file = File::open(input)?;
    let reader = BufReader::new(file);

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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let response_part_1 = find_reports(&message);
        let response_part_2 = find_safe_report(&message);

        assert!(response_part_1 == 2);
        assert!(response_part_2 == 4);
    }
}
