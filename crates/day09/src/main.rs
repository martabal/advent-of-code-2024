use helpers::read_file;

#[derive(Clone)]
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
        files[index..]
            .iter()
            .take_while(|file| matches!(file, File::Free))
            .count()
    }

    fn detect_block(files: &[File], index_end: usize) -> Option<(usize, usize)> {
        let mut current_block: Option<(u32, usize)> = None;

        for i in (0..index_end).rev() {
            match &files[i] {
                File::Free => {
                    if let Some(block) = current_block {
                        return Some((i + 1, block.1));
                    }
                    current_block = None;
                }
                File::Taken(id) => {
                    if let Some(block) = current_block {
                        if *id != block.0 {
                            return Some((i + 1, block.1));
                        }
                        current_block = Some((*id, block.1 + 1));
                    } else {
                        current_block = Some((*id, 1));
                    }
                }
            }
        }

        None
    }

    fn fragment_files(&mut self) {
        let mut last_index = self.defragmented_files.len();
        for i in 0..self.defragmented_files.len() {
            match self.defragmented_files.get(i) {
                Some(File::Free) => {
                    let mut check_index: Option<usize> = None;
                    for j in (i..last_index).rev() {
                        match self.defragmented_files.get(j) {
                            Some(File::Free) | None => {}
                            Some(File::Taken(_)) => {
                                check_index = Some(j);
                                break;
                            }
                        }
                    }

                    if let Some(index) = check_index {
                        last_index = index;
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
            .enumerate()
            .filter_map(|(index, file)| match file {
                File::Taken(id) => Some(index as u64 * u64::from(*id)),
                File::Free => None,
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
