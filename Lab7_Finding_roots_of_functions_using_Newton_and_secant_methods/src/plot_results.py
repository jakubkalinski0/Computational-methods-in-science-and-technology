import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import seaborn as sns
import numpy as np
import os
import sys
import re

# --- Konfiguracja ---
script_dir = os.path.dirname(os.path.abspath(__file__))
project_dir = os.path.dirname(script_dir)
DATA_DIR = os.path.join(project_dir, "data")
OUTPUT_DIR = os.path.join(project_dir, "plots")
TABLE_DIR = os.path.join(project_dir, "tables")
LATEX_FORMAT_TABLE_DIR = os.path.join(project_dir, "tables_latex_format")

CSV_FILE = os.path.join(DATA_DIR, "root_finding_results.csv")

CMAP_ITER_LABEL = "Liczba iteracji"
CMAP_ERROR_LABEL = "Błąd bezwzględny pierwiastka"
CMAP_ITER = "viridis"
CMAP_ERROR = "magma_r"
OUTPUT_FORMAT = 'png'
VECTOR_FORMATS = ['pdf', 'svg', 'eps']
RASTER_DPI = 300
ANNOT_FONT_SIZE = 6

REFERENCE_ROOT_1 = 0.0
REFERENCE_ROOT_2 = -1.0

STOP_CRITERION_TRANSLATIONS = {
    "Stop_dX": "kryt. |x⁽ⁱ⁺¹⁾ - x⁽ⁱ⁾| < ρ",
    "Stop_fX": "kryt. |f(x⁽ⁱ⁾)| < ρ",
    "Stop_Both": "kryt. |x⁽ⁱ⁺¹⁾ - x⁽ⁱ⁾| < ρ  i  |f(x⁽ⁱ⁾)| < ρ"
}

METHOD_TRANSLATIONS = {
    "Newton": "Metoda Newtona",
    "Secant": "Metoda siecznych"
}

os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(TABLE_DIR, exist_ok=True)
os.makedirs(LATEX_FORMAT_TABLE_DIR, exist_ok=True)

def sanitize_filename(name):
    name = name.replace(" ", "_")
    name = re.sub(r'[^\w\s.-]', '', name)
    return name

def format_root_for_latex(root_value, status):
    if status == 1: return "MaxIter"
    if status == 2: return "Stagnacja"
    if status != 0: return f"Błąd ({status})"
    if pd.isna(root_value): return "NaN"
    return f"{root_value:.6f}"

def format_root_error_pl(root_value, status):
    if status == 1: return f"BŁĄD (MaxIter)"
    if status == 2: return f"BŁĄD (Stagnacja)"
    if status != 0: return f"BŁĄD ({status})"
    if pd.isna(root_value): return "NaN"
    err1 = abs(root_value - REFERENCE_ROOT_1)
    err2 = abs(root_value - REFERENCE_ROOT_2)
    min_err = min(err1, err2)
    return f"{root_value:.5f} (Błąd: {min_err:.1e})"

def create_pivot_table(df, method_name, stop_criterion_name,
                       fixed_x0_val_filter=None, # Filtr dla stałego x0 (używane dla metody siecznych)
                       # x_col_iterated nie jest już potrzebny jako argument, wywnioskujemy go
                       y_col='PrecisionRho', val_col='Iterations', aggfunc='first'):
    """Tworzy tabelę przestawną. Dla metody siecznych, x0 jest stałe, x1 iterowane. Dla Newtona, x0 jest iterowane."""
    df_filtered = df[(df['Method'] == method_name) & (df['StopCriterion'] == stop_criterion_name)].copy()

    if method_name == 'Secant':
        if fixed_x0_val_filter is not None:
            df_filtered = df_filtered[np.isclose(df_filtered['x0'], fixed_x0_val_filter)]
        else: # Powinno być zawsze podane dla Secant w tej logice
            print(f"Ostrzeżenie: Dla metody siecznych ({method_name}) oczekiwano filtra fixed_x0_val_filter.")
            return None
        iterated_col_for_pivot = 'x1' # Dla siecznych, x1 jest iterowane
    elif method_name == 'Newton':
        iterated_col_for_pivot = 'x0' # Dla Newtona, x0 jest iterowane
    else:
        print(f"Nierozpoznana metoda: {method_name}")
        return None

    if df_filtered.empty:
        # print(f"Brak danych po filtracji dla: Metoda={method_name}, Kryt={stop_criterion_name}, Stałe x0={fixed_x0_val_filter}")
        return None

    if iterated_col_for_pivot in df_filtered.columns:
        df_filtered.loc[:, iterated_col_for_pivot] = df_filtered[iterated_col_for_pivot].round(1)

    try:
        # Kolumna dla osi X heatmapy to iterowana kolumna
        heatmap_data = pd.pivot_table(df_filtered, index=y_col, columns=iterated_col_for_pivot, values=val_col, aggfunc=aggfunc)
    except Exception as e:
        print(f"Błąd tworzenia tabeli przestawnej dla {method_name}, {stop_criterion_name}, stałe x0={fixed_x0_val_filter}, iterowana kolumna={iterated_col_for_pivot}: {e}")
        return None

    heatmap_data = heatmap_data.sort_index(ascending=False) # Precisions
    heatmap_data = heatmap_data.sort_index(axis=1, ascending=True) # Iterated points
    return heatmap_data


