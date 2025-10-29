//----------------------------------------------------------------------------//
//                                                                            //
// ozz-animation is hosted at http://github.com/guillaumeblanc/ozz-animation  //
// and distributed under the MIT License (MIT).                               //
//                                                                            //
// Copyright (c) Guillaume Blanc                                              //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
//                                                                            //
//----------------------------------------------------------------------------//

#include <cstdlib>
#include <iostream>

#include "cal3d_wrapper_internal.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/base/memory/allocator.h"

// Code for ozz-animation HowTo: "How to convert Cal3D CalCoreSkeleton to ozz Skeleton?"

int main(int argc, char const* argv[]) {
  (void)argc;
  (void)argv;

  std::cout << "Converting CalCoreSkeleton to ozz::animation::Skeleton\n";
  std::cout << "========================================================\n\n";

  //////////////////////////////////////////////////////////////////////////////
  // Step 1: Create a CalCoreSkeleton with some bones
  //////////////////////////////////////////////////////////////////////////////

  CalCoreSkeleton coreSkeleton;
  
  // Create root bone
  CalCoreBone* rootBone = new CalCoreBone();
  rootBone->name = "root";
  rootBone->id = 0;
  rootBone->parentId = -1;
  rootBone->translation = CalVector(0.0f, 1.0f, 0.0f);
  rootBone->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  rootBone->coreSkeleton = &coreSkeleton;
  
  // Create left arm bone
  CalCoreBone* leftArm = new CalCoreBone();
  leftArm->name = "left_arm";
  leftArm->id = 1;
  leftArm->parentId = 0;
  leftArm->translation = CalVector(1.0f, 0.0f, 0.0f);
  leftArm->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  leftArm->coreSkeleton = &coreSkeleton;
  
  // Create right arm bone
  CalCoreBone* rightArm = new CalCoreBone();
  rightArm->name = "right_arm";
  rightArm->id = 2;
  rightArm->parentId = 0;
  rightArm->translation = CalVector(-1.0f, 0.0f, 0.0f);
  rightArm->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  rightArm->coreSkeleton = &coreSkeleton;
  
  // Setup hierarchy
  rootBone->childIds.push_back(1);
  rootBone->childIds.push_back(2);
  
  // Add bones to skeleton
  coreSkeleton.bones.push_back(rootBone);
  coreSkeleton.bones.push_back(leftArm);
  coreSkeleton.bones.push_back(rightArm);
  coreSkeleton.rootBoneIds.push_back(0);
  coreSkeleton.boneNameToId["root"] = 0;
  coreSkeleton.boneNameToId["left_arm"] = 1;
  coreSkeleton.boneNameToId["right_arm"] = 2;

  std::cout << "Created CalCoreSkeleton with " << coreSkeleton.bones.size() 
            << " bones:\n";
  for (const auto* bone : coreSkeleton.bones) {
    std::cout << "  - " << bone->name << " (parent: " << bone->parentId << ")\n";
  }
  std::cout << "\n";

  //////////////////////////////////////////////////////////////////////////////
  // Step 2: Convert CalCoreSkeleton to ozz::animation::offline::RawSkeleton
  //////////////////////////////////////////////////////////////////////////////

  std::cout << "Converting to ozz::animation::offline::RawSkeleton...\n";
  ozz::animation::offline::RawSkeleton* rawSkeleton = 
      WrapperUtil::BuildRawSkeletonFromCal3D(&coreSkeleton);
  
  if (!rawSkeleton) {
    std::cerr << "ERROR: Failed to convert to RawSkeleton\n";
    return EXIT_FAILURE;
  }
  
  std::cout << "  RawSkeleton created with " << rawSkeleton->num_joints() 
            << " joints\n\n";

  //////////////////////////////////////////////////////////////////////////////
  // Step 3: Convert RawSkeleton to runtime ozz::animation::Skeleton
  //////////////////////////////////////////////////////////////////////////////

  std::cout << "Converting to ozz::animation::Skeleton...\n";
  ozz::unique_ptr<ozz::animation::Skeleton> skeleton(
      WrapperUtil::BuildOzzSkeleton(&coreSkeleton));
  
  if (!skeleton) {
    std::cerr << "ERROR: Failed to convert to runtime Skeleton\n";
    ozz::Delete(rawSkeleton);
    return EXIT_FAILURE;
  }
  
  std::cout << "  Runtime Skeleton created with " << skeleton->num_joints() 
            << " joints\n\n";

  //////////////////////////////////////////////////////////////////////////////
  // Step 4: Display information about the converted skeleton
  //////////////////////////////////////////////////////////////////////////////

  std::cout << "Skeleton information:\n";
  std::cout << "  Number of joints: " << skeleton->num_joints() << "\n";
  std::cout << "  Number of SoA elements: " << skeleton->num_soa_joints() << "\n";
  
  std::cout << "\n  Joint names:\n";
  auto joint_names = skeleton->joint_names();
  for (int i = 0; i < skeleton->num_joints(); ++i) {
    std::cout << "    [" << i << "] " << joint_names[i] << "\n";
  }
  
  std::cout << "\n  Joint parents:\n";
  auto joint_parents = skeleton->joint_parents();
  for (int i = 0; i < skeleton->num_joints(); ++i) {
    std::cout << "    [" << i << "] parent: " << joint_parents[i] << "\n";
  }

  //////////////////////////////////////////////////////////////////////////////
  // Step 5: Clean up
  //////////////////////////////////////////////////////////////////////////////

  ozz::Delete(rawSkeleton);
  
  // Clean up Cal3D bones
  delete rootBone;
  delete leftArm;
  delete rightArm;

  std::cout << "\nConversion successful!\n";
  std::cout << "\nThe conversion functions are available in WrapperUtil namespace:\n";
  std::cout << "  - WrapperUtil::BuildRawSkeletonFromCal3D()\n";
  std::cout << "  - WrapperUtil::BuildOzzSkeleton()\n";

  return EXIT_SUCCESS;
}
