import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

if len(sys.argv) < 3:
    sys.exit(1)

input_csv = sys.argv[1]
output_img = sys.argv[2]

data = pd.read_csv(input_csv)

plt.figure(figsize=(12, 7))
plt.style.use('seaborn-v0_8-darkgrid')

data_sorted = data.sort_values('Ticks', ascending=False)
bars = plt.bar(data_sorted['Version'], data_sorted['Ticks'], color='skyblue', edgecolor='navy')

plt.title('Mandelbrot Optimization Performance Comparison', fontsize=16, fontweight='bold')
plt.xlabel('Optimization Version', fontsize=12)
plt.ylabel('Average Ticks (lower is better)', fontsize=12)
plt.xticks(rotation=45, ha='right')

for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, yval, f'{yval:,.0f}', va='bottom', ha='center', fontsize=9)

plt.tight_layout()

output_dir = os.path.dirname(output_img)
if output_dir and not os.path.exists(output_dir):
    os.makedirs(output_dir)

plt.savefig(output_img)