# Project name
PROJECT := bnl


# Directories
SRC := src
BUILD := build
BIN := bin

# Directories management
MKDIR := mkdir -pv
RM := rm -rfv
CP := cp -ruv


# Compiler
FLAGS = -ansi -Wall -Wextra -Wpedantic


# Main target
TARGET := $(BIN)/$(PROJECT)

# Targets
.PHONY: release debug clean

release: FLAGS += -O3
release: $(TARGET)

debug: FLAGS += -Og -ggdb3
debug: $(TARGET)

clean:
	$(RM) $(BUILD) $(BIN)


# Directories creation
.PRECIOUS: $(BUILD)/ $(BUILD)%/ $(BIN)/
.SECONDEXPANSION:

$(BUILD)/:
	$(MKDIR) $@

$(BUILD)%/:
	$(MKDIR) $@

$(BIN)/:
	$(MKDIR) $@

# CXX files
CXXSOURCES := $(shell find $(SRC) -type f -name *.cpp)
CXXOBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CXXSOURCES:.cpp=.o))


# Compilation
$(TARGET): $(CXXOBJECTS) | $$(@D)/
	$(CXX) -o $@ $^

$(BUILD)/%.o: $(SRC)/%.cpp | $$(@D)/
	$(CXX) $(FLAGS) -o $@ -c $<