def create_heatmap(data, title, base_filename, x_axis_label, output_format='png', raster_dpi=150, # ZMIANA: x_axis_label
                   cmap=CMAP_ITER, val_label='Iteracje', log_norm=False,
                   annot=False, annot_fmt=".0f"):
    if data is None or data.empty:
        print(f"Pominięto heatmapę: {title} - Brak danych.")
        return
    safe_base_filename = sanitize_filename(base_filename)
    output_path = os.path.join(OUTPUT_DIR, f"{safe_base_filename}.{output_format}")
    plt.figure(figsize=(14, 10))
    norm = None
    cbar_label = val_label
    if log_norm:
        valid_data = pd.to_numeric(data.unstack().dropna(), errors='coerce')
        positive_data = valid_data[valid_data > 0]
        min_val = positive_data.min() if not positive_data.empty else 1e-16
        max_val = positive_data.max() if not positive_data.empty else 1.0
        if min_val > 0 and max_val > min_val:
            norm = colors.LogNorm(vmin=min_val, vmax=max_val)
            cbar_label = f'{val_label} (Skala log.)'
        else:
            cbar_label = f'{val_label} (Skala lin.)'
    try:
        sns.heatmap(data, annot=annot, fmt=annot_fmt if annot else "",
                    linewidths=.5 if output_format not in VECTOR_FORMATS else 0.1,
                    cmap=cmap, norm=norm, cbar_kws={'label': cbar_label},
                    annot_kws={"size": ANNOT_FONT_SIZE})
    except Exception as e:
        print(f"BŁĄD podczas sns.heatmap dla {title}: {e}")
        plt.close()
        return
    plt.title(title, fontsize=14)
    plt.xlabel(x_axis_label, fontsize=12) # Użycie przekazanego x_axis_label
    plt.ylabel(f"Precyzja ρ ({data.index.name})", fontsize=12)
    if plt.gca().get_yaxis().get_scale() == 'log':
        plt.gca().yaxis.set_major_formatter(plt.LogFormatterSciNotation())
    try:
        save_kwargs = {'bbox_inches': 'tight', 'format': output_format}
        if output_format not in VECTOR_FORMATS: save_kwargs['dpi'] = raster_dpi
        plt.savefig(output_path, **save_kwargs)
        print(f"Zapisano wykres: {output_path}")
    except Exception as e:
        print(f"Błąd zapisu wykresu {output_path}: {e}")
    plt.close()

