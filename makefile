# ---- Project layout ----
#   include/   all .h files
#   src/       all .cpp files (including main.cpp)
#   build/     object files (created automatically)

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -Iinclude
# Add -Werror above if you want warnings to fail the build (useful in CI)

SRC_DIR   := src
BUILD_DIR := build
TARGET    := tividy

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all run asan clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# -MMD -MP writes a .d file per object so editing a header rebuilds only what includes it
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

# Memory-error / leak / undefined-behaviour check (builds everything in one go)
asan:
	$(CXX) $(CXXFLAGS) -fsanitize=address,undefined -o $(TARGET)_asan $(SRCS)
	./$(TARGET)_asan

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TARGET)_asan

-include $(DEPS)
