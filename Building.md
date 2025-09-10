# Building and Pushing to PyPI

1. Make changes to module  
2. Update the version in `pyproject.toml`.

---

## If you want to push to **TestPyPI**

3. Set a **dev version** (must include `.dev`), e.g.

```toml
[project]
name = "epiclibcpp"
version = "0.1.0.dev1"
```

4. **Commit and push to `main`.**  
   → This builds wheels and uploads to **TestPyPI**. It can take ~30 minutes.

5. **Test install from TestPyPI** (requires `uv`):

```bash
./run_test_from_test_pypi.py
```

> If that dev version already exists on TestPyPI, bump the suffix (e.g., `.dev2`).

---

## If you want to push to the **regular PyPI**

3. Set a **release version** (no `.dev`), e.g.

```toml
[project]
name = "epiclibcpp"
version = "0.1.0"
```

4. **Commit and push to `main`.**  
   → This builds wheels (no upload yet).

5. **Tag the same commit and push the tag** to trigger the PyPI upload:

```bash
git tag v0.1.0 && git push origin v0.1.0
```

6. **Test install from PyPI**:

```bash
./run_test_from_pypi.py
```

---

## Summary

- Branch push with a `.dev*` version → builds and uploads to **TestPyPI**.  
- Tag push with a non-dev version → builds and uploads to **PyPI**.  
- Always increment your version number before publishing to avoid conflicts.
