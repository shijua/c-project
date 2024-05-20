#! /usr/bin/python3
from __future__ import annotations
from dataclasses import dataclass

# standard library
import sys
from functools import reduce
import shutil
import subprocess
from pathlib import Path
from typing import IO, Any, Callable, Dict, List, Optional
import argparse
from enum import Enum
import json
import tempfile

# third party
from result import Ok, Err, Result
from colorama import Fore

# ours
import test.emulate_test_data as emulate_test_data
from test.run_tests import STUDENT_MODE
from test.writer import StringWriter, Writer, ListWriter, IOTextWriter

TestError = Enum("TestError", ["FAILED", "INCORRECT"])

def wrap_open(path: Optional[Path], mode: str, *args, **kw_args) -> IO[Any]:
    if path is None:
        return tempfile.TemporaryFile(*args, mode=mode, **kw_args)
    return open(path, mode, *args, **kw_args)


def write_process_state(process: subprocess.CompletedProcess, out: Writer[str]):
    text, err = process.communicate()
    if text: out.tell(text.decode())
    if err: out.tell(err.decode())

def compare_raw_bin(exp: Path, act: Path) -> Result[None, Dict[int, Dict[str, int]]]:
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


def dump_assembler_diffs(self, test: Path, diffs: Dict[int, Dict[str, int]]):
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
def dump_emulator_diffs(self, test: Path, diffs: Dict[int, Dict[str, int]]):
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
        

CORRECT_STR = "CORRECT"
INCORRECT_STR = "INCORRECT"
FAIL_STR = "FAIL"

def result_as_string(result: TResult) -> str:
    if result.is_ok():
        return CORRECT_STR
    elif result.value is TestError.INCORRECT:
        return INCORRECT_STR
    else:
        return FAIL_STR


class Test(object):
    def __init__(self, path: Path, rootdir: Path, actdir: Path, expdir: Path) -> None:
        super().__init__()
        self._path: Path = path
        
        sub_test = path.relative_to(rootdir)
        act_base = actdir / sub_test.with_suffix("")
        act_base.parent.mkdir(parents=True, exist_ok=True)
        
        exp_base = expdir / sub_test
        exp_base = exp_base.parent / f"{exp_base.stem}_exp"
        
        self.relative = sub_test
        
        self._act_lst = act_base.with_suffix(".lst")
        self._act_bin = act_base.with_suffix(".bin")
        self._act_out = act_base.with_suffix(".out")
        self._act_json = act_base.with_suffix(".json")
        self._exp_lst = exp_base.with_suffix(".lst")
        self._exp_bin = exp_base.with_suffix(".bin")
        self._exp_out = exp_base.with_suffix(".out")
        
    def assemble_only(self) -> None:
        self._act_out = None
        self._exp_out = None
        
    def emulate_only(self) -> None:
        self._act_lst = None
        self._act_bin = None
        self._exp_lst = None
        self._exp_bin = None

    def _open_out_file(self, outfile: Optional[Path], mode: str) -> IO[Any]:
        if outfile is not None:
            return outfile.open(mode)
        else:
            return tempfile.NamedTemporaryFile(mode=mode)

    def open_actual_listing(self) -> IO[Any]:
        return open(self._act_lst, "w")

    def open_expected_listing(self) -> IO[Any]:
        return open(self._exp_lst, "w")

    def open_actual_binary(self) -> IO[Any]:
        return open(self._act_bin, "wb")

    def open_expected_binary(self) -> IO[Any]:
        return open(self._exp_bin, "wb")

    def open_actual_out_state(self) -> IO[Any]:
        return open(self._act_out, "w")

    def open_expected_out_state(self) -> IO[Any]:
        return open(self._exp_out, "w")
    
    def __str__(self) -> str:
        return str(self._path)


TResult = Result[None, TestError]
TestType = Enum("TestType", ["ASSEMBLER", "EMULATOR"])

class TestResult():
    def __init__(self, test: Test, ttype: TestType) -> None:
        self.test: Test = test
        self.type: TestType = ttype 
        self.result: TResult = None
        self.diff: Dict = None
        self.log: str  = None
    
    def write_to_json(self, result_json: Path) -> None:
        js = {"test": str(self.test._path)}
        if result_json.exists():
            with open(result_json, "r") as f:
                js = json.load(f)
            
        to_write = {
            "result": result_as_string(self.result),
            "log": self.log,
            "diff": self.diff
        }
        js[self.type.name.lower()] = to_write
        with open(result_json, "w") as f:
            json.dump(js, f, indent=4)
    
    def with_result(self, result: TResult) -> TestResult:
        self.result = result
        return self
    
    def with_log_exception(self, e: Exception) -> TestResult:
        self.log = str(e)
        return self
    
    def save_diffs(self, diffs: Dict) -> None:
        self.diff = diffs
    
    def save_process(self, proc: subprocess.CompletedProcess) -> None:
        s = StringWriter()
        write_process_state(proc, s)
        self.log = s.to_string()

