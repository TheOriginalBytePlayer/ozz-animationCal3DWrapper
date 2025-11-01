//****************************************************************************//
// cal3d_wrapper_internal.h                                                   //
// Internal structures for Cal3D to ozz-animation wrapper                     //
//****************************************************************************//

#ifndef CAL3D_WRAPPER_INTERNAL_H
#define CAL3D_WRAPPER_INTERNAL_H

#include <vector>
#include <map>
#include <string>
#include <memory>

// Forward declare ALL struct types used in cal3d_wrapper.h
// These are needed BEFORE including the header
struct CalAnimation;
struct CalAnimationAction;
struct CalAnimationCycle;
struct CalBone;
struct CalCoreAnimation;
struct CalCoreMorphAnimation;
struct CalCoreBone;
struct CalCoreMaterial;
struct CalCoreMesh;
struct CalCoreModel;
struct CalCoreSkeleton;
struct CalCoreSubmesh;
struct CalLoader;
struct CalMatrix;
struct CalMesh;
struct CalMixer;
struct CalMorphTargetMixer;
struct CalModel;
struct CalPhysique;
struct CalPlatform;
struct CalQuaternion;
struct CalRenderer;
struct CalSaver;
struct CalSkeleton;
struct CalSpringSystem;
struct CalSubmesh;
struct CalVector;
struct CalCoreSubMorphTargetDiffMap;
struct CalCoreSubMorphTarget;

// Define CAL3D_WRAPPER_EXPORTS after struct declarations
#ifndef CAL3D_WRAPPER_EXPORTS
#define CAL3D_WRAPPER_EXPORTS
#endif

// ozz-animation includes
#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/blending_job.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/offline/skeleton_builder.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"
#include "ozz/base/maths/quaternion.h"
#include "ozz/base/maths/simd_math.h"
#include "ozz/base/containers/vector.h"
#include "ozz/base/memory/unique_ptr.h"

// Cal3D wrapper types
typedef void* CalUserData;
typedef unsigned long CalColorValue;

// For C compatibility
#ifndef __cplusplus
typedef int bool;
#define true 1
#define false 0
#endif

// Forward declarations of Cal3D loader/saver for format compatibility
namespace Cal {
    class CalCoreModel;
    class CalCoreSkeleton;
    class CalCoreBone;
    class CalCoreAnimation;
    class CalCoreKeyframe;
    class CalCoreMesh;
    class CalCoreSubmesh;
    class CalCoreMaterial;
}

// Wrapper structures that bridge Cal3D API to ozz-animation

struct CalVector {
    float x, y, z;
    
    CalVector() : x(0), y(0), z(0) {}
    CalVector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    ozz::math::Float3 ToOzz() const {
        return ozz::math::Float3(x, y, z);
    }
    
    void FromOzz(const ozz::math::Float3& v) {
        x = v.x; y = v.y; z = v.z;
    }
};

struct CalQuaternion {
    float x, y, z, w;
    
    CalQuaternion() : x(0), y(0), z(0), w(1) {}
    CalQuaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    
    ozz::math::Quaternion ToOzz() const {
        return ozz::math::Quaternion(x, y, z, w);
    }
    
    void FromOzz(const ozz::math::Quaternion& q) {
        x = q.x; y = q.y; z = q.z; w = q.w;
    }
};

// Core structures

struct CalCoreBone {
    std::string name;
    int id;
    int parentId;
    CalVector translation;
    CalQuaternion rotation;
    CalVector translationAbsolute;
    CalQuaternion rotationAbsolute;
    CalVector translationBoneSpace;
    CalQuaternion rotationBoneSpace;
    std::vector<int> childIds;
    CalUserData userData;
    struct CalCoreSkeleton* coreSkeleton;
};

struct CalCoreSkeleton {
    std::vector<CalCoreBone*> bones;
    std::map<std::string, int> boneNameToId;
    std::vector<int> rootBoneIds;
    std::unique_ptr<ozz::animation::Skeleton> ozzSkeleton;
};

struct CalCoreAnimation {
    std::string name;
    float duration;
    std::unique_ptr<ozz::animation::Animation> ozzAnimation;
    // Store raw keyframe data for Cal3D format compatibility
    struct Track {
        std::vector<float> times;
        std::vector<CalVector> translations;
        std::vector<CalQuaternion> rotations;
    };
    std::map<int, Track> boneTracks; // bone id -> track
};

struct CalCoreMorphAnimation {
    std::string name;
    std::vector<std::pair<int, int>> morphTargets; // mesh id, morph id pairs
};

struct CalCoreSubmesh {
    int coreMaterialThreadId;
    int vertexCount;
    int faceCount;
    int lodCount;
    int springCount;
    
    std::vector<CalVector> vertices;
    std::vector<CalVector> normals;
    std::vector<std::vector<std::pair<float, float>>> textureCoords; // [map][vertex]
    std::vector<int> faces; // indices, 3 per face
    
    struct Influence {
        int boneId;
        float weight;
    };
    std::vector<std::vector<Influence>> influences; // [vertex][influence]
    
    bool tangentsEnabled;
    std::vector<struct CalCoreSubMorphTarget*> morphTargets;
};

