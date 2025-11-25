import subprocess
import os
import sys

# Directories to format
directories = ["Engine", "Sandbox", "Game", "GameFramework"]

# Directories to ignore
ignore_dirs = ["vendor", "dxc"]

# File extensions to format
extensions = (".cpp", ".h", ".hpp", ".c", ".hlsl")

# Path to the .clang-format file (same folder as script)
CUR_DIR = os.path.dirname(os.path.abspath(__file__))
PARENT_DIR = os.path.dirname(CUR_DIR)
CLANG_FORMAT_CONFIG = os.path.join(PARENT_DIR, ".clang-format")

def should_ignore_path(path):
    return any(ignored in path.split(os.sep) for ignored in ignore_dirs)

def format_files():
    if not os.path.isfile(CLANG_FORMAT_CONFIG):
        print(f"❌ Error: .clang-format not found at {CLANG_FORMAT_CONFIG}")
        sys.exit(1)  # Exit with error

    for directory in directories:
        print(f"🔍 Formatting files in {directory}...")
        for root, dirs, files in os.walk(directory):
            dirs[:] = [d for d in dirs if not should_ignore_path(os.path.join(root, d))]

            if should_ignore_path(root):
                continue

            for file in files:
                if file.endswith(extensions):
                    file_path = os.path.join(root, file)
                    print(f"🛠️  Formatting: {file_path}")
                    subprocess.run([
                        "clang-format",
                        "-i",
                        "-style=file",
                        f"-assume-filename={CLANG_FORMAT_CONFIG}",
                        file_path
                    ], check=True)
    print("✅ All files formatted!")

if __name__ == "__main__":
    format_files()
