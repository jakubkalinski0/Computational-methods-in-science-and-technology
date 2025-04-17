# Compiler and flags
CC = gcc
# Flags: -Wall (all warnings), -Wextra (more warnings), -O3 (optimization), -Iinclude (find headers), -g (debugging symbols)
CFLAGS = -Wall -Wextra -O3 -Iinclude -g -Wno-unused-parameter #-Wmaybe-uninitialized # Removed maybe-uninitialized as it was addressed
LDFLAGS = -lm # Link math library

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
PLOTS_DIR = plots
SCRIPT_DIR = scripts

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/interpolation

# Default rule
all: directories $(EXECUTABLE)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR)

# Compile object files - Ensure header dependencies are checked
$(OBJ_DIR)/error.o: $(SRC_DIR)/error.c include/error.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/fileio.o: $(SRC_DIR)/fileio.c include/fileio.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/function.o: $(SRC_DIR)/function.c include/function.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/interpolation.o: $(SRC_DIR)/interpolation.c include/interpolation.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/nodes.o: $(SRC_DIR)/nodes.c include/nodes.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
# Explicit rule for main.o to show all dependencies clearly
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c include/common.h include/function.h include/nodes.h include/interpolation.h include/error.h include/fileio.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link the program
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run the program
run: all
	@echo "Running program $(EXECUTABLE)..."
	@./$(EXECUTABLE)
	@echo "Program finished."

# Generate plots
plots: run
	@echo "Generating plots using Gnuplot..."
	@gnuplot "$(SCRIPT_DIR)/plot_interpolation.gp"
	@gnuplot "$(SCRIPT_DIR)/plot_errors.gp"
	@echo "Generated CSV files in directory $(DATA_DIR):"
	@ls -l $(DATA_DIR)/*_errors.csv
	@echo "Generated PNG files in directory $(PLOTS_DIR):"
	@ls -l $(PLOTS_DIR)/*.png
	@echo "Plots generated."

# Clean temporary files
clean:
	@echo "Cleaning temporary files..."
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
	@echo "  make all       - Compiles the program (default)"
	@echo "  make run       - Compiles and runs the program"
	@echo "  make plots     - Compiles, runs the program, and generates plots and CSV files"
	@echo "  make clean     - Removes object files, executable, scripts, and generated data/plots"
	@echo "  make distclean - Performs 'clean' and removes generated directories"
	@echo "  make help      - Displays this help message"

# Declare phony targets (targets that are not files)
.PHONY: all directories run plots clean distclean help