def generate_tables(df, precision_subset=None):
    if precision_subset is None: precision_subset = df['PrecisionRho'].unique()
    methods = df['Method'].unique()
    stop_criteria = df['StopCriterion'].unique()

    for stop_crit_key in stop_criteria:
        stop_crit_pl = STOP_CRITERION_TRANSLATIONS.get(stop_crit_key, stop_crit_key)
        print(f"\n\n=== Tabele dla kryterium stopu: {stop_crit_pl} ===")
        df_crit_filtered = df[df['StopCriterion'] == stop_crit_key]

        for method_key in methods:
            method_pl = METHOD_TRANSLATIONS.get(method_key, method_key)
            df_method_current = df_crit_filtered[df_crit_filtered['Method'] == method_key].copy()

            if method_key == 'Secant':
                fixed_x0_values_for_secant = df_method_current['x0'].dropna().unique()
                if not fixed_x0_values_for_secant.any():
                    print(f"  Brak danych dla metody siecznych ({method_pl}) dla kryterium {stop_crit_pl} do grupowania po x0.")
                    continue

                for x0_val in sorted(fixed_x0_values_for_secant):
                    case_info = f"x0={x0_val:.2f}"
                    df_sec_case_current = df_method_current[np.isclose(df_method_current['x0'], x0_val)].copy()

                    iterated_point_col_name = 'x1' # Dla siecznych, x1 jest iterowane
                    print(f"\n  Pełna tabela TXT: {method_pl} ({stop_crit_pl}), stałe {case_info}, {iterated_point_col_name} iterowane")
                    generate_single_table_full_txt(df_sec_case_current, precision_subset, x_col=iterated_point_col_name,
                                                   method_name_pl=method_pl,
                                                   fixed_val_info=case_info,
                                                   stop_criterion_key=stop_crit_key,
                                                   stop_criterion_pl=stop_crit_pl)

                    print(f"\n  Tabela pierwiastków (format LaTeX do TXT): {method_pl} ({stop_crit_pl}), stałe {case_info}, {iterated_point_col_name} iterowane")
                    generate_single_table_roots_latex_to_txt(df_sec_case_current, precision_subset, x_col=iterated_point_col_name,
                                                             method_name_pl=method_pl,
                                                             fixed_val_info=case_info,
                                                             stop_criterion_key=stop_crit_key,
                                                             stop_criterion_pl=stop_crit_pl)
            else: # Newton
                iterated_point_col_name = 'x0' # Dla Newtona, x0 jest iterowane
                print(f"\n  Pełna tabela TXT: {method_pl} ({stop_crit_pl}), {iterated_point_col_name} iterowane")
                generate_single_table_full_txt(df_method_current, precision_subset, x_col=iterated_point_col_name,
                                               method_name_pl=method_pl,
                                               stop_criterion_key=stop_crit_key,
                                               stop_criterion_pl=stop_crit_pl)

                print(f"\n  Tabela pierwiastków (format LaTeX do TXT): {method_pl} ({stop_crit_pl}), {iterated_point_col_name} iterowane")
                generate_single_table_roots_latex_to_txt(df_method_current, precision_subset, x_col=iterated_point_col_name,
                                                         method_name_pl=method_pl,
                                                         stop_criterion_key=stop_crit_key,
                                                         stop_criterion_pl=stop_crit_pl)

