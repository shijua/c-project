#! /usr/bin/python3
from __future__ import annotations
from dataclasses import dataclass

STUDENT_MODE = True

# standard library
import sys
from functools import reduce
import shutil
import subprocess
from pathlib import Path
from typing import IO, Any, Dict, List, Optional
import argparse
from enum import Enum
import json
import tempfile

# third party
from result import Ok, Err, Result
from colorama import Fore

# ours
import test.emulate_test_data as emulate_test_data

from test.test_runner import *


class JSONPrinter:
    results: RunnerResult = None
    outfile = None

    ASSEMBLER_STR = "assembler"
    EMULATOR_STR = "emulator"

    def with_results(self, results: RunnerResult) -> PrettyPrinter:
        self._results = results
        return self

    def with_outfile(self, outfile: Path) -> PrettyPrinter:
        self.outfile = outfile
        return self

    def write(self, assemble_only=False, emulate_only=False):
        if self.outfile is None:
            raise Exception("outfile not set")
        
        js = {}
        if self.outfile.exists():
            with open(self.outfile, "r") as f:
                js = json.load(f)
        
        if not assemble_only:
            js["assembler_summary"] = self._results.asm_summary
            js[JSONPrinter.EMULATOR_STR] = {
                str(name): result_as_string(value.result)
                for name, value in self._results.emulator.items()
            }
        if not emulate_only:
            js["emulator_summary"] = self._results.emu_summary
            js[JSONPrinter.ASSEMBLER_STR] = {
                str(name): result_as_string(value.result)
                for name, value in self._results.assembler.items()
            }
        
        with open(self.outfile, "w") as f:
            json.dump(js, f, indent=4)


class PrettyPrinter:
    failures_only: bool = False
    results: RunnerResult = None
    _assembler_only: bool = False
    _emulator_only: bool = False
    outfile = None

    def with_failures_only(self) -> PrettyPrinter:
        self.failures_only = True
        return self

    def with_results(self, results: RunnerResult) -> PrettyPrinter:
        self.results = results
        return self

    def with_outfile(self, outfile) -> PrettyPrinter:
        self.outfile = outfile
        return self

        # Formats success or fail string

    def _right_or_wrong(self, succeeded: bool, success: str, fail: str) -> str:
        """Formats success or fail string.

        Returns a string in green if b is True, red if False
        """
        return (Fore.GREEN + success if succeeded else Fore.RED + fail) + Fore.RESET

    def _result_as_string(self, result) -> str:
        if result.is_ok():
            return "CORRECT"
        elif result.value is TestError.INCORRECT:
            return "INCORRECT"
        else:
            return "FAIL"

    def _tell_summary(
        self, component_name: str, component_results: Dict[Test, TestResult]
    ):
        # Format Summary
        self.tell(f"———————————————————————\n{component_name} Summary:\n\n")

        component_results = {
            str(name): v.result for name, v in component_results.items()
        }

        maxTestNameLen = max([len("test")] + [len(x) for x in component_results.keys()])
        self.tell(f"{'Test':{maxTestNameLen}}  Correct  Incorrect  Failed  Total\n")

        total = len(component_results)
        failed = incorrect = correct = 0

        for result in component_results.values():
            if result.is_ok():
                correct += 1
            elif result.value is TestError.FAILED:
                failed += 1
            else:
                incorrect += 1

        self.tell(
            f"\n{'Total':{maxTestNameLen}}  {correct:>7}  {incorrect:>9}  {failed:>6}  {total:>5}\n"
        )

    def _tell_results(
        self, component_name: str, component_results: Dict[Test, TestResult]
    ) -> None:
        component_results = {
            str(name): v.result for name, v in component_results.items()
        }

        self.tell(f"———————————————————————\n{component_name} Tests:\n")
        if len(component_results) == 0:
            self.tell(f"  {Fore.CYAN}None{Fore.RESET}\n")
            return
        # Fixes formatting if a test name is too long
        fmt_length = max(
            (len(str(name)) for name in component_results.keys()), default=0
        )

        all_passed: bool = True
        for name, result in sorted(component_results.items()):
            name = str(name)
            completed = result.is_ok() or (
                result.is_err() and (result.value is not TestError.FAILED)
            )
            completed_result = self._right_or_wrong(completed, "SUCCEEDED", "FAILED")
            correct_result = (
                self._right_or_wrong(result.is_ok(), "CORRECT", "INCORRECT")
                if completed
                else "-"
            )
            if (not self.failures_only) or result.is_err():
                self.tell(
                    f"   {name:{fmt_length}}:   {completed_result:{19}} {correct_result:}\n"
                )
            if result.is_err():
                all_passed = False
        if all_passed:
            self.tell(f"  {Fore.GREEN}All tests passed!{Fore.RESET}\n")

    def tell(self, s: str, *args, **kwargs):
        self.print(s, *args, end="", **kwargs)

    def _tell_results_when(
        self,
        when: bool,
        component_name: str,
        component_results: Dict[Test, TestResult],
    ) -> None:
        if when:
            self._tell_results(component_name, component_results)

    def _tell_summary_when(
        self,
        when: bool,
        component_name: str,
        component_results: Dict[Test, TestResult],
    ) -> None:
        if when:
            self._tell_summary(component_name, component_results)

    def print(self, s: str, *args, **kwargs):
        print(s, *args, file=self.outfile, **kwargs)

    def write(self):
        self.tell("\n")

        self._tell_results_when(
            not self._emulator_only, "Assembler", self.results.assembler
        )
        self._tell_results_when(
            not self._assembler_only, "Emulator", self.results.emulator
        )
        self._tell_summary_when(
            not self._emulator_only, "Assembler", self.results.assembler
        )
        self._tell_summary_when(
            not self._assembler_only, "Emulator", self.results.emulator
        )
        self.tell("\n")


