//****************************************************************************//
// bone_length_example.cpp                                                   //
// Example demonstrating how to calculate bone lengths in Cal3D              //
//****************************************************************************//
// This example shows how to calculate bone lengths using both the           //
// TRANSFORMATIONS (absolute positions) and LOCALTRANSFORMATIONS             //
// (relative positions) in a Cal3D skeleton.                                 //
//****************************************************************************//

#include <iostream>
#include <iomanip>
#include "src/cal3d/coreskeleton.h"
#include "src/cal3d/corebone.h"
#include "src/cal3d/vector.h"
#include "src/cal3d/quaternion.h"

/**
 * This example demonstrates the algorithm for calculating bone lengths
 * in a Cal3D skeleton. The algorithm takes into account:
 * 
 * 1. LOCALTRANSFORMATIONS (relative to parent):
 *    - m_translation: The bone's position relative to its parent
 *    - m_rotation: The bone's rotation relative to its parent
 * 
 * 2. TRANSFORMATIONS (absolute in world space):
 *    - m_translationAbsolute: The bone's position in world space
 *    - m_rotationAbsolute: The bone's rotation in world space
 * 
 * The bone length is calculated as the distance from the bone's absolute
 * position to its parent's absolute position. For root bones, the length
 * is the magnitude of the bone's local translation.
 */

int main()
{
    std::cout << "Cal3D Bone Length Calculation Example\n";
    std::cout << "======================================\n\n";
    
    // Create a simple skeleton hierarchy for demonstration
    // CalCoreSkeleton uses reference counting, so we use RefPtr
    // The RefPtr constructor will properly initialize the reference count
    CalCoreSkeletonPtr pSkeleton(new CalCoreSkeleton());
    
    // Create root bone (e.g., "Hip")
    CalCoreBone* pRootBone = new CalCoreBone("Hip");
    pRootBone->setParentId(-1);  // No parent (root bone)
    pRootBone->setTranslation(CalVector(0.0f, 1.0f, 0.0f));  // 1 unit above origin
    pRootBone->setRotation(CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f));  // Identity rotation
    pRootBone->setCoreSkeleton(pSkeleton.get());
    int rootId = pSkeleton->addCoreBone(pRootBone);
    
    // Create child bone 1 (e.g., "Spine")
    CalCoreBone* pChildBone1 = new CalCoreBone("Spine");
    pChildBone1->setParentId(rootId);
    pChildBone1->setTranslation(CalVector(0.0f, 0.5f, 0.0f));  // 0.5 units above parent
    pChildBone1->setRotation(CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f));  // Identity rotation
    pChildBone1->setCoreSkeleton(pSkeleton.get());
    int child1Id = pSkeleton->addCoreBone(pChildBone1);
    pRootBone->addChildId(child1Id);
    
    // Create child bone 2 (e.g., "Chest")
    CalCoreBone* pChildBone2 = new CalCoreBone("Chest");
    pChildBone2->setParentId(child1Id);
    pChildBone2->setTranslation(CalVector(0.0f, 0.6f, 0.0f));  // 0.6 units above parent
    pChildBone2->setRotation(CalQuaternion(0.0f, 0.0f, 0.0f, 1.0f));  // Identity rotation
    pChildBone2->setCoreSkeleton(pSkeleton.get());
    int child2Id = pSkeleton->addCoreBone(pChildBone2);
    pChildBone1->addChildId(child2Id);
    
    // Create a child bone with rotation (e.g., "LeftArm")
    CalCoreBone* pChildBone3 = new CalCoreBone("LeftArm");
    pChildBone3->setParentId(child2Id);
    // Rotate 90 degrees around Z axis (quaternion for 90 deg around Z)
    pChildBone3->setTranslation(CalVector(0.3f, 0.0f, 0.0f));  // 0.3 units to the left
    pChildBone3->setRotation(CalQuaternion(0.0f, 0.0f, 0.707f, 0.707f));  // 90 deg rotation
    pChildBone3->setCoreSkeleton(pSkeleton.get());
    int child3Id = pSkeleton->addCoreBone(pChildBone3);
    pChildBone2->addChildId(child3Id);
    
    std::cout << "Created skeleton hierarchy:\n";
    std::cout << "  Hip (root)\n";
    std::cout << "    └─ Spine\n";
    std::cout << "        └─ Chest\n";
    std::cout << "            └─ LeftArm\n\n";
    
    // IMPORTANT: Calculate the state to compute absolute transformations
    std::cout << "Calculating skeleton state (absolute transformations)...\n\n";
    pSkeleton->calculateState();
    
    std::cout << "Bone Length Calculation Algorithm:\n";
    std::cout << "==================================\n\n";
    
    // Display the algorithm explanation
    std::cout << "For each bone, the length is calculated as follows:\n\n";
    std::cout << "1. If bone has no parent (root bone):\n";
    std::cout << "   length = magnitude of local translation vector\n";
    std::cout << "   length = sqrt(x² + y² + z²)\n\n";
    
    std::cout << "2. If bone has a parent:\n";
    std::cout << "   a) Calculate absolute translation:\n";
    std::cout << "      - Rotate local translation by parent's absolute rotation\n";
    std::cout << "      - Add parent's absolute translation\n";
    std::cout << "   b) Calculate bone-to-parent vector:\n";
    std::cout << "      vector = bone_absolute_pos - parent_absolute_pos\n";
    std::cout << "   c) Calculate length:\n";
    std::cout << "      length = magnitude of vector\n\n";
    
    // Get all bone lengths
    std::vector<float> boneLengths = pSkeleton->calculateBoneLengths();
    
    std::cout << "Calculated Bone Lengths:\n";
    std::cout << "========================\n\n";
    
    // Display results for each bone
    std::vector<CalCoreBone*>& bones = pSkeleton->getVectorCoreBone();
    for(size_t i = 0; i < bones.size(); ++i)
    {
        CalCoreBone* pBone = bones[i];
        std::cout << "Bone " << i << ": " << pBone->getName() << "\n";
        std::cout << "  Parent ID: " << pBone->getParentId() << "\n";
        
        const CalVector& localTrans = pBone->getTranslation();
        std::cout << "  Local Translation: (" 
                  << std::fixed << std::setprecision(3)
                  << localTrans.x << ", " 
                  << localTrans.y << ", " 
                  << localTrans.z << ")\n";
        
        const CalVector& absTrans = pBone->getTranslationAbsolute();
        std::cout << "  Absolute Translation: (" 
                  << absTrans.x << ", " 
                  << absTrans.y << ", " 
                  << absTrans.z << ")\n";
        
        std::cout << "  Bone Length: " << boneLengths[i] << "\n\n";
    }
    
    std::cout << "Summary:\n";
    std::cout << "========\n";
    std::cout << "The bone lengths account for both:\n";
    std::cout << "- Local transformations (position/rotation relative to parent)\n";
    std::cout << "- Accumulated parent transformations (world space position)\n\n";
    std::cout << "This ensures accurate bone length calculation regardless of\n";
    std::cout << "the hierarchy depth and parent transformations.\n";
    
    // Clean up is automatic with RefPtr
    // pSkeleton will be automatically cleaned up when it goes out of scope
    
    return 0;
}
