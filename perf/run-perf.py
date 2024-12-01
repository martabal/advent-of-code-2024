#!/bin/python3
import os
import questionary

COMPILERS = {
    "rust": {"compiler": "cargo", "folder": "rust"},
    "gcc": {"compiler": "gcc", "folder": "c", "extension": ".c"},
    "g++": {"compiler": "g++", "folder": "cpp", "extension": ".cc"},
    "clang": {"compiler": "clang", "folder": "c", "extension": ".c"},
    "clang++": {"compiler": "clang++", "folder": "cpp", "extension": ".cc"},
}


def run_perf(day: str, folder: str, compiler: str, output_file: str, flag: str = ""):
    output_path = f"./perf/day{day}/{folder}-{compiler}{flag}.txt"
    binary_path = f"./build/day{day}/{output_file}"
    os.system(f"perf stat -o {output_path} -r 1000 -B {binary_path} > /dev/null")


def compile_and_run(
    day: str, compiler_info: dict, check_run_perf: bool, optimization: bool = False
):
    folder = compiler_info["folder"]
    compiler = compiler_info["compiler"]
    extension = compiler_info["extension"]
    flag = "-O3" if optimization else ""
    output_file = f"day{day}-{folder}-{compiler}{flag}.out"

    print(f"{compiler} - compiling {'with -O3' if optimization else ''}")
    os.system(
        f"{compiler} {flag} ./{folder}/day{day}{extension} -o ./build/day{day}/{output_file}"
    )

    if check_run_perf:
        print(
            f"Running perf for program compiled with {compiler} {'and -O3 flag' if optimization else ''}"
        )
        run_perf(day, folder, compiler, output_file, flag)


def rust_compile_and_run(day: str, check_run_perf: bool, unstable: bool = False):
    print("Rust - compiling with stable")
    os.system(f"cargo build --release --package day{day} ")
    if run_perf:
        print("Running perf for program compiled with rust")
        os.system(
            f"perf stat -o perf/day{day}/rust.txt  -r 1000 -B target/release/day{day} > /dev/null"
        )
    if unstable:
        print("Rust - compiling with unstable")
        os.system(f"cargo build --release --features unstable --package day{day} ")
        if check_run_perf:
            print("Running perf for program compiled with unstable features with rust")
            os.system(
                f"perf stat -o perf/day{day}/rust-unstable.txt  -r 1000 -B target/release/day{day} > /dev/null"
            )


if __name__ == "__main__":
    day = questionary.text("What day?").ask()
    day = f"{int(day):02}"
    available_compilers = {}
    if os.path.exists(f"./c/day{day}.c"):
        available_compilers["gcc"] = COMPILERS["gcc"]
        available_compilers["clang"] = COMPILERS["clang"]

    if os.path.exists(f"cpp/day{day}.cc"):
        available_compilers["g++"] = COMPILERS["g++"]
        available_compilers["clang++"] = COMPILERS["clang++"]

    if os.path.exists(f"crates/day{day}"):
        available_compilers["rust"] = COMPILERS["rust"]

    if len(available_compilers.keys()) == 0:
        print("No solution for that day")
        exit(0)

    selected_compilers = questionary.checkbox(
        "What compiler to use", choices=list(available_compilers.keys())
    ).ask()

    check_run_perf = questionary.confirm("Run perf?").ask()

    run_with_o3 = (
        any(c in {"gcc", "g++", "clang", "clang++"} for c in selected_compilers)
        and questionary.confirm("Run with -O3 optimization?").ask()
    )

    if "rust" in selected_compilers:
        rust_unstable = questionary.confirm("Compiling with unstable?").ask()
        rust_compile_and_run(day, check_run_perf=check_run_perf, unstable=rust_unstable)

    for compiler in selected_compilers:
        if compiler in COMPILERS and compiler != "rust":
            compile_and_run(
                day,
                COMPILERS[compiler],
                check_run_perf=check_run_perf,
                optimization=False,
            )
            if run_with_o3:
                compile_and_run(
                    day,
                    COMPILERS[compiler],
                    check_run_perf=check_run_perf,
                    optimization=True,
                )
