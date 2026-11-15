# EPICLibCpp

> Python Module Interface for Dave Kieras's C++ version of epiclib.

<mark>NOTE</mark>: This is not 1:1 with DK's epiclib. Several tweaks were used to facilitate interop wth Python.

---

## Dev Notes

This is the EPIC cognitive architecture C++ core library wrapped with pybind11 as the `epiclib` Python extension.

### Install

```bash
python -m pip install epiclibcpp
```

### Development

#### Releasing a New Version

1. Update the version in `pyproject.toml`:
   ```toml
   [project]
   version = "0.2.2"
   ```
2. Commit and push to `main`:
   ```bash
   git add pyproject.toml
   git commit -m "Bump version to 0.2.2"
   git push origin main
   ```
3. CI automatically builds wheels for Linux, macOS (Intel + ARM), and Windows for Python 3.11–3.14, and publishes them to **PyPI**.

#### Tagging (Optional)

Git tags are optional and serve as documentation for significant milestones:

```bash
git tag v0.2.2
git push origin v0.2.2
```

> Note: Tags do not control the version number. The version is always read from `pyproject.toml`.
> Check existing tags with `git tag -l "v*"`

### Example

```bash
python -m pip install -U epiclibcpp
python -c "import epiclibcpp.epiclib, importlib.metadata as m; print('epiclibcpp:', epiclibcpp, 'version:', m.version('epiclibcpp'))"
```

### Project Links

- [Homepage](https://github.com/travisseymour/epiclibcpp)
- [Issues](https://github.com/travisseymour/epiclibcpp/issues)
