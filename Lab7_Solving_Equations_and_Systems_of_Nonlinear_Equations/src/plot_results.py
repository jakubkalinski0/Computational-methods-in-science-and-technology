import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import seaborn as sns
import numpy as np
import os
import sys

# --- Configuration ---
script_dir = os.path.dirname(os.path.abspath(__file__))
project_dir = os.path.dirname(script_dir)
DATA_DIR = os.path.join(project_dir, "data")
OUTPUT_DIR = os.path.join(project_dir, "plots")
TABLE_DIR = os.path.join(project_dir, "tables")

CSV_FILE = os.path.join(DATA_DIR, "root_finding_results.csv")

CMAP_ITER = "viridis"
CMAP_ERROR = "magma_r"
OUTPUT_FORMAT = 'png'
VECTOR_FORMATS = ['pdf', 'svg', 'eps']
RASTER_DPI = 300
ANNOT_FONT_SIZE = 6

REFERENCE_ROOT_1 = 0.0
REFERENCE_ROOT_2 = -1.0

os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(TABLE_DIR, exist_ok=True)

def format_root_error(root_value, status):
    if status != 0:
        return f"ERR ({status})"
    if pd.isna(root_value):
        return "NAN"

    err1 = abs(root_value - REFERENCE_ROOT_1)
    err2 = abs(root_value - REFERENCE_ROOT_2)
    min_err = min(err1, err2)

    return f"{root_value:.5f} (Err: {min_err:.1e})"


def create_pivot_table(df, method_name, stop_criterion_name, x1_val_filter=None, x_col='x0', y_col='PrecisionRho', val_col='Iterations', aggfunc='first'):
    """Creates a pivot table for heatmap plotting, filtering by stop_criterion_name."""
    df_filtered = df[(df['Method'] == method_name) & (df['StopCriterion'] == stop_criterion_name)].copy()

    if x1_val_filter is not None:
        df_filtered = df_filtered[np.isclose(df_filtered['x1'], x1_val_filter)]

    if df_filtered.empty:
        print(f"Warning: No data found for {method_name}, StopCriterion: {stop_criterion_name} with x1 filter {x1_val_filter}.")
        return None

    if x_col in df_filtered.columns:
        df_filtered[x_col] = df_filtered[x_col].round(1)

    try:
        heatmap_data = pd.pivot_table(df_filtered, index=y_col, columns=x_col, values=val_col, aggfunc=aggfunc)
    except Exception as e:
        print(f"Error creating pivot table for {method_name}, {stop_criterion_name}: {e}")
        # print(f"Problematic df_filtered head for {method_name}, {stop_criterion_name}:\n{df_filtered.head().to_string()}")
        # problematic_rows = df_filtered[df_filtered.duplicated(subset=[y_col, x_col], keep=False)]
        # if not problematic_rows.empty:
        #     print(f"Duplicate (index, column) entries found for {method_name}, {stop_criterion_name}:\n{problematic_rows.to_string()}")
        return None

    heatmap_data = heatmap_data.sort_index(ascending=False)
    heatmap_data = heatmap_data.sort_index(axis=1, ascending=True)

    return heatmap_data


