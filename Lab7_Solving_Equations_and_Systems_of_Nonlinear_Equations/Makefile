# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -g
LDFLAGS = -lm
PYTHON = python3

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
PLOTS_DIR = plots
SCRIPT_DIR = scripts
TABLE_DIR = tables
LATEX_FORMAT_TABLE_DIR = tables_latex_format

# Source and object files for root finding
SOURCES = $(SRC_DIR)/root_finding.c \
          $(SRC_DIR)/fileio.c \
          $(SRC_DIR)/function.c \
          $(SRC_DIR)/main.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Executable name
EXECUTABLE = $(BIN_DIR)/root_finder_app

# --- Generated Files ---
RESULTS_CSV = $(DATA_DIR)/root_finding_results.csv
FUNCTION_DATA = $(DATA_DIR)/function_data.dat
FUNCTION_PLOT_SCRIPT = $(SCRIPT_DIR)/plot_function.gp
FUNCTION_PLOT_SCRIPT_ZOOMED = $(SCRIPT_DIR)/plot_function_zoomed.gp

PYTHON_PLOT_SCRIPT = $(SRC_DIR)/plot_results.py
OUTPUT_FORMAT = png
EXAMPLE_PY_PLOT = $(PLOTS_DIR)/heatmap_iteracje_Metoda_Newtona_Stop_dX.$(OUTPUT_FORMAT)

GNUPLOT_FUNC_PLOT_OUTPUT = $(PLOTS_DIR)/function_plot.png
GNUPLOT_FUNC_PLOT_ZOOMED_OUTPUT = $(PLOTS_DIR)/function_plot_zoomed.png


