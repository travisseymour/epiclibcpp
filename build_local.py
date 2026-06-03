#!/usr/bin/env -S uv run --python 3.12
# /// script
# requires-python = ">=3.12"
# dependencies = ["build"]
# ///

"""Build a local wheel for epiclibcpp using `uv run build_local.py`."""

import glob
import os
import shutil
import subprocess
import sys
from pathlib import Path


def main():
    script_dir = Path(__file__).resolve().parent
    os.chdir(script_dir)

    print("=== Building local wheel ===\n")

    # Clean previous builds
    for d in ("dist", "build"):
        if Path(d).is_dir():
            shutil.rmtree(d)
    for egg in glob.glob("*.egg-info"):
        shutil.rmtree(egg)

    # Build the wheel
    result = subprocess.run(
        [sys.executable, "-m", "build", "--wheel"],
        check=False,
    )

    if result.returncode != 0:
        print("\n=== Build FAILED ===")
        sys.exit(result.returncode)

    wheels = glob.glob("dist/*.whl")
    print("\n=== Build successful ===")
    print("Wheel(s) created in dist/:")
    for whl in wheels:
        size = os.path.getsize(whl)
        print(f"  {whl}  ({size:,} bytes)")

    print("\nTo test the local build, run:")
    print("  ./run_tests/run_test_from_local_build.py")


if __name__ == "__main__":
    main()
