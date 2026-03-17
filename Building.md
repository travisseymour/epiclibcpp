# Building and Pushing to PyPI

## How Versioning Works

This project uses **setuptools_scm** to automatically determine the version from git tags. You do not manually set a version in `pyproject.toml`.

- Tagged commits (e.g., `v0.1.0`) get that exact version
- Commits after a tag get a dev version (e.g., `0.1.1.dev3+g1234567`)

To check the current version locally:

```bash
pip install setuptools_scm  # if not already installed
python -m setuptools_scm
```

---

## Local Testing (without uploading)

Build and test locally before pushing anywhere:

1. Build the wheel:

```bash
./build_local.sh
```

2. Test the local build:

```bash
./run_tests/run_test_from_local_build.py
```

This builds a wheel for your current platform only and tests it in a temporary virtualenv.

---

## Pushing to TestPyPI

Any push to the `main` branch automatically builds wheels and uploads to **TestPyPI**.

1. Make changes to module
2. Commit and push to `main`
   → Wheels are built and uploaded to TestPyPI (takes ~30-45 minutes)
3. Test install from TestPyPI:

```bash
./run_test_from_test_pypi.py
```

---

## Pushing to PyPI

Pushing a version tag triggers a build and upload to **PyPI**.

1. Make changes to module
2. Commit and push to `main`
3. Tag the commit and push the tag:

```bash
git tag v0.1.0
git push origin v0.1.0
```

**If** `git push origin ...` gives a "Permission denied" error, you may need to run:

```bash
git remote set-url origin git@github.com:travisseymour/epiclibcpp.git
```

4. Test install from PyPI:

```bash
./run_test_from_pypi.py
```

---

## Summary

| Action | Destination |
|--------|-------------|
| Push to `main` branch | TestPyPI |
| Push a `v*.*.*` tag | PyPI |

Always increment your version tag before publishing to PyPI to avoid conflicts.
