
#!/bin/bash

# Ensure the script stops on any errors
set -e

# Directory to search for files (default is current directory)
SEARCH_DIR=${1:-.}

# File extensions to format
EXTENSIONS=("*.cpp" "*.h" "*.hpp" "*.c" "*.cc")

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "clang-format not found! Please install it with 'brew install clang-format'."
    exit 1
fi

# Find all matching files and format them
echo "Running clang-format on source files in directory: $SEARCH_DIR"
for EXT in "${EXTENSIONS[@]}"; do
    find "$SEARCH_DIR" -type f -name "$EXT" -exec clang-format -i {} \;
done

echo "Clang-format completed successfully."
