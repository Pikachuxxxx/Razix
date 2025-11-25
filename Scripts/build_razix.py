import argparse
import os
import platform
import subprocess
import sys
import multiprocessing
import glob
import re

# These project names (without extension) will be skipped
BLACKLISTED_PROJECTS = {
    "Premake Re-Generate Project Files",
}

def run_command(cmd, cwd=None):
    """Run a command, optionally in a given working directory, and exit on failure."""
    print(f"[COMMAND] {' '.join(cmd)} (cwd={cwd or os.getcwd()})")
    result = subprocess.run(cmd, cwd=cwd, shell=False)
    if result.returncode != 0:
        print(f"[ERROR] Command failed with exit code {result.returncode}")
        sys.exit(result.returncode)

def is_inside_scripts_dir():
    # Modify this according to your actual detection logic
    return os.path.basename(os.getcwd()).lower() == "scripts"

def build_windows(config, github_ci=False):
    print(f"[INFO] Starting Windows build with config: {config}, GitHub CI: {github_ci}")

    # Choose MSBuild path based on context
    if github_ci:
        msbuild_path = r"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
        print(f"[INFO] Using GitHub CI Enterprise MSBuild path: {msbuild_path}")
    else:
        msbuild_path = r"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
        print(f"[INFO] Using local Community MSBuild path: {msbuild_path}")
    
    # Check if MSBuild exists
    if not os.path.exists(msbuild_path):
        print(f"[ERROR] MSBuild not found at {msbuild_path}. Please verify Visual Studio installation.")
        sys.exit(1)
    
    # Determine the build directory
    build_dir = "../build" if is_inside_scripts_dir() else "./build"
    print(f"[INFO] Build directory resolved to: {build_dir}")

    if not os.path.exists(build_dir):
        print(f"[WARNING] Build directory {build_dir} does not exist. No solution files to build.")
        sys.exit(0)

    found_any_solution = False

    # Walk and compile all solution files
    for root, _, files in os.walk(build_dir):
        for file in files:
            if file.endswith(".sln"):
                found_any_solution = True
                sln_path = os.path.join(root, file)
                print(f"[INFO] Found solution: {sln_path}")
                print(f"[INFO] Running MSBuild on: {sln_path}")
                run_command([
                    msbuild_path,
                    sln_path,
                    f"/p:Configuration={config}",
                    "/m"
                ])

    if not found_any_solution:
        print(f"[WARNING] No .sln files found in {build_dir}")
    else:
        print(f"[SUCCESS] Finished building all .sln files in {build_dir}")

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

def output_exists_for(project_name, config, platform_string="macosx-ARM64"):
    """Check if output exists for a given project and config"""
    bin_path = os.path.abspath(f"./bin/{config}-{platform_string}")

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

def build_macos_xcode(config, platform_string="macosx-arm64"):
    """Build macOS projects using xcodebuild."""
    build_dir = "./build"
    if not os.path.exists(build_dir):
        print(f"[WARNING] Build directory {build_dir} does not exist. No Xcode projects to build.")
        sys.exit(0)

    for entry in os.listdir(build_dir):
        if entry.endswith(".xcodeproj"):
            project_path = os.path.join(build_dir, entry)
            project_name = os.path.splitext(entry)[0]
            if not has_xcode_targets(project_path):
                print(f"[INFO] Skipping {project_path} (no targets found)")
                continue
            
            if output_exists_for(project_name, config, platform_string):
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
                "-parallelizeTargets",
                "-UseModernBuildSystem=YES",
                "CODE_SIGNING_ALLOWED=NO",
                "CODE_SIGN_IDENTITY=",
                "CODE_SIGN_STYLE=Manual",
                "DEVELOPMENT_TEAM=",
                "-jobs", jobs,
            ])

