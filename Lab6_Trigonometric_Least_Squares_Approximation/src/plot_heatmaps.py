import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import seaborn as sns
import numpy as np
import os
import sys
# import matplotlib # Uncomment if needed for font embedding
# matplotlib.rcParams['pdf.fonttype'] = 42
# matplotlib.rcParams['ps.fonttype'] = 42

# --- Configuration ---
script_dir = os.path.dirname(os.path.abspath(__file__))
project_dir = os.path.dirname(script_dir)
DATA_DIR = os.path.join(project_dir, "data")
OUTPUT_DIR = os.path.join(project_dir, "plots")
# --- !!! UPDATE CSV FILENAME !!! ---
CSV_FILE = os.path.join(DATA_DIR, "trig_approx_direct_heatmap_errors.csv")

CMAP = "viridis" # Colormap (e.g., viridis, plasma, inferno, magma, cividis)
# --- Match OUTPUT_FORMAT with Makefile if desired ---
OUTPUT_FORMAT = 'svg' # Change to 'pdf' or 'png' as needed
VECTOR_FORMATS = ['pdf', 'svg', 'eps']
RASTER_DPI = 300 # DPI for raster formats like PNG
# --- Adjust Annotation Limits if needed (m is max harmonic) ---
ANNOT_N_LIMIT = 51 # Upper limit for N in annotated heatmaps
ANNOT_M_LIMIT = 25 # Upper limit for m (max harmonic) in annotated heatmaps
ANNOT_FONT_SIZE = 4 # Font size for annotations

os.makedirs(OUTPUT_DIR, exist_ok=True)

def create_heatmap(df, value_col, title, base_filename, output_format='png', raster_dpi=150, annotated=False, fmt=".1e", annot_kws=None, n_limit=None, m_limit=None):
    """Generates and saves a heatmap."""
    # --- Updated output filename construction ---
    output_path = os.path.join(OUTPUT_DIR, f"{base_filename}.{output_format}")
    print(f"Generating heatmap: {title} -> {output_path}")

    plot_df = df.copy()

    # --- Filter data if limits are provided ---
    if n_limit is not None:
        plot_df = plot_df[plot_df['N'] <= n_limit]
    if m_limit is not None:
        # Filter based on max harmonic 'm'
        plot_df = plot_df[plot_df['m'] <= m_limit] # Use 'm' column name

    if plot_df.empty:
        print(f"Warning: No data available for {title} with specified limits (n<={n_limit}, m<={m_limit}). Skipping plot.")
        return

    plot_df.replace([np.inf, -np.inf], np.nan, inplace=True) # Treat Inf as NaN for LogNorm
    try:
        # Pivot: N on Y-axis (index), m on X-axis (columns)
        heatmap_data = plot_df.pivot(index='N', columns='m', values=value_col) # Use 'm' for columns
    except ValueError as ve:
        if 'duplicate entries' in str(ve):
            print(f"Warning: Duplicate (N, m) pairs found for {title}. Aggregating using mean.")
            # Use 'm' in pivot_table
            heatmap_data = pd.pivot_table(plot_df, index='N', columns='m', values=value_col, aggfunc=np.mean)
        else:
            print(f"Error pivoting data for {title}: {ve}. Check CSV.")
            return
    except Exception as e:
        print(f"Error pivoting data for {title}: {e}. Check CSV.")
        return

    # Sorting axes
    heatmap_data = heatmap_data.sort_index(ascending=True) # Sort N (rows)
    heatmap_data = heatmap_data.sort_index(axis=1, ascending=True) # Sort m (columns)

    plt.figure(figsize=(12, 8)) # Adjust figure size if needed

    # Determine min/max for LogNorm, excluding non-positive values
    valid_data = heatmap_data.unstack().dropna()
    positive_data = valid_data[valid_data > 0]
    min_val = positive_data.min() if not positive_data.empty else None
    max_val = valid_data.max()

    if min_val is None or pd.isna(min_val) or min_val <= 0: # Check min_val > 0 for LogNorm
        print(f"Warning: Min value <= 0 or NaN/empty for {title}. Using linear scale.")
        norm = None # Default linear scale
        cbar_label = f'{value_col} (Linear Scale)'
    else:
        # Use LogNorm only if min_val is positive and valid
        norm = colors.LogNorm(vmin=min_val, vmax=max_val)
        cbar_label = f'{value_col} (Log Scale)'

    # Annotation settings
    default_annot_kws = {"size": ANNOT_FONT_SIZE}
    current_annot_kws = annot_kws if annot_kws is not None else default_annot_kws

    # Create heatmap
    sns.heatmap(
        heatmap_data,
        annot=annotated,
        fmt=fmt if annotated else "",
        linewidths=.5 if output_format not in VECTOR_FORMATS else 0.1, # Thinner lines for vector
        cmap=CMAP,
        norm=norm, # Apply LogNorm or linear norm
        cbar_kws={'label': cbar_label},
        annot_kws=current_annot_kws # Pass annotation keywords
    )

    plt.title(title)
    # --- !!! UPDATE AXIS LABELS !!! ---
    plt.xlabel("Max Harmonic Order (m)") # Updated X-axis label
    plt.ylabel("Number of Points (N)")   # Y-axis label remains the same
    # plt.gca().invert_yaxis() # Keep default y-axis direction (N increasing upwards)

    # --- SAVING ---
    try:
        save_kwargs = {'bbox_inches': 'tight', 'format': output_format}
        if output_format not in VECTOR_FORMATS:
            save_kwargs['dpi'] = raster_dpi # Add DPI for raster formats

        plt.savefig(output_path, **save_kwargs)
        print(f"Saved plot: {output_path}")
    except Exception as e:
        print(f"Error saving plot {output_path}: {e}")
    plt.close() # Close the plot figure to free memory

