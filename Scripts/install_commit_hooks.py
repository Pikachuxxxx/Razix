# install_commit_hook.py
import os

HOOKS_DIR = os.path.join(".git", "hooks")
HOOK_FILE = os.path.join(HOOKS_DIR, "commit-msg")

# This script installs a commit-msg hook that enforces Conventional Commit style with components
HOOK_SCRIPT = """#!/usr/bin/env python3
import sys
import re

commit_msg_file = sys.argv[1]

with open(commit_msg_file, 'r') as f:
    msg = f.readline().strip()

pattern = r"^(feat|fix|refactor|docs|chore|test|perf)\\((core|gfx|rhi|audio|ui|build|tools|vfs|net|ai|physics|gameplay|art|asset)\\): .+"

if not re.match(pattern, msg):
    print("Invalid commit message format!")
    print("Expected: <type>(<component>): <description>")
    print("Types: feat, fix, refactor, docs, chore, test, perf")
    print("Components: core, gfx, rhi, audio, ui, build, tools, vfs, net, ai, physics, gameplay, art, asset")
    sys.exit(1)
"""

def install_hook():
    os.makedirs(HOOKS_DIR, exist_ok=True)
    # Write hook with Unix line endings to avoid CRLF issues
    with open(HOOK_FILE, "w", newline='\n') as f:
        f.write(HOOK_SCRIPT)
    os.chmod(HOOK_FILE, 0o755)
    print("Git commit-msg hook installed.")

if __name__ == "__main__":
    install_hook()

