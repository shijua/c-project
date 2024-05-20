# Test Server

## TLDR:
You have a choice of using your system python environment, or using `venv` (an isolated environment)

### System Python:
Ensure dependencies with:
```
$ python3 -m pip install -r requirements.txt
```
Then you can run with `./run <args>`, or `python3 run.py <args>`.

### Pipenv:
The install script `./install` will set up the `venv` with:
```
$ python3 -m venv .venv
```
Then, it will activate the `venv` and install the dependencies with
```
$ pip3 install -r requirements.txt
```
You can then run the tests with `./run <args>` or by running:
```
$ source ./.venv/bin/activate
$ python3 run.py <args>
```


### Running Tests:
- To run specific tests (or folders), use `./run <path_to_test/folder>`.
- Use `-pf` to `p`retty print `f`ailures only.
- Use `make clean` to remove old test outputs.
- Put your `assemble` and `emulate` executables in `testserver/solution`, as the script looks for them here by default. 
    You can change where it looks for them with `-a <assembler_path>` and `-e <emulator_path>`.

## Requirements
- Python 3.10.6 (preferred); at least python 3.8+.
- The libraries needed are specified in `requirements.txt`. 
    The key dependencies are:
  - `result`
  - `colorama`
  - `flask` (for the server)

## Running the Server

Not yet done, sorry!


## Running the Script

You can use `./run` as an entrypoint to the script.
See `./run -h` for the arguments and flags.

The `Makefile` has a `clean` that deletes all actual result files *except* for the output `.json`s.
`make test` will run `./run -pf`.

## Test Outputs

The results of running your solution will be saved in `test/actual_results`.
These will consist of `.bin` and `.out` files for the results of your assembler and emulator (respectively).
If you have an `aarch64-none-elf` toolchain installed, the script will also produce listings of the `.bin` files your assembler produced.
By default, it tries to find `aarch64-none-elf` on the system `PATH`, but you can tell it where to look for the binaries with `--toolchain <path_to_bin/>`.

If any `.bin` or `.out` is incorrect, the differences between the expected and actual results are saved in a corresponding `.diff` file.

# Errors

## On Installation


For this error when installing: 
```
.AttributeError: module 'virtualenv.create.via_global_ref.builtin.cpython.mac_os' has no attribute 'CPython2macOsFramework'
```
See this [post](https://techoverflow.net/2022/02/03/how-to-fix-tox-attributeerror-module-virtualenv-create-via_global_ref-builtin-cpython-mac_os-has-no-attribute-cpython2macosarmframework/).
 

