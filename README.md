# EPICLibCpp

> Python Module Interface for Dave Kieras's C++ version of epiclib. 

<mark>NOTE</mark>: It's not Not 1:1 with DK's epiclib, several tweaks were used to facilitate interop wth Python.

---

## Dev Notes

This is the EPIC cognitive architecture C++ core library wrapped with pybind11 as the `epiclib` Python extension.

### Install (development builds from TestPyPI)

```bash
python -m pip install -i https://test.pypi.org/simple epiclibcpp
python -c "import epiclib, importlib.metadata as m; print('epiclib:', epiclib, 'version:', m.version('epiclibcpp'))"
```

### Install (official releases from PyPI)

```bash
python -m pip install epiclibcpp
```

### Development

- Pushes to `main` automatically build wheels for Linux, macOS (Intel + ARM), and Windows for Python 3.10–3.13, and publish them to **TestPyPI**.
- Git tags like `v0.1.0` trigger a release build and upload wheels to **PyPI**.

e.g.:

```bash
git tag v0.1.0
git push origin v0.1.0
```

### Example

```python
import epiclib
print("epiclib loaded:", epiclib)
```

### Project Links

- [Homepage](https://github.com/travisseymour/epiclibcpp)
- [Issues](https://github.com/travisseymour/epiclibcpp/issues)
