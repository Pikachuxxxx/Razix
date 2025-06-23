import argparse
import os
import platform
import subprocess
import sys
import multiprocessing
import glob

# These project names (without extension) will be skipped
BLACKLISTED_PROJECTS = {
    "Premake Re-Generate Project Files",
}

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

def has_xcode_targets(xcodeproj_path):
    try:
        result = subprocess.run(
            ["xcodebuild", "-list", "-project", xcodeproj_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=5
        )
        if result.returncode != 0:
            print(f"[WARN] Could not query targets for {xcodeproj_path}")
            return False

        # Look for section like:
        # Targets:
        #     Target1
        return "Targets:\n" in result.stdout or re.search(r"Targets:\s*\n\s+\w+", result.stdout)

    except Exception as e:
        print(f"[ERROR] Failed to inspect {xcodeproj_path}: {e}")
        return False

def output_exists_for(project_name, config):
    bin_path = os.path.abspath(f"./bin/{config}-macosx-ARM64")

    # Try to find a matching .dylib or .app or binary
    patterns = [
        f"lib{project_name}.dylib",    # dynamic lib
        f"{project_name}.app",         # macOS bundle
        f"{project_name}"              # plain binary
    ]

    for pattern in patterns:
        if glob.glob(os.path.join(bin_path, pattern)):
            return True
    return False

def build_macos(config):
    build_dir = "./build"
    for entry in os.listdir(build_dir):
        if entry.endswith(".xcodeproj"):
            project_path = os.path.join(build_dir, entry)
            project_name = os.path.splitext(entry)[0]
            if not has_xcode_targets(project_path):
                print(f"[INFO] Skipping {project_path} (no targets found)")
                continue
            
            if output_exists_for(project_name, config):
                print(f"[INFO] Skipping {project_name} (already built)")
                continue
            
            if project_name in BLACKLISTED_PROJECTS:
                print(f"[INFO] Skipping {project_name} (blacklisted)")
                continue
            
            jobs = str(multiprocessing.cpu_count())
            print(f"[INFO] Using -jobs {jobs} for parallel compilation")

            print(f"[INFO] Building Xcode project: {project_path}")
            run_command([
                "xcodebuild",
                "-project", project_path,
                "-configuration", config,
                "build",
                "-jobs", jobs,
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
