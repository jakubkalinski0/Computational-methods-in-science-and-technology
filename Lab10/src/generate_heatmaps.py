import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import os

# --- Konfiguracja ---
CSV_FILE_PATH = "data/jacobi_solver_results.csv"
PLOTS_DIR = "plots/heatmaps" # Dedykowany katalog na heatmapy
os.makedirs(PLOTS_DIR, exist_ok=True)

# Funkcja do tworzenia heatmapy
def create_heatmap(df, value_col, title, filename, target_x0_type, target_criterion_type):
    # Filtrowanie danych dla konkretnego x0_type i criterion_type
    filtered_df = df[(df['X0_Type'] == target_x0_type) & (df['Criterion_Type'] == target_criterion_type)].copy()

    if filtered_df.empty:
        print(f"Brak danych dla heatmapy: {title} (X0_Type={target_x0_type}, Criterion_Type={target_criterion_type})")
        return

    # Przygotowanie danych do pivot table - mogą wystąpić duplikaty jeśli parametry nie są unikalne
    # Np. jeśli dla tego samego N, Rho, X0, Crit jest wiele wpisów (co nie powinno mieć miejsca przy dobrym projekcie eksperymentu)
    # Dla bezpieczeństwa można uśrednić, lub wziąć pierwszy/ostatni wpis.
    # Tutaj zakładamy, że kombinacja (N, Rho) jest unikalna dla danego x0 i crit.
    try:
        heatmap_data = filtered_df.pivot_table(index='N', columns='Rho', values=value_col, aggfunc='mean') # Użyj 'mean' jeśli mogą być duplikaty
    except Exception as e:
        print(f"Błąd podczas tworzenia pivot table dla {title}: {e}")
        print(filtered_df[['N', 'Rho', value_col]].head())
        return

    # Sortowanie dla lepszej czytelności osi
    heatmap_data = heatmap_data.sort_index(axis=0) # Sortuj N
    heatmap_data = heatmap_data.sort_index(axis=1) # Sortuj Rho

    plt.figure(figsize=(12, 8))

    # Użycie skali logarytmicznej dla wartości, jeśli to ma sens (np. dla błędów, liczby iteracji)
    # Dla iteracji często używa się normy logarytmicznej, ale można też liniowej.
    # Dla błędów prawie zawsze logarytmicznej.
    from matplotlib.colors import LogNorm
    norm = None
    if value_col == 'Iterations' and heatmap_data.min().min() > 0 : # Sprawdź czy są wartości >0
        norm = LogNorm(vmin=heatmap_data.min().min(), vmax=heatmap_data.max().max()) # Uważaj na wartości zerowe/ujemne
    elif value_col == 'Final_Abs_Error_Max' and heatmap_data.min().min() > 0:
        norm = LogNorm(vmin=heatmap_data.min().min(), vmax=heatmap_data.max().max())


    sns.heatmap(heatmap_data, annot=True, fmt=".0f" if value_col == 'Iterations' else ".1e", cmap="viridis", norm=norm, linewidths=.5)
    plt.title(title)
    plt.xlabel("Tolerancja (Rho)")
    plt.ylabel("Rozmiar macierzy (N)")

    output_path = os.path.join(PLOTS_DIR, filename)
    plt.savefig(output_path)
    plt.close()
    print(f"Heatmapa zapisana do: {output_path}")

# --- Główna część skryptu ---
if __name__ == "__main__":
    if not os.path.exists(CSV_FILE_PATH):
        print(f"Plik danych {CSV_FILE_PATH} nie został znaleziony. Uruchom najpierw program C.")
    else:
        try:
            main_df = pd.read_csv(CSV_FILE_PATH)
        except pd.errors.EmptyDataError:
            print(f"Plik {CSV_FILE_PATH} jest pusty.")
            exit()

        # --- Generowanie heatmap ---
        # Musisz wiedzieć, jakie wartości X0_Type i Criterion_Type są w Twoim CSV
        # X0_ZEROS = 0, X0_ALTERNATING_HIGH = 3
        # INCREMENT_CRITERION = 0, RESIDUAL_CRITERION = 1

        # Heatmapa dla liczby iteracji, X0_ZEROS, INCREMENT_CRITERION
        create_heatmap(main_df,
                       value_col='Iterations',
                       title='Liczba iteracji (X0: Zera, Kryterium: Przyrostowe)',
                       filename='heatmap_iterations_x0zeros_incr.png',
                       target_x0_type=0, # X0_ZEROS
                       target_criterion_type=0) # INCREMENT_CRITERION

        # Heatmapa dla błędu absolutnego, X0_ZEROS, INCREMENT_CRITERION
        create_heatmap(main_df,
                       value_col='Final_Abs_Error_Max',
                       title='Błąd absolutny (X0: Zera, Kryterium: Przyrostowe)',
                       filename='heatmap_abs_error_x0zeros_incr.png',
                       target_x0_type=0, # X0_ZEROS
                       target_criterion_type=0) # INCREMENT_CRITERION

        # Heatmapa dla liczby iteracji, X0_ALTERNATING_HIGH, RESIDUAL_CRITERION
        create_heatmap(main_df,
                       value_col='Iterations',
                       title='Liczba iteracji (X0: AltHigh, Kryterium: Rezydualne)',
                       filename='heatmap_iterations_x0althigh_resid.png',
                       target_x0_type=3, # X0_ALTERNATING_HIGH
                       target_criterion_type=1) # RESIDUAL_CRITERION

        # Heatmapa dla błędu absolutnego, X0_ALTERNATING_HIGH, RESIDUAL_CRITERION
        create_heatmap(main_df,
                       value_col='Final_Abs_Error_Max',
                       title='Błąd absolutny (X0: AltHigh, Kryterium: Rezydualne)',
                       filename='heatmap_abs_error_x0althigh_resid.png',
                       target_x0_type=3, # X0_ALTERNATING_HIGH
                       target_criterion_type=1) # RESIDUAL_CRITERION

        print("Generowanie heatmap zakończone.")