def generate_single_table_full_txt(df_table, precision_subset, x_col, method_name_pl, stop_criterion_key, stop_criterion_pl, fixed_val_info=""):
    # x_col to nazwa iterowanej kolumny (x0 dla Newtona, x1 dla Siecznych)
    if df_table.empty:
        print(f"  (Pełna TXT) Brak danych dla: {method_name_pl}, {stop_criterion_pl}, {fixed_val_info if fixed_val_info else f'{x_col} iterowane'}.")
        return
    try:
        if x_col in df_table.columns:
            df_table.loc[:, x_col] = df_table[x_col].round(1)
        df_pivot = df_table.pivot_table(index=x_col, columns='PrecisionRho', values=['Root', 'Iterations', 'Status'], aggfunc='first')
    except Exception as e:
        print(f"  (Pełna TXT) Błąd tworzenia tabeli przestawnej dla {method_name_pl}, {fixed_val_info}, kryt. {stop_criterion_pl}: {e}")
        return

    actual_precisions_in_pivot = []
    if not df_pivot.empty and isinstance(df_pivot.columns, pd.MultiIndex) and len(df_pivot.columns.levels) > 1:
        actual_precisions_in_pivot = [p for p in df_pivot.columns.levels[1] if ('Root', p) in df_pivot.columns]
    valid_precisions = [p for p in precision_subset if p in actual_precisions_in_pivot]

    if not valid_precisions:
        print(f"  (Pełna TXT) Brak danych dla podanych precyzji dla {method_name_pl}, {fixed_val_info}, kryt. {stop_criterion_pl}.")
        return

    desired_value_types = ['Root', 'Iterations', 'Status']
    multi_index_for_reindex = pd.MultiIndex.from_product(
        [desired_value_types, valid_precisions], names=['ValueType', 'PrecisionRho']
    )
    df_pivot = df_pivot.reindex(columns=multi_index_for_reindex)
    df_pivot.sort_index(ascending=True, inplace=True)
    df_pivot.sort_index(axis=1, inplace=True)
    df_display = pd.DataFrame(index=df_pivot.index)

    for rho in valid_precisions:
        col_name_root = f'Pierwiastek (ρ={rho:.0e})'
        col_name_iter = f'Iteracje (ρ={rho:.0e})'
        root_col, iter_col, status_col = ('Root', rho), ('Iterations', rho), ('Status', rho)
        if root_col not in df_pivot.columns or status_col not in df_pivot.columns:
            df_display[col_name_root], df_display[col_name_iter] = "BŁĄD_STRUKTURY_PIVOT", "BŁĄD_STRUKTURY_PIVOT"
            continue
        roots_data = df_pivot.get(root_col, pd.Series(dtype=float))
        status_data = df_pivot.get(status_col, pd.Series(dtype=int))
        iter_data = df_pivot.get(iter_col, pd.Series(dtype=float))
        df_display[col_name_root] = [format_root_error_pl(r,s) for r,s in zip(roots_data, status_data)]
        if iter_col not in df_pivot.columns:
            df_display[col_name_iter] = "BRAK_ITERACJI"
        else:
            df_display[col_name_iter] = [int(i) if s==0 and pd.notna(i) else f"BŁĄD ({int(s) if pd.notna(s) else '?'})" for i,s in zip(iter_data, status_data)]

    sorted_cols = [col for rho_val in valid_precisions
                   for col in (f'Pierwiastek (ρ={rho_val:.0e})', f'Iteracje (ρ={rho_val:.0e})')
                   if col in df_display.columns]
    if not sorted_cols:
        print(f"  (Pełna TXT) Brak kolumn do wyświetlenia dla {method_name_pl}, {fixed_val_info}, kryt. {stop_criterion_pl}.")
        return
    df_display = df_display[sorted_cols]
    df_display.index = df_display.index.map('{:.1f}'.format)
    df_display.index.name = f"Iterowany punkt {x_col}" # Nazwa indeksu to iterowana kolumna

    filename_parts = ["pelna_tabela", method_name_pl.lower().replace(" ", "_"), stop_criterion_key.lower()]
    if fixed_val_info: filename_parts.append(sanitize_filename(fixed_val_info.replace('x0=','xstale_').replace('.', 'p'))) # Zmieniono nazwę pliku
    txt_filename = sanitize_filename("_".join(filename_parts)) + ".txt"
    txt_path = os.path.join(TABLE_DIR, txt_filename)
    try:
        with open(txt_path, 'w', encoding='utf-8') as f:
            header_info = f"Metoda: {method_name_pl}, Kryterium stopu: {stop_criterion_pl}"
            if fixed_val_info: header_info += f", Stały punkt: {fixed_val_info}"
            header_info += f", Iterowany punkt: {x_col}"
            f.write(header_info)
            f.write("\n\n" + df_display.to_string(index=True, justify='center'))
        print(f"Zapisano pełną tabelę TXT: {txt_path}")
    except Exception as e:
        print(f"Błąd zapisu pełnej tabeli TXT {txt_path}: {e}")

