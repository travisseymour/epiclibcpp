#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== Building local wheel ==="

# Clean previous builds
rm -rf dist/ build/ *.egg-info

# Install build if needed
pip install --quiet build

# Build the wheel
python -m build --wheel

echo ""
echo "=== Build successful ==="
echo "Wheel(s) created in dist/:"
ls -la dist/*.whl

echo ""
echo "To test the local build, run:"
echo "  ./run_tests/run_test_from_local_build.py"
