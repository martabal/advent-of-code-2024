use helpers::read_file;

struct Calibration {
    equations: Vec<(u64, Vec<u64>)>,
}

impl Calibration {
    const fn add(a: u64, b: u64) -> u64 {
        a + b
    }

    const fn multiply(a: u64, b: u64) -> u64 {
        a * b
    }

    fn concatenate(a: u64, b: u64) -> u64 {
        let concatenated = format!("{a}{b}");
        concatenated.parse().unwrap()
    }
    fn new(content: &str) -> Self {
        let equations = content
            .lines()
            .filter_map(|line| {
                // Split the line into key and values
                let mut parts = line.split(':');
                let key = parts.next()?.trim().parse::<u64>().ok()?;
                let values = parts
                    .next()?
                    .split_whitespace()
                    .filter_map(|v| v.parse::<u64>().ok())
                    .collect::<Vec<u64>>();
                Some((key, values))
            })
            .collect();

        Self { equations }
    }

    fn compute_calibration(&self, operators: &[fn(u64, u64) -> u64]) -> u64 {
        let mut count = 0;

        for (result, equation) in &self.equations {
            let results = Self::compute_combinations(equation, operators);
            if results.contains(result) {
                count += result;
            }
        }

        count
    }

    fn compute_combinations(values: &[u64], operators: &[fn(u64, u64) -> u64]) -> Vec<u64> {
        let num_values = values.len();
        let num_operators = num_values - 1;
        let total_combinations = operators.len().pow(u32::try_from(num_operators).unwrap());

        let mut results = Vec::new();

        for combination in 0..total_combinations {
            let mut result = values[0];
            let mut current_combination = combination;

            for i in 0..num_operators {
                let operator_index = current_combination % operators.len(); // Extract operator index
                current_combination /= operators.len(); // Move to the next operator

                // Apply the operator
                result = operators[operator_index](result, values[i + 1]);
            }

            results.push(result);
        }

        results
    }
}

fn main() {
    let message = read_file("crates/day07/input.txt").unwrap();

    // --- Part One ---
    let checker = Calibration::new(&message);
    let mut operators: Vec<fn(u64, u64) -> u64> = vec![Calibration::add, Calibration::multiply];
    let mut count = checker.compute_calibration(&operators);
    println!("Part One solution: {count}");
    // --- Part Two ---
    operators.push(Calibration::concatenate);
    count = checker.compute_calibration(&operators);
    println!("Part Two solution: {count}");
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let checker = Calibration::new(&message);
        let mut operators: Vec<fn(u64, u64) -> u64> = vec![Calibration::add, Calibration::multiply];
        let response_part_1 = checker.compute_calibration(&operators);
        operators.push(Calibration::concatenate);
        let response_part_2 = checker.compute_calibration(&operators);

        assert!(response_part_1 == 3749);
        assert!(response_part_2 == 11387);
    }
}
