TEST_DIR ?= ./test
TEST_ACT_DIR = $(TEST_DIR)/actual_results

.PHONY: all test clean server

all: clean
	./run -pf

server:
	. ./.venv/bin/activate
	python3 -m flask --app testserver.py run

TO_CLEAN = $(filter-out %.json, $(wildcard $(TEST_ACT_DIR)/*))
clean:
	$(RM) -rf $(TO_CLEAN)

