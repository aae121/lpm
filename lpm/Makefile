# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lncurses -lpthread

# Directories
SRC_DIR = src
BUILD_DIR = build

# Target executable
TARGET = lpm

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target: build the executable
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run the executable
run: $(TARGET)
	./$(TARGET)

# Run in interactive mode
interactive: $(TARGET)
	./$(TARGET) --interactive

# Run with list view
list: $(TARGET)
	./$(TARGET) --list --detailed

# Show process tree
tree: $(TARGET)
	./$(TARGET) --tree

# Show top processes
top: $(TARGET)
	./$(TARGET) --top 20

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Install (requires root)
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Phony targets
.PHONY: all run interactive list tree top clean install uninstall
