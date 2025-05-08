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
TABLE_DIR = os.path.join(project_dir, "tables") # Optional: for saving tables

CSV_FILE = os.path.join(DATA_DIR, "root_finding_results.csv")

CMAP_ITER = "viridis" # Colormap for iterations (e.g., viridis, plasma, inferno, magma, cividis)
CMAP_ERROR = "magma_r" # Colormap for errors (reversed magma: high errors are light)
OUTPUT_FORMAT = 'png' # Change to 'svg' or 'pdf' as needed
VECTOR_FORMATS = ['pdf', 'svg', 'eps']
RASTER_DPI = 300 # DPI for raster formats like PNG
# ANNOT_MAX_ITER = 25 # Max iteration count for which annotation value is SHOWN (Removed as create_heatmap simplified)
ANNOT_FONT_SIZE = 6 # Font size for annotations

# Reference root (find one accurately, e.g., using WolframAlpha or high-precision run)
# For x^14 + x^13 = x^13(x+1) = 0, roots are x=0 and x=-1. Both are in [-1.4, 0.6]
REFERENCE_ROOT_1 = 0.0
REFERENCE_ROOT_2 = -1.0
# Depending on the starting point, methods will converge to one or the other (or fail).

os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(TABLE_DIR, exist_ok=True) # Create table directory

# Removed unused function format_float_or_nan

def format_root_error(root_value, status):
    if status != 0:
        return f"ERR ({status})" # Indicate error status
    if pd.isna(root_value):
        return "NAN"

    # Calculate absolute error from the closest reference root
    err1 = abs(root_value - REFERENCE_ROOT_1)
    err2 = abs(root_value - REFERENCE_ROOT_2)
    min_err = min(err1, err2)

    return f"{root_value:.5f} (Err: {min_err:.1e})"


def create_pivot_table(df, method_name, x1_val_filter=None, x_col='x0', y_col='PrecisionRho', val_col='Iterations', aggfunc='first'):
    """Creates a pivot table for heatmap plotting."""
    df_filtered = df[df['Method'] == method_name].copy()

    # Additional filtering for Secant cases based on x1
    if x1_val_filter is not None:
        # Need to handle potential floating point inaccuracies in x1
        df_filtered = df_filtered[np.isclose(df_filtered['x1'], x1_val_filter)]

    if df_filtered.empty:
        print(f"Warning: No data found for {method_name} with x1 filter {x1_val_filter}.")
        return None

    # Round the column used for heatmap columns (typically x0) to 1 decimal place for cleaner labels
    if x_col in df_filtered.columns:
        df_filtered[x_col] = df_filtered[x_col].round(1)

    try:
        # Use pivot_table to handle potential duplicate (x,y) entries if any
        heatmap_data = pd.pivot_table(df_filtered, index=y_col, columns=x_col, values=val_col, aggfunc=aggfunc)
    except Exception as e:
        print(f"Error creating pivot table for {method_name}: {e}")
        return None

    # Sort axes
    heatmap_data = heatmap_data.sort_index(ascending=False) # Precision: High (small) at top
    heatmap_data = heatmap_data.sort_index(axis=1, ascending=True) # x0: Left to right

    return heatmap_data


