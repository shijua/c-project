#! /usr/bin/python3
from __future__ import annotations

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

STUDENT_MODE = True



TestError = Enum("TestError", ["FAILED", "INCORRECT"])


class RunnerResult:
    assembler: Dict[str, Result[None, TestError]]
    emulator: Dict[str, Result[None, TestError]]

    def __init__(self, assembler, emulator):
        self.assembler = assembler
        self.emulator = emulator
        
    # def from_json(self, json_file: Path):
    #     with open(json, "r") as f:
    #         data = json.loads(f)
    #     self.assembler = data["assembler"]
    #     self.emulator = data["emulator"]
        


DEFAULT_ACT_OUTDIR = "out"
DEFAULT_EXP_OUTDIR = "test/expected"
DEFAULT_OUT_JSON = "out/results.json"
DEFAULT_TEST_CASES_DIR = "test/test_cases"
DEFAULT_GEN_DIR = "test/test_cases/generated"
DEFAULT_ASSEMBLER = "build/assemble"
DEFAULT_EMULATOR = "build/emulate"

class Runner:
    """Where to write test reports

    Attributes:
        _logfile: Path to logfile
        _outdir: Path to test output directory
        _assembler: Path to assembler to run
        _emulator: Path to emulator to run
        _toolchain_prefix: Prefix for toolchain binaries
        _verbose: Whether to print verbose output
        _assembler_only: Whether to only run the assembler
        _emulator_only: Whether to only run the emulator
        _assembler_results: Results of running the assembler
        _emulator_results: Results of running the emulator
    """

    _logfile: Path = None

    _actdir: Path = None
    _expdir: Path = None

    _assembler: Path = None
    _emulator: Path = None
    
    _testdir: Path = None

    _toolchain_prefix: Path = None

    _verbose: bool = False
    _assembler_only: bool = False
    _emulator_only: bool = False
    _expected_only: bool = False
    _actual_only: bool = False
    
    _do_listings: bool = False

    _assembler_results: Dict[str, Result[None, TestError]] = {}
    _emulator_results: Dict[str, Result[None, TestError]] = {}

    def with_test_files(self, test_files: List[str]) -> Runner:
        self._test_files = test_files
        assert all(
            file.match("*.s") for file in test_files
        ), f"test files must be .s files but found {' '.join(str(file) for file in test_files)}"
        return self

    def with_verbose(self) -> Runner:
        self._verbose = True
        return self

    def with_assembler_only(self) -> Runner:
        self._assembler_only = True
        return self

    def with_emulator_only(self) -> Runner:
        self._emulator_only = True
        return self
    
    def with_expected_only(self) -> Runner:
        self._expected_only = True
        return self

    def with_actual_only(self) -> Runner:
        self._actual_only = True
        return self
    
    def with_expdir(self, expdir: Path) -> Runner:
        self._expdir = expdir
        return self
    
    def with_actdir(self, actdir: Path) -> Runner:
        self._actdir = actdir
        return self
    
    def with_testdir(self, testdir: Path) -> Runner:
        self._testdir = testdir
        return self

    def with_reference_toolchain_prefix(self, prefix) -> Runner:
        self._toolchain_prefix = prefix
        return self

    def with_assembler_under_test(self, path) -> Runner:
        self._assembler = path
        return self

    def with_emulator_under_test(self, path) -> Runner:
        self._emulator = path
        return self

    def with_logfile(self, logfile) -> Runner:
        self._logfile = logfile
        return self

    def _assert_is_configured(self):
        assert self._emulator is not None, "emulator has not been initialised"
        assert self._assembler is not None, "assembler has not been initialised"
        # assert (
        #     self._toolchain_prefix is not None
        # ), "toolchain prefix has not been initialised"
        assert self._expdir is not None, "expected result directory has not been initialised"
        assert self._actdir is not None, "actual output directory has not been initialised"

    def _log(self, *args, **kwargs):
        if self._logfile is not None:
            with open(self._logfile, "a") as f:
                print(*args, **kwargs, file=f)

        if self._verbose:
            print(*args, **kwargs)

    def _compare_raw_bin(
        self, exp: Path, act: Path
    ) -> Result[None, Dict[int, Dict[str, int]]]:
        """Compares two raw assembled binaries and returns their difference, if any"""
        res = True
        diffs = {}
        with open(exp, "rb") as f1, open(act, "rb") as f2:
            """Compare non-zero words from start of files"""
            i = 0
            exp_word = f1.read(4)
            act_word = f2.read(4)
            while exp_word and act_word:
                ew = int.from_bytes(exp_word, byteorder="little")
                aw = int.from_bytes(act_word, byteorder="little")
                if ew != aw:
                    diffs[i] = {"exp": ew, "act": aw}
                    # Don't return yet, there might be more diffs
                    res = False

                exp_word = f1.read(4)
                act_word = f2.read(4)
                i += 4

            """
            Sometimes, the ARM assembler will pad the end of `exp` with all zero instructions, 
            so is longer than `act`.
            In this case, check all remaining instructions in `exp` are zero.
            """
            j = 0
            if exp_word and not act_word:
                while exp_word:
                    exp_word = f1.read(4)
                    ew = int.from_bytes(exp_word, byteorder="little")
                    if ew != 0:
                        res = False
                        diffs[i + j] = {"exp": ew, "act": 0}
                    j += 4

        return Ok(None) if res else Err(diffs)

    def _prepare_assembler_test(
        self, test: Path, expected_binary: Path, expected_listing: Path
    ) -> Result[None, None]:
        """Generate expected assembly, binary and listing"""
        self._log(f"prepare assembly: {test}")

        if self._actual_only:
            return Ok()

        try:
            with subprocess.Popen(
                [f"{self._toolchain_prefix}-as", test, "-o", expected_binary]
            ) as p:
                p.wait(1)
                if p.returncode != 0:
                    return Err(None)

            with subprocess.Popen(
                [f"{self._toolchain_prefix}-objcopy", "-O", "binary", expected_binary]
            ) as p:
                p.wait(1)
                if p.returncode != 0:
                    return Err(None)

            self._run_listing(expected_listing, expected_binary)

            return Ok(None)

        except subprocess.TimeoutExpired:
            return Err(None)

    def _dump_process_state(self, process: subprocess.CompletedProcess):
        text, err = process.communicate()
        self._log(text.decode()) if text else ()
        self._log(err) if err else ()

    def _run_listing(self, listing: Optional[Path], binary: Path):
        if self._do_listings and listing is not None:
            with open(listing, "w") as lst, subprocess.Popen(
                [
                    f"{self._toolchain_prefix}-objdump",
                    "-b",
                    "binary",
                    "-m",
                    "aarch64",
                    "-D",
                    binary,
                ],
                stdout=lst,
            ) as p:
                p.wait(1)
                if p.returncode != 0:
                    return Err(None)

    def _run_assembler_test(
        self, test: Path, out_act: Path, out_exp: Path, lst_act: Path
    ) -> Result[None, TestError]:
        """Run (student) assembler on test file `t`"""
        # Generate actual assembly

        if self._emulator_only or self._expected_only:
            return Ok()
        if not STUDENT_MODE and self._verbose:

            cmd = [f"./{self._assembler}", test, out_act, "-v"]
        else:
            cmd = [f"./{self._assembler}", test, out_act]

        self._log(f'run: {reduce(lambda x, y: f"{x} {y}", cmd, "")}')
        with subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
        ) as p:
            try:
                p.wait(1)
                self._log("Assembler Output:\n")
                for line in p.stdout.readlines():
                    line = line.decode()
                    if line.endswith("\n"):
                        line = line[:-1]
                    self._log(line)

                if p.returncode != 0:
                    self._log(
                        f"\n{Fore.RED} ASSEMBLER FAILED {Fore.RESET}: test {test}"
                    )
                    self._dump_process_state(p)
                    return Err(TestError.FAILED)
                self._run_listing(lst_act, out_act)
                result = self._compare_raw_bin(out_exp, out_act)

                if result.is_err():
                    diffs = result.value
                    self._log(
                        f"\n{Fore.RED} ASSEMBLER INCORRECT {Fore.RESET}: test {test}"
                    )
                    self._dump_assembler_diffs(test, diffs)
                    self._dump_process_state(p)
                    return Err(TestError.INCORRECT)

                self._log(f"\n{Fore.GREEN} ASSEMBLER CORRECT {Fore.RESET}: test {test}")
                return Ok()

            except subprocess.TimeoutExpired:
                self._log("Assembler Output:\n")
                for line in p.stdout.readlines():
                    line = line.decode()
                    if line.endswith("\n"):
                        line = line[:-1]
                    self._log(line)
                self._log(f"Timeout in test: {cmd}")
                self._log(f"\n{Fore.RED} ASSEMBLER CRASHED {Fore.RESET}: test {test}")
                return Err(TestError.FAILED)
    def _dump_assembler_diffs(self, test: Path, diffs: Dict[int, Dict[str, int]]):
        with self._open_diff_file(test, "w") as out:
            def _log_and_print(*args, **kwargs):
                self._log(*args, **kwargs)
                print(*args, file=out, **kwargs)
                
            
            _log_and_print(f"----------------------------------\nAssembled Binary: {test}\n")
            self._log("Differences:")
            for T, NM in [('act', 'Actual'), 
                                ('exp', 'Expected')]:
                _log_and_print(f"\n{NM}:")
                for k, v in diffs.items():
                    _log_and_print(f"    {f'0x{k:x}':<6}: {v[T]:08x}")
    def _dump_emulator_diffs(self, test: Path, diffs: Dict[int, Dict[str, int]]):
        """Dumps differences between expected and actual emulation to `out`"""

        with self._open_diff_file(test, "a") as out:
            def _log_and_print(*args, **kwargs):
                self._log(*args, **kwargs)
                print(*args, file=out, **kwargs)
                
            
            _log_and_print(f"\n----------------------------------\nEmulator Output: {test}\n")
            for T, NM in [('act', 'Actual'), 
                                ('exp', 'Expected')]:
                _log_and_print(f"\n{NM}:")
                self._log("Differences: ")
                if "regs" in diffs:
                    for k, v in diffs["regs"].items():
                        _log_and_print(f"{k.name:<3} = {v[T]:016x}")
                if "pstate" in diffs:
                    _log_and_print(f"PSTATE: {diffs['pstate'][T]}")
                if "nz_mem" in diffs:
                    for k, v in diffs["nz_mem"].items():
                        _log_and_print(f"0x{k:08x}: 0x{v[T]:08x}")
            


    def _prepare_emulator_test(
        self, test: Path, expected_binary: Path
    ) -> Result[emulate_test_data.CPU_State, None]:
        # Expected Emulation (this is expensive, so only done if the log isn't available)
        
        cachefile = self._get_expected_out_state(test)
        cachefile.parent.mkdir(parents=True, exist_ok=True)
        if cachefile.exists():
            expected_emulator_state = emulate_test_data.parse_out_file(cachefile)
        elif self._actual_only:
            raise Exception(f"Expected emulator output state not found at {cachefile}")
        else:
            return Err(None)

        return Ok(expected_emulator_state)

    def _run_emulator_test(
        self, test: Path, actual_binary: Path, expected_state: emulate_test_data.CPU_State
    ) -> Result[None, TestError]:
        """Run expected and actual emulation on test file `as_file`."""
        # Run emulation
        if self._assembler_only or self._expected_only:
            return Ok()
        
        with self._open_actual_out_state(test) as actual_state:
            cmd = [f"./{self._emulator}", actual_binary, actual_state.name]
            self._log(f'run: {reduce(lambda x, y: f"{x} {y}", cmd, "")}')
            with subprocess.Popen(
                cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
            ) as p:
                try:
                    p.wait(1)
                    self._log("Assembler Output:\n")
                    for line in p.stdout.readlines():
                        line = line.decode()
                        if line.endswith("\n"):
                            line = line[:-1]
                        self._log(line)

                    if p.returncode != 0:
                        self._log(
                            f"\n{Fore.RED} EMULATOR FAILED {Fore.RESET}: test {test}"
                        )
                        self._dump_process_state(p)
                        return Err(TestError.FAILED)

                    # Keeping Result changes local to this file
                    output_same, differences = expected_state.compare(
                        emulate_test_data.parse_out_file(actual_state.name)
                    )
                    if not output_same:
                        self._log(
                            f"\n{Fore.RED} EMULATOR INCORRECT {Fore.RESET}: test {test}"
                        )
                        text, err = p.communicate()
                        self._log(text.decode()) if text else ()
                        self._log(err) if err else ()
                        self._dump_emulator_diffs(test, differences)
                        return Err(TestError.INCORRECT)
                    
                    self._log(
                        f"\n{Fore.GREEN} EMULATOR CORRECT {Fore.RESET}: test {test}"
                    )
                    return Ok(None)

                except subprocess.TimeoutExpired:
                    self._log("Emulator Output:\n")
                    for line in p.stdout.readlines():
                        self._log(line)

                    self._log(f"Timeout in test: {cmd}")
                    self._log(
                        f"\n{Fore.RED} EMULATOR CRASHED {Fore.RESET}: test {test}"
                    )
                    return Err(TestError.FAILED)

    def _get_out_file(self, test: Path, suffix: str, root_dir: Path) -> Optional[Path]:
        if root_dir :
            sub_test = test.relative_to(self._testdir)
            res = root_dir / sub_test.with_suffix(suffix)
            res.parent.mkdir(parents=True, exist_ok=True)
            return res
        else:
            return None

    def _get_expected_file(self, test: Path, suffix: str) -> Optional[Path]:
        exp = test.parent / f"{test.stem}_exp"
        return self._get_out_file(exp, suffix, self._expdir)

    def _get_actual_file(self, test: Path, suffix: str) -> Optional[Path]:
        act = test
        return self._get_out_file(act, suffix, self._actdir)

    def _get_expected_binary(self, test: Path) -> Optional[Path]:
        return self._get_expected_file(test, ".bin")

    def _get_actual_binary(self, test: Path) -> Optional[Path]:
        return self._get_actual_file(test, ".bin")

    def _get_expected_out_state(self, test: Path) -> Optional[Path]:
        return self._get_expected_file(test, ".out")

    def _get_actual_out_state(self, test: Path) -> Optional[Path]:
        return self._get_actual_file(test, ".out")

    def _get_actual_listing(self, test: Path) -> Optional[Path]:
        if self._do_listings:
            return self._get_actual_file(test, ".lst")
        else: return None

    def _get_expected_listing(self, test: Path) -> Optional[Path]:
        return self._get_expected_file(test, ".lst") if self._do_listings else None
    
    def _get_diff_file(self, test: Path) -> Optional[Path]:
        return self._get_out_file(test, ".diff", self._actdir)

    def _open_diff_file(self, test: Path, mode: str) -> IO[Any]:
        return self._open_out_file(self._get_diff_file(test), mode)

    def _open_out_file(self, outfile: Path, mode: str) -> IO[Any]:
        if self._expdir or self._actdir:
            return outfile.open(mode)
        else:
            return tempfile.NamedTemporaryFile(mode=mode)
        
    def _open_out_act_file(self, outfile: Path, mode: str) -> IO[Any]:
        if not self._expected_only:
            return outfile.open(mode)
        else:
            return tempfile.NamedTemporaryFile(mode=mode)
        
    def _open_exp_file(self, out: Path, mode: str) -> IO[Any]:
        if not self._actual_only:
            return out.open(mode)
        else:
            return out.open("r")

    def _open_actual_listing(self, test: Path) -> IO[Any]:
        return self._open_out_act_file(self._get_actual_listing(test), "w")

    def _open_expected_listing(self, test: Path) -> IO[Any]:
        return self._open_exp_file(self._get_expected_listing(test), "w")

    def _open_actual_binary(self, test: Path) -> IO[Any]:
        if self._emulator_only:
            return self._open_expected_binary(test)

        return self._open_out_act_file(self._get_actual_binary(test), "wb")

    def _open_expected_binary(self, test: Path) -> IO[Any]:
        return self._open_exp_file(self._get_expected_binary(test), "wb")

    def _open_actual_out_state(self, test: Path) -> IO[Any]:
        return self._open_out_act_file(self._get_actual_out_state(test), "w")

    def _open_expected_out_state(self, test: Path) -> IO[Any]:
        return self._open_exp_file(self._get_expected_out_state(test), "w")

    def _run_test(self, test: Path):
        """Runs expected/actual assembly and emulation tests for a single test in assembly file `t`"""
        self._log(f"Running test {test}")
        self._get_diff_file(test).unlink(missing_ok=True)
        with open(test) as f:
            is_runnable = "RUNNABLE: False" not in f.readline()

        with self._open_expected_binary(
            test
        ) as expected_binary, self._open_actual_binary(test) as actual_binary:
            if self._prepare_assembler_test(
                test, expected_binary.name, self._get_expected_listing(test)
            ).is_err():
                self._log(
                    f"\n{Fore.RED} PREPARE ASSEMBLER FAILED {Fore.RESET}: test {test}"
                )
                return

            # The binary to use for the emulator
            binary_to_use = actual_binary

            # Run the tests
            if not self._emulator_only:
                assembler_result = self._run_assembler_test(
                    test,
                    Path(actual_binary.name),
                    Path(expected_binary.name),
                    self._get_actual_listing(test),
                )
                self._assembler_results[str(test)] = assembler_result
                
                # If the assembler was wrong, run the emulator on the expected binary
                if not assembler_result.is_ok():
                    binary_to_use = expected_binary

            if ((not is_runnable) or self._assembler_only): return

            expected_state = self._prepare_emulator_test(
                test, Path(expected_binary.name)
            )
            if not expected_state.is_ok():
                self._log(
                    f"\n{Fore.RED} PREPARE EMULATOR FAILED {Fore.RESET}: test {test}"
                )
                return



            if not self._assembler_only:
                self._emulator_results[str(test)] = self._run_emulator_test(
                    test, binary_to_use.name, expected_state.value
                )

    def run(self) -> RunnerResult:
        self._assert_is_configured()
        for t in self._test_files:
            self._run_test(t)
        return RunnerResult(self._assembler_results, self._emulator_results)


