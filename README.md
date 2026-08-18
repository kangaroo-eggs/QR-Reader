# QR (CMake) — build & run instructions

This folder contains a CMake build wrapper so you can build the QR project without Qt/qmake.

Prerequisites (macOS):
- CMake
- OpenCV installed (Homebrew or system)

Quick build from workspace root:

```bash
# configure
cmake -S QR -B QR/build
# build
cmake --build QR/build --config Debug -j$(sysctl -n hw.ncpu)
# run
./QR/build/qr
```

If Homebrew OpenCV is installed and CMake cannot find it, set `OpenCV_DIR` to the installed OpenCV cmake folder, e.g.:

```bash
export OpenCV_DIR=$(brew --prefix opencv)/lib/cmake/opencv4
cmake -S QR -B QR/build
```
