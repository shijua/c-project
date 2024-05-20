import test.run_tests as run_tests
import testserver as server
import sys

DEFAULT_ARGS = {
    "--expdir":  server.TEST_EXPECTED_DIR, 
    "--actdir":  server.TEST_ACTUAL_DIR, 
    "--testdir": server.TEST_CASES_DIR,
    "--outdir":  server.TEST_ACTUAL_DIR,
    "--outfile": server.OUT_ALL,
    "-a":        server.ASSEMBLER_PATH,
    "-e":        server.EMULATOR_PATH
    }

def main_try():
    args = sys.argv[1:]
    initial_args = []
            
    for (arg, val) in DEFAULT_ARGS.items():
        if arg not in args:
            initial_args += [arg, val]
    
    run_tests.run_tests_with_args(initial_args + args)
    
def main():
    main_try()

if __name__ == '__main__':
    main()
