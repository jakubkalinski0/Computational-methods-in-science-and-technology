# Compiler and flags
CC = gcc
# Flags: -Wall (all warnings), -Wextra (more warnings), -O3 (optimization), -Iinclude (find headers), -g (debugging symbols)
CFLAGS = -Wall -Wextra -O3 -Iinclude -g -Wno-unused-parameter
LDFLAGS = -lm # Link math library

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
PLOTS_DIR = plots
SCRIPT_DIR = scripts

# Source and object files
# List all .c files EXCEPT the old main.c for interpolation if it exists
# Use the new main.c
SOURCES = $(SRC_DIR)/approximation.c $(SRC_DIR)/error.c $(SRC_DIR)/fileio.c $(SRC_DIR)/function.c $(SRC_DIR)/linear_algebra.c $(SRC_DIR)/nodes.c $(SRC_DIR)/main.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/approximation_app # Changed executable name

# Default rule
all: directories $(EXECUTABLE)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR)

# Compile object files - Add dependencies for new files
$(OBJ_DIR)/approximation.o: $(SRC_DIR)/approximation.c include/approximation.h include/linear_algebra.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/error.o: $(SRC_DIR)/error.c include/error.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/fileio.o: $(SRC_DIR)/fileio.c include/fileio.h include/common.h include/nodes.h # Added nodes.h dependency? check usage
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/function.o: $(SRC_DIR)/function.c include/function.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/linear_algebra.o: $(SRC_DIR)/linear_algebra.c include/linear_algebra.h
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/nodes.o: $(SRC_DIR)/nodes.c include/nodes.h include/common.h
	$(CC) $(CFLAGS) -c $< -o $@
# Rule for main.o
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c include/common.h include/function.h include/nodes.h include/approximation.h include/error.h include/fileio.h include/linear_algebra.h
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
	@gnuplot "$(SCRIPT_DIR)/plot_approximations.gp"
	@gnuplot "$(SCRIPT_DIR)/plot_approx_errors.gp"
	@echo "Generated CSV files in directory $(DATA_DIR):"
	@ls -l $(DATA_DIR)/*.csv
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
	@echo "  make all       - Compiles the approximation program (default)"
	@echo "  make run       - Compiles and runs the program"
	@echo "  make plots     - Compiles, runs the program, and generates plots and CSV files"
	@echo "  make clean     - Removes object files, executable, scripts, and generated data/plots"
	@echo "  make distclean - Performs 'clean' and removes generated directories"
	@echo "  make help      - Displays this help message"

# Declare phony targets
.PHONY: all directories run plots clean distclean help