class AssemblerResult(TestResult):
    def __init__(self, test: Test) -> None:
        super().__init__(test, TestType.ASSEMBLER)
            
class EmulatorResult(TestResult):
    def __init__(self, test: Test) -> None:
        super().__init__(test, TestType.EMULATOR)

        
    

class RunnerResult:
    assembler: Dict[Test, TestResult]
    emulator: Dict[Test, TestResult]
    exceptions: Dict[Test, Exception]
    asm_summary: Dict[str, int]
    emu_summary: Dict[str, int]

    def __init__(self, assembler, emulator, exceptions) -> None:
        self.assembler = assembler
        self.emulator = emulator
        self.exceptions = exceptions
        
        self.asm_summary = self._summarise(assembler)
        self.emu_summary = self._summarise(emulator)
        
    def _summarise(self, results: Dict[Test, TestResult]) -> Dict[str, int]:
        summary = {}
        summary["total"] = len(results)
        summary["correct"] = len([x for _, x in results.items() if x.result.is_ok()])
        summary["failed"] = len([x for _, x in results.items() if x.result.value is TestError.FAILED])
        summary["incorrect"] = len([x for _, x in results.items() if x.result.value is TestError.INCORRECT])
        return summary
        


DEFAULT_ACT_OUTDIR = "out"
DEFAULT_EXP_OUTDIR = "test/expected"
DEFAULT_OUT_JSON = "out/results.json"
DEFAULT_TEST_CASES_DIR = "test/test_cases"
DEFAULT_GEN_DIR = "test/test_cases/generated"
DEFAULT_ASSEMBLER = "build/assemble"
DEFAULT_EMULATOR = "build/emulate"


def when_assemble(method):
    def wrapper(self, *args, **kwargs):
        if self._emulator_only:
            return
        return method(self, *args, **kwargs)
    return wrapper
    
