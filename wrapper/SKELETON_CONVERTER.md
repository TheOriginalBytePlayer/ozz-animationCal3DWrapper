# CalCoreSkeleton to ozz::animation::Skeleton Converter

This module provides functions to convert Cal3D's `CalCoreSkeleton` format to ozz-animation's `Skeleton` format.

## Overview

The conversion functions are located in `wrapper/src/skeleton_converter.cpp` and are declared in the `WrapperUtil` namespace in `wrapper/include/cal3d_wrapper_internal.h`.

## Functions

### BuildRawSkeletonFromCal3D

Converts a `CalCoreSkeleton` to an ozz-animation `RawSkeleton` (the offline intermediate format).

```cpp
ozz::animation::offline::RawSkeleton* BuildRawSkeletonFromCal3D(
    const CalCoreSkeleton* coreSkeleton);
```

**Parameters:**
- `coreSkeleton`: Pointer to a Cal3D `CalCoreSkeleton` structure

**Returns:**
- Pointer to a newly allocated `RawSkeleton`, or `nullptr` if conversion fails
- The caller is responsible for deallocating the returned skeleton using `ozz::Delete()`

**Features:**
- Converts bone hierarchy (parent-child relationships)
- Preserves bone names
- Copies local transformations (translation and rotation)
- Handles multiple root bones
- Validates the skeleton structure

### BuildOzzSkeleton

Converts a `CalCoreSkeleton` directly to an ozz-animation runtime `Skeleton`.

```cpp
ozz::animation::Skeleton* BuildOzzSkeleton(
    const CalCoreSkeleton* coreSkeleton);
```

**Parameters:**
- `coreSkeleton`: Pointer to a Cal3D `CalCoreSkeleton` structure

**Returns:**
- Pointer to a newly allocated runtime `Skeleton`, or `nullptr` if conversion fails
- The caller should use `ozz::unique_ptr` to manage the returned skeleton

**Features:**
- Performs a complete conversion from Cal3D to ozz runtime format
- Internally uses `BuildRawSkeletonFromCal3D` followed by `SkeletonBuilder`
- Returns an optimized, runtime-ready skeleton

## Usage Example

```cpp
#include "cal3d_wrapper_internal.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/base/memory/allocator.h"

// Create a CalCoreSkeleton
CalCoreSkeleton coreSkeleton;

// ... populate coreSkeleton with bones ...

// Method 1: Convert to RawSkeleton (for further processing)
ozz::animation::offline::RawSkeleton* rawSkeleton = 
    WrapperUtil::BuildRawSkeletonFromCal3D(&coreSkeleton);

if (rawSkeleton) {
    // Use rawSkeleton...
    ozz::Delete(rawSkeleton);
}

// Method 2: Convert directly to runtime Skeleton (recommended)
ozz::unique_ptr<ozz::animation::Skeleton> skeleton(
    WrapperUtil::BuildOzzSkeleton(&coreSkeleton));

if (skeleton) {
    // Use skeleton for animation...
    int numJoints = skeleton->num_joints();
    auto jointNames = skeleton->joint_names();
}
```

## CalCoreSkeleton Structure Requirements

For successful conversion, the `CalCoreSkeleton` must have:

1. **Non-empty bone list**: At least one bone in `bones` vector
2. **Valid bone hierarchy**: Each bone's `parentId` must reference a valid parent or be -1 for root bones
3. **Root bone indices**: `rootBoneIds` should contain indices of all root bones (bones with `parentId == -1`)
4. **Child relationships**: Each bone's `childIds` should list the indices of its children
5. **Valid transformations**: Each bone should have valid `translation` and `rotation` values

## Mapping Details

### Bone to Joint Mapping

| Cal3D (CalCoreBone) | ozz-animation (Joint) |
|---------------------|----------------------|
| `name` | `name` |
| `translation` | `transform.translation` |
| `rotation` | `transform.rotation` |
| N/A | `transform.scale` (set to 1,1,1) |
| `childIds` | `children` vector |
| `parentId` | Implicit in hierarchy |

### Coordinate System

Both Cal3D and ozz-animation use:
- Right-handed coordinate system
- Quaternions in (x, y, z, w) format
- Local (relative to parent) transformations

## Testing

Tests for the conversion functions are located in `test/wrapper/skeleton_converter_tests.cc`.

To run the tests:
```bash
cd build
make test_skeleton_converter
./test/wrapper/test_skeleton_converter
```

## Example Program

A complete working example is available in `howtos/cal3d_skeleton_converter.cc`.

To build and run:
```bash
cd build
make cal3d_skeleton_converter
./howtos/cal3d_skeleton_converter
```

## Notes

- The conversion functions use ozz memory allocators (`ozz::New` and `ozz::Delete`)
- Scale is not typically used in Cal3D skeletons, so it defaults to (1, 1, 1)
- The conversion validates the skeleton structure before returning
- Memory management: Use `ozz::Delete()` for `RawSkeleton` and `ozz::unique_ptr` for `Skeleton`