def create_heatmap(data, title, base_filename, output_format='png', raster_dpi=150,
                   cmap=CMAP_ITER, val_label='Iterations', log_norm=False,
                   annot=False, annot_fmt=".0f"):
    """Generates and saves a heatmap."""
    if data is None or data.empty:
        print(f"Skipping heatmap: {title} - No data.")
        return

    # Ensure base_filename is filesystem-safe
    safe_base_filename = base_filename.replace(" ", "_").replace("(", "").replace(")", "").replace("=", "").replace("|","")
    output_path = os.path.join(OUTPUT_DIR, f"{safe_base_filename}.{output_format}")
    print(f"Generating heatmap: {title} -> {output_path}")

    plt.figure(figsize=(12, 8))
    norm = None
    cbar_label = val_label
    if log_norm:
        valid_data = pd.to_numeric(data.unstack().dropna(), errors='coerce')
        positive_data = valid_data[valid_data > 0]
        min_val = positive_data.min() if not positive_data.empty else 1e-16
        max_val = positive_data.max() if not positive_data.empty else 1.0
        if min_val > 0 and max_val > min_val:
            norm = colors.LogNorm(vmin=min_val, vmax=max_val)
            cbar_label = f'{val_label} (Log Scale)'
        else:
            print(f"Warning: Cannot use LogNorm for {title}. Using linear scale.")
            cbar_label = f'{val_label} (Linear Scale)'

    try:
        sns.heatmap(
            data,
            annot=annot,
            fmt=annot_fmt if annot else "",
            linewidths=.5 if output_format not in VECTOR_FORMATS else 0.1,
            cmap=cmap,
            norm=norm,
            cbar_kws={'label': cbar_label},
            annot_kws={"size": ANNOT_FONT_SIZE}
        )
    except Exception as e:
        print(f"ERROR during sns.heatmap for {title}: {e}")
        plt.close()
        return

    plt.title(title)
    plt.xlabel(data.columns.name)
    plt.ylabel(data.index.name)

    if plt.gca().get_yaxis().get_scale() == 'log':
        plt.gca().yaxis.set_major_formatter(plt.LogFormatterSciNotation())

    try:
        save_kwargs = {'bbox_inches': 'tight', 'format': output_format}
        if output_format not in VECTOR_FORMATS:
            save_kwargs['dpi'] = raster_dpi
        plt.savefig(output_path, **save_kwargs)
        print(f"Saved plot: {output_path}")
    except Exception as e:
        print(f"Error saving plot {output_path}: {e}")
    plt.close()


def generate_tables(df, precision_subset=None):
    """Generates and prints/saves formatted tables for each method and stop criterion."""
    if precision_subset is None:
        precision_subset = df['PrecisionRho'].unique()

    methods = df['Method'].unique()
    stop_criteria = df['StopCriterion'].unique()

    for stop_crit in stop_criteria:
        print(f"\n\n=== Tables for Stopping Criterion: {stop_crit} ===")
        df_crit_filtered = df[df['StopCriterion'] == stop_crit]

        for method in methods:
            print(f"\n--- Results Table: {method} Method (Criterion: {stop_crit}) ---")
            df_method = df_crit_filtered[df_crit_filtered['Method'] == method].copy()

            if method == 'Secant':
                try:
                    secant_x1_values = df_method['x1'].dropna().unique()
                    if len(secant_x1_values) == 0: # No secant data for this criterion
                        print("  No Secant data for this criterion.")
                        continue

                    a_val = np.min(secant_x1_values) if len(secant_x1_values) > 0 else infer_bounds(df)[0]
                    b_val = np.max(secant_x1_values) if len(secant_x1_values) > 0 else infer_bounds(df)[1]

                    # Handle case where only one x1 value might exist for a particular criterion if data is sparse
                    distinct_x1s = sorted(secant_x1_values)


                except Exception as e:
                    print(f"Warning: Error determining Secant x1 bounds ({e}) for criterion {stop_crit}. Using inferred.")
                    a_inf, b_inf = infer_bounds(df)
                    distinct_x1s = [a_inf, b_inf] # Fallback

                if not distinct_x1s:
                    print("  No x1 values found for Secant method.")
                    continue

                for x1_val in distinct_x1s:
                    print(f"\n  Case: x1 = {x1_val:.2f} (fixed)")
                    df_sec_case = df_method[np.isclose(df_method['x1'], x1_val)]
                    generate_single_table(df_sec_case, precision_subset, x_col='x0',
                                          fixed_val_info=f"x1={x1_val:.2f}", stop_criterion_name=stop_crit)
            else: # Newton
                generate_single_table(df_method, precision_subset, x_col='x0', stop_criterion_name=stop_crit)


