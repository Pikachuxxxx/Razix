import subprocess
import os

# Directories to format
directories = ["Engine", "Sandbox", "Game", "GameFramework"]
ignore_directories = ["vendor"]


# File extensions to format
extensions = (".cpp", ".h", ".hpp", ".c")

def format_files():
    for directory in directories:
        print(f"Formatting files in {directory}...")
        for root, _, files in os.walk(directory):
            for file in files:
                if file.endswith(extensions):
                    file_path = os.path.join(root, file)
                    subprocess.run(["clang-format", "-i", file_path], check=True)
    print("All files formatted!")

if __name__ == "__main__":
    format_files()