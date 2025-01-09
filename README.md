# Build PDFium with cmake


## Updating

The PDFium sources and additional third party sources which would be fetched by GN cloning a
repository into the source tree are included as overlaid git subtrees. Before updating you'll need
to find the Pdfium SHA1 for a release from
https://chromiumdash.appspot.com/commits?repo=pdfium&platform=Android. The pdfium subtree can then
be advanced with:

```
git subtree pull --prefix pdfium https://pdfium.googlesource.com/pdfium <SHA1> --squash
```

Then the overlaid repositories need to be updated to the revisions specified in
[`pdfium/DEPS`](pdfium/DEPS). The directories to be updated and their keys are:

| Repository                          | Key                  |
| ----------------------------------- | -------------------- |
| `pdfium/build`                      | `build_revision`     |
| `pdfium/third_party/abseil-cpp`     | `abseil_revision`    |
| `pdfium/third_party/fast_float/src` | `fast_float_revision`|

Update them with the commands:

```
git subtree pull --prefix pdfium/build https://chromium.googlesource.com/chromium/src/build <SHA1> --squash
git subtree pull --prefix pdfium/third_party/abseil-cpp https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp <SHA1> --squash
git subtree pull --prefix pdfium/third_party/fast_float/src https://github.com/fastfloat/fast_float.git <SHA1> --squash
```

Then build, find what fails and what fixes it. If new files need to be added to sources you'll probably see
unresolved symbols when running `ld` on the `.so` file of a desktop linux build.

Finally update the version in the `project` statement in [`CMakeLists.txt`](CMakeLists.txt).
