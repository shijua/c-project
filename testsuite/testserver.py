
################################################ imports #################################################
#                                                                                                        #  
#                                                                                                        #   
#                                                                                                        #   
##########################################################################################################

#pip install Flask
from typing import Dict, List, Optional, Tuple
from flask import Flask, render_template
# from helper_functions import *

import json
import flask

from result import Result
import helper_functions
import test.run_tests as run_tests
import test.test_runner as test_runner
from test.test_runner import Test, TestType
from pathlib import Path

app = Flask(__name__, template_folder='templates')

TEST_CASES_DIR = './test/test_cases/'
TEST_CASES_DIR_PATH = Path(TEST_CASES_DIR)

TEST_EXPECTED_DIR = './test/expected_results/'
TEST_ACTUAL_DIR = './test/actual_results/'
OUT_JSON_DIR = './test/actual_results'
OUT_ASSEMBLE_ALL = f'{OUT_JSON_DIR}/assembler_all.json'
OUT_EMULATE_ALL = f'{OUT_JSON_DIR}/emulator_all.json'
OUT_ALL = f'{OUT_JSON_DIR}/results.json'
ASSEMBLER_PATH = './solution/assemble'
EMULATOR_PATH = './solution/emulate'

STANDARD_ARGS = [
    "--expdir", TEST_EXPECTED_DIR, 
    "--actdir", TEST_ACTUAL_DIR, 
    "--testdir", TEST_CASES_DIR,
    "--outdir", TEST_ACTUAL_DIR,
    "-a", ASSEMBLER_PATH,
    "-e", EMULATOR_PATH,
    ]

def out_json_file(fname: str) -> str:
    return f"{OUT_JSON_DIR}/{fname}.json"

def out_assemble_file(fname: str) -> str:
    return out_json_file(fname + "_asm")

def out_emulate_file(fname: str) -> str:
    return out_json_file(fname + "_emu")

###################################################################################################
# Template Pages

#homepage
@app.route('/')
def index():
    return render_template('index.html')

#page to find specific test to assemble
@app.route('/find-test-<fname>')
def find_test(fname):
    root = Path(TEST_CASES_DIR)
    names = [f.relative_to(root) for f in root.rglob("*.s")]
    
    return render_template('find_test.html', names = names, fname = fname, title = 'List of Tests')


def count_results(res: Dict[str, Result[None, run_tests.TestError]]):
    """ Count the number of correct, incorrect, and failed tests in a dictionary of results """
    vals = list(res.values())
    res = {
        "vals": vals,
        "num_cor" : vals.count(test_runner.CORRECT_STR),
        "num_inc" : vals.count(test_runner.INCORRECT_STR),
        "num_fail": vals.count(test_runner.FAIL_STR),
        "files": list(res.keys())
    }
    return res

###################################################################################################
# Run All Tests

def run_all_common(json_path: str, args: List[str], 
    sub_type: str, results_name: str, template_name: str):
    """ Run all tests of a given type, and return the results in a template """
    out_json = Path(json_path)
    if not out_json.exists():
        run_tests.run_tests_with_args(STANDARD_ARGS + args)
    with open(out_json, 'r') as f:
        res = json.load(f)

    res_old = res
    res = res[sub_type]
    res_summary = res_old[f"{sub_type}_summary"]
    
    files, results = zip(*res.items())
    
    return render_template(
        template_name,
        results = results,
        files = files,
        num_cor = res_summary["correct"],
        num_inc = res_summary["failed"],
        num_fail = res_summary["incorrect"],
        total_tests= res_summary["total"],
        title = results_name,
        perpage = 100)

#page to assemble all tests
@app.route('/assembler-results')
def assemble_all_tests():
    return run_all_common(
        OUT_ASSEMBLE_ALL,
        ["-A", "--outfile", OUT_ASSEMBLE_ALL],
        run_tests.JSONPrinter.ASSEMBLER_STR,
        "Assembler Results",
        'assemble_all_tests.html'
    )

#page to emulate all tests
@app.route('/emulator-results')
def emulate_all_tests():
    app.logger.info("Emulating all tests")
    return run_all_common(
        OUT_EMULATE_ALL, 
        ["-E", "--outfile", OUT_EMULATE_ALL],
        run_tests.JSONPrinter.EMULATOR_STR, 
        "Emulator Results", 
        'emulate_all_tests.html'
    )

