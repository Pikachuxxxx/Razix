import argparse
import os
import platform
import subprocess
import sys

def run_command(cmd, cwd=None):
    try:
        print(f"[INFO] Running: {' '.join(cmd)}")
        subprocess.check_call(cmd, cwd=cwd)
    except subprocess.CalledProcessError as e:
        print(f"[ERROR] Command failed with exit code {e.returncode}")
        sys.exit(e.returncode)

def build_windows(config):
    msbuild_path = r"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    if not os.path.exists(msbuild_path):
        print("[ERROR] MSBuild not found. Please check the path.")
        sys.exit(1)

    # Recursively find all .sln files in ./build
    for root, _, files in os.walk("./build"):
        for file in files:
            if file.endswith(".sln"):
                sln_path = os.path.join(root, file)
                run_command([
                    msbuild_path,
                    sln_path,
                    f"/p:Configuration={config}",
                    "/m"
                ])

def build_macos(config):
    # Recursively find all .xcodeproj in ./build
    for root, _, files in os.walk("./build"):
        for file in files:
            if file.endswith(".xcodeproj"):
                project_name = file
                project_path = os.path.join(root, file)

                run_command([
                    "xcodebuild",
                    "-project", project_path,
                    "-configuration", config,
                    "build"
                ])

def main():
    parser = argparse.ArgumentParser(description="Cross-platform project builder")
    parser.add_argument("--platform", choices=["windows", "macos"], required=True,
                        help="Target platform: windows or macos")
    parser.add_argument("--config", choices=["Debug", "Release", "GoldMaster"], default="Debug",
                        help="Build configuration")

    args = parser.parse_args()

    if args.platform == "windows":
        if platform.system() != "Windows":
            print("[ERROR] You're not on Windows but selected platform = windows.")
            sys.exit(1)
        build_windows(args.config)
    elif args.platform == "macos":
        if platform.system() != "Darwin":
            print("[ERROR] You're not on macOS but selected platform = macos.")
            sys.exit(1)
        build_macos(args.config)

if __name__ == "__main__":
    main()
