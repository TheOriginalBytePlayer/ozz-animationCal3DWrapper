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

#include "gtest/gtest.h"

#include "cal3d_wrapper_internal.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/offline/raw_skeleton.h"
#include "ozz/base/maths/gtest_math_helper.h"
#include "ozz/base/memory/allocator.h"

using ozz::animation::Skeleton;
using ozz::animation::offline::RawSkeleton;

// Test conversion of an empty CalCoreSkeleton
TEST(SkeletonConverter, EmptySkeleton) {
  CalCoreSkeleton coreSkeleton;
  
  // Empty skeleton should return nullptr
  RawSkeleton* rawSkeleton = WrapperUtil::BuildRawSkeletonFromCal3D(&coreSkeleton);
  EXPECT_EQ(rawSkeleton, nullptr);
  
  // Also test the full conversion
  Skeleton* skeleton = WrapperUtil::BuildOzzSkeleton(&coreSkeleton);
  EXPECT_EQ(skeleton, nullptr);
}

// Test conversion of a simple single-bone skeleton
TEST(SkeletonConverter, SingleBone) {
  CalCoreSkeleton coreSkeleton;
  
  // Create a single root bone
  CalCoreBone* rootBone = new CalCoreBone();
  rootBone->name = "root";
  rootBone->id = 0;
  rootBone->parentId = -1;
  rootBone->translation = CalVector(0.0f, 1.0f, 0.0f);
  rootBone->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  rootBone->coreSkeleton = &coreSkeleton;
  
  coreSkeleton.bones.push_back(rootBone);
  coreSkeleton.rootBoneIds.push_back(0);
  coreSkeleton.boneNameToId["root"] = 0;
  
  // Convert to RawSkeleton
  RawSkeleton* rawSkeleton = WrapperUtil::BuildRawSkeletonFromCal3D(&coreSkeleton);
  ASSERT_NE(rawSkeleton, nullptr);
  EXPECT_EQ(rawSkeleton->num_joints(), 1);
  EXPECT_EQ(rawSkeleton->roots.size(), 1u);
  EXPECT_EQ(rawSkeleton->roots[0].name, "root");
  EXPECT_FLOAT3_EQ(rawSkeleton->roots[0].transform.translation, 0.0f, 1.0f, 0.0f);
  EXPECT_QUATERNION_EQ(rawSkeleton->roots[0].transform.rotation, 0.0f, 0.0f, 0.0f, 1.0f);
  EXPECT_FLOAT3_EQ(rawSkeleton->roots[0].transform.scale, 1.0f, 1.0f, 1.0f);
  EXPECT_EQ(rawSkeleton->roots[0].children.size(), 0u);
  
  ozz::Delete(rawSkeleton);
  
  // Convert to runtime Skeleton
  ozz::unique_ptr<Skeleton> skeleton(WrapperUtil::BuildOzzSkeleton(&coreSkeleton));
  ASSERT_TRUE(skeleton);
  EXPECT_EQ(skeleton->num_joints(), 1);
  
  // Verify joint name
  auto joint_names = skeleton->joint_names();
  EXPECT_STREQ(joint_names[0], "root");
  
  delete rootBone;
}