class JSONPrinter:
    results: RunnerResult = None
    outfile = None
    
    CORRECT_STR = "CORRECT"
    INCORRECT_STR = "INCORRECT"
    FAIL_STR = "FAIL"
    ASSEMBLER_STR = "assembler"
    EMULATOR_STR = "emulator"

    def with_results(self, results: RunnerResult) -> PrettyPrinter:
        self._results = results
        return self

    def with_outfile(self, outfile) -> PrettyPrinter:
        self.outfile = outfile
        return self

    def _result_as_string(self, result) -> str:
        if result.is_ok():
            return JSONPrinter.CORRECT_STR
        elif result.value is TestError.INCORRECT:
            return JSONPrinter.INCORRECT_STR
        else:
            return JSONPrinter.FAIL_STR

    def write(self):
        if self.outfile is None:
            raise Exception("outfile not set")
        with open(self.outfile, "w") as f:
            json.dump(
                {
                    JSONPrinter.ASSEMBLER_STR: {
                        name: self._result_as_string(value)
                        for name, value in self._results.assembler.items()
                    },
                    JSONPrinter.EMULATOR_STR: {
                        name: self._result_as_string(value)
                        for name, value in self._results.emulator.items()
                    },
                },
                f,
            )


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
        self, component_name: str, component_results: Dict[str, Result[None, TestError]]
    ):
        # Format Summary
        self.tell(f"———————————————————————\n{component_name} Summary:\n\n")

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
        self, component_name: str, component_results: Dict[str, Result[None, TestError]]
    ) -> None:
        self.tell(f"———————————————————————\n{component_name} Tests:\n")
        if len(component_results) == 0:
            self.tell(f"  {Fore.CYAN}None{Fore.RESET}\n")
            return
        # Fixes formatting if a test name is too long
        fmt_length = max((len(name) for name in component_results.keys()), default=0)

        all_passed: bool = True
        for name, result in sorted(component_results.items()):
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
        component_results: Dict[str, Result[None, TestError]],
    ) -> None:
        if when:
            self._tell_results(component_name, component_results)

    def _tell_summary_when(
        self,
        when: bool,
        component_name: str,
        component_results: Dict[str, Result[None, TestError]],
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
    extra.add_argument("-o", "--outfile", help="file to write results", default=DEFAULT_OUT_JSON)

    extra.add_argument(
        "--outdir",
        help="directory to write intermediate test files",
        default=DEFAULT_ACT_OUTDIR,
    )
    
    extra.add_argument( "--actdir", help="directory to write test results ", default=DEFAULT_ACT_OUTDIR)
    extra.add_argument( "--expdir", help="directory containing expected results ", default=DEFAULT_EXP_OUTDIR)
    
    
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
        supervisor.add_argument("-g", "--generate", action="store_true", help="generate tests")
        supervisor.add_argument(
            "-X", "--expected-only", action="store_true", help="only generate expected results"
        )
        
        supervisor.add_argument(
        "-Y", "--actual-only", action="store_true", help="only generate actual results"
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
    """ Adds arm toolchain if installed """
    raise_err = False
    try:
        if path_to_toolchain is None:
            toolchain = "aarch64-none-elf"
        else:
            toolchain = f"{path_to_toolchain}/aarch64-none-elf"
        subprocess.call([f"{toolchain}-objdump", "--version"], stdout=subprocess.DEVNULL)
        
        runner.with_reference_toolchain_prefix(toolchain)
    except FileNotFoundError:
        raise_err = True
    if raise_err and path_to_toolchain is not None:
        raise FileNotFoundError(f"aarch64-none-elf toolchain binaries not found at: {path_to_toolchain}")


def run_tests_with_args(args: List[str]) -> RunnerResult:
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

    runner = Runner()
    
    tests_to_run = []
    
    test_dir = Path(args.testdir)

    if len(args.testfiles) == 0:
        tests_to_run = [path for path in test_dir.rglob("*.s")]
    else:
        for testfile in args.testfiles:
            testfilepath = Path(testfile)
            assert testfilepath.exists(), f"{testfilepath} does not exist"
            if testfilepath.is_dir():
                tests_to_run += [path for path in Path(testfilepath).rglob("*.s")]
            else:
                tests_to_run += [testfilepath]

#######################################################################################################

    setup_arm_toolchain(runner, args.toolchain)
    
    runner.with_test_files(tests_to_run)\
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
            raise Exception("Can't have both -X (--expected-only) and -Y (--actual-only)")

        if args.expected_only:
            if STUDENT_MODE:
                raise Exception("Cannot Generate Expected Results (missing dependencies)")
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

    try:
        results = runner.run()
    except FileNotFoundError as e:
        print(f"File {e.filename} not found")
        return
    except Exception as e:
        raise e
        # print(f"Error: {e}")
        # return

        

    if args.pretty_print:
        pp = PrettyPrinter().with_results(results)
        pp._emulator_only = args.emulator_only
        pp._assembler_only = args.assembler_only

        # if args.outfile:
        #     pp.with_outfile(args.outfile)

        if args.failures:
            pp.with_failures_only()

        pp.write()
    
    jp = JSONPrinter().with_results(results)

    if args.outfile:
        jp.with_outfile(args.outfile)

    jp.write()
    
    return results

def main():
    run_tests_with_args(sys.argv[1:])


if __name__ == "__main__":
    main()
