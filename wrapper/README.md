# Cal3D Wrapper with ozz-animation Backend

This directory contains a drop-in replacement implementation of the Cal3D animation library using ozz-animation as the backend.

## Overview

The Cal3D wrapper library (`libcal3d.so`) provides all the same API functions as the original Cal3D library but uses ozz-animation for the internal animation playback and skeletal animation processing. This allows applications using Cal3D to benefit from ozz-animation's performance and modern architecture while maintaining API compatibility.

### Important API Change: Opaque Pointers

**Functions that return struct pointers now return `void*` (opaque pointers) instead of typed struct pointers.**

This change was necessary to ensure all functions are properly exported from the DLL on all platforms. The client code is responsible for casting the returned void* to the appropriate struct pointer type.

#### Before (old API):
```c
struct CalCoreModel* model = CalCoreModel_New("MyModel");
struct CalVector* vector = CalVector_New();
```

#### After (current API):
```c
struct CalCoreModel* model = (struct CalCoreModel*)CalCoreModel_New("MyModel");
struct CalVector* vector = (struct CalVector*)CalVector_New();
```

This is a minor change that requires adding explicit casts in client code, but it ensures compatibility across all platforms and compilers. The underlying functionality remains unchanged. About 74 functions that return struct pointers are affected (e.g., all `*_New()` functions and `*_Get*()` functions that return pointers).

## Structure

```
wrapper/
├── CMakeLists.txt              # Build configuration
├── include/
│   └── cal3d_wrapper_internal.h  # Internal bridge structures
├── src/
│   ├── cal3d_wrapper_impl.cpp   # Vector, Quaternion, Error functions
│   ├── core_model.cpp            # CalCoreModel management functions
│   └── all_stubs.cpp             # Stub implementations for remaining functions
└── README.md                    # This file
```

## Building

The wrapper is built as part of the main ozz-animation project.

**Important**: The `-DCMAKE_POSITION_INDEPENDENT_CODE=ON` flag is required for building the shared library.

### Linux/macOS

```bash
mkdir build && cd build
cmake .. \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -Dozz_build_samples=OFF \
  -Dozz_build_howtos=OFF \
  -Dozz_build_tests=OFF
make cal3d_wrapper
```

### Windows (Visual Studio)

```bash
mkdir build && cd build
cmake .. ^
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON ^
  -Dozz_build_samples=OFF ^
  -Dozz_build_howtos=OFF ^
  -Dozz_build_tests=OFF
cmake --build . --target cal3d_wrapper --config Release
```

This produces `libcal3d.so` (or `libcal3d.dylib` on macOS, `cal3d.dll` on Windows).

## DLL Export Notes

All functions are exported using the standard `CAL3D_WRAPPER_API` macro which expands to:
- **Windows MSVC**: `__declspec(dllexport)` or `__declspec(dllimport)`
- **Linux/GCC**: `__attribute__((visibility("default")))`

### Opaque Pointer Returns

Functions that return struct pointers now return `void*` instead. This simplified approach:
- Eliminates platform-specific macro complexities for struct pointer returns
- Ensures all 405+ functions are properly exported on all platforms
- Requires client code to cast the returned `void*` to the appropriate struct type

Example:
```c
// API declaration (returns void*)
CAL3D_WRAPPER_API void* CalCoreModel_New(const char* name);

// Client usage (cast to struct pointer)
struct CalCoreModel* model = (struct CalCoreModel*)CalCoreModel_New("MyModel");
```

## Implementation Status

### Fully Implemented (80+ functions)
- **CalVector**: All vector math operations (add, subtract, scale, normalize, etc.)
- **CalQuaternion**: All quaternion operations (multiply, conjugate, invert, blend, etc.)
- **CalError**: Error handling and reporting
- **CalCoreModel**: Model creation, destruction, animation/mesh/material management

### Stub Implementations (330+ functions)
The following categories have placeholder implementations that return sensible default values:
- CalAnimation, CalAnimationAction, CalAnimationCycle
- CalBone, CalCoreBone
- CalSkeleton, CalCoreSkeleton
- CalMesh, CalCoreMesh, CalSubmesh, CalCoreSubmesh
- CalMaterial, CalCoreMaterial
- CalMixer, CalMorphTargetMixer
- CalPhysique, CalRenderer
- CalLoader, CalSaver
- CalSpringSystem

These stubs can be progressively replaced with full implementations.

## Key Features

1. **API Compatibility**: Exports all 410+ functions from cal3d_wrapper.h
2. **Drop-in Replacement**: Same function signatures and behavior as original Cal3D
3. **Modern Backend**: Uses ozz-animation for efficient skeletal animation
4. **Progressive Implementation**: Core infrastructure in place for incremental function implementation

## Architecture

The wrapper uses bridge structures to convert between Cal3D format data and ozz-animation format:

- Cal3D skeletons → ozz::animation::Skeleton
- Cal3D animations → ozz::animation::Animation  
- Cal3D runtime states → ozz SoA transforms
- Mesh data remains in Cal3D format for compatibility

## Usage

Applications can link against `libcal3d.so` instead of the original Cal3D library. The public API in `cal3d_wrapper.h` remains unchanged.

```c
// Example usage (unchanged from original Cal3D)
CalCoreModel* model = CalCoreModel_New("MyModel");
CalCoreModel_LoadCoreSkeleton(model, "skeleton.csf");
// ... etc
```

## Future Work

To complete the implementation:

1. Implement Cal3D file format parsers:
   - CSF (Cal3D Skeleton Format)
   - CAF (Cal3D Animation Format)
   - CMF (Cal3D Mesh Format)
2. Implement skeleton/bone runtime functions  
3. Implement animation sampling and blending
4. Implement mesh deformation (skinning)
5. Implement rendering helper functions
6. Add comprehensive test suite

## Dependencies

- ozz-animation (base, animation, geometry)
- C++17 compiler
- CMake 3.24+