// Test conversion of a hierarchical skeleton with multiple bones
TEST(SkeletonConverter, HierarchicalSkeleton) {
  CalCoreSkeleton coreSkeleton;
  
  // Create root bone
  CalCoreBone* rootBone = new CalCoreBone();
  rootBone->name = "root";
  rootBone->id = 0;
  rootBone->parentId = -1;
  rootBone->translation = CalVector(0.0f, 1.0f, 0.0f);
  rootBone->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  rootBone->coreSkeleton = &coreSkeleton;
  
  // Create left child
  CalCoreBone* leftBone = new CalCoreBone();
  leftBone->name = "left";
  leftBone->id = 1;
  leftBone->parentId = 0;
  leftBone->translation = CalVector(1.0f, 0.0f, 0.0f);
  leftBone->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  leftBone->coreSkeleton = &coreSkeleton;
  
  // Create right child
  CalCoreBone* rightBone = new CalCoreBone();
  rightBone->name = "right";
  rightBone->id = 2;
  rightBone->parentId = 0;
  rightBone->translation = CalVector(-1.0f, 0.0f, 0.0f);
  rightBone->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  rightBone->coreSkeleton = &coreSkeleton;
  
  // Setup hierarchy
  rootBone->childIds.push_back(1);
  rootBone->childIds.push_back(2);
  
  coreSkeleton.bones.push_back(rootBone);
  coreSkeleton.bones.push_back(leftBone);
  coreSkeleton.bones.push_back(rightBone);
  coreSkeleton.rootBoneIds.push_back(0);
  coreSkeleton.boneNameToId["root"] = 0;
  coreSkeleton.boneNameToId["left"] = 1;
  coreSkeleton.boneNameToId["right"] = 2;
  
  // Convert to RawSkeleton
  RawSkeleton* rawSkeleton = WrapperUtil::BuildRawSkeletonFromCal3D(&coreSkeleton);
  ASSERT_NE(rawSkeleton, nullptr);
  EXPECT_EQ(rawSkeleton->num_joints(), 3);
  EXPECT_EQ(rawSkeleton->roots.size(), 1u);
  EXPECT_EQ(rawSkeleton->roots[0].name, "root");
  EXPECT_EQ(rawSkeleton->roots[0].children.size(), 2u);
  EXPECT_EQ(rawSkeleton->roots[0].children[0].name, "left");
  EXPECT_EQ(rawSkeleton->roots[0].children[1].name, "right");
  
  // Verify transforms
  EXPECT_FLOAT3_EQ(rawSkeleton->roots[0].children[0].transform.translation, 
                   1.0f, 0.0f, 0.0f);
  EXPECT_FLOAT3_EQ(rawSkeleton->roots[0].children[1].transform.translation, 
                   -1.0f, 0.0f, 0.0f);
  
  ozz::Delete(rawSkeleton);
  
  // Convert to runtime Skeleton
  ozz::unique_ptr<Skeleton> skeleton(WrapperUtil::BuildOzzSkeleton(&coreSkeleton));
  ASSERT_TRUE(skeleton);
  EXPECT_EQ(skeleton->num_joints(), 3);
  
  // Verify joint names
  auto joint_names = skeleton->joint_names();
  EXPECT_STREQ(joint_names[0], "root");
  EXPECT_STREQ(joint_names[1], "left");
  EXPECT_STREQ(joint_names[2], "right");
  
  delete rootBone;
  delete leftBone;
  delete rightBone;
}

// Test conversion with multiple root bones
TEST(SkeletonConverter, MultipleRoots) {
  CalCoreSkeleton coreSkeleton;
  
  // Create first root bone
  CalCoreBone* root1 = new CalCoreBone();
  root1->name = "root1";
  root1->id = 0;
  root1->parentId = -1;
  root1->translation = CalVector(0.0f, 0.0f, 0.0f);
  root1->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  root1->coreSkeleton = &coreSkeleton;
  
  // Create second root bone
  CalCoreBone* root2 = new CalCoreBone();
  root2->name = "root2";
  root2->id = 1;
  root2->parentId = -1;
  root2->translation = CalVector(1.0f, 0.0f, 0.0f);
  root2->rotation = CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
  root2->coreSkeleton = &coreSkeleton;
  
  coreSkeleton.bones.push_back(root1);
  coreSkeleton.bones.push_back(root2);
  coreSkeleton.rootBoneIds.push_back(0);
  coreSkeleton.rootBoneIds.push_back(1);
  coreSkeleton.boneNameToId["root1"] = 0;
  coreSkeleton.boneNameToId["root2"] = 1;
  
  // Convert to RawSkeleton
  RawSkeleton* rawSkeleton = WrapperUtil::BuildRawSkeletonFromCal3D(&coreSkeleton);
  ASSERT_NE(rawSkeleton, nullptr);
  EXPECT_EQ(rawSkeleton->num_joints(), 2);
  EXPECT_EQ(rawSkeleton->roots.size(), 2u);
  EXPECT_EQ(rawSkeleton->roots[0].name, "root1");
  EXPECT_EQ(rawSkeleton->roots[1].name, "root2");
  
  ozz::Delete(rawSkeleton);
  
  // Convert to runtime Skeleton
  ozz::unique_ptr<Skeleton> skeleton(WrapperUtil::BuildOzzSkeleton(&coreSkeleton));
  ASSERT_TRUE(skeleton);
  EXPECT_EQ(skeleton->num_joints(), 2);
  
  delete root1;
  delete root2;
}