struct CalCoreSubMorphTarget {
    std::vector<CalVector> vertexOffsets;
    std::vector<CalVector> normalOffsets;
    std::vector<int> vertexIds;
};

struct CalCoreSubMorphTargetDiffMap {
    std::vector<int> vertexIds;
    std::vector<CalVector> positionOffsets;
    std::vector<CalVector> normalOffsets;
};

struct CalCoreMesh {
    std::string name;
    std::vector<CalCoreSubmesh*> submeshes;
};

struct CalCoreMaterial {
    std::string name;
    CalUserData userData;
    float shininess;
    CalColorValue ambientColor;
    CalColorValue diffuseColor;
    CalColorValue specularColor;
    CalColorValue emissiveColor;
    
    struct Map {
        std::string filename;
        CalUserData userData;
    };
    std::vector<Map> maps;
};

struct CalCoreModel {
    std::string name;
    CalCoreSkeleton* coreSkeleton;
    std::vector<CalCoreAnimation*> coreAnimations;
    std::vector<CalCoreMorphAnimation*> coreMorphAnimations;
    std::vector<CalCoreMesh*> coreMeshes;
    std::vector<CalCoreMaterial*> coreMaterials;
    std::map<std::string, int> meshNameToId;
    
    // Material threads (for material sets)
    std::map<int, std::map<int, int>> materialThreads; // [threadId][setId] -> materialId
    
    CalUserData userData;
};

// Runtime structures

struct CalBone {
    CalCoreBone* coreBone;
    struct CalSkeleton* skeleton;
    CalVector translation;
    CalQuaternion rotation;
    CalVector translationAbsolute;
    CalQuaternion rotationAbsolute;
};

struct CalSkeleton {
    CalCoreSkeleton* coreSkeleton;
    std::vector<CalBone*> bones;
    
    // ozz runtime data
    ozz::vector<ozz::math::SoaTransform> localTransforms;
    ozz::vector<ozz::math::Float4x4> modelSpaceMatrices;
};

struct CalAnimation {
    CalCoreAnimation* coreAnimation;
    float time;
    float weight;
    int state;
    int type;
};

struct CalAnimationCycle {
    CalAnimation base;
    bool async;
    float asyncTime;
    float asyncDuration;
};

struct CalAnimationAction {
    CalAnimation base;
    float delayIn;
    float delayOut;
};

struct CalSubmesh {
    CalCoreSubmesh* coreSubmesh;
    int coreMaterialId;
    float lodLevel;
    bool hasInternalData;
    std::vector<float> morphWeights;
    
    // Dynamic vertex data
    std::vector<CalVector> vertices;
    std::vector<CalVector> normals;
};

struct CalMesh {
    CalCoreMesh* coreMesh;
    std::vector<CalSubmesh*> submeshes;
    struct CalModel* model;
    float lodLevel;
    int materialSet;
};

struct CalMixer {
    struct CalModel* model;
    std::vector<CalAnimationCycle*> cycles;
    std::vector<CalAnimationAction*> actions;
    
    // ozz blending data
    ozz::vector<ozz::math::SoaTransform> blendedLocals;
    ozz::animation::BlendingJob::Layer layers[16];
    int layerCount;
};

struct CalMorphTargetMixer {
    struct CalModel* model;
    std::map<int, float> morphWeights; // morph anim id -> weight
};

struct CalModel {
    CalCoreModel* coreModel;
    CalSkeleton* skeleton;
    CalMixer* mixer;
    CalMorphTargetMixer* morphTargetMixer;
    struct CalPhysique* physique;
    struct CalRenderer* renderer;
    struct CalSpringSystem* springSystem;
    std::vector<CalMesh*> meshes;
    CalUserData userData;
    float lodLevel;
    int materialSet;
};

struct CalPhysique {
    CalModel* model;
    bool dualQuat; // dual quaternion skinning
};

struct CalRenderer {
    CalModel* model;
    int currentMeshId;
    int currentSubmeshId;
};

struct CalSpringSystem {
    CalModel* model;
    CalVector gravity;
    CalVector force;
};

struct CalSaver {
    // Placeholder for saver functionality
};

struct CalLoader {
    // Placeholder for loader functionality
};

// Utility functions

namespace WrapperUtil {
    // Convert Cal3D skeleton to ozz RawSkeleton (intermediate format)
    ozz::animation::offline::RawSkeleton* BuildRawSkeletonFromCal3D(
        const CalCoreSkeleton* coreSkeleton);
    
    // Convert Cal3D skeleton to ozz runtime skeleton
    ozz::animation::Skeleton* BuildOzzSkeleton(const CalCoreSkeleton* coreSkeleton);
    
    // Convert Cal3D animation to ozz animation
    ozz::animation::Animation* BuildOzzAnimation(const CalCoreAnimation* coreAnimation, 
                                                   const CalCoreSkeleton* coreSkeleton);
    
    // Convert ozz transforms to Cal3D bone states
    void OzzTransformsToCal3D(const ozz::vector<ozz::math::Float4x4>& modelSpaceMatrices,
                              CalSkeleton* skeleton);
}

#endif // CAL3D_WRAPPER_INTERNAL_H
