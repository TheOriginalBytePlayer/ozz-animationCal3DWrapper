# Cal3D Bone Length Calculation Algorithm

This document explains the algorithm for calculating bone lengths in Cal3D skeletons, taking into account both **TRANSFORMATIONS** (absolute positions) and **LOCALTRANSFORMATIONS** (relative positions).

## Overview

In Cal3D, each bone has two types of transformations:

1. **Local Transformations** (relative to parent):
   - `m_translation`: Position relative to parent bone
   - `m_rotation`: Rotation relative to parent bone

2. **Absolute Transformations** (world space):
   - `m_translationAbsolute`: Position in world space
   - `m_rotationAbsolute`: Rotation in world space

## Bone Length Definition

The **bone length** is defined as the distance from the bone's position to its parent's position in world space (using absolute transformations).

## Algorithm

### Step 1: Calculate Absolute Transformations

Before calculating bone lengths, the skeleton's state must be calculated using `CalCoreSkeleton::calculateState()`. This method computes absolute transformations for all bones in the hierarchy.

For each bone, the absolute transformation is calculated as follows:

#### Root Bones (no parent):
```cpp
m_translationAbsolute = m_translation
m_rotationAbsolute = m_rotation
```

#### Child Bones (has parent):
```cpp
// Transform relative translation by parent's absolute rotation
m_translationAbsolute = m_translation * parent.m_rotationAbsolute
// Add parent's absolute translation
m_translationAbsolute += parent.m_translationAbsolute

// Combine rotations
m_rotationAbsolute = m_rotation * parent.m_rotationAbsolute
```

This recursive process ensures that all parent transformations are accumulated down the hierarchy.

### Step 2: Calculate Bone Length

Once absolute transformations are calculated, the bone length can be computed:

#### For Root Bones (parentId == -1):
```cpp
length = magnitude(m_translation)
length = sqrt(x² + y² + z²)
```

The root bone's length is simply the magnitude of its local translation vector.

#### For Child Bones (has parent):
```cpp
// Get bone-to-parent vector
vector = m_translationAbsolute - parent.m_translationAbsolute

// Calculate magnitude (Euclidean distance)
length = sqrt(vector.x² + vector.y² + vector.z²)
```

## Implementation

### CalCoreBone::calculateLength()

```cpp
float CalCoreBone::calculateLength()
{
  // Root bone: return magnitude of local translation
  if(m_parentId == -1)
  {
    return m_translation.length();
  }
  
  // Child bone: return distance to parent
  CalCoreBone *pParent = m_pCoreSkeleton->getCoreBone(m_parentId);
  CalVector boneToParent = m_translationAbsolute - pParent->getTranslationAbsolute();
  return boneToParent.length();
}
```

### CalCoreSkeleton::calculateBoneLengths()

```cpp
std::vector<float> CalCoreSkeleton::calculateBoneLengths()
{
  std::vector<float> boneLengths;
  boneLengths.reserve(m_vectorCoreBone.size());
  
  for(size_t boneId = 0; boneId < m_vectorCoreBone.size(); ++boneId)
  {
    float length = m_vectorCoreBone[boneId]->calculateLength();
    boneLengths.push_back(length);
  }
  
  return boneLengths;
}
```

## Usage Example

```cpp
// Load or create a skeleton
CalCoreSkeleton* pSkeleton = new CalCoreSkeleton();
// ... add bones to skeleton ...

// IMPORTANT: Calculate state first to compute absolute transformations
pSkeleton->calculateState();

// Calculate all bone lengths
std::vector<float> boneLengths = pSkeleton->calculateBoneLengths();

// Access individual bone length
for(size_t i = 0; i < boneLengths.size(); ++i)
{
    std::cout << "Bone " << i << " length: " << boneLengths[i] << "\n";
}

// Or calculate a single bone's length
float singleBoneLength = pSkeleton->getCoreBone(boneId)->calculateLength();
```

## Key Points

1. **Always call `calculateState()` first**: The absolute transformations must be up-to-date before calculating bone lengths.

2. **Both transformations matter**: 
   - Local transformations define the bone's position relative to its parent
   - Absolute transformations accumulate all parent transformations
   - Bone length uses the absolute positions to account for the entire hierarchy

3. **Handles rotation correctly**: When a parent bone rotates, it affects the absolute position of all child bones. The algorithm accounts for this by rotating the local translation by the parent's absolute rotation.

4. **Works for any hierarchy depth**: The recursive nature of `calculateState()` ensures that bones at any depth in the hierarchy are correctly positioned.

## Mathematical Details

### Vector Rotation by Quaternion

When transforming a local translation by a parent's rotation:

```cpp
CalVector result = localTranslation;
result *= parentRotation;  // Quaternion rotation
result += parentTranslation;
```

This is equivalent to:
```
result = Rotate(localTranslation, parentRotation) + parentTranslation
```

Where `Rotate()` applies the quaternion rotation to the vector.

### Distance Calculation

The Euclidean distance between two points in 3D space:

```
distance = sqrt((x2-x1)² + (y2-y1)² + (z2-z1)²)
```

This is implemented using `CalVector::length()`:

```cpp
float length = sqrt(x * x + y * y + z * z);
```

## Example Hierarchy

Consider this skeleton:

```
Hip (root) at (0, 1, 0)
  └─ Spine at (0, 0.5, 0) relative to Hip
      └─ Chest at (0, 0.6, 0) relative to Spine
```

After `calculateState()`:
- Hip absolute: (0, 1, 0)
- Spine absolute: (0, 1.5, 0) = Hip + Spine_local
- Chest absolute: (0, 2.1, 0) = Spine + Chest_local

Bone lengths:
- Hip: sqrt(0² + 1² + 0²) = 1.0
- Spine: sqrt((0-0)² + (1.5-1)² + (0-0)²) = 0.5
- Chest: sqrt((0-0)² + (2.1-1.5)² + (0-0)²) = 0.6

## References

- `cal3d-src/src/cal3d/corebone.h` - CalCoreBone class declaration
- `cal3d-src/src/cal3d/corebone.cpp` - CalCoreBone implementation with `calculateLength()`
- `cal3d-src/src/cal3d/coreskeleton.h` - CalCoreSkeleton class declaration
- `cal3d-src/src/cal3d/coreskeleton.cpp` - CalCoreSkeleton implementation with `calculateBoneLengths()`
- `cal3d-src/bone_length_example.cpp` - Complete working example
