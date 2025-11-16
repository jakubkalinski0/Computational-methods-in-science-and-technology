# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -g
LDFLAGS = -lm # Link math library
PYTHON = python3 # Definicja interpretera Python

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
PLOTS_DIR = plots
SCRIPT_DIR = scripts
LATEX_DIR = latex_out
HEATMAP_PLOTS_DIR = $(PLOTS_DIR)/heatmaps # Dedykowany katalog dla heatmap

# Source and object files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/matrix_operations.c \
          $(SRC_DIR)/problem_definition.c \
          $(SRC_DIR)/jacobi_solver.c \
          $(SRC_DIR)/spectral_radius.c \
          $(SRC_DIR)/file_io.c

OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/jacobi_lab_app

# --- Generated Files from C program ---
RESULTS_CSV = $(DATA_DIR)/jacobi_solver_results.csv
SPECTRAL_CSV = $(DATA_DIR)/spectral_radii_data.csv # Zgodnie z kodem C
RESULTS_GP_PATTERN = $(SCRIPT_DIR)/plot_jacobi_solver_results_N*.gp # Wzorzec dla skryptów GP
SPECTRAL_GP = $(SCRIPT_DIR)/plot_spectral_radii_data.gp
LATEX_SUMMARY_PATTERN = $(LATEX_DIR)/latex_jacobi_summary_N*.tex
LATEX_SPECTRAL = $(LATEX_DIR)/latex_table_spectral_radii.tex

# --- Python script for heatmaps ---
PYTHON_HEATMAP_SCRIPT = generate_heatmaps.py # Zakładamy, że jest w głównym katalogu

# --- Generated Heatmap Plot Files (Przykładowe nazwy, skrypt Pythona je tworzy) ---
# Dodaj tutaj nazwy plików PNG, które generuje Twój skrypt Pythona, aby Makefile mógł je śledzić
# To są tylko przykłady, dostosuj do faktycznych nazw plików generowanych przez generate_heatmaps.py
HEATMAP_ITER_X0Z_INCR = $(HEATMAP_PLOTS_DIR)/heatmap_iterations_x0zeros_incr.png
HEATMAP_ERR_X0Z_INCR = $(HEATMAP_PLOTS_DIR)/heatmap_abs_error_x0zeros_incr.png
HEATMAP_ITER_X0AH_RES = $(HEATMAP_PLOTS_DIR)/heatmap_iterations_x0althigh_resid.png
HEATMAP_ERR_X0AH_RES = $(HEATMAP_PLOTS_DIR)/heatmap_abs_error_x0althigh_resid.png

ALL_HEATMAP_PLOTS = $(HEATMAP_ITER_X0Z_INCR) $(HEATMAP_ERR_X0Z_INCR) $(HEATMAP_ITER_X0AH_RES) $(HEATMAP_ERR_X0AH_RES)


# Default rule: build the executable
all: directories $(EXECUTABLE)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR) $(LATEX_DIR) $(HEATMAP_PLOTS_DIR)

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard include/*.h)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the program
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Target to run the C program
# Generuje CSV, skrypty Gnuplot, tabele LaTeX
run: $(EXECUTABLE)
	@echo "Running C program $(EXECUTABLE) to generate data, scripts, and LaTeX tables..."
	./$(EXECUTABLE)
	@echo "C program finished. Check $(DATA_DIR), $(SCRIPT_DIR), and $(LATEX_DIR)."

# Target to generate line plots using Gnuplot.
# Zależność od CSV, ponieważ skrypty Gnuplot ich używają.
generate_line_plots: $(wildcard $(SCRIPT_DIR)/*.gp) $(RESULTS_CSV) $(SPECTRAL_CSV)
	@echo "Generating line plots using Gnuplot..."
	@for gp_script in $(wildcard $(SCRIPT_DIR)/*.gp); do \
		echo "  Running $$gp_script"; \
		gnuplot $$gp_script || echo "Warning: Error running $$gp_script"; \
	done
	@echo "Gnuplot line plot processing finished. Check $(PLOTS_DIR) and its subdirectories."

# Target to generate heatmap plots using the Python script.
# Zależność od skryptu Pythona i pliku CSV z wynikami z programu C.
generate_heatmaps: $(PYTHON_HEATMAP_SCRIPT) $(RESULTS_CSV)
	@echo "Generating heatmap plots using Python script..."
	$(PYTHON) $(PYTHON_HEATMAP_SCRIPT)
	@echo "Python heatmap generation finished. Check $(HEATMAP_PLOTS_DIR)."

# Główny cel "plots": generuje wszystkie wykresy (liniowe i heatmapy)
# Najpierw uruchamia program C, jeśli dane nie istnieją lub są przestarzałe
plots: run generate_line_plots generate_heatmaps

# Główny cel "analysis": uruchamia program C, generuje wszystkie wykresy i podsumowuje
analysis: run generate_line_plots generate_heatmaps
	@echo ""
	@echo "--- Full Analysis Summary ---"
	@echo "Generated CSV data files in: $(DATA_DIR)"
	@echo "Generated Gnuplot scripts in: $(SCRIPT_DIR)"
	@echo "Generated LaTeX tables in: $(LATEX_DIR)"
	@echo "Generated Line Plots (Gnuplot) in: $(PLOTS_DIR) (and subdirectories like plots/N_100/)"
	@echo "Generated Heatmap Plots (Python) in: $(HEATMAP_PLOTS_DIR)"
	@echo "---------------------------------"

# --- Cleaning ---
clean:
	@echo "Cleaning temporary files and generated outputs..."
	rm -f $(OBJ_DIR)/*.o
	rm -f $(EXECUTABLE)
	rm -f $(DATA_DIR)/*.csv
	rm -rf $(PLOTS_DIR)/* # Usuwa wszystko z plots, w tym podkatalogi N_xxx i heatmaps
	rm -f $(SCRIPT_DIR)/*.gp
	rm -f $(LATEX_DIR)/*.tex
	@echo "Cleaning finished."

distclean: clean
	@echo "Removing all generated directories (except src/ and include/)..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR) $(LATEX_DIR)
	@echo "Deep cleaning finished."

# --- Help ---
help:
	@echo "Available commands:"
	@echo "  make all                - Compiles the C program (default)"
	@echo "  make run                - Compiles and runs the C program to generate data, Gnuplot scripts, and LaTeX tables"
	@echo "  make generate_line_plots  - Runs Gnuplot to generate line plots (requires output from 'make run')"
	@echo "  make generate_heatmaps  - Runs Python script to generate heatmap plots (requires CSV from 'make run')"
	@echo "  make plots              - Runs C program, then Gnuplot (line plots), then Python (heatmaps)"
	@echo "  make analysis           - Same as 'make plots' and provides a summary"
	@echo "  make clean              - Removes object files, executable, and all generated output files/directories"
	@echo "  make distclean          - Performs 'clean' and removes top-level generated directories"
	@echo "  make help               - Displays this help message"

# Declare phony targets
.PHONY: all directories run generate_line_plots generate_heatmaps plots analysis clean distclean help