def setup_params() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Run emulator and assembler tests")

    common = parser.add_argument_group("common options")

    common.add_argument(
        "testfiles", metavar="testfiles", nargs="*", help="input file(s)"
    )
    common.add_argument(
        "-a", "--assembler", default=DEFAULT_ASSEMBLER, help="path to assembler to test"
    )
    common.add_argument(
        "-e", "--emulator", default=DEFAULT_EMULATOR, help="path to emulator to test"
    )
    common.add_argument(
        "-H",
        "--help-extra",
        help="show this help message and exit",
        action="store_true",
        default=False,
    )

    common.add_argument(
        "-t",
        "--testdir",
        help="directory containing test files",
        default=DEFAULT_TEST_CASES_DIR,
    )

    extra = parser.add_argument_group("extra options")
    extra.add_argument("-v", "--verbose", action="store_true", help="run verbose")
    extra.add_argument(
        "-p",
        "--pretty-print",
        action="store_true",
        help="pretty print results",
        default=False,
    )
    extra.add_argument("-l", "--logfile", help="file to log output")
    extra.add_argument(
        "-o", "--outfile", help="file to write results", default=DEFAULT_OUT_JSON
    )

    extra.add_argument(
        "--outdir",
        help="directory to write intermediate test files",
        default=DEFAULT_ACT_OUTDIR,
    )

    extra.add_argument(
        "--actdir", help="directory to write test results ", default=DEFAULT_ACT_OUTDIR
    )
    extra.add_argument(
        "--expdir",
        help="directory containing expected results ",
        default=DEFAULT_EXP_OUTDIR,
    )

    extra.add_argument(
        "-A",
        "--assembler-only",
        action="store_true",
        help="run assembler tests only",
        default=False,
    )

    extra.add_argument(
        "-E",
        "--emulator-only",
        action="store_true",
        help="run emulator tests only",
        default=False,
    )

    extra.add_argument(
        "-f",
        "--failures",
        action="store_true",
        help="output failures only",
        default=False,
    )

    extra.add_argument(
        "--toolchain",
        help="path to arm toolchain bin folder",
    )

    # These are only for non-student use
    if not STUDENT_MODE:
        supervisor = parser.add_argument_group("supervisor options (NOT FOR STUDENTS)")
        supervisor.add_argument(
            "-g", "--generate", action="store_true", help="generate tests"
        )
        supervisor.add_argument(
            "-X",
            "--expected-only",
            action="store_true",
            help="only generate expected results",
        )

        supervisor.add_argument(
            "-Y",
            "--actual-only",
            action="store_true",
            help="only generate actual results",
        )

        supervisor.add_argument(
            "-G", "--generate-only", action="store_true", help="generate tests only"
        )

        supervisor.add_argument(
            "--thorough-memory-check",
            action="store_true",
            help="run unicorn memory checker",
            default=False,
        )

        supervisor.add_argument(
            "--seed", help="seed to initialise random number generator", default=42
        )
        supervisor.add_argument(
            "--noout",
            action="store_true",
            help="Don't save intermediate results and files",
            default=False,
        )
    return parser