# Modified create_heatmap: removed na_color, changed annotation logic
def create_heatmap(data, title, base_filename, output_format='png', raster_dpi=150,
                   cmap=CMAP_ITER, val_label='Iterations', log_norm=False,
                   annot=False, annot_fmt=".0f"): # usunięto na_color i annot_max
    """Generates and saves a heatmap (Simplified Debug Version)."""
    if data is None or data.empty:
        print(f"Skipping heatmap: {title} - No data.")
        return

    output_path = os.path.join(OUTPUT_DIR, f"{base_filename}.{output_format}")
    print(f"[DEBUG] Generating heatmap (Simplified): {title} -> {output_path}")

    plt.figure(figsize=(12, 8))

    # Normalization (bez zmian)
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

    # Bezpośrednie wywołanie heatmap bez maskowania adnotacji
    try:
        sns.heatmap(
            data, # Użyj oryginalnych danych do kolorów i adnotacji
            annot=annot, # Pokaż wszystkie adnotacje, jeśli annot=True
            fmt=annot_fmt if annot else "",
            linewidths=.5 if output_format not in VECTOR_FORMATS else 0.1,
            cmap=cmap,
            norm=norm,
            cbar_kws={'label': cbar_label},
            annot_kws={"size": ANNOT_FONT_SIZE}
            # na_color i mask usunięte
        )
    except Exception as e:
        print(f"ERROR during sns.heatmap for {title}: {e}")
        plt.close() # Zamknij figurę w razie błędu
        return # Nie próbuj zapisywać

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
    """Generates and prints/saves formatted tables similar to example PDF."""
    if precision_subset is None:
        precision_subset = df['PrecisionRho'].unique()

    methods = df['Method'].unique()

    for method in methods:
        print(f"\n--- Results Table: {method} Method ---")
        df_method = df[df['Method'] == method].copy()

        if method == 'Secant':
            # Identify fixed x1 values (a and b)
            # Use inferred bounds or default if necessary
            try:
                # Find the actual unique values used for x1 in Secant method runs
                secant_x1_values = df_method['x1'].dropna().unique()
                a_val = np.min(secant_x1_values)
                b_val = np.max(secant_x1_values)
                # Check if we actually found two distinct values expected
                if len(secant_x1_values) < 2 or np.isclose(a_val, b_val):
                    print("Warning: Could not reliably determine distinct fixed x1 values (a, b) for Secant.")
                    # Attempt to use inferred bounds as fallback
                    a_bound_inf, b_bound_inf = infer_bounds(df)
                    a_val = a_bound_inf
                    b_val = b_bound_inf
            except Exception as e:
                print(f"Warning: Error determining Secant x1 bounds ({e}). Using defaults.")
                a_val = -1.4 # Fallback
                b_val = 0.6  # Fallback


            print(f"\n  Case 1: x1 = {a_val:.2f} (fixed)")
            # Use np.isclose for robust float comparison
            df_sec_a = df_method[np.isclose(df_method['x1'], a_val)]
            generate_single_table(df_sec_a, precision_subset, x_col='x0', fixed_val_info=f"x1={a_val:.2f}")

            print(f"\n  Case 2: x1 = {b_val:.2f} (fixed)")
            # Use np.isclose for robust float comparison
            df_sec_b = df_method[np.isclose(df_method['x1'], b_val)]
            generate_single_table(df_sec_b, precision_subset, x_col='x0', fixed_val_info=f"x1={b_val:.2f}")

        else: # Newton
            generate_single_table(df_method, precision_subset, x_col='x0')


