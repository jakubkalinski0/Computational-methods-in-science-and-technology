# === Compiler and Flags ===
# Define the C compiler to use (GNU C Compiler)
CC = gcc
# Define compiler flags:
# -Wall -Wextra: Enable most standard and extra warning messages.
# -O2:          Enable level 2 optimizations.
# -std=c11:     Use the C11 standard.
# -Iinclude:    Specify the directory for header files ('include/').
CFLAGS = -Wall -Wextra -O2 -std=c11 -Iinclude
# Define linker flags:
# -lm: Link against the math library (required for functions like pow, log, exp).
LDFLAGS = -lm

# === Directories ===
# Define directory paths used in the project. NO TRAILING SPACES HERE!
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include
DATA_DIR = data
GP_SCRIPTS_DIR = scripts
PLOT_IMAGES_DIR = plots

# === Files ===
# Find all .c files in the source directory automatically.
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Generate corresponding object file names by replacing 'src/' with 'obj/' and '.c' with '.o'.
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
# Define the name and path of the final executable file.
EXECUTABLE = $(BIN_DIR)/computer_arithmetic

# === Targets ===

# Declare phony targets (targets that don't represent actual files).
# This prevents conflicts if files with these names exist and ensures the rules always run.
.PHONY: all directories run plots clean distclean help

# Default target: Executed when 'make' is run without arguments.
# Depends on 'directories' and the executable file. Builds the entire project.
all: directories $(EXECUTABLE)

# Target to create necessary directories if they don't exist.
# Uses mkdir -p to create parent directories as needed and avoid errors if directories already exist.
# '@' suppresses the command echoing.
directories:
	@echo "Creating necessary directories..."
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(GP_SCRIPTS_DIR) $(PLOT_IMAGES_DIR)

# Rule to link the final executable file from the object files.
# Depends on all object files ($(OBJECTS)).
# Uses the compiler $(CC) with flags $(CFLAGS), input object files ($^), output file ($@), and linker flags $(LDFLAGS).
$(EXECUTABLE): $(OBJECTS)
	@echo "Linking $(EXECUTABLE)..."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Pattern rule to compile a .c file from SRC_DIR into a .o file in OBJ_DIR.
# $<: The first prerequisite (the .c file).
# $@: The target file (the .o file).
# Depends on the corresponding .c file, all header files in INC_DIR, and the Makefile itself.
# This ensures recompilation if source code, headers, or build rules change.
# The warning "mixed implicit and normal rules" might still appear due to listing specific files (headers, Makefile)
# alongside the pattern rule, but it shouldn't prevent compilation.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard $(INC_DIR)/*.h) Makefile
	@echo "Compiling $< -> $@..."
	$(CC) $(CFLAGS) -c $< -o $@

# Target to run the C program.
# First, ensures the program is built by depending on 'all'.
# Then, executes the compiled program, passing the required directory paths defined
# in this Makefile ($(DATA_DIR), $(GP_SCRIPTS_DIR), $(PLOT_IMAGES_DIR)) as command-line arguments.
run: all
	@echo "Running $(EXECUTABLE) -> Data:'$(DATA_DIR)' Scripts:'$(GP_SCRIPTS_DIR)' Images:'$(PLOT_IMAGES_DIR)'"
	@$(EXECUTABLE) $(DATA_DIR) $(GP_SCRIPTS_DIR) $(PLOT_IMAGES_DIR)
	@echo "Program C finished generating data and script files."

# Target to execute gnuplot scripts to generate plots.
# First, depends on 'run' to ensure the C program has run and generated the necessary data and scripts.
# Uses 'find' to locate all .gp files in the GP_SCRIPTS_DIR directory.
# '-exec gnuplot {} \;' executes gnuplot for each found .gp file.
# The gnuplot scripts themselves contain the paths to read data from DATA_DIR and write images to PLOT_IMAGES_DIR.
# Includes 'ls' commands to list the generated files for verification (suppressing errors if no files are found).
plots: run
	@echo "Executing Gnuplot scripts from $(GP_SCRIPTS_DIR)..."
	@find $(GP_SCRIPTS_DIR) -maxdepth 1 -name '*.gp' -print -exec gnuplot {} \;
	@echo "--- Gnuplot execution finished (check for errors above if any). ---"
	# List generated files for confirmation
	@echo "--- Data Files ($(DATA_DIR)): ---"
	@ls -l $(DATA_DIR)/*.csv 2>/dev/null || echo "  No .csv files found."
	@echo "--- Plot Scripts ($(GP_SCRIPTS_DIR)): ---"
	@ls -l $(GP_SCRIPTS_DIR)/*.gp 2>/dev/null || echo "  No .gp scripts found."
	@echo "--- Plot Images ($(PLOT_IMAGES_DIR)): ---"
	@ls -l $(PLOT_IMAGES_DIR)/*.png 2>/dev/null || echo "  No .png files found."


# Target to clean generated files (object files, executable, data, scripts, plots).
# Uses 'rm -f' to force removal and suppress errors if files don't exist.
# Now correctly removes files within directories and the executable itself.
clean:
	@echo "Cleaning generated files..."
	rm -f $(OBJ_DIR)/*.o \
	      $(EXECUTABLE) \
	      $(DATA_DIR)/*.csv \
	      $(GP_SCRIPTS_DIR)/*.gp \
	      $(PLOT_IMAGES_DIR)/*.png
	# Remove potential stray files from the main directory (optional, but can be helpful)
	rm -f *.csv *.gp *.png
	@echo "Clean finished."

# Target to clean everything 'clean' does PLUS the created directories.
# Useful for a complete cleanup back to the state of the source repository.
# Depends on 'clean' target. Uses 'rm -rf' to remove directories recursively and forcefully.
distclean: clean
	@echo "Cleaning directories..."
	# Remove all directories created by the 'directories' target
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(GP_SCRIPTS_DIR) $(PLOT_IMAGES_DIR)
	@echo "Distclean finished."

# Target to display help information about the Makefile usage.
# Uses '@echo' to print messages to the console.
help:
	@echo "Makefile for $(EXECUTABLE)"
	@echo ""
	@echo "Usage:"
	@echo "  make all       - Compile the project (default)."
	@echo "  make run       - Compile and run the C program. The Makefile automatically passes the correct"
	@echo "                   directory paths ($(DATA_DIR), $(GP_SCRIPTS_DIR), $(PLOT_IMAGES_DIR)) to the executable."
	@echo "                   Generates data in '$(DATA_DIR)' and Gnuplot scripts in '$(GP_SCRIPTS_DIR)'."
	@echo "  make plots     - Compile, run the C program (as in 'make run'), then execute the generated"
	@echo "                   Gnuplot scripts from '$(GP_SCRIPTS_DIR)' to create plot images in '$(PLOT_IMAGES_DIR)'."
	@echo "  make clean     - Remove generated files (.o, executable, .csv, .gp, .png)."
	@echo "  make distclean - Remove generated files AND directories (obj, bin, data, $(GP_SCRIPTS_DIR), $(PLOT_IMAGES_DIR))."
	@echo "  make help      - Show this help message."