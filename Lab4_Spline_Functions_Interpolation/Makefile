# Compiler and flags
CC = gcc
# Flags: -Wall (all warnings), -Wextra (more warnings), -O3 (optimization), -Iinclude (find headers), -g (debugging symbols)
# -Wno-unused-parameter: Suppress warnings about unused deriv_a/b in spline calls when BC doesn't need them.
# -lm: Link math library
CFLAGS = -Wall -Wextra -O3 -Iinclude -g -Wno-unused-parameter
LDFLAGS = -lm

# Directories (using variables for consistency)
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
PLOTS_DIR = plots
SCRIPT_DIR = scripts
INCLUDE_DIR = include # Define include directory

# Source and object files
# Automatically find all .c files in SRC_DIR
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Replace src/%.c with obj/%.o for each source file
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
# Define the executable path
EXECUTABLE = $(BIN_DIR)/spline_interpolator # Changed executable name

# Header files (for dependency tracking)
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

# Default rule: Build the executable
all: directories $(EXECUTABLE)

# Rule to create necessary directories
# Use $@ to refer to the target name (directories)
directories:
	@echo "Creating directories..."
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR)

# --- Pattern Rule for Compiling Object Files ---
# This rule tells make how to create any .o file in OBJ_DIR
# from its corresponding .c file in SRC_DIR.
# $<: The first prerequisite (the .c file).
# $@: The target file (the .o file).
# It automatically handles dependencies on the .c file.
# We add $(HEADERS) as prerequisites to ensure recompilation if any header changes.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@

# Link the program
# $^: All prerequisites (all object files).
$(EXECUTABLE): $(OBJECTS)
	@echo "Linking object files -> $@"
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run the program
run: all
	@echo "Running program $(EXECUTABLE)..."
	@./$(EXECUTABLE)
	@echo "Program finished."

# Generate plots (depends on running the program first to create data/scripts)
plots: run
	@echo "Generating plots using Gnuplot..."
	@gnuplot "$(SCRIPT_DIR)/plot_spline_interpolation.gp" && echo "Generated individual plots." || echo "Gnuplot failed for individual plots."
	@gnuplot "$(SCRIPT_DIR)/plot_spline_errors.gp" && echo "Generated error summary plot." || echo "Gnuplot failed for error plot."
	@echo "--- Generated Files ---"
	@echo "CSV error files in $(DATA_DIR):"
	@ls -1 $(DATA_DIR)/*_errors.csv 2>/dev/null || echo "  (No CSV files found)"
	@echo "Plot files in $(PLOTS_DIR):"
	@ls -1 $(PLOTS_DIR)/*.png 2>/dev/null || echo "  (No PNG files found)"
	@echo "-----------------------"
	@echo "Plot generation attempt finished."


# Clean temporary files (object files, executable) and generated output
clean:
	@echo "Cleaning temporary and generated files..."
	rm -f $(OBJ_DIR)/*.o
	rm -f $(EXECUTABLE)
	rm -f $(DATA_DIR)/*.dat $(DATA_DIR)/*.csv
	rm -f $(PLOTS_DIR)/*.png
	rm -f $(SCRIPT_DIR)/*.gp
	@echo "Cleaning finished."

# Deep clean (remove generated directories too)
distclean: clean
	@echo "Removing all generated directories..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR)
	@echo "Deep cleaning finished."

# Help message
help:
	@echo "Available commands:"
	@echo "  make all       - Compiles the program '$(EXECUTABLE)' (default)"
	@echo "  make run       - Compiles and runs the program"
	@echo "  make plots     - Compiles, runs the program, and generates plots and CSV files"
	@echo "  make clean     - Removes object files, executable, scripts, and generated data/plots"
	@echo "  make distclean - Performs 'clean' and removes generated directories"
	@echo "  make help      - Displays this help message"

# Declare phony targets (targets that are not files)
.PHONY: all directories run plots clean distclean help