def generate_single_table_roots_latex_to_txt(df_table, precision_subset, x_col, method_name_pl, stop_criterion_key, stop_criterion_pl, fixed_val_info=""):
    # x_col to nazwa iterowanej kolumny
    if df_table.empty:
        print(f"  (Format LaTeX do TXT - Pierwiastki) Brak danych dla: {method_name_pl}, {stop_criterion_pl}, {fixed_val_info if fixed_val_info else f'{x_col} iterowane'}.")
        return
    try:
        if x_col in df_table.columns:
            df_table.loc[:, x_col] = df_table[x_col].round(1)
        df_pivot_roots = df_table.pivot_table(index=x_col, columns='PrecisionRho',
                                              values=['Root', 'Status'], aggfunc='first')
    except Exception as e:
        print(f"  (Format LaTeX do TXT - Pierwiastki) Błąd tworzenia tabeli przestawnej dla {method_name_pl}, {fixed_val_info}, kryt. {stop_criterion_pl}: {e}")
        return

    actual_precisions_in_pivot = []
    if not df_pivot_roots.empty and isinstance(df_pivot_roots.columns, pd.MultiIndex) and len(df_pivot_roots.columns.levels) > 1:
        actual_precisions_in_pivot = [p for p in df_pivot_roots.columns.levels[1] if ('Root', p) in df_pivot_roots.columns]
    valid_precisions = [p for p in precision_subset if p in actual_precisions_in_pivot]

    if not valid_precisions:
        print(f"  (Format LaTeX do TXT - Pierwiastki) Brak danych dla podanych precyzji dla {method_name_pl}, {fixed_val_info}, kryt. {stop_criterion_pl}.")
        return

    df_latex_roots = pd.DataFrame(index=df_pivot_roots.index)
    for rho in valid_precisions:
        root_col_data = df_pivot_roots.get(('Root', rho), pd.Series(dtype=float))
        status_col_data = df_pivot_roots.get(('Status', rho), pd.Series(dtype=int))
        formatted_roots = [format_root_for_latex(r, s) for r, s in zip(root_col_data, status_col_data)]
        rho_str_latex = f"{rho:.0e}".replace("e-0", "e-").replace("e+0", "e")
        col_name_latex = f"$\\rho={rho_str_latex}$"
        df_latex_roots[col_name_latex] = formatted_roots

    if df_latex_roots.empty:
        print(f"  (Format LaTeX do TXT - Pierwiastki) Tabela jest pusta po przetworzeniu dla {method_name_pl}, {fixed_val_info}, kryt. {stop_criterion_pl}.")
        return

    df_latex_roots.index = df_latex_roots.index.map('{:.1f}'.format)
    # Etykieta indeksu tabeli LaTeX - używamy x_col, który jest przekazywany (x0 lub x1)
    index_label_latex = f"${x_col}$"
    if method_name_pl == METHOD_TRANSLATIONS['Newton'] and x_col == 'x0':
        index_label_latex = "$x_0$" # Dla Newtona, iterowany jest x0
    elif method_name_pl == METHOD_TRANSLATIONS['Secant'] and x_col == 'x1':
        index_label_latex = "$x_1$" # Dla siecznych, gdy x0 stałe, iterowany jest x1
    df_latex_roots.index.name = index_label_latex

    caption_text = f"Wartości znalezionych pierwiastków dla {method_name_pl.lower()} ({stop_criterion_pl}). "
    if fixed_val_info: # Dla metody siecznych
        caption_text += f"Stały punkt: {fixed_val_info.replace('x0=','$x_0 = ')}. Iterowany punkt: {index_label_latex}."
    else: # Dla metody Newtona
        caption_text += f"Iterowany punkt startowy: {index_label_latex}."

    label_base = f"tab:{sanitize_filename(method_name_pl.lower())}_{stop_criterion_key.lower()}"
    if fixed_val_info: # Dla metody siecznych, dodaj info o stałym x0
        label_base += f"_{sanitize_filename(fixed_val_info.lower().replace('=','').replace('.','p').replace('x0','stalex0'))}"

    try:
        latex_string = df_latex_roots.to_latex(
            column_format='|l|' + 'c|' * len(df_latex_roots.columns),
            escape=False, caption=caption_text, label=label_base,
            header=True, index=True, longtable=False
        )
    except Exception as e:
        print(f"  (Format LaTeX do TXT - Pierwiastki) Błąd podczas df.to_latex() dla {method_name_pl}, {fixed_val_info}, kryt. {stop_criterion_pl}: {e}")
        return

    filename_parts = ["latex_format_pierwiastki", method_name_pl.lower().replace(" ", "_"), stop_criterion_key.lower()]
    if fixed_val_info: # Dla metody siecznych
        filename_parts.append(sanitize_filename(fixed_val_info.replace('x0=','xstale_').replace('.', 'p')))
    output_filename = sanitize_filename("_".join(filename_parts)) + ".txt"
    output_path = os.path.join(LATEX_FORMAT_TABLE_DIR, output_filename)
    try:
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(f"% Plik: {output_filename}\n")
            f.write(f"% Metoda: {method_name_pl}, Kryterium stopu: {stop_criterion_pl}\n")
            if fixed_val_info: f.write(f"% Stały punkt: {fixed_val_info}, Iterowany punkt: {x_col}\n")
            else: f.write(f"% Iterowany punkt: {x_col}\n")
            f.write("% Aby użyć w LaTeX, skopiuj poniższą zawartość i upewnij się, że masz pakiety: booktabs, amsmath\n")
            f.write("% Możesz potrzebować dostosować caption i label.\n\n")
            f.write(latex_string)
        print(f"Zapisano tabelę w formacie LaTeX do pliku TXT: {output_path}")
    except Exception as e:
        print(f"Błąd zapisu tabeli w formacie LaTeX do pliku TXT {output_path}: {e}")