# Default rule: build the executable
all: directories $(EXECUTABLE)

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR) $(TABLE_DIR) $(LATEX_FORMAT_TABLE_DIR) # Zaktualizowano

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard include/*.h)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# --- Data Generation and Plotting ---

run: $(EXECUTABLE)
	@echo "Running C program $(EXECUTABLE) to generate root finding results and plot scripts..."
	@./$(EXECUTABLE)
	@echo "C program finished. Results CSV: $(RESULTS_CSV)"

$(RESULTS_CSV) $(FUNCTION_PLOT_SCRIPT) $(FUNCTION_PLOT_SCRIPT_ZOOMED) $(FUNCTION_DATA): $(EXECUTABLE)
	@if [ ! -f $@ ]; then \
		echo "Warning: $@ not found. Attempting to run C program first."; \
		$(MAKE) -s run; \
	fi

py_plots: $(PYTHON_PLOT_SCRIPT) $(RESULTS_CSV) $(EXAMPLE_PY_PLOT)

$(EXAMPLE_PY_PLOT): $(PYTHON_PLOT_SCRIPT) $(RESULTS_CSV)
	@echo "Generating root finding tables (TXT and LaTeX-formatted TXT) and heatmap plots using Python script..." # Zaktualizowano opis
	$(PYTHON) $(PYTHON_PLOT_SCRIPT)

gnuplot_func_plot: $(GNUPLOT_FUNC_PLOT_OUTPUT)

$(GNUPLOT_FUNC_PLOT_OUTPUT): $(FUNCTION_PLOT_SCRIPT) $(FUNCTION_DATA)
	@echo "Generating function plot using Gnuplot..."
	@gnuplot $(FUNCTION_PLOT_SCRIPT)
	@echo "Gnuplot function plot generated: $(GNUPLOT_FUNC_PLOT_OUTPUT)"

gnuplot_func_plot_zoomed: $(GNUPLOT_FUNC_PLOT_ZOOMED_OUTPUT)

$(GNUPLOT_FUNC_PLOT_ZOOMED_OUTPUT): $(FUNCTION_PLOT_SCRIPT_ZOOMED) $(FUNCTION_DATA)
	@echo "Generating ZOOMED function plot using Gnuplot..."
	@gnuplot $(FUNCTION_PLOT_SCRIPT_ZOOMED)
	@echo "Gnuplot ZOOMED function plot generated: $(GNUPLOT_FUNC_PLOT_ZOOMED_OUTPUT)"

plots: run py_plots gnuplot_func_plot gnuplot_func_plot_zoomed
	@echo ""
	@echo "--- Plot Generation Summary (Root Finding) ---"
	@echo "Generated CSV data file:"
	@ls -l $(RESULTS_CSV) 2>/dev/null || echo "  $(RESULTS_CSV) not found."
	@echo "Generated TXT Tables (Python):"
	@ls -l $(TABLE_DIR)/*.txt 2>/dev/null || echo "  No Python TXT table files found in $(TABLE_DIR)."
	@echo "Generated LaTeX-Formatted TXT Tables (Python):" # Zaktualizowano opis
	@ls -l $(LATEX_FORMAT_TABLE_DIR)/*.txt 2>/dev/null || echo "  No Python LaTeX-formatted TXT table files found in $(LATEX_FORMAT_TABLE_DIR)."
	@echo "Generated Heatmap plots (Python, format: .$(OUTPUT_FORMAT)):"
	@ls -l $(PLOTS_DIR)/heatmap_*.$(OUTPUT_FORMAT) 2>/dev/null || echo "  No Python heatmap plots found in $(PLOTS_DIR)."
	@echo "Generated Function Plot (Gnuplot):"
	@ls -l $(GNUPLOT_FUNC_PLOT_OUTPUT) 2>/dev/null || echo "  $(GNUPLOT_FUNC_PLOT_OUTPUT) not found."
	@echo "Generated ZOOMED Function Plot (Gnuplot):"
	@ls -l $(GNUPLOT_FUNC_PLOT_ZOOMED_OUTPUT) 2>/dev/null || echo "  $(GNUPLOT_FUNC_PLOT_ZOOMED_OUTPUT) not found."
	@echo "------------------------------------------------"
	@echo "Plot generation finished. Check relevant directories."

# --- Cleaning ---

clean:
	@echo "Cleaning temporary files and generated outputs..."
	rm -f $(OBJ_DIR)/*.o
	rm -f $(EXECUTABLE)
	rm -f $(DATA_DIR)/*.csv $(DATA_DIR)/*.dat
	rm -f $(PLOTS_DIR)/*.*
	rm -f $(SCRIPT_DIR)/*.gp
	rm -f $(TABLE_DIR)/*.txt
	rm -f $(LATEX_FORMAT_TABLE_DIR)/*.txt # Zaktualizowano
	@echo "Cleaning finished."

distclean: clean
	@echo "Removing all generated directories..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR) $(SCRIPT_DIR) $(TABLE_DIR) $(LATEX_FORMAT_TABLE_DIR) # Zaktualizowano
	@echo "Deep cleaning finished."

# --- Help ---

help:
	@echo "Available commands for Root Finding Analysis:"
	@echo "  make all              - Compiles the C root finding program (default)"
	@echo "  make run              - Compiles and runs the C program to generate data (.csv, .dat) and function plot scripts"
	@echo "  make py_plots         - Runs the Python script (src/plot_results.py) to generate tables (in $(TABLE_DIR) and $(LATEX_FORMAT_TABLE_DIR)) and heatmap plots" # Zaktualizowano
	@echo "  make gnuplot_func_plot        - Runs Gnuplot to generate the main function plot"
	@echo "  make gnuplot_func_plot_zoomed - Runs Gnuplot to generate the ZOOMED function plot"
	@echo "  make plots            - Runs 'make run', then generates all Python tables/heatmaps AND BOTH Gnuplot function plots"
	@echo "  make clean            - Removes object files, executable, generated data, plots, scripts, and tables"
	@echo "  make distclean        - Performs 'clean' and removes generated directories"
	@echo "  make help             - Displays this help message"

# Declare phony targets
.PHONY: all directories run plots py_plots gnuplot_func_plot gnuplot_func_plot_zoomed clean distclean help