use helpers::read_file;

#[derive(Clone, Copy, Debug)]
enum File {
    Free,
    Taken(u32),
}

struct FileSystem {
    files: Vec<File>,
    defragmented_files: Vec<File>,
}

impl FileSystem {
    fn new(content: &str) -> Self {
        let mut files: Vec<File> = Vec::new();
        let mut id = 0;
        for line in content.lines() {
            for (i, c) in line.chars().enumerate() {
                if let Some(digit) = c.to_digit(10) {
                    if i % 2 == 0 {
                        files.extend(
                            std::iter::repeat(File::Taken(id)).take(digit.try_into().unwrap()),
                        );
                        id += 1;
                    } else {
                        files.extend(std::iter::repeat(File::Free).take(digit.try_into().unwrap()));
                    }
                }
            }
        }
        let defragmented_files = files.clone();
        Self {
            files,
            defragmented_files,
        }
    }

    fn count_free_files(files: &[File], index: usize) -> usize {
        let mut count = 0;
        for i in index..files.len() {
            match files.get(i) {
                Some(File::Free) => {
                    count += 1;
                }
                Some(File::Taken(_)) | None => {
                    break;
                }
            }
        }

        count
    }

    fn detect_block(files: &[File], index_end: usize) -> Option<(usize, usize)> {
        let mut current_block: Option<(u32, usize)> = None;

        for i in (0..index_end).rev() {
            match files.get(i) {
                Some(File::Free) => {
                    if let Some(new_block) = current_block {
                        return Some((i + 1, new_block.1));
                    }
                    current_block = None;
                }
                Some(File::Taken(digit)) => {
                    if current_block.is_none() {
                        current_block = Some((*digit, 1));
                    } else if *digit != current_block.unwrap().0 {
                        return Some((i + 1, current_block.unwrap().1));
                    } else {
                        current_block = Some((*digit, current_block.unwrap().1 + 1));
                    }
                }
                None => {
                    break;
                }
            }
        }

        None
    }

    fn fragment_files(&mut self) {
        for i in 0..self.defragmented_files.len() {
            match self.defragmented_files.get(i) {
                Some(File::Free) => {
                    if let Some((index, _)) = self
                        .defragmented_files
                        .iter()
                        .enumerate()
                        .rfind(|&(j, file)| j > i && matches!(file, File::Taken(_)))
                    {
                        self.defragmented_files.swap(i, index);
                    }
                }
                Some(File::Taken(_)) | None => {}
            }
        }
    }

    fn fragment_files_block(&mut self) {
        let mut end_vec = self.files.len();
        while let Some((start_index, size)) = Self::detect_block(&self.files, end_vec) {
            let mut following_free_file = false;
            for (index, file) in self.files.iter().enumerate() {
                match file {
                    File::Free => {
                        if !following_free_file {
                            following_free_file = true;
                            let number_free_files = Self::count_free_files(&self.files, index);
                            if number_free_files >= size && index < start_index {
                                for j in 0..size {
                                    self.files.swap(index + j, start_index + j);
                                }

                                break;
                            }
                        }
                    }
                    File::Taken(_) => following_free_file = false,
                }
            }
            end_vec = start_index;
        }
    }

    fn calculate_checksum(files: &[File]) -> u64 {
        files
            .iter()
            .enumerate() // Get position (index) along with each file
            .filter_map(|(pos, file)| {
                if let File::Taken(id) = file {
                    // println!("{pos} * {id} [{file:?}]");
                    Some(pos as u64 * u64::from(*id))
                } else {
                    None
                }
            })
            .sum()
    }
}

fn main() {
    let message = read_file("crates/day09/input.txt").unwrap();

    // --- Part One ---
    let mut checker = FileSystem::new(&message);
    checker.fragment_files();
    println!(
        "Part One solution: {:?}",
        FileSystem::calculate_checksum(&checker.defragmented_files)
    );
    // --- Part Two ---
    checker.fragment_files_block();
    println!(
        "Part Two solution: {:?}",
        FileSystem::calculate_checksum(&checker.files)
    );
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn check_exemple() {
        let message = read_file("example_input.txt").unwrap();

        let mut checker = FileSystem::new(&message);
        checker.fragment_files();
        checker.fragment_files_block();
        let response_part_1 = FileSystem::calculate_checksum(&checker.defragmented_files);
        let response_part_2 = FileSystem::calculate_checksum(&checker.files);

        assert!(response_part_1 == 1928);
        assert!(response_part_2 == 2858);
    }

    #[test]
    fn check_result() {
        let message = read_file("input.txt").unwrap();

        let mut checker = FileSystem::new(&message);
        checker.fragment_files();
        checker.fragment_files_block();
        let response_part_1 = FileSystem::calculate_checksum(&checker.defragmented_files);
        let response_part_2 = FileSystem::calculate_checksum(&checker.files);

        assert!(response_part_1 == 6288707484810);
        assert!(response_part_2 == 6311837662089);
    }
}