def infer_bounds(df):
    x0_min, x0_max = df['x0'].min(), df['x0'].max()
    x1_min, x1_max = np.nan, np.nan
    if 'x1' in df.columns and not df['x1'].isna().all():
        x1_data = df['x1'].dropna()
        if not x1_data.empty: x1_min, x1_max = x1_data.min(), x1_data.max()
    a_bound = np.nanmin([x0_min, x1_min]) if not (pd.isna(x0_min) and pd.isna(x1_min)) else -1.4
    b_bound = np.nanmax([x0_max, x1_max]) if not (pd.isna(x0_max) and pd.isna(x1_max)) else 0.6
    if pd.isna(a_bound) or pd.isna(b_bound): return -1.4, 0.6
    return a_bound, b_bound

def main():
    print(f"Odczytywanie wyników z: {CSV_FILE}")
    if not os.path.exists(CSV_FILE):
        print(f"Błąd: Plik danych nie znaleziony: {CSV_FILE}")
        sys.exit(1)
    try:
        df = pd.read_csv(CSV_FILE)
    except Exception as e:
        print(f"Błąd odczytu pliku CSV {CSV_FILE}: {e}")
        sys.exit(1)
    required_cols = ['Method', 'StopCriterion', 'x0', 'x1', 'PrecisionRho', 'Root', 'Iterations', 'FinalError', 'Status']
    if not all(col in df.columns for col in required_cols):
        print("Błąd: Plik CSV nie zawiera wymaganych kolumn.")
        sys.exit(1)

    df.replace(['NAN', 'nan', 'NaN', 'inf', 'Inf', '-inf', '-Inf'], np.nan, inplace=True)
    numeric_cols = ['x0', 'x1', 'PrecisionRho', 'Root', 'Iterations', 'FinalError']
    for col in numeric_cols: df[col] = pd.to_numeric(df[col], errors='coerce')
    df['Status'] = pd.to_numeric(df['Status'], errors='coerce').fillna(-1).astype(int)

    generate_tables(df)

    a_bound, b_bound = infer_bounds(df)
    print(f"Wywnioskowany przedział dla wykresów: [{a_bound:.2f}, {b_bound:.2f}]") # `a` i `b` zdefiniowane w function.c
    # `a_bound` i `b_bound` to granice przedziału `a` i `b`
    # używane jako stałe punkty dla metody siecznych
    stop_criteria_to_plot = df['StopCriterion'].unique()

    for sc_key in stop_criteria_to_plot:
        sc_pl = STOP_CRITERION_TRANSLATIONS.get(sc_key, sc_key)
        print(f"\nGenerowanie wykresów dla kryterium stopu: {sc_pl} (klucz: {sc_key})")
        newton_pl = METHOD_TRANSLATIONS["Newton"]
        secant_pl = METHOD_TRANSLATIONS["Secant"]

        # Metoda Newtona - x0 iterowane
        pivot_newton_iter = create_pivot_table(df, 'Newton', sc_key, val_col='Iterations') # Dla Newtona, fixed_x0_val_filter jest None, iterowana jest x0
        title_newton_iter = f"{newton_pl} - Liczba iteracji\n({sc_pl}, iterowany $x_0$)"
        fname_newton_iter = f"heatmap_iteracje_{newton_pl}_x0iter_{sc_key}"
        create_heatmap(pivot_newton_iter, title_newton_iter, fname_newton_iter, x_axis_label="Iterowany punkt startowy $x_0$", cmap=CMAP_ITER, val_label=CMAP_ITER_LABEL, annot=True, output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        # Metoda siecznych - x0 = a_bound (stałe), x1 iterowane
        pivot_sec_a_iter = create_pivot_table(df, 'Secant', sc_key, fixed_x0_val_filter=a_bound, val_col='Iterations')
        title_sec_a_iter = f"{secant_pl} (stałe $x_0={a_bound:.2f}$) - Liczba iteracji\n({sc_pl}, iterowany $x_1$)"
        fname_sec_a_iter = f"heatmap_iteracje_{secant_pl}_x0staleA_x1iter_{sc_key}"
        create_heatmap(pivot_sec_a_iter, title_sec_a_iter, fname_sec_a_iter, x_axis_label="Iterowany punkt $x_1$", cmap=CMAP_ITER, val_label=CMAP_ITER_LABEL, annot=True, output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        # Metoda siecznych - x1 = b_bound (stałe), x1 iterowane
        pivot_sec_b_iter = create_pivot_table(df, 'Secant', sc_key, fixed_x0_val_filter=b_bound, val_col='Iterations')
        title_sec_b_iter = f"{secant_pl} (stałe $x_1={b_bound:.2f}$) - Liczba iteracji\n({sc_pl}, iterowany $x_0$)"
        fname_sec_b_iter = f"heatmap_iteracje_{secant_pl}_x1staleB_x0iter_{sc_key}"
        create_heatmap(pivot_sec_b_iter, title_sec_b_iter, fname_sec_b_iter, x_axis_label="Iterowany punkt $x_0$", cmap=CMAP_ITER, val_label=CMAP_ITER_LABEL, annot=True, output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        df['RootError'] = df.apply(lambda row: min(abs(row['Root'] - REFERENCE_ROOT_1), abs(row['Root'] - REFERENCE_ROOT_2)) if row['Status'] == 0 and pd.notna(row['Root']) else np.nan, axis=1)
        max_possible_error_proxy = 10.0
        df['RootError_Plot'] = df['RootError'].fillna(max_possible_error_proxy)

        pivot_err_newton = create_pivot_table(df, 'Newton', sc_key, val_col='RootError_Plot', aggfunc='min')
        title_err_newton = f"{newton_pl} - Błąd bezwzględny pierwiastka\n({sc_pl}, iterowany $x_0$)"
        fname_err_newton = f"heatmap_blad_{newton_pl}_x0iter_{sc_key}"
        create_heatmap(pivot_err_newton, title_err_newton, fname_err_newton, x_axis_label="Iterowany punkt startowy $x_0$", cmap=CMAP_ERROR, val_label=CMAP_ERROR_LABEL, log_norm=True, annot=True, annot_fmt=".1e", output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        pivot_err_sec_a = create_pivot_table(df, 'Secant', sc_key, fixed_x0_val_filter=a_bound, val_col='RootError_Plot', aggfunc='min')
        title_err_sec_a = f"{secant_pl} (stałe $x_0={a_bound:.2f}$) - Błąd bezwzględny pierwiastka\n({sc_pl}, iterowany $x_1$)"
        fname_err_sec_a = f"heatmap_blad_{secant_pl}_x0staleA_x1iter_{sc_key}"
        create_heatmap(pivot_err_sec_a, title_err_sec_a, fname_err_sec_a, x_axis_label="Iterowany punkt $x_1$", cmap=CMAP_ERROR, val_label=CMAP_ERROR_LABEL, log_norm=True, annot=True, annot_fmt=".1e", output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        pivot_err_sec_b = create_pivot_table(df, 'Secant', sc_key, fixed_x0_val_filter=b_bound, val_col='RootError_Plot', aggfunc='min')
        title_err_sec_b = f"{secant_pl} (stałe $x_1={b_bound:.2f}$) - Błąd bezwzględny pierwiastka\n({sc_pl}, iterowany $x_0$)"
        fname_err_sec_b = f"heatmap_blad_{secant_pl}_x1staleB_x0iter_{sc_key}"
        create_heatmap(pivot_err_sec_b, title_err_sec_b, fname_err_sec_b, x_axis_label="Iterowany punkt $x_0$", cmap=CMAP_ERROR, val_label=CMAP_ERROR_LABEL, log_norm=True, annot=True, annot_fmt=".1e", output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    print("\nZakończono generowanie wykresów w języku polskim.")

if __name__ == "__main__":
    main()