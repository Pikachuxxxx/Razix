import subprocess
import os
import argparse
from datetime import datetime

test_names = [
    "EngineTests",
    "GfxTest-HelloTriangleTest",
    "GfxTest-HelloTextureTest",
    "GfxTest-WaveIntrinsicsTest",
]

DEFAULT_CONFIG = "Debug"
DEFAULT_PLATFORM = "windows-x86_64"
VALID_CONFIGS = ["Debug", "Release", "GoldMaster"]
VALID_PLATFORMS = ["windows-x86_64", "macosx-ARM64", "Prospero"]

# -------------------------------------------------------------------
# Safe print that won't crash on non-ASCII terminals (like MSBuild)
# -------------------------------------------------------------------
def safe_print(s):
    try:
        print(s)
    except UnicodeEncodeError:
        print(s.encode('ascii', errors='replace').decode())

def parse_args():
    parser = argparse.ArgumentParser(description="Gfx Test Runner ~uwu~")
    parser.add_argument("config", nargs="?", default=DEFAULT_CONFIG, choices=VALID_CONFIGS,
        help="Build config (Debug, Release, GoldMaster). Default is Debug uwu")
    parser.add_argument("platform", nargs="?", default=DEFAULT_PLATFORM, choices=VALID_PLATFORMS,
        help="Platform (windows-x86_64, macosx-ARM64, Prospero). Default is windows-x86_64 owo")
    parser.add_argument("--verbose", action="store_true", help="Show test output in console, teehee~")
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

def run_tests(config, platform, verbose):
    bin_root = os.path.join("bin", f"{config}-{platform}")
    results_dir = os.path.join("TestResults", platform, config)
    os.makedirs(results_dir, exist_ok=True)

    print_header(config, platform)
    all_passed = True

    for name in test_names:
        exe_path = os.path.join(bin_root, f"{name}.exe" if platform.startswith("windows") else name)
        log_path = os.path.join(results_dir, f"{name}.log")

        if platform.startswith("macos"):
            lib_path = os.path.abspath(f"./bin/{config}-macosx-ARM64")
            
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
            with open(log_path, "w") as log_file:
                safe_print(f"  ~(*^‿^)~ Launching test: {name}")
                subprocess.run([exe_path], stdout=log_file, stderr=subprocess.STDOUT, check=True, env=env if platform.startswith("macos") else None)
            print_result(name, "pass")
            if verbose:
                safe_print("    (づ｡◕‿‿◕｡)づ showing log content ~\n")
                with open(log_path, "r") as f:
                    for line in f:
                        safe_print("    " + line.rstrip())
        except subprocess.CalledProcessError:
            print_result(name, "fail", log_path)
            all_passed = False

    safe_print("\n" + "=" * 66)
    if all_passed:
        safe_print(" UwU All tests passed! So p-p-proud of you senpai~ (≧◡≦) ".center(66))
    else:
        safe_print(" Q_Q Some tests failed... but dw I still wuv u... Check logs~ ".center(66))
    safe_print("=" * 66 + "\n")

if __name__ == "__main__":
    args = parse_args()
    run_tests(args.config, args.platform, args.verbose)