def get_macos_ncpu():
    """Get number of CPUs on macOS using sysctl -n hw.ncpu, fallback to multiprocessing."""
    try:
        result = subprocess.run(
            ["sysctl", "-n", "hw.ncpu"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=True
        )
        ncpu_str = result.stdout.strip()
        ncpu = int(ncpu_str)
        if ncpu > 0:
            return ncpu
    except Exception as e:
        print(f"[WARN] Failed to get hw.ncpu via sysctl, falling back to multiprocessing: {e}")
    return multiprocessing.cpu_count()

def build_macos_make(config):
    """Build for macOS using Makefiles under ./build, with -j$(sysctl -n hw.ncpu)."""
    build_dir = "./build"
    if not os.path.exists(build_dir):
        print(f"[WARNING] Build directory {build_dir} does not exist. No makefiles to build.")
        sys.exit(0)

    ncpu = get_macos_ncpu()
    print(f"[INFO] macOS make build: using -j{ncpu} (from hw.ncpu)")

    found_makefile = False
    for root, _, files in os.walk(build_dir):
        if "Makefile" in files:
            found_makefile = True
            makefile_path = os.path.join(root, "Makefile")
            print(f"[INFO] Found Makefile: {makefile_path}")
            make_cmd = ["make", f"config={config.lower()}", f"-j{ncpu}"]
            print(f"[INFO] Running make: {' '.join(make_cmd)}")
            run_command(make_cmd, cwd=root)

    if not found_makefile:
        print(f"[WARNING] No Makefiles found in {build_dir}")
    else:
        print(f"[SUCCESS] Finished building all Makefiles in {build_dir}")

def build_linux(config):
    """Build for Linux using make or similar build system."""
    build_dir = "./build"
    
    if not os.path.exists(build_dir):
        print(f"[WARNING] Build directory {build_dir} does not exist. No makefiles to build.")
        sys.exit(0)
    
    found_makefile = False
    jobs = multiprocessing.cpu_count()
    print(f"[INFO] Linux make build: using -j{jobs}")

    for root, _, files in os.walk(build_dir):
        if "Makefile" in files:
            found_makefile = True
            makefile_path = os.path.join(root, "Makefile")
            print(f"[INFO] Found Makefile: {makefile_path}")
            make_cmd = ["make", f"config={config.lower()}", f"-j{jobs}"]
            print(f"[INFO] Running make: {' '.join(make_cmd)}")
            run_command(make_cmd, cwd=root)
    
    if not found_makefile:
        print(f"[WARNING] No Makefiles found in {build_dir}")
    else:
        print(f"[SUCCESS] Finished building all Makefiles in {build_dir}")

def main():
    parser = argparse.ArgumentParser(description="Cross-platform project builder")
    parser.add_argument(
        "--platform",
        choices=["windows-x86_64", "windows-arm64", "macosx-arm64", "linux-x86_64", "linux-arm64"],
        required=True,
        help="Target platform"
    )
    parser.add_argument(
        "--config",
        choices=["Debug", "GoldMaster"],
        default="Debug",
        help="Build configuration"
    )
    parser.add_argument(
        "--github-ci",
        action="store_true",
        help="Use GitHub CI MSBuild path (Enterprise edition)"
    )
    parser.add_argument(
        "--args",
        nargs=argparse.REMAINDER,
        help="Additional arguments to pass to the build executable"
    )
    parser.add_argument(
        "--mac-build-make",
        action="store_true",
        help="On macOS, build using Makefiles under ./build instead of xcodebuild"
    )

    args = parser.parse_args()

    # Extract platform family for backward compatibility
    platform_family = args.platform.split('-')[0]  # windows-x86_64 -> windows

    if platform_family == "windows":
        if platform.system() != "Windows":
            print(f"[ERROR] You're not on Windows but selected platform = {args.platform}.")
            sys.exit(1)
        build_windows(args.config, args.github_ci)

    elif platform_family == "macosx":
        if platform.system() != "Darwin":
            print(f"[ERROR] You're not on macOS but selected platform = {args.platform}.")
            sys.exit(1)

        if args.mac_build_make:
            print("[INFO] macOS build mode: Makefiles (gmake2).")
            build_macos_make(args.config)
        else:
            print("[INFO] macOS build mode: Xcode projects (xcodebuild).")
            build_macos_xcode(args.config, args.platform)

    elif platform_family == "linux":
        if platform.system() != "Linux":
            print(f"[ERROR] You're not on Linux but selected platform = {args.platform}.")
            sys.exit(1)
        build_linux(args.config)

    else:
        print(f"[ERROR] Unsupported platform: {args.platform}")
        sys.exit(1)

if __name__ == "__main__":
    main()