def generate_single_table(df_table, precision_subset, x_col, stop_criterion_name, fixed_val_info=""):
    """Helper to generate one table for a specific method/case and stop criterion."""
    if df_table.empty:
        print("  No data for this case.")
        return

    try:
        if x_col in df_table.columns:
            df_table[x_col] = df_table[x_col].round(1)

        df_pivot = df_table.pivot_table(
            index=x_col,
            columns='PrecisionRho',
            values=['Root', 'Iterations', 'Status'],
            aggfunc='first'
        )
    except Exception as e:
        print(f"  Error creating pivot table for {fixed_val_info}, criterion {stop_criterion_name}: {e}")
        return

    valid_precisions = [p for p in precision_subset if ('Root', p) in df_pivot.columns]
    if not valid_precisions:
        print("  No data for specified precision values in this pivot table.")
        return

    multi_index = pd.MultiIndex.from_product([['Root', 'Iterations', 'Status'], valid_precisions], names=['ValueType', 'PrecisionRho'])
    df_pivot = df_pivot.reindex(columns=multi_index)
    df_pivot.sort_index(ascending=True, inplace=True)

    df_display = pd.DataFrame(index=df_pivot.index)
    for rho in valid_precisions:
        col_name_root = f'Root (rho={rho:.0e})'
        col_name_iter = f'Iter (rho={rho:.0e})'

        root_col = ('Root', rho)
        iter_col = ('Iterations', rho)
        status_col = ('Status', rho)

        if root_col not in df_pivot.columns or status_col not in df_pivot.columns:
            df_display[col_name_root] = "MISSING_DATA"
            df_display[col_name_iter] = "MISSING_DATA"
            continue
        if iter_col not in df_pivot.columns:
            df_display[col_name_root] = [format_root_error(root, status)
                                         for root, status in zip(df_pivot[root_col], df_pivot[status_col])]
            df_display[col_name_iter] = "MISSING_DATA"
            continue

        df_display[col_name_root] = [format_root_error(root, status)
                                     for root, status in zip(df_pivot[root_col], df_pivot[status_col])]
        df_display[col_name_iter] = [int(iter) if status == 0 and pd.notna(iter) else f"ERR ({int(status) if pd.notna(status) else '?'})"
                                     for iter, status in zip(df_pivot[iter_col], df_pivot[status_col])]

    sorted_cols = [col for rho in valid_precisions for col in (f'Root (rho={rho:.0e})', f'Iter (rho={rho:.0e})')]
    # Ensure all columns in sorted_cols actually exist in df_display
    sorted_cols = [col for col in sorted_cols if col in df_display.columns]
    df_display = df_display[sorted_cols]

    df_display.index = df_display.index.map('{:.1f}'.format)
    df_display.index.name = x_col

    print(df_display.to_string(index=True, justify='center'))

    table_filename_parts = [
        "table",
        df_table['Method'].iloc[0].lower(),
        stop_criterion_name.lower()
    ]
    if fixed_val_info:
        safe_fixed_info = fixed_val_info.replace('=', '').replace('.', 'p').replace('-', 'neg').replace(' ', '_')
        table_filename_parts.append(safe_fixed_info)

    table_filename = "_".join(table_filename_parts) + ".txt"
    table_path = os.path.join(TABLE_DIR, table_filename)
    try:
        with open(table_path, 'w') as f:
            f.write(f"Method: {df_table['Method'].iloc[0]}, Criterion: {stop_criterion_name}")
            if fixed_val_info: f.write(f", Case: {fixed_val_info}")
            f.write("\n\n")
            f.write(df_display.to_string(index=True, justify='center'))
        print(f"Saved table to {table_path}")
    except Exception as e:
        print(f"Error saving table {table_path}: {e}")

def infer_bounds(df):
    x0_min, x0_max = df['x0'].min(), df['x0'].max()
    x1_min, x1_max = np.nan, np.nan
    if 'x1' in df.columns and not df['x1'].isna().all():
        x1_data = df['x1'].dropna()
        if not x1_data.empty:
            x1_min = x1_data.min()
            x1_max = x1_data.max()

    a_bound = np.nanmin([x0_min, x1_min]) if not (pd.isna(x0_min) and pd.isna(x1_min)) else -1.4 # Fallback
    b_bound = np.nanmax([x0_max, x1_max]) if not (pd.isna(x0_max) and pd.isna(x1_max)) else 0.6  # Fallback

    if pd.isna(a_bound) or pd.isna(b_bound): # Should not happen with fallbacks
        print("Warning: Could not determine valid bounds, using hardcoded defaults.")
        return -1.4, 0.6
    return a_bound, b_bound

