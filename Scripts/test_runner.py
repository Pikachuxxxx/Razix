import subprocess
import os
import argparse
from datetime import datetime
import sys

test_names = [
    "EngineTests",
    "GfxTest-HelloTriangleTest",
    "GfxTest-HelloTextureTest",
    "GfxTest-WaveIntrinsicsTest",
    "GfxTest-ComputeTest",
    "GfxTest-PrimitiveTest",
]

DEFAULT_CONFIG = "Debug"
DEFAULT_PLATFORM = "windows-x86_64"
VALID_CONFIGS = ["Debug", "GoldMaster"]
VALID_PLATFORMS = ["windows-x86_64", "windows-arm64", "macosx-arm64", "linux-x86_64", "linux-arm64", "Prospero"]

# -------------------------------------------------------------------
# Safe print that won't crash on non-ASCII terminals (like MSBuild)
# -------------------------------------------------------------------
def safe_print(s):
    try:
        print(s)
    except UnicodeEncodeError:
        print(s.encode('ascii', errors='replace').decode())

def parse_args():
    parser = argparse.ArgumentParser(description="Gfx Test Runner")
    parser.add_argument("--config", choices=VALID_CONFIGS, default=DEFAULT_CONFIG,
        help="Build config (Debug, GoldMaster). Default is Debug")
    parser.add_argument("--platform", choices=VALID_PLATFORMS, default=DEFAULT_PLATFORM,
        help="Platform. Default is windows-x86_64")
    parser.add_argument("--verbose", action="store_true", help="Show test output in console (when using log files)")
    parser.add_argument(
        "--stdout",
        action="store_true",
        help="Write test output to STDOUT instead of log files (useful for CI debugging)"
    )
    parser.add_argument("--args", nargs=argparse.REMAINDER,
                        help="Additional arguments to pass to test executables")
    parser.add_argument("--github-ci", action="store_true",
                    help="Skips engine gfx tests on github CI as it's GPU dependent")
    return parser.parse_args()

def print_header(config, platform):
    header = r"""
  ____________________________________________________________
 /                                                            \ 
|   ~*~ Gfx Test Wunnah Launching in UwU mode ~*~              |
|--------------------------------------------------------------|
|  ✧ Config   : {0:<15}                                |
|  ✧ Pwatform : {1:<15}                                |
 \____________________________________________________________/
""".format(config, platform)

    safe_print(header)
    safe_print("Ready to zoom through tests! Let's gooo~!! (ﾉ◕ヮ◕)ﾉ*:･ﾟ✧\n")

def print_result(name, result, log_path=None):
    status = {
        "pass": "[ OK UwU ]",
        "fail": "[ FAIL QwQ ]",
        "skip": "[ Skipped >w< ]"
    }
    line = f"  > Running {name.ljust(40)} ... {status[result]}"
    if log_path:
        line += f"  (log: {log_path})"
    safe_print(line)

def run_tests(github_ci, config, platform, verbose, user_args=None, to_stdout=False):
    if user_args is None:
        user_args = []

    bin_root = os.path.join("bin", f"{config}-{platform}")
    results_dir = os.path.join("TestResults", platform, config)

    # Only create results dir if we are going to write logs
    if not to_stdout:
        os.makedirs(results_dir, exist_ok=True)

    print_header(config, platform)
    all_passed = True

    for name in test_names:
        exe_path = os.path.join(bin_root, f"{name}.exe" if platform.startswith("windows") else name)
        log_path = os.path.join(results_dir, f"{name}.log") if not to_stdout else None

        # Skip GPU-dependent tests on GitHub CI
        if github_ci and name.startswith("GfxTest"):
            print_result(name, "skip")
            continue

        # Handle library path for macOS
        env = None
        if platform.startswith("macosx"):
            lib_path = os.path.abspath(f"./bin/{config}-{platform}")

            if not os.path.exists(os.path.join(lib_path, "libRazix.dylib")):
                print(f"[WARNING] libRazix.dylib not found in {lib_path}")

            env = os.environ.copy()
            prev_dyld = env.get("DYLD_LIBRARY_PATH", "")
            combined_dyld = f"{lib_path}:{prev_dyld}" if prev_dyld else lib_path
            env["DYLD_LIBRARY_PATH"] = combined_dyld

            print(f"[INFO] Set DYLD_LIBRARY_PATH={combined_dyld}")

        if not os.path.exists(exe_path):
            print(os.getcwd())
            print(exe_path)
            print_result(name, "skip")
            all_passed = False
            continue

        try:
            safe_print(f"  ~(*^‿^)~ Launching test: {name}")
            cmd = [exe_path] + user_args
            print(f"[INFO] Running: {' '.join(cmd)}")

            if to_stdout:
                # Stream directly to console
                subprocess.run(cmd, check=True, env=env)
            else:
                # Log to file
                with open(log_path, "w") as log_file:
                    subprocess.run(
                        cmd,
                        stdout=log_file,
                        stderr=subprocess.STDOUT,
                        check=True,
                        env=env
                    )

            print_result(name, "pass", None if to_stdout else log_path)

            # Verbose only makes sense when using log files
            if verbose and not to_stdout and log_path:
                safe_print("    (づ｡◕‿‿◕｡)づ showing log content ~\n")
                with open(log_path, "r") as f:
                    for line in f:
                        safe_print("    " + line.rstrip())

        except subprocess.CalledProcessError:
            print_result(name, "fail", None if to_stdout else log_path)
            all_passed = False

    safe_print("\n" + "=" * 66)
    if all_passed:
        safe_print(" UwU All tests passed! So p-p-proud of you senpai~ (≧◡≦) ".center(66))
    else:
        safe_print(" Q_Q Some tests failed... but dw I still wuv u... Check logs~ ".center(66))
        sys.exit(69)
    safe_print("=" * 66 + "\n")
    sys.exit(0)  # Success


if __name__ == "__main__":
    args = parse_args()
    run_tests(
        args.github_ci,
        args.config,
        args.platform,
        args.verbose,
        args.args,
        to_stdout=args.stdout,
    )
