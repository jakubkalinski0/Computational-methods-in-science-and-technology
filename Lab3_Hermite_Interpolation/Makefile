# Kompilator i flagi
CC = gcc
CFLAGS = -Wall -Wextra -O3 -Iinclude
LDFLAGS = -lm

# Katalogi
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
PLOTS_DIR = plots

# Pliki źródłowe i obiektowe
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/interpolation

# Domyślna reguła
all: directories $(EXECUTABLE)

# Tworzenie katalogów
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR)

# Kompilacja plików obiektowych
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Linkowanie programu
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Uruchomienie programu
run: all
	@$(EXECUTABLE)

# Generowanie wykresów
plots: run
	@gnuplot plot_interpolation.gp
	@gnuplot plot_errors.gp
	@echo "Wygenerowane pliki CSV w katalogu $(DATA_DIR):"
	@ls -l $(DATA_DIR)/*_errors.csv

# Czyszczenie
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(DATA_DIR)/*.dat $(DATA_DIR)/*.csv $(PLOTS_DIR)/*

distclean: clean
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(PLOTS_DIR)

# Pomoc
help:
	@echo "Dostępne polecenia:"
	@echo "  make all       - kompiluje program (domyślne)"
	@echo "  make run       - kompiluje i uruchamia program"
	@echo "  make plots     - generuje wykresy i pliki CSV po uruchomieniu programu"
	@echo "  make clean     - usuwa pliki obiektowe, wykonywalne i wygenerowane dane"
	@echo "  make distclean - usuwa wszystkie wygenerowane pliki i katalogi"
	@echo "  make help      - wyświetla tę pomoc"

.PHONY: all directories run plots clean distclean help