def main():
    print(f"Reading root finding results from: {CSV_FILE}")
    if not os.path.exists(CSV_FILE):
        print(f"Error: Data file not found: {CSV_FILE}")
        sys.exit(1)

    try:
        df = pd.read_csv(CSV_FILE)
    except pd.errors.EmptyDataError:
        print(f"Error: Data file {CSV_FILE} is empty.")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading CSV file {CSV_FILE}: {e}")
        sys.exit(1)

    required_cols = ['Method', 'StopCriterion', 'x0', 'x1', 'PrecisionRho', 'Root', 'Iterations', 'FinalError', 'Status']
    if not all(col in df.columns for col in required_cols):
        print(f"Error: CSV file {CSV_FILE} is missing required columns.")
        print(f"Expected: {required_cols}")
        print(f"Found: {list(df.columns)}")
        sys.exit(1)

    df.replace(['NAN', 'nan', 'NaN', 'inf', 'Inf', '-inf', '-Inf'], np.nan, inplace=True)
    numeric_cols = ['x0', 'x1', 'PrecisionRho', 'Root', 'Iterations', 'FinalError']
    for col in numeric_cols:
        df[col] = pd.to_numeric(df[col], errors='coerce')
    df['Status'] = pd.to_numeric(df['Status'], errors='coerce').fillna(-1).astype(int)

    generate_tables(df)

    a_bound, b_bound = infer_bounds(df)
    print(f"Inferred interval from data for plotting: [{a_bound}, {b_bound}]")

    stop_criteria_to_plot = df['StopCriterion'].unique()

    for sc_name in stop_criteria_to_plot:
        print(f"\nGenerating plots for Stop Criterion: {sc_name}")
        sc_suffix = f"_{sc_name.lower()}" # For filenames

        # Newton Iterations
        pivot_newton_iter = create_pivot_table(df, 'Newton', sc_name, val_col='Iterations')
        create_heatmap(pivot_newton_iter, f'Newton Method Iterations ({sc_name})', f'heatmap_iter_newton{sc_suffix}',
                       cmap=CMAP_ITER, val_label='Iterations', annot=True,
                       output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        # Secant Case 1 (x1=a_bound) Iterations
        pivot_sec_a_iter = create_pivot_table(df, 'Secant', sc_name, x1_val_filter=a_bound, val_col='Iterations')
        create_heatmap(pivot_sec_a_iter, f'Secant Method Iterations (x1={a_bound:.2f}, {sc_name})', f'heatmap_iter_secant_a{sc_suffix}',
                       cmap=CMAP_ITER, val_label='Iterations', annot=True,
                       output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        # Secant Case 2 (x1=b_bound) Iterations
        pivot_sec_b_iter = create_pivot_table(df, 'Secant', sc_name, x1_val_filter=b_bound, val_col='Iterations')
        create_heatmap(pivot_sec_b_iter, f'Secant Method Iterations (x1={b_bound:.2f}, {sc_name})', f'heatmap_iter_secant_b{sc_suffix}',
                       cmap=CMAP_ITER, val_label='Iterations', annot=True,
                       output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        # Root Error Heatmaps
        df['RootError'] = df.apply(lambda row: min(abs(row['Root'] - REFERENCE_ROOT_1), abs(row['Root'] - REFERENCE_ROOT_2)) if row['Status'] == 0 and pd.notna(row['Root']) else np.nan, axis=1)
        max_possible_error_proxy = 1.0
        df['RootError_Plot'] = df['RootError'].fillna(max_possible_error_proxy)

        # Newton Error
        pivot_err_newton = create_pivot_table(df, 'Newton', sc_name, val_col='RootError_Plot', aggfunc='min')
        create_heatmap(pivot_err_newton, f'Newton Method Root Error ({sc_name}, Failures high)', f'heatmap_error_newton{sc_suffix}',
                       cmap=CMAP_ERROR, val_label='Absolute Error', log_norm=True,
                       annot=True, annot_fmt=".1e", output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        # Secant Case 1 Error
        pivot_err_sec_a = create_pivot_table(df, 'Secant', sc_name, x1_val_filter=a_bound, val_col='RootError_Plot', aggfunc='min')
        create_heatmap(pivot_err_sec_a, f'Secant Root Error (x1={a_bound:.2f}, {sc_name}, Failures high)', f'heatmap_error_secant_a{sc_suffix}',
                       cmap=CMAP_ERROR, val_label='Absolute Error', log_norm=True,
                       annot=True, annot_fmt=".1e", output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

        # Secant Case 2 Error
        pivot_err_sec_b = create_pivot_table(df, 'Secant', sc_name, x1_val_filter=b_bound, val_col='RootError_Plot', aggfunc='min')
        create_heatmap(pivot_err_sec_b, f'Secant Root Error (x1={b_bound:.2f}, {sc_name}, Failures high)', f'heatmap_error_secant_b{sc_suffix}',
                       cmap=CMAP_ERROR, val_label='Absolute Error', log_norm=True,
                       annot=True, annot_fmt=".1e", output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    print("\nPython plotting finished.")

if __name__ == "__main__":
    main()