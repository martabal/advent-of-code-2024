use std::collections::HashMap;

use helpers::read_file;

struct PrintQueueChecker {
    rules: HashMap<i32, Vec<i32>>,
    page_numbers: Vec<Vec<i32>>,
}

fn main() {
    let message = read_file("crates/day05/input.txt").unwrap();
    // --- Part One ---
    let checker = PrintQueueChecker::new(&message);
    println!("Part One solution: {}", checker.check_page_order());

    // --- Part Two ---
    println!("Part Two solution: {}", checker.check_and_sort_page_order());
}

impl PrintQueueChecker {
    fn new(content: &str) -> Self {
        let mut rules = HashMap::new();
        content
            .lines()
            .take_while(|l| !l.is_empty())
            .filter_map(|l| {
                let numbers = l.split('|').collect::<Vec<&str>>();
                let left = numbers.first()?.parse::<i32>().ok()?;
                let right = numbers.get(1)?.parse::<i32>().ok()?;
                Some((left, right))
            })
            .for_each(|(left, right)| {
                rules.entry(right).or_insert(vec![]).push(left);
            });

        let page_numbers = content
            .lines()
            .skip_while(|x| !x.is_empty())
            .skip(1)
            .filter_map(|l| l.split(',').map(|x| x.parse().ok()).collect())
            .collect();
        Self {
            rules,
            page_numbers,
        }
    }

    fn check_valid(&self, order: &[i32]) -> bool {
        assert!(order.len() % 2 == 1);
        for (idx, val) in order.iter().enumerate() {
            let Some(rule) = self.rules.get(val) else {
                continue;
            };

            for rule_val in rule {
                if order[idx..].contains(rule_val) {
                    return false;
                }
            }
        }
        true
    }

    fn check_page_order(&self) -> i32 {
        self.page_numbers
            .iter()
            .filter(|order| self.check_valid(order))
            .map(|order| order[order.len() / 2])
            .sum()
    }

    fn check_and_sort_page_order(&self) -> i32 {
        self.page_numbers
            .iter()
            .filter(|order| !self.check_valid(order))
            .map(|order| {
                let mut order_clone = order.clone();
                while !self.check_valid(&order_clone) {
                    let mut found_rule_break = false;
                    let mut old_index = 0;
                    let mut new_index = 0;
                    let mut new_val = 0;

                    for (index, val) in order_clone.iter().enumerate() {
                        let Some(rule) = self.rules.get(val) else {
                            continue;
                        };

                        for (check_idx, check_val) in
                            order_clone.iter().enumerate().skip(index).rev()
                        {
                            if rule.contains(check_val) {
                                new_index = check_idx;
                                found_rule_break = true;
                                old_index = index;
                                new_val = *val;
                                break;
                            }
                        }
                    }

                    if found_rule_break {
                        order_clone.remove(old_index);
                        order_clone.insert(new_index, new_val);
                    }
                }
                assert!(order_clone.len() % 2 == 1);
                order_clone[order_clone.len() / 2]
            })
            .sum()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let checker = PrintQueueChecker::new(&message);
        let response_part_1 = checker.check_page_order();
        let response_part_2 = checker.check_and_sort_page_order();

        assert!(response_part_1 == 143);
        assert!(response_part_2 == 123);
    }

    #[test]
    fn check_result() {
        let message = read_file("input.txt").unwrap();

        let checker = PrintQueueChecker::new(&message);
        let response_part_1 = checker.check_page_order();
        let response_part_2 = checker.check_and_sort_page_order();

        assert!(response_part_1 == 7365);
        assert!(response_part_2 == 5770);
    }
}
