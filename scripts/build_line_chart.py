import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

if len(sys.argv) < 4:
    sys.exit(1)

input_csv = sys.argv[1]
output_img = sys.argv[2]
target_col_name = sys.argv[3]

def load_hwinfo_csv(path):
    encodings = ['cp1251', 'utf-16', 'utf-8-sig', 'utf-8', 'cp1252']
    for enc in encodings:
        try:
            for sep in [',', ';']:
                df = pd.read_csv(path, encoding=enc, sep=sep, skipinitialspace=True, on_bad_lines='skip')
                if len(df.columns) > 1:
                    return df
        except:
            continue
    return None

data = load_hwinfo_csv(input_csv)

if data is None:
    print("Error: Could not read CSV file. Check encoding or if file is empty.")
    sys.exit(1)

data.columns = [col.replace('"', '').strip() for col in data.columns]
data.columns = ["".join([c for c in col if ord(c) < 128 or c == '°']) for col in data.columns]

def find_column(search_name, cols):
    search_name = search_name.lower()
    for c in cols:
        if search_name in c.lower():
            return c
    return None

actual_target_col = find_column(target_col_name, data.columns)
actual_time_col = find_column('Time', data.columns)

if not actual_target_col:
    print(f"Error: Column containing '{target_col_name}' not found.")
    sys.exit(1)

data[actual_target_col] = pd.to_numeric(data[actual_target_col], errors='coerce')

data = data.dropna(subset=[actual_target_col])
if actual_time_col:
    data = data.dropna(subset=[actual_time_col])

y_values = data[actual_target_col].tolist()

if actual_time_col:
    x_values = [str(t) for t in data[actual_time_col].tolist()]
    x_label = actual_time_col
else:
    x_values = list(range(len(y_values)))
    x_label = "Sample Index"

plt.figure(figsize=(15, 6))
plt.style.use('seaborn-v0_8-darkgrid')

plt.plot(x_values, y_values, color='crimson', linewidth=1.5)

plt.title(f'CPU Metrics: {actual_target_col}', fontsize=14, fontweight='bold')
plt.xlabel(x_label, fontsize=12)
plt.ylabel('Value', fontsize=12)

plt.gca().xaxis.set_major_locator(plt.MaxNLocator(10))
plt.xticks(rotation=30)
plt.tight_layout()

output_dir = os.path.dirname(output_img)
if output_dir and not os.path.exists(output_dir):
    os.makedirs(output_dir)

plt.savefig(output_img)