# Wrapper Makefile - configures and builds tests via CMake.
# Usage (from repo root): make -f tests/Makefile
# To use a specific compiler:  CXX=g++-14 make -f tests/Makefile

BUILD_DIR ?= tests/build/cmake
TMPFILE    = $(BUILD_DIR)/runtests.out

ifdef CXX
	CMAKE_COMPILER := -DCMAKE_CXX_COMPILER=$(CXX)
endif

.PHONY: all tests clean

all: tests

tests: $(BUILD_DIR)/CMakeCache.txt
	@cmake --build $(BUILD_DIR) --target runtests
		@$(BUILD_DIR)/runtests --out=$(TMPFILE) && \
		echo "[√] Unit tests passed: $(notdir $(CURDIR))" || \
		(cat $(TMPFILE); exit 1)

$(BUILD_DIR)/CMakeCache.txt:
	@cmake -S . -B $(BUILD_DIR) \
		-DCPPUTIL_BUILD_TESTS=ON \
		$(CMAKE_COMPILER)

clean:
	@rm -rf $(BUILD_DIR)