def setup_arm_toolchain(runner: Runner, path_to_toolchain: Optional[str]) -> None:
    """Adds arm toolchain if installed"""
    raise_err = False
    try:
        if path_to_toolchain is None:
            toolchain = "aarch64-none-elf"
        else:
            toolchain = f"{path_to_toolchain}/aarch64-none-elf"
        subprocess.call(
            [f"{toolchain}-objdump", "--version"], stdout=subprocess.DEVNULL
        )

        runner.with_reference_toolchain_prefix(toolchain)
    except FileNotFoundError:
        raise_err = True
    if raise_err and path_to_toolchain is not None:
        raise FileNotFoundError(
            f"aarch64-none-elf toolchain binaries not found at: {path_to_toolchain}"
        )


def run_tests_with_args(args: List[str], with_runner: Runner = None) -> RunnerResult:
    parser = setup_params()
    args = parser.parse_args(args)

    make_cmd = ["make"]
    if not args.verbose:
        make_cmd += ["--silent"]
    if not STUDENT_MODE and not args.generate_only:
        subprocess.check_call(make_cmd)

    if args.emulator_only and args.assembler_only:
        raise Exception(
            "Can't have both -A (--assembler-only) and -E (--emulator-only)"
        )

    #######################################################################################################
    # Which tests to run

    runner = Runner() if with_runner is None else with_runner

    tests_to_run = []

    test_dir = Path(args.testdir)

    if len(args.testfiles) == 0:
        is_run_all = True
        tests_to_run = [path for path in test_dir.rglob("*.s")]
    else:
        is_run_all = False
        for testfile in args.testfiles:
            testfilepath = Path(testfile)
            assert testfilepath.exists(), f"{testfilepath} does not exist"
            if testfilepath.is_dir():
                tests_to_run += [path for path in Path(testfilepath).rglob("*.s")]
            else:
                tests_to_run += [testfilepath]

    #######################################################################################################

    setup_arm_toolchain(runner, args.toolchain)

    runner\
        .with_test_files(tests_to_run)\
        .with_assembler_under_test(args.assembler)\
        .with_emulator_under_test(args.emulator)

    if args.verbose:
        runner.with_verbose()

    if args.logfile:
        runner.with_logfile(Path(args.logfile))

    if args.assembler_only:
        runner.with_assembler_only()

    if args.emulator_only:
        runner.with_emulator_only()

    if not STUDENT_MODE:
        if args.thorough_memory_check:
            runner.with_thorough_memory_check()

        if args.expected_only and args.actual_only:
            raise Exception(
                "Can't have both -X (--expected-only) and -Y (--actual-only)"
            )

        if args.expected_only:
            if STUDENT_MODE:
                raise Exception(
                    "Cannot Generate Expected Results (missing dependencies)"
                )
            runner.with_expected_only()

        if args.actual_only:
            runner.with_actual_only()

    if STUDENT_MODE:
        runner.with_actual_only()

    if args.expdir:
        expdir = Path(args.expdir)
        expdir.mkdir(parents=True, exist_ok=True)
        runner.with_expdir(expdir)

    if args.actdir:
        actdir = Path(args.actdir)
        actdir.mkdir(parents=True, exist_ok=True)
        runner.with_actdir(actdir)

    if args.testdir:
        testdir = Path(args.testdir)
        testdir.mkdir(parents=True, exist_ok=True)
        runner.with_testdir(testdir)


    val_res = runner.validate()
    if val_res.is_err():
        raise Exception(val_res.err())

    try:
        results = runner.run()
    except FileNotFoundError as e:
        print(f"File {e.filename} not found")
        return
    except Exception as e:
        raise e

    if args.pretty_print:
        pp = PrettyPrinter().with_results(results)
        pp._emulator_only = args.emulator_only
        pp._assembler_only = args.assembler_only

        if args.failures:
            pp.with_failures_only()

        pp.write()

    jp = JSONPrinter().with_results(results)

    if args.outfile and is_run_all:
        out_path = Path(args.outfile)
        out_path.parent.mkdir(parents=True, exist_ok=True)
        jp.with_outfile(out_path)
        jp.write()

    return results


def main():
    run_tests_with_args(sys.argv[1:])


if __name__ == "__main__":
    main()
