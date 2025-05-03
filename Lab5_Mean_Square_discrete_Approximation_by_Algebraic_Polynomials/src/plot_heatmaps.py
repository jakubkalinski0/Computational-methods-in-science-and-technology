import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import seaborn as sns
import numpy as np
import os
import sys
# You might need to adjust rcParams for font embedding, especially for PDF
# import matplotlib
# matplotlib.rcParams['pdf.fonttype'] = 42 # Embed TrueType fonts
# matplotlib.rcParams['ps.fonttype'] = 42

# --- More robust path configuration ---
# Find the directory where this script is located (src)
script_dir = os.path.dirname(os.path.abspath(__file__))
# Find the project directory (Lab5_...) - one level above src
project_dir = os.path.dirname(script_dir)

# Define paths relative to the Lab5_... project directory
DATA_DIR = os.path.join(project_dir, "data")
OUTPUT_DIR = os.path.join(project_dir, "plots")
CSV_FILE = os.path.join(DATA_DIR, "approximation_heatmap_errors.csv")

# --- Other configuration settings ---
CMAP = "viridis" # Colormap for heatmaps (e.g., viridis, plasma, inferno, magma, cividis)
OUTPUT_FORMAT = 'svg' # You can change this to 'pdf' or 'png'
VECTOR_FORMATS = ['pdf', 'svg', 'eps']
RASTER_DPI = 300 # DPI for raster formats (e.g., PNG). Ignored for vector formats. 300 DPI is good for printing.
ANNOT_N_LIMIT = 30 # Upper limit for N in annotated heatmaps
ANNOT_M_LIMIT = 20 # Upper limit for M in annotated heatmaps
ANNOT_FONT_SIZE = 5 # Reduced font size for annotations on plots

# --- Ensure output directory exists ---
# Now using the full OUTPUT_DIR path
os.makedirs(OUTPUT_DIR, exist_ok=True)

