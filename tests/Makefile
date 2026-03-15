# Wrapper Makefile - configures and builds tests via CMake.
# Usage (from repo root): make -f tests/Makefile
# To use a specific compiler:  CXX=g++-14 make -f tests/Makefile
# For coverage:  make -f tests/Makefile coverage

BUILD_DIR          ?= tests/build/cmake
COVERAGE_BUILD_DIR ?= tests/build/coverage
COVERAGE_REPORT    ?= tests/build/coverage-report
TMPFILE             = $(BUILD_DIR)/runtests.out

ifdef CXX
	CMAKE_COMPILER := -DCMAKE_CXX_COMPILER=$(CXX)
	# Detect gcov tool matching the compiler
	ifneq ($(findstring clang,$(notdir $(CXX))),)
		GCOV_TOOL ?= llvm-cov gcov
	else ifneq ($(findstring g++,$(notdir $(CXX))),)
		GCOV_TOOL ?= $(subst g++,gcov,$(notdir $(CXX)))
	endif
endif
GCOV_TOOL ?= gcov

.PHONY: all tests coverage clean

all: tests

tests: $(BUILD_DIR)/CMakeCache.txt
	@cmake --build $(BUILD_DIR) --target runtests
		@$(BUILD_DIR)/runtests --out=$(TMPFILE) && \
		echo "[√] Unit tests passed: $(notdir $(CURDIR))" || \
		(cat $(TMPFILE); exit 1)

coverage: $(COVERAGE_BUILD_DIR)/CMakeCache.txt
	@cmake --build $(COVERAGE_BUILD_DIR) --target runtests
	@$(COVERAGE_BUILD_DIR)/runtests --no-intro
	@mkdir -p $(COVERAGE_REPORT)
	@gcovr --root . --filter util/ \
		--gcov-executable "$(GCOV_TOOL)" \
		--print-summary \
		--html-details $(COVERAGE_REPORT)/index.html \
		$(COVERAGE_BUILD_DIR)
	@echo "HTML report: $(COVERAGE_REPORT)/index.html"

$(BUILD_DIR)/CMakeCache.txt:
	@cmake -S . -B $(BUILD_DIR) \
		-DCPPUTIL_BUILD_TESTS=ON \
		$(CMAKE_COMPILER)

$(COVERAGE_BUILD_DIR)/CMakeCache.txt:
	@cmake -S . -B $(COVERAGE_BUILD_DIR) \
		-DCPPUTIL_BUILD_TESTS=ON \
		-DCPPUTIL_COVERAGE=ON \
		$(CMAKE_COMPILER)

clean:
	@rm -rf $(BUILD_DIR) $(COVERAGE_BUILD_DIR) $(COVERAGE_REPORT)
