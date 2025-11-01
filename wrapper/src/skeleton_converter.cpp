//****************************************************************************//
// skeleton_converter.cpp - Convert CalCoreSkeleton to ozz Skeleton          //
//****************************************************************************//

#include "cal3d_wrapper_internal.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/animation/offline/skeleton_builder.h"
#include "ozz/base/memory/allocator.h"
#include <algorithm>

namespace {

// Helper function to recursively build RawSkeleton joints from CalCoreSkeleton
void BuildJointHierarchy(const CalCoreSkeleton* coreSkeleton,
                         int boneId,
                         ozz::animation::offline::RawSkeleton::Joint& joint) {
    if (!coreSkeleton || boneId < 0 || 
        boneId >= static_cast<int>(coreSkeleton->bones.size())) {
        return;
    }

    const CalCoreBone* bone = coreSkeleton->bones[boneId];
    if (!bone) {
        return;
    }

    // Set joint name
    joint.name = bone->name;

    // Set joint transform from Cal3D bone's local transformation
    // Cal3D stores local (relative to parent) transformation
    joint.transform.translation = bone->translation.ToOzz();
    joint.transform.rotation = bone->rotation.ToOzz();
    
    // Cal3D doesn't typically use scale in skeleton, default to 1
    joint.transform.scale = ozz::math::Float3(1.f, 1.f, 1.f);

    // Recursively process children
    joint.children.resize(bone->childIds.size());
    for (size_t i = 0; i < bone->childIds.size(); ++i) {
        BuildJointHierarchy(coreSkeleton, bone->childIds[i], joint.children[i]);
    }
}

} // anonymous namespace

namespace WrapperUtil {

// Convert CalCoreSkeleton to ozz RawSkeleton
ozz::animation::offline::RawSkeleton* BuildRawSkeletonFromCal3D(
    const CalCoreSkeleton* coreSkeleton) {
    
    if (!coreSkeleton || coreSkeleton->bones.empty()) {
        return nullptr;
    }

    auto rawSkeleton = ozz::New<ozz::animation::offline::RawSkeleton>();

    // Build the hierarchy starting from root bones
    rawSkeleton->roots.resize(coreSkeleton->rootBoneIds.size());
    
    for (size_t i = 0; i < coreSkeleton->rootBoneIds.size(); ++i) {
        int rootBoneId = coreSkeleton->rootBoneIds[i];
        BuildJointHierarchy(coreSkeleton, rootBoneId, rawSkeleton->roots[i]);
    }

    // Validate the skeleton
    if (!rawSkeleton->Validate()) {
        ozz::Delete(rawSkeleton);
        return nullptr;
    }

    return rawSkeleton;
}

// Convert CalCoreSkeleton to ozz Skeleton
ozz::animation::Skeleton* BuildOzzSkeleton(const CalCoreSkeleton* coreSkeleton) {
    
    // First, convert to RawSkeleton
    ozz::animation::offline::RawSkeleton* rawSkeleton = 
        BuildRawSkeletonFromCal3D(coreSkeleton);
    
    if (!rawSkeleton) {
        return nullptr;
    }

    // Use SkeletonBuilder to convert RawSkeleton to runtime Skeleton
    ozz::animation::offline::SkeletonBuilder builder;
    ozz::unique_ptr<ozz::animation::Skeleton> skeleton = builder(*rawSkeleton);

    // Clean up raw skeleton
    ozz::Delete(rawSkeleton);

    // Return the built skeleton (transfer ownership)
    return skeleton.release();
}

} // namespace WrapperUtil
