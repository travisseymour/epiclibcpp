# Building and Publishing

## Quick Reference

| What you do | What happens |
|-------------|--------------|
| `python -m setuptools_scm` | Show current version |
| `./build_local.py` | Build wheel locally for your platform |
| Push to `main` | Build + test all wheels → publish to TestPyPI |
| Push a `v*.*.*` tag | Build + test all wheels → publish to PyPI |

CI automatically runs the test suite on every wheel before publishing.

---

## 1. Local Development

Build and test on your machine before pushing:

```bash
./build_local.py                           # Build wheel
./run_tests/run_test_from_local_build.py   # Test it
```

---

## 2. TestPyPI (dev builds)

Push to `main` to trigger CI:

```bash
git add . && git commit -m "Your changes"
git push origin main
```

CI builds wheels for all platforms/versions, runs tests, then uploads to TestPyPI.

To verify the TestPyPI package locally:

```bash
./run_tests/run_test_from_test_pypi.py
```

---

## 3. PyPI (releases)

Tag and push to trigger a release:

```bash
git tag v0.2.0
git push origin v0.2.0
```

CI builds, tests, and uploads to PyPI.

To verify the PyPI package locally:

```bash
./run_tests/run_test_from_pypi.py
```

---

## Versioning

Version is determined automatically from git tags (via setuptools_scm):

| Git state | Version example |
|-----------|-----------------|
| On tag `v0.2.0` | `0.2.0` |
| 3 commits after `v0.2.0` | `0.2.1.dev3+g1a2b3c4` |

Check current version:

```bash
python -m setuptools_scm
```

---

## Troubleshooting

**"Permission denied" when pushing tags:**

```bash
git remote set-url origin git@github.com:travisseymour/epiclibcpp.git
```
