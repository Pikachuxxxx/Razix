import subprocess
import os
import sys
from datetime import datetime

# List of test executable base names
test_names = [
    "EngineTests",
    "GfxTest-HelloTriangleTest",
    "GfxTest-HelloTextureTest",
    "GfxTest-WaveIntrinsicsTest",
]

DEFAULT_CONFIG = "Debug"
DEFAULT_PLATFORM = "windows-x86_64"
VALID_CONFIGS = {"Debug", "Release", "GoldMaster"}
VALID_PLATFORMS = {"windows-x86_64", "macos-arm64", "prospero"}

def parse_args():
    config = DEFAULT_CONFIG
    platform = DEFAULT_PLATFORM
    verbose = False

    for arg in sys.argv[1:]:
        arg = arg.lower()
        if arg in {"--release", "release"}:
            config = "Release"
        elif arg in {"--gm", "--goldmaster", "goldmaster"}:
            config = "GoldMaster"
        elif arg in VALID_CONFIGS:
            config = arg.capitalize()
        elif arg in VALID_PLATFORMS:
            platform = arg
        elif arg == "--verbose":
            verbose = True
        else:
            print(f"⚠️ Unknown argument: {arg}")
    return config, platform, verbose

def run_tests(config, platform, verbose):
    bin_root = os.path.join("bin", f"{config}-{platform}")
    results_dir = os.path.join("TestResults", platform, config)

    os.makedirs(results_dir, exist_ok=True)
    all_passed = True

    for name in test_names:
        exe_path = os.path.join(bin_root, f"{name}.exe" if platform.startswith("windows") else name)
        log_path = os.path.join(results_dir, f"{name}.log")

        if not os.path.exists(exe_path):
            print(f"❌ Missing: {exe_path}")
            all_passed = False
            continue

        print(f"\n▶ Running {exe_path}...")

        with open(log_path, 'w') as log_file:
            try:
                result = subprocess.run([exe_path], stdout=log_file, stderr=subprocess.STDOUT, check=True)
                if verbose:
                    with open(log_path) as read_back:
                        print(read_back.read())
            except subprocess.CalledProcessError:
                print(f"❌ Failed: {name} (log: {log_path})")
                all_passed = False

    if all_passed:
        print(f"\n✅ All tests passed for {config}-{platform}")
    else:
        print(f"\n❌ Some tests failed. Check logs in '{results_dir}'.")

if __name__ == "__main__":
    config, platform, verbose = parse_args()
    print(f"🧪 Running Gfx tests for: {config}-{platform}")
    run_tests(config, platform, verbose)
