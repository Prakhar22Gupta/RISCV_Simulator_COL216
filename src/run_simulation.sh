#!/bin/bash

# filepath: /home/prakhar/RISCV_Simulator_COL216/src/run_simulation.sh

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input_file> <bypass_mode>"
    echo "bypass_mode: forbid or forward"
    exit 1
fi

INPUT_FILE="../inputfiles/$1"
BYPASS_MODE="$2"
OUTPUT_DIR="../outputfiles"
FINAL_INPUT="given_input.txt"
OUTPUT_FILE=""

# Validate input file
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: Input file '$INPUT_FILE' does not exist."
    exit 1
fi

# Validate bypass mode
if [ "$BYPASS_MODE" != "noforward" ] && [ "$BYPASS_MODE" != "forward" ]; then
    echo "Error: Invalid bypass mode. Use 'noforward' or 'forward'."
    exit 1
fi

# Extract the second column (machine code) from the input file and copy it to given_input.txt
awk '{print $1}' "$INPUT_FILE" > "$FINAL_INPUT"

# Run make commands
make compile

if [ "$BYPASS_MODE" == "noforward" ]; then
    make run_bypass_forbid
    OUTPUT_FILE="${OUTPUT_DIR}/${1%.*}_no_forward.txt"
else
    make run_bypass
    OUTPUT_FILE="${OUTPUT_DIR}/${1%.*}_forward.txt"
fi

# Ensure the output directory exists
mkdir -p "$OUTPUT_DIR"

if [ -f "output.txt" ]; then
    cp output.txt "$OUTPUT_FILE"
    echo "Output written to $OUTPUT_FILE"
else
    echo "Error: output.txt not found."
    exit 1
fi

# Clean up
make clean