def generate_single_table(df_table, precision_subset, x_col, fixed_val_info=""):
    """Helper to generate one table for a specific method/case."""
    if df_table.empty:
        print("  No data for this case.")
        return

    try:
        # Round x_col *before* pivoting for cleaner index in the final table
        if x_col in df_table.columns:
            df_table[x_col] = df_table[x_col].round(1)

        df_pivot = df_table.pivot_table(
            index=x_col,
            columns='PrecisionRho',
            values=['Root', 'Iterations', 'Status'], # Pivot multiple values
            aggfunc='first' # Assuming unique entries per (x0, rho)
        )
    except Exception as e:
        print(f"  Error creating pivot table for {fixed_val_info}: {e}")
        # print(df_table.head()) # Uncomment for debugging
        return


    # Select only the desired precision columns that actually exist in the pivoted data
    valid_precisions = [p for p in precision_subset if ('Root', p) in df_pivot.columns]
    if not valid_precisions:
        print("  No data for specified precision values.")
        return

    # Reindex to ensure desired columns are present and in order
    multi_index = pd.MultiIndex.from_product([['Root', 'Iterations', 'Status'], valid_precisions], names=['ValueType', 'PrecisionRho'])
    df_pivot = df_pivot.reindex(columns=multi_index)

    df_pivot.sort_index(ascending=True, inplace=True) # Sort by x0

    # --- Format the table ---
    # Select and format Root and Iterations
    df_display = pd.DataFrame(index=df_pivot.index)
    for rho in valid_precisions:
        col_name_root = f'Root (rho={rho:.0e})'
        col_name_iter = f'Iter (rho={rho:.0e})'

        # Check if columns exist before accessing
        root_col = ('Root', rho)
        iter_col = ('Iterations', rho)
        status_col = ('Status', rho)

        if root_col not in df_pivot.columns or status_col not in df_pivot.columns:
            print(f"Warning: Missing Root/Status column for rho={rho:.0e}")
            df_display[col_name_root] = "MISSING"
            df_display[col_name_iter] = "MISSING"
            continue
        if iter_col not in df_pivot.columns:
            print(f"Warning: Missing Iterations column for rho={rho:.0e}")
            # Still try to format root if possible
            df_display[col_name_root] = [format_root_error(root, status)
                                         for root, status in zip(df_pivot[root_col], df_pivot[status_col])]
            df_display[col_name_iter] = "MISSING"
            continue


        # Apply formatting function using status
        df_display[col_name_root] = [format_root_error(root, status)
                                     for root, status in zip(df_pivot[root_col], df_pivot[status_col])]

        # Format iterations, show 'ERR' if status is not 0
        df_display[col_name_iter] = [int(iter) if status == 0 and pd.notna(iter) else f"ERR ({int(status) if pd.notna(status) else '?'})"
                                     for iter, status in zip(df_pivot[iter_col], df_pivot[status_col])]


    # Sort columns alternating Root and Iter
    sorted_cols = [col for rho in valid_precisions for col in (f'Root (rho={rho:.0e})', f'Iter (rho={rho:.0e})')]
    df_display = df_display[sorted_cols]

    # Format index (x0) to one decimal place for printing
    df_display.index = df_display.index.map('{:.1f}'.format)
    df_display.index.name = x_col # Restore index name


    print(df_display.to_string(index=True, justify='center'))

    # Optional: Save table to file
    table_filename = f"table_{df_table['Method'].iloc[0].lower()}"
    if fixed_val_info:
        # Sanitize filename from fixed_val_info
        safe_fixed_info = fixed_val_info.replace('=', '').replace('.', 'p').replace('-','neg')
        table_filename += f"_{safe_fixed_info}"
    table_path = os.path.join(TABLE_DIR, f"{table_filename}.txt")
    try:
        with open(table_path, 'w') as f:
            f.write(df_display.to_string(index=True, justify='center'))
        print(f"Saved table to {table_path}")
    except Exception as e:
        print(f"Error saving table {table_path}: {e}")

def infer_bounds(df):
    # Ensure we handle cases where x1 might be all NaN (like for Newton)
    x0_min, x0_max = df['x0'].min(), df['x0'].max()
    x1_min, x1_max = np.nan, np.nan
    if 'x1' in df.columns and not df['x1'].isna().all():
        x1_min = df['x1'].dropna().min()
        x1_max = df['x1'].dropna().max()

    # Combine valid bounds
    a_bound = np.nanmin([x0_min, x1_min])
    b_bound = np.nanmax([x0_max, x1_max])

    if pd.isna(a_bound) or pd.isna(b_bound):
        raise ValueError("Could not determine valid bounds")
    return a_bound, b_bound

def main():
    """Main function to read data and generate plots."""
    print(f"Reading root finding results from: {CSV_FILE}")
    print(f"Output format set to: {OUTPUT_FORMAT}")
    if not os.path.exists(CSV_FILE):
        print(f"Error: Data file not found: {CSV_FILE}")
        print("Please run the C program first ('make run' or './bin/root_finder_app')")
        sys.exit(1)

    try:
        df = pd.read_csv(CSV_FILE)
    except pd.errors.EmptyDataError:
        print(f"Error: Data file {CSV_FILE} is empty.")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading CSV file {CSV_FILE}: {e}")
        sys.exit(1)

    # --- Basic Data Validation ---
    required_cols = ['Method', 'x0', 'x1', 'PrecisionRho', 'Root', 'Iterations', 'FinalError', 'Status']
    if not all(col in df.columns for col in required_cols):
        print(f"Error: CSV file {CSV_FILE} is missing required columns.")
        print(f"Expected: {required_cols}")
        print(f"Found: {list(df.columns)}")
        sys.exit(1)

    # Handle potential string representations of NAN/INF
    df.replace(['NAN', 'nan', 'NaN', 'inf', 'Inf', '-inf', '-Inf'], np.nan, inplace=True)

    # Convert columns to numeric, coercing errors to NaN
    numeric_cols = ['x0', 'x1', 'PrecisionRho', 'Root', 'Iterations', 'FinalError']
    for col in numeric_cols:
        df[col] = pd.to_numeric(df[col], errors='coerce')
    df['Status'] = pd.to_numeric(df['Status'], errors='coerce').fillna(-1).astype(int) # Handle non-numeric status


    # --- Generate Tables ---
    generate_tables(df) # Generate tables for all precisions

    # --- Get Interval Boundaries for Plotting ---
    try:
        a_bound, b_bound = infer_bounds(df)
        print(f"Inferred interval from data: [{a_bound}, {b_bound}]")
    except Exception as e:
        a_bound, b_bound = -1.4, 0.6 # Fallback to default
        print(f"Warning: Could not infer interval ({e}). Using default [{a_bound}, {b_bound}]")


    # --- Generate Iteration Heatmaps ---
    print("\nGenerating Iteration Count Heatmaps...")
    # Newton
    pivot_newton = create_pivot_table(df, 'Newton', x_col='x0', y_col='PrecisionRho', val_col='Iterations')
    create_heatmap(pivot_newton, 'Newton Method Iterations', 'heatmap_iter_newton',
                   cmap=CMAP_ITER, val_label='Iterations', annot=True,
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    # Secant Case 1 (x1=a)
    pivot_sec_a = create_pivot_table(df, 'Secant', x1_val_filter=a_bound, x_col='x0', y_col='PrecisionRho', val_col='Iterations')
    create_heatmap(pivot_sec_a, f'Secant Method Iterations (x1={a_bound:.2f} fixed)', 'heatmap_iter_secant_a',
                   cmap=CMAP_ITER, val_label='Iterations', annot=True,
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    # Secant Case 2 (x1=b)
    pivot_sec_b = create_pivot_table(df, 'Secant', x1_val_filter=b_bound, x_col='x0', y_col='PrecisionRho', val_col='Iterations')
    create_heatmap(pivot_sec_b, f'Secant Method Iterations (x1={b_bound:.2f} fixed)', 'heatmap_iter_secant_b',
                   cmap=CMAP_ITER, val_label='Iterations', annot=True,
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)


    # --- Generate Root Error Heatmaps (Optional) ---
    print("\nGenerating Root Absolute Error Heatmaps...")
    # Calculate absolute error from closest reference root
    df['RootError'] = df.apply(lambda row: min(abs(row['Root'] - REFERENCE_ROOT_1), abs(row['Root'] - REFERENCE_ROOT_2)) if row['Status'] == 0 and pd.notna(row['Root']) else np.nan, axis=1)

    # Fill NaN in RootError with a large value proxy BEFORE pivoting for the error heatmap
    max_possible_error_proxy = 1.0 # Proxy value for failed cases
    df['RootError_Plot'] = df['RootError'].fillna(max_possible_error_proxy)

    # Newton Error - use RootError_Plot
    pivot_err_newton = create_pivot_table(df, 'Newton', x_col='x0', y_col='PrecisionRho', val_col='RootError_Plot', aggfunc='min')
    create_heatmap(pivot_err_newton, 'Newton Method Root Error |Root - Actual| (Failures marked high)', 'heatmap_error_newton', # Updated Title
                   cmap=CMAP_ERROR, val_label='Absolute Error', log_norm=True,
                   annot=True, annot_fmt=".1e",
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    # Secant Case 1 Error - use RootError_Plot
    pivot_err_sec_a = create_pivot_table(df, 'Secant', x1_val_filter=a_bound, x_col='x0', y_col='PrecisionRho', val_col='RootError_Plot', aggfunc='min')
    create_heatmap(pivot_err_sec_a, f'Secant Method Root Error |Root - Actual| (x1={a_bound:.2f} fixed, Failures marked high)', 'heatmap_error_secant_a', # Updated Title
                   cmap=CMAP_ERROR, val_label='Absolute Error', log_norm=True,
                   annot=True, annot_fmt=".1e",
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    # Secant Case 2 Error - use RootError_Plot
    pivot_err_sec_b = create_pivot_table(df, 'Secant', x1_val_filter=b_bound, x_col='x0', y_col='PrecisionRho', val_col='RootError_Plot', aggfunc='min')
    create_heatmap(pivot_err_sec_b, f'Secant Method Root Error |Root - Actual| (x1={b_bound:.2f} fixed, Failures marked high)', 'heatmap_error_secant_b', # Updated Title
                   cmap=CMAP_ERROR, val_label='Absolute Error', log_norm=True,
                   annot=True, annot_fmt=".1e",
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    print("\nPython plotting finished.")

if __name__ == "__main__":
    main()