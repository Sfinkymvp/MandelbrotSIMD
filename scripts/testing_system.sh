#! /bin/bash

ITERATIONS=100

RESULT_DIRECTORY=results
IMAGE_DIRECTORY=images
BIN_DIRECTORY=bin
SCRIPTS_DIRECTORY=scripts

EXEC_FILE=mandelbrot.out
DIAGRAM_EXEC_FILE=build_bar_chart.py
PLOT_EXEC_FILE=build_line_chart.py

RESULTS_FILE=results.csv
DIAGRAM_OUTPUT_FILE=comparison.png
TEMP_PLOT_OUTPUT_FILE=temperature.png


declare -A TARGET_COMMANDS
TARGET_COMMANDS["No optimization O0"]="      no_opti      BENCHMARK=YES ITERS=$ITERATIONS OPTI=O0"
TARGET_COMMANDS["No optimization O2"]="      no_opti      BENCHMARK=YES ITERS=$ITERATIONS OPTI=O2"
TARGET_COMMANDS["No optimization O3"]="      no_opti      BENCHMARK=YES ITERS=$ITERATIONS OPTI=O3"
TARGET_COMMANDS["Package optimization O0"]=" package_opti BENCHMARK=YES ITERS=$ITERATIONS OPTI=O0"
TARGET_COMMANDS["Package optimization O2"]=" package_opti BENCHMARK=YES ITERS=$ITERATIONS OPTI=O2"
TARGET_COMMANDS["Package optimization O3"]=" package_opti BENCHMARK=YES ITERS=$ITERATIONS OPTI=O3"
TARGET_COMMANDS["AVX512 optimization O0"]="  avx512_opti  BENCHMARK=YES ITERS=$ITERATIONS OPTI=O0"
TARGET_COMMANDS["AVX512 optimization O2"]="  avx512_opti  BENCHMARK=YES ITERS=$ITERATIONS OPTI=O2"
TARGET_COMMANDS["AVX512 optimization O3"]="  avx512_opti  BENCHMARK=YES ITERS=$ITERATIONS OPTI=O3"
TARGET_COMMANDS["AVX512 optimization Ofast"]="  avx512_opti  BENCHMARK=YES ITERS=$ITERATIONS OPTI=Ofast"

ORDER=(
    "No optimization O0"
    "No optimization O2"
    "No optimization O3"
    "Package optimization O0" 
    "Package optimization O2" 
    "Package optimization O3" 
    "AVX512 optimization O0" 
    "AVX512 optimization O2" 
    "AVX512 optimization O3" 
    "AVX512 optimization Ofast"
)


mkdir -p "$RESULT_DIRECTORY"
echo "Version,Ticks" > "$RESULT_DIRECTORY/$RESULTS_FILE"


for name in "${ORDER[@]}"
do
    echo "Testing configuration <$name>"
    make clean_programs
    make ${TARGET_COMMANDS[$name]}

    sleep 3

    OUTPUT=$(taskset -c 0 "./$BIN_DIRECTORY/$EXEC_FILE" | grep "Average ticks:")
    TICKS=$(echo "$OUTPUT" | grep -o -E '[0-9]+')

    echo "$name,$TICKS" >> "$RESULT_DIRECTORY/$RESULTS_FILE"
done

python3 "$SCRIPTS_DIRECTORY/$PLOT_EXEC_FILE" "$RESULT_DIRECTORY/$RESULTS_FILE" "$IMAGE_DIRECTORY/$DIAGRAM_OUTPUT_FILE"