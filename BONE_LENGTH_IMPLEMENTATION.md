# Bone Length Calculation Implementation

## Summary

This implementation provides methods to calculate the lengths of bones in a Cal3D skeleton, taking into account both **TRANSFORMATIONS** (absolute positions in world space) and **LOCALTRANSFORMATIONS** (relative positions to parent bones).

## Implementation Files

### Core Implementation
- **cal3d-src/src/cal3d/corebone.h** - Added `calculateLength()` method declaration
- **cal3d-src/src/cal3d/corebone.cpp** - Added `calculateLength()` method implementation
- **cal3d-src/src/cal3d/coreskeleton.h** - Added `calculateBoneLengths()` method declaration
- **cal3d-src/src/cal3d/coreskeleton.cpp** - Added `calculateBoneLengths()` method implementation
- **cal3d-src/src/cal3d/global.h** - Enabled `Cal` namespace alias for proper compilation

### Documentation and Examples
- **cal3d-src/BONE_LENGTH_ALGORITHM.md** - Comprehensive documentation of the algorithm
- **cal3d-src/bone_length_example.cpp** - Complete working example demonstrating usage
- **cal3d-src/Makefile.example** - Makefile for building the example

## Algorithm Overview

The bone length calculation algorithm works as follows:

### Step 1: Calculate Absolute Transformations
Before calculating bone lengths, the skeleton state must be computed using `CalCoreSkeleton::calculateState()`:

```cpp
pSkeleton->calculateState();
```

This recursively calculates absolute transformations for all bones:

**For root bones (no parent):**
```
absoluteTranslation = localTranslation
absoluteRotation = localRotation
```

**For child bones:**
```
absoluteTranslation = (localTranslation rotated by parent.absoluteRotation) + parent.absoluteTranslation
absoluteRotation = localRotation * parent.absoluteRotation
```

### Step 2: Calculate Bone Length

**For root bones (parentId == -1):**
```cpp
length = magnitude(localTranslation)
       = sqrt(x² + y² + z²)
```

**For child bones:**
```cpp
vector = boneAbsolutePosition - parentAbsolutePosition
length = magnitude(vector)
       = sqrt((x2-x1)² + (y2-y1)² + (z2-z1)²)
```

## API Usage

### Calculate a Single Bone Length

```cpp
// Load or create a skeleton
CalCoreSkeletonPtr pSkeleton = new CalCoreSkeleton();
// ... add bones to skeleton ...

// Calculate state to update absolute transformations
pSkeleton->calculateState();

// Get a specific bone
CalCoreBone* pBone = pSkeleton->getCoreBone(boneId);

// Calculate the bone's length
float boneLength = pBone->calculateLength();
```

### Calculate All Bone Lengths

```cpp
// Load or create a skeleton
CalCoreSkeletonPtr pSkeleton = new CalCoreSkeleton();
// ... add bones to skeleton ...

// Calculate state to update absolute transformations
pSkeleton->calculateState();

// Calculate all bone lengths at once
std::vector<float> boneLengths = pSkeleton->calculateBoneLengths();

// Access individual bone lengths
for(size_t i = 0; i < boneLengths.size(); ++i)
{
    std::cout << "Bone " << i << " length: " << boneLengths[i] << "\n";
}
```

## Key Points

1. **Always call `calculateState()` first**: The absolute transformations must be up-to-date before calculating bone lengths.

2. **Both transformations are considered**:
   - Local transformations define the bone's position relative to its parent
   - Absolute transformations accumulate all parent transformations in the hierarchy
   - Bone length uses the absolute positions to account for the entire hierarchy

3. **Handles rotation correctly**: When a parent bone rotates, it affects the absolute position of all child bones. The algorithm accounts for this by rotating the local translation by the parent's absolute rotation.

4. **Works for any hierarchy depth**: The recursive nature of `calculateState()` ensures that bones at any depth in the hierarchy are correctly positioned.

## Example Output

Running the example program produces output like:

```
Cal3D Bone Length Calculation Example
======================================

Created skeleton hierarchy:
  Hip (root)
    └─ Spine
        └─ Chest
            └─ LeftArm

Calculating skeleton state (absolute transformations)...

Calculated Bone Lengths:
========================

Bone 0: Hip
  Parent ID: -1
  Local Translation: (0.000, 1.000, 0.000)
  Absolute Translation: (0.000, 1.000, 0.000)
  Bone Length: 1.000

Bone 1: Spine
  Parent ID: 0
  Local Translation: (0.000, 0.500, 0.000)
  Absolute Translation: (0.000, 1.500, 0.000)
  Bone Length: 0.500

Bone 2: Chest
  Parent ID: 1
  Local Translation: (0.000, 0.600, 0.000)
  Absolute Translation: (0.000, 2.100, 0.000)
  Bone Length: 0.600

Bone 3: LeftArm
  Parent ID: 2
  Local Translation: (0.300, 0.000, 0.000)
  Absolute Translation: (0.300, 2.100, 0.000)
  Bone Length: 0.300
```

## Mathematical Details

### Vector Rotation by Quaternion

The rotation of a vector by a quaternion is implemented in `CalVector::operator*=(const CalQuaternion&)`. This operation transforms a local translation into the parent's coordinate space.

### Distance Calculation

The Euclidean distance between two 3D points:
```
distance = sqrt((x2-x1)² + (y2-y1)² + (z2-z1)²)
```

This is implemented using `CalVector::length()`:
```cpp
float length() const
{
    return (float)sqrt(x * x + y * y + z * z);
}
```

## Testing

To test the implementation:

1. **Compile the example**:
   ```bash
   cd cal3d-src
   # Compile all necessary source files
   g++ -std=c++11 -I./src bone_length_example.cpp \
       src/cal3d/corebone.cpp \
       src/cal3d/coreskeleton.cpp \
       src/cal3d/vector.cpp \
       src/cal3d/quaternion.cpp \
       src/cal3d/matrix.cpp \
       src/cal3d/error.cpp \
       src/cal3d/global.cpp \
       src/cal3d/platform.cpp \
       -o bone_length_example
   ```

2. **Run the example**:
   ```bash
   ./bone_length_example
   ```

## Integration

The implementation is fully integrated into the Cal3D library and can be used with any existing Cal3D skeleton. The methods are:

- **Non-invasive**: Only adds new functionality, doesn't modify existing behavior
- **Efficient**: Uses already-computed absolute transformations
- **Consistent**: Follows Cal3D coding style and conventions

## References

- Cal3D Documentation: http://cal3d.sourceforge.net/
- Source code: `cal3d-src/src/cal3d/`
- Algorithm details: `cal3d-src/BONE_LENGTH_ALGORITHM.md`
- Example code: `cal3d-src/bone_length_example.cpp`