def main():
    """Main function to read data and generate plots."""
    print(f"Reading heatmap data from: {CSV_FILE}")
    print(f"Output format set to: {OUTPUT_FORMAT}")
    if not os.path.exists(CSV_FILE):
        print(f"Error: Data file not found: {CSV_FILE}")
        print("Please run the C program first ('make run' or './bin/trig_approx_direct_app')")
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
    # --- !!! UPDATE required_cols !!! ---
    required_cols = ['N', 'm', 'MaxAbsoluteError', 'MeanSquaredError'] # Use 'm'
    if not all(col in df.columns for col in required_cols):
        print(f"Error: CSV file {CSV_FILE} is missing required columns.")
        print(f"Expected: {required_cols}")
        print(f"Found: {list(df.columns)}")
        sys.exit(1)

    # Handle potential string representations of NAN/INF
    df.replace(['NAN', 'nan', 'NaN'], np.nan, inplace=True)
    df.replace(['INF', 'inf', 'Inf', '-INF', '-inf', '-Inf'], np.nan, inplace=True) # Treat inf as nan for plotting
    # Convert columns to numeric, coercing errors to NaN
    for col in required_cols:
        df[col] = pd.to_numeric(df[col], errors='coerce')

    # --- !!! UPDATE columns for dropna and astype !!! ---
    df.dropna(subset=['N', 'm'], inplace=True) # Drop rows where N or m couldn't be parsed
    # Ensure N and m are integers after potential float conversion by to_numeric
    df['N'] = df['N'].astype(int)
    df['m'] = df['m'].astype(int) # Use 'm'

    # --- Generate Standard Heatmaps ---
    # --- !!! UPDATE TITLES AND FILENAMES !!! ---
    create_heatmap(df, 'MaxAbsoluteError', 'Max Absolute Error (Log Scale, m vs N) - Direct Method',
                   'trig_direct_max_error_heatmap', # Updated filename
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)
    create_heatmap(df, 'MeanSquaredError', 'Mean Squared Error (Log Scale, m vs N) - Direct Method',
                   'trig_direct_mse_heatmap', # Updated filename
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    # --- Generate Annotated Heatmaps (Limited Range) ---
    max_n_data = df['N'].max()
    max_m_data = df['m'].max() # Use 'm'
    # Filter the dataframe for the annotated plot range
    # --- !!! UPDATE column name for filtering !!! ---
    filtered_df = df[(df['N'] <= ANNOT_N_LIMIT) & (df['m'] <= ANNOT_M_LIMIT)] # Use 'm'

    if not filtered_df.empty:
        print(f"\nGenerating annotated plots for N <= {ANNOT_N_LIMIT}, m <= {ANNOT_M_LIMIT}")
        # --- !!! UPDATE TITLES, FILENAMES, and limits !!! ---
        create_heatmap(filtered_df,
                       'MaxAbsoluteError',
                       f'Max Absolute Error (Log Scale, m<={ANNOT_M_LIMIT}, N<={ANNOT_N_LIMIT}) - Direct',
                       'annotated_trig_direct_max_error_heatmap', # Updated filename
                       output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI,
                       annotated=True, fmt=".1e", # Enable annotations
                       annot_kws={"size": ANNOT_FONT_SIZE}, # Pass annotation font size
                       n_limit=ANNOT_N_LIMIT, m_limit=ANNOT_M_LIMIT) # Pass limits again

        create_heatmap(filtered_df,
                       'MeanSquaredError',
                       f'Mean Squared Error (Log Scale, m<={ANNOT_M_LIMIT}, N<={ANNOT_N_LIMIT}) - Direct',
                       'annotated_trig_direct_mse_heatmap', # Updated filename
                       output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI,
                       annotated=True, fmt=".1e", # Enable annotations
                       annot_kws={"size": ANNOT_FONT_SIZE}, # Pass annotation font size
                       n_limit=ANNOT_N_LIMIT, m_limit=ANNOT_M_LIMIT) # Pass limits again
    else:
        print(f"\nSkipping annotated plots: No data found within the specified limits (N<={ANNOT_N_LIMIT}, m<={ANNOT_M_LIMIT}).")
        # Provide context if data range is smaller than annotation limits
        if max_n_data < ANNOT_N_LIMIT or max_m_data < ANNOT_M_LIMIT:
            print(f"(Data range max N={max_n_data}, max m={max_m_data})") # Use 'm'

    print("\nPython plotting finished.")

if __name__ == "__main__":
    main()