###################################################################################################
# Single Tests

def nice_bin_str(bin_str: bytes) -> List[str]:
    """ Convert a binary string to a nice string representation """
    lines = []
    for i in range(0, len(bin_str), 4):
        curr_word = bin_str[i:i+4]
        lines.append(f"{i:04x}:    {curr_word.hex()}")
    
    return lines

def nice_out_str(out_str: str) -> List[str]:
    return out_str.splitlines()

_test_type_to_template_args = {
    TestType.ASSEMBLER: { 
        "test_type": run_tests.JSONPrinter.ASSEMBLER_STR, 
        "test_type_name": "Assembler",
        "template_name": "assemble_individual.html",
        "title_suffix": "Assembler",
        "args": ["-A"],
        "read_mode": "rb"
    },
    TestType.EMULATOR: {
        "test_type": run_tests.JSONPrinter.EMULATOR_STR,
        "test_type_name": "Emulator",
        "template_name": "emulate_individual.html",
        "title_suffix": "Emulator",
        "args": ["-E"],
        "read_mode": "r"
    }
}

def test_type_to_template_args(test_type: TestType) -> Dict[str, str]:
    return _test_type_to_template_args[test_type]

def test_type_get_out_files(test_type: TestType, test: Test) -> Tuple[Path, Path]:
    if test_type == TestType.ASSEMBLER:
        return (test._act_bin, test._exp_bin)
    elif test_type == TestType.EMULATOR:
        return (test._act_out, test._exp_out)
    else:
        raise ValueError(f"Invalid test type: {test_type}")

def run_single_common(test_file_name: str, test_type: TestType):
    
    get_args = test_type_to_template_args(test_type)
    sub_type = get_args["test_type"]
    template_name = get_args["template_name"]
    title_suffix = get_args["title_suffix"]
    args = get_args["args"]
    read_mode = get_args["read_mode"]

    
    test_file: Path = TEST_CASES_DIR_PATH / Path(test_file_name).with_suffix(".s")
    test = test_runner.Test(test_file, Path(TEST_CASES_DIR), Path(TEST_ACTUAL_DIR), Path(TEST_EXPECTED_DIR))
    act_file, exp_file = test_type_get_out_files(test_type, test)
    
    if not exp_file.exists():
        raise ValueError(f"Expected file {exp_file} does not exist")
    
    out_json: Path = test._act_json

    if any([not f.exists() for f in [act_file, test._act_json]]):
        runner = run_tests.Runner()
        run_tests.run_tests_with_args(
            STANDARD_ARGS + args + [str(test_file)], #+ ["--outfile", str(out_json)]
            with_runner=runner
        )
    
    with open(out_json, 'r') as f: 
        res = json.load(f)
        
    result_type: str = res[sub_type]["result"]
    
    with open(act_file, read_mode) as f: 
        act_out = f.read()
    with open(exp_file, read_mode) as f: 
        exp_out = f.read()
        
    if test_type == TestType.ASSEMBLER:
        act_nice = nice_bin_str(act_out)
        exp_nice = nice_bin_str(exp_out)
    else:
        act_nice = nice_out_str(act_out)
        exp_nice = nice_out_str(exp_out)
    
    if result_type == test_runner.FAIL_STR:
        return render_template(
            template_name, 
            execution_error = res[0], 
            result = res[1], 
            fname=str(test_file), 
            title = test_file_name + f' {title_suffix} Execution Error')
    else:
        return render_template(
            template_name, 
            results = result_type,
            files = test_file, 
            actual_output = act_nice, 
            expected_output = exp_nice, 
            title = test_file_name + f' {title_suffix} Results')


#page to assemble a single tests
@app.route('/assembler-results/single')
def assemble_individual():
    fname = flask.request.args.get("test_file")
    app.logger.info(f"Retrieve individual assembler test: {fname}")
    tmpl = run_single_common(fname, TestType.ASSEMBLER)
    return tmpl

# page to emulate a specific test
@app.route('/emulator-results/single')
def emulate_individual():
    fname = flask.request.args.get("test_file")
    app.logger.info("Emulating individual test: " + fname)
    
    tmpl = run_single_common(fname, TestType.EMULATOR)
    return tmpl    

###################################################################################################
# Main

def main():
    app.run(debug=True)

if __name__ == "__main__":
    main()