def when_emulate(method):
    def wrapper(self, *args, **kwargs):
        if self._assembler_only:
            return
        return method(self, *args, **kwargs)
    return wrapper

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


    _assembler_results: Dict[Test, TResult] = {}
    _emulator_results: Dict[Test, TResult] = {}
    _exceptions: Dict[Test, Exception] = {}

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
        self._do_listings = True
        return self

    def with_assembler_under_test(self, path: str) -> Runner:
        self._assembler = Path(path)
        return self

    def with_emulator_under_test(self, path: str) -> Runner:
        self._emulator = Path(path)
        return self

    def with_logfile(self, logfile) -> Runner:
        self._logfile = logfile
        return self

    def _assert_is_configured(self):
        assert self._emulator is not None, "emulator has not been initialised"
        assert self._assembler is not None, "assembler has not been initialised"
        assert self._expdir is not None, "expected result directory has not been initialised"
        assert self._actdir is not None, "actual output directory has not been initialised"

    def validate(self) -> Result[None, str]:
        self._assert_is_configured()
        if not self._expected_only:
            if not self._emulator_only and not self._assembler.exists():
                return Err(f"assembler {self._assembler} cannot be found")
            if not self._assembler_only and not self._emulator.exists():
                return Err(f"emulator {self._emulator} cannot be found")
        return Ok(None)

    def _log(self, *args, **kwargs):
        if self._logfile is not None:
            with open(self._logfile, "a") as f:
                print(*args, **kwargs, file=f)

        if self._verbose:
            print(*args, **kwargs)

    def _log_when(self, when, *args, **kwargs):
        if when:
            if len(args) == 0:
                self._log(when, **kwargs)
            else:
                self._log(*args, **kwargs)

    def _log_pipe(self, p: subprocess.Popen[bytes]):
        for line in p.stdout.readlines():
            line = line.decode()
            if line.endswith("\n"):
                line = line[:-1]
            self._log(line)



    def _prepare_assembler_test(self, test: Test) -> Result[None, None]:
        """Generate expected assembly, binary and listing"""
        self._log(f"prepare assembly: {test}")

        if self._actual_only:
            return Ok()

        try:
            with subprocess.Popen([f"{self._toolchain_prefix}-as", test, "-o", test._exp_bin]) as p:
                p.wait(1)
                if p.returncode != 0:
                    return Err(None)

            with subprocess.Popen([f"{self._toolchain_prefix}-objcopy", "-O", "binary", test._exp_bin]) as p:
                p.wait(1)
                if p.returncode != 0:
                    return Err(None)

            self._run_listing(test._exp_lst, test._exp_bin)

            return Ok(None)

        except subprocess.TimeoutExpired:
            return Err(None)

    def _dump_process_state(self, process: subprocess.CompletedProcess):
        l = StringWriter()
        write_process_state(process, l)
        self._log(l)

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

    def _log_outcome(self, testtype: str, outcome: str, test: str, is_bad: bool = True):
        self._log(f"\n{Fore.RED if is_bad else Fore.GREEN} {testtype} {outcome} {Fore.RESET}: test {test}")

    def _process_assembler_out(self, test: Test, res: TestResult, p: subprocess.Popen[bytes]) -> AssemblerResult:
        """ Assuming the assembler has been run, process the output and compare with expected """
        p.wait(1)
        self._log("Assembler Output:\n")
        self._log_pipe(p)

        if p.returncode != 0:
            self._log_outcome("ASSEMBLER", "FAILED", test)
            self._dump_process_state(p)
            res.save_process(p)
            return res.with_result(Err(TestError.FAILED))

        if not test._act_bin.exists():
            self._log_outcome("ASSEMBLER", "FAILED", test)
            self._dump_process_state(p)
            if res.log is None: res.log = ""
            res.log += f"\nActual binary file not found: {test._act_bin}"
            res.save_process(p)
            return res.with_result(Err(TestError.FAILED))
        
        self._run_listing(test._act_lst, test._act_bin)
        result = compare_raw_bin(test._exp_bin, test._act_bin)

        if result.is_err():
            diffs = result.value
            self._log_outcome("ASSEMBLER", "INCORRECT", test)
            self._dump_process_state(p)

            res.save_diffs(diffs)
            res.save_process(p)
            return res.with_result(TestError.INCORRECT)

        self._log_outcome("ASSEMBLER", "CORRECT", test, is_bad=False)
        return res.with_result(Ok())

    def _handle_asm_timeout(self, p: subprocess.Popen[bytes], cmd: str, test: Test, res: TestResult) -> AssemblerResult:
        """ Handle a timeout in the assembler """
        self._log("Assembler Output:\n")
        for line in p.stdout.readlines():
            line = line.decode()
            if line.endswith("\n"):
                line = line[:-1]
            self._log(line)
            
        res.save_process(p)
        res.log += "\nTimeout"
        self._log(f"Timeout in test: {cmd}")
        self._log(f"\n{Fore.RED} ASSEMBLER CRASHED {Fore.RESET}: test {test._path}")
        return res.with_result(Err(TestError.FAILED))

    def _run_assembler_test(self, test: Test) -> AssemblerResult:
        """Run (student) assembler on test file `t`"""
        res = AssemblerResult(test)

        if self._emulator_only or self._expected_only:
            return res.with_result(Ok())

        if not STUDENT_MODE and self._verbose:
            cmd = [f"./{self._assembler}", test._path, test._act_bin, "-v"]
        else:
            cmd = [f"./{self._assembler}", test._path, test._act_bin]

        self._log(f'run: {reduce(lambda x, y: f"{x} {y}", cmd, "")}')
        with subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as p:
            try:
                return self._process_assembler_out(test, res, p)
            except subprocess.TimeoutExpired:
                return self._handle_asm_timeout(p, cmd, test, res)

    def _prepare_emulator_test(self, test: Test) -> Result[emulate_test_data.CPU_State, Optional[List[emulate_test_data.ParseError]]]:
        # Expected Emulation (this is expensive, so only done if the log isn't available)
        
        cachefile = test._exp_out
        if cachefile.exists():
            expected_emulator_state = emulate_test_data.parse_out_file(cachefile)
        elif self._actual_only:
            raise Exception(f"Expected emulator output state not found at {cachefile}")
        else:
            return Err(None)

        return expected_emulator_state


    def _process_emulator_test(self, test: Test, res: TestResult, expected_state: emulate_test_data.CPU_State, p: subprocess.Popen[bytes]) -> TestResult:
        p.wait(1)
        self._log("Emulator Output:\n")
        self._log_pipe(p)

        if p.returncode != 0:
            self._log_outcome("EMULATOR", "FAILED", test)
            res.save_process(p)
            self._log(res.log)
            return res.with_result(Err(TestError.FAILED))
        
        if not test._act_out.exists():
            self._log_outcome("EMULATOR", "FAILED", test)
            res.save_process(p)
            res.log += f"\nActual .out file not found: {test._act_out}"
            self._log(res.log)
            return res.with_result(Err(TestError.FAILED))

        # Keeping Result changes local to this file
        parsed_out_state = emulate_test_data.parse_out_file(test._act_out)
        if parsed_out_state.is_err():
            self._log_outcome("EMULATOR", "FAILED", test)
            res.save_process(p)
            res.log += f"\nError Parsing .out File {test._act_out}:\n" + str(parsed_out_state.err()) + "\n"
            return res.with_result(Err(TestError.FAILED))
        parsed_out_state = parsed_out_state.ok()
        
        output_same, differences = expected_state.compare(parsed_out_state, string_keys=True)
        if not output_same:
            self._log_outcome("EMULATOR", "INCORRECT", test)
            res.save_process(p)
            res.save_diffs(differences)
            
            self._log(res.log)
            return res.with_result(Err(TestError.INCORRECT))
        
        self._log_outcome("EMULATOR", "CORRECT", test, is_bad=False)
        return res.with_result(Ok(None))

    def _handle_emu_timeout(self, p: subprocess.Popen[bytes], cmd: List[str], test: Test, res: TestResult) -> TestResult:
        res.save_process(p)
        res.log += "\nTimeout"
        
        self._log(f"Timeout in test: {cmd}")
        self._log_outcome("EMULATOR", "FAILED", test)
        self._log("Emulator Output:\n")
        self._log(res.log)
        return res.with_result(Err(TestError.FAILED))

    def _run_emulator_test(self, test: Test, expected_state: emulate_test_data.CPU_State, use_expected_binary: bool = False) -> TestResult:
        """Run expected and actual emulation on test file `as_file`."""
        res = TestResult(test, TestType.EMULATOR)
        if self._assembler_only or self._expected_only:
            return res.with_result(Ok())
        
        bin_to_run = test._exp_bin if use_expected_binary else test._act_bin
        
        cmd = [f"./{self._emulator}", bin_to_run, test._act_out]
        self._log(f'run: {reduce(lambda x, y: f"{x} {y}", cmd, "")}')

        with subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as p:
            try:
                return self._process_emulator_test(test, res, expected_state, p)
            except subprocess.TimeoutExpired:
                return self._handle_emu_timeout(p, cmd, test, res)

    def _run_test(self, test: Test):
        """Runs expected/actual assembly and emulation tests for a single test in assembly file `t`"""
        self._log(f"Running test {test}")
        with open(test._path) as f:
            is_runnable = "RUNNABLE: False" not in f.readline()

        prep_res = self._prepare_assembler_test(test)
        if prep_res.is_err():
            self._log_outcome("PREPARE ASSEMBLER", "FAILED", test)
            return

        # Run the tests
        if not self._emulator_only:
            try:
                assembler_result = self._run_assembler_test(test)
            except Exception as e:
                assembler_result = \
                    AssemblerResult(test)\
                        .with_result(Err(TestError.FAILED))\
                        .with_log_exception(e)
            self._assembler_results[test] = assembler_result
            assembler_result.write_to_json(test._act_json)
            
        if ((not is_runnable) or self._assembler_only): 
            return
        
        expected_state = self._prepare_emulator_test(test)
        if not expected_state.is_ok():
            
            self._log_outcome("PREPARE EMULATOR", "FAILED", test)
            return


        if not self._assembler_only:
            try:
                em_res = self._run_emulator_test(test, expected_state.value, use_expected_binary=True)
            except Exception as e:
                em_res = \
                    EmulatorResult(test)\
                        .with_result(Err(TestError.FAILED))\
                        .with_log_exception(e)
            self._emulator_results[test] = em_res
            em_res.write_to_json(test._act_json)
    

    def run(self) -> RunnerResult:
        self._assert_is_configured()
        for t in self._test_files:
            test = Test(t, self._testdir, self._actdir, self._expdir)
            try: 
                self._run_test(test)
            except Exception as e:
                self._exceptions[test] = e
                print("Warning: Exception in test", test, e)
        return RunnerResult(self._assembler_results, self._emulator_results, self._exceptions)

