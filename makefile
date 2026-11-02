# TIVIDY TUI - GNU Make build (C++11)
# Build:       make
# Run default: make run
# Run script:  make run WORKFLOW=workflows/scenario_a_game_patch.txt
# Clean:       make clean

CXX ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra -pedantic
CXXFLAGS += -std=c++11

# Keep the source-root path in the binary so the bundled demo can be found
# regardless of the directory from which the executable is launched.
CPPFLAGS += -Iinclude -Itui/include -DTIVIDY_SOURCE_DIR='"$(CURDIR)"'

# Debian/Ubuntu ncurses development package provides these link libraries.
NCURSES_LIBS ?= -lncurses -ltinfo
THREAD_LIBS ?= -pthread
LDLIBS += $(NCURSES_LIBS) $(THREAD_LIBS)

BUILD_DIR := build
TARGET := main

ENGINE_SOURCES := $(wildcard src/*.cpp)
TUI_SOURCES := $(wildcard tui/src/*.cpp)
SOURCES := $(ENGINE_SOURCES) $(TUI_SOURCES)
OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPFILES := $(OBJECTS:.o=.d)

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPFILES)

run: $(TARGET)
ifdef WORKFLOW
	./$(TARGET) "$(WORKFLOW)"
else
	./$(TARGET)
endif

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

help:
	@echo "make                                      Build the TIVIDY TUI"
	@echo "make run                                  Run the bundled demo workflow"
	@echo "make run WORKFLOW=path/to/workflow.txt   Run a specific workflow"
	@echo "make clean                               Remove build output"