def create_heatmap(df, value_col, title, base_filename, output_format='png', raster_dpi=150, annotated=False, fmt=".1e", annot_kws=None, n_limit=None, m_limit=None):
    """
    Generates and saves a heatmap from the provided DataFrame.

    Args:
        df (pd.DataFrame): DataFrame containing the error data.
        value_col (str): Column name containing the error values to plot.
        title (str): Title for the heatmap plot.
        base_filename (str): Base filename (without extension).
        output_format (str): Output file format ('png', 'pdf', 'svg', 'eps').
        raster_dpi (int): DPI for raster formats (ignored for vector).
        annotated (bool): Whether to overlay values on the heatmap cells.
        fmt (str): Format string for annotations.
        annot_kws (dict): Additional keyword arguments for annotations.
        n_limit (int, optional): Maximum N value to include. Defaults to None.
        m_limit (int, optional): Maximum M value to include. Defaults to None.
    """
    # Create the FULL path to the output file using OUTPUT_DIR
    output_path = os.path.join(OUTPUT_DIR, f"{base_filename}.{output_format}")
    print(f"Generating heatmap: {title} -> {output_path}") # Shows the full path

    plot_df = df.copy()

    # --- Filter data if limits are provided ---
    if n_limit is not None:
        plot_df = plot_df[plot_df['N'] <= n_limit]
    if m_limit is not None:
        plot_df = plot_df[plot_df['M'] <= m_limit]

    if plot_df.empty:
        print(f"Warning: No data available for {title} with specified limits (n<={n_limit}, m<={m_limit}). Skipping plot.")
        return

    # --- Pivot the data ---
    # Axes swapped: N as index (Y-axis), M as columns (X-axis)
    plot_df.replace([np.inf, -np.inf], np.nan, inplace=True)
    try:
        heatmap_data = plot_df.pivot(index='N', columns='M', values=value_col)
    except ValueError as ve:
        if 'duplicate entries' in str(ve):
            print(f"Warning: Duplicate (N, M) pairs found for {title}. Aggregating using mean.")
            heatmap_data = pd.pivot_table(plot_df, index='N', columns='M', values=value_col, aggfunc=np.mean)
        else:
            print(f"Error pivoting data for {title}: {ve}")
            print("Check your CSV file format and content.")
            return
    except Exception as e:
        print(f"Error pivoting data for {title}: {e}")
        print("Check your CSV file format and content.")
        return

    # Sorting: index (N) ascending, columns (M) ascending
    heatmap_data = heatmap_data.sort_index(ascending=True)
    heatmap_data = heatmap_data.sort_index(axis=1, ascending=True)

    # --- Plotting ---
    plt.figure(figsize=(12, 8)) # Figure size might need adjustment

    valid_data = heatmap_data.unstack().dropna()
    positive_data = valid_data[valid_data > 0]
    min_val = positive_data.min() if not positive_data.empty else None
    max_val = valid_data.max()

    if min_val is None or pd.isna(min_val):
        print(f"Warning: No positive data found or min value is non-positive for {title}. Using linear scale.")
        norm = None # Use default linear normalization
        cbar_label = f'{value_col} (Linear Scale)'
    else:
        # Use LogNorm only if min_val is positive and valid
        norm = colors.LogNorm(vmin=min_val, vmax=max_val)
        cbar_label = f'{value_col} (Log Scale)'

    default_annot_kws = {"size": ANNOT_FONT_SIZE}
    current_annot_kws = annot_kws if annot_kws is not None else default_annot_kws

    sns.heatmap(
        heatmap_data,
        annot=annotated,
        fmt=fmt if annotated else "",
        linewidths=.5 if output_format not in VECTOR_FORMATS else 0.1, # Adjust linewidth for vector formats
        cmap=CMAP,
        norm=norm, # Apply LogNorm or linear norm
        cbar_kws={'label': cbar_label},
        annot_kws=current_annot_kws # Pass annotation keywords
    )

    plt.title(title)
    # Axis labels swapped
    plt.xlabel("Approximation Degree (M)")
    plt.ylabel("Number of Points (N)")
    # plt.gca().invert_yaxis()

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
    # Message now uses the full CSV_FILE path
    print(f"Reading heatmap data from: {CSV_FILE}")
    print(f"Output format set to: {OUTPUT_FORMAT}")
    if not os.path.exists(CSV_FILE):
        print(f"Error: Data file not found: {CSV_FILE}")
        print("Please run the C program first ('make run' or './bin/approximation_app')")
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
    required_cols = ['N', 'M', 'MaxAbsoluteError', 'MeanSquaredError']
    if not all(col in df.columns for col in required_cols):
        print(f"Error: CSV file {CSV_FILE} is missing required columns.")
        print(f"Expected: {required_cols}")
        print(f"Found: {list(df.columns)}")
        sys.exit(1)

    # Handle potential string representations of NAN/INF
    df.replace(['NAN', 'nan', 'NaN'], np.nan, inplace=True)
    df.replace(['INF', 'inf', 'Inf'], np.inf, inplace=True)
    df.replace(['-INF', '-inf', '-Inf'], -np.inf, inplace=True)
    # Convert columns to numeric, coercing errors to NaN
    for col in required_cols:
        df[col] = pd.to_numeric(df[col], errors='coerce')

    df.dropna(subset=['N', 'M'], inplace=True) # Drop rows where N or M couldn't be parsed
    # Ensure N and M are integers after potential float conversion by to_numeric
    df['N'] = df['N'].astype(int)
    df['M'] = df['M'].astype(int)

    # --- Generate Standard Heatmaps ---
    # Titles and filenames remain the same (reflecting swapped axes)
    create_heatmap(df, 'MaxAbsoluteError', 'Max Absolute Error (Log Scale, M vs N)',
                   'approximation_max_error_heatmap',
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)
    create_heatmap(df, 'MeanSquaredError', 'Mean Squared Error (Log Scale, M vs N)',
                   'approximation_mse_heatmap',
                   output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI)

    # --- Generate Annotated Heatmaps (Limited Range) ---
    max_n_data = df['N'].max()
    max_m_data = df['M'].max()
    # Filter the dataframe for the annotated plot range
    filtered_df = df[(df['N'] <= ANNOT_N_LIMIT) & (df['M'] <= ANNOT_M_LIMIT)]

    if not filtered_df.empty:
        print(f"\nGenerating annotated plots for N <= {ANNOT_N_LIMIT}, M <= {ANNOT_M_LIMIT}")
        create_heatmap(filtered_df,
                       'MaxAbsoluteError',
                       f'Max Absolute Error (Log Scale, M<={ANNOT_M_LIMIT}, N<={ANNOT_N_LIMIT})',
                       'annotated_max_error_heatmap',
                       output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI,
                       annotated=True, fmt=".1e", # Enable annotations with scientific format
                       annot_kws={"size": ANNOT_FONT_SIZE}, # Pass annotation font size
                       n_limit=ANNOT_N_LIMIT, m_limit=ANNOT_M_LIMIT) # Pass limits again (optional, but good practice)

        create_heatmap(filtered_df,
                       'MeanSquaredError',
                       f'Mean Squared Error (Log Scale, M<={ANNOT_M_LIMIT}, N<={ANNOT_N_LIMIT})',
                       'annotated_mse_heatmap',
                       output_format=OUTPUT_FORMAT, raster_dpi=RASTER_DPI,
                       annotated=True, fmt=".1e", # Enable annotations with scientific format
                       annot_kws={"size": ANNOT_FONT_SIZE}, # Pass annotation font size
                       n_limit=ANNOT_N_LIMIT, m_limit=ANNOT_M_LIMIT) # Pass limits again
    else:
        print(f"\nSkipping annotated plots: No data found within the specified limits (N<={ANNOT_N_LIMIT}, M<={ANNOT_M_LIMIT}).")
        # Provide context if data range is smaller than annotation limits
        if max_n_data < ANNOT_N_LIMIT or max_m_data < ANNOT_M_LIMIT:
            print(f"(Data range max N={max_n_data}, max M={max_m_data})")

    print("\nPython plotting finished.")

if __name__ == "__main__":
    main()