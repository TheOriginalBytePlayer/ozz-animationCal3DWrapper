/**
 * Example usage of the Cal3D wrapper library with ozz-animation backend
 * 
 * This example demonstrates how to use the wrapper as a drop-in replacement
 * for the original Cal3D library.
 * 
 * NOTE: Functions that return struct pointers now return void* (opaque pointers).
 * The client code is responsible for casting them to the appropriate struct type.
 */

#include "cal3d_wrapper.h"
#include <stdio.h>

int main() {
    printf("Cal3D Wrapper Example\n");
    printf("=====================\n\n");
    
    // Create a core model - returns void* which we cast to struct CalCoreModel*
    struct CalCoreModel* coreModel = (struct CalCoreModel*)CalCoreModel_New("ExampleModel");
    if (!coreModel) {
        printf("Error: Failed to create core model\n");
        return 1;
    }
    printf("✓ Created core model: %s\n", CalCoreModel_GetName(coreModel));
    
    // Vector operations example - returns void* which we cast to struct CalVector*
    struct CalVector* vec1 = (struct CalVector*)CalVector_New();
    struct CalVector* vec2 = (struct CalVector*)CalVector_New();
    struct CalVector* result = (struct CalVector*)CalVector_New();
    
    CalVector_Set(vec1, 1.0f, 2.0f, 3.0f);
    CalVector_Set(vec2, 4.0f, 5.0f, 6.0f);
    
    printf("\n✓ Created and initialized vectors\n");
    float* v1 = CalVector_Get(vec1);
    float* v2 = CalVector_Get(vec2);
    printf("  vec1 = (%.1f, %.1f, %.1f)\n", v1[0], v1[1], v1[2]);
    printf("  vec2 = (%.1f, %.1f, %.1f)\n", v2[0], v2[1], v2[2]);
    
    // Add vectors
    CalVector_Op_Add(result, vec1, vec2);
    float* res = CalVector_Get(result);
    printf("  vec1 + vec2 = (%.1f, %.1f, %.1f)\n", res[0], res[1], res[2]);
    
    // Dot product
    float dot = CalVector_Op_Scalar(vec1, vec2);
    printf("  vec1 · vec2 = %.1f\n", dot);
    
    // Cross product
    CalVector_Op_Cross(result, vec1, vec2);
    res = CalVector_Get(result);
    printf("  vec1 × vec2 = (%.1f, %.1f, %.1f)\n", res[0], res[1], res[2]);
    
    // Normalize
    CalVector_Set(vec1, 3.0f, 4.0f, 0.0f);
    float length = CalVector_Length(vec1);
    v1 = CalVector_Get(vec1);
    printf("\n✓ Vector normalization:\n");
    printf("  vec = (%.1f, %.1f, %.1f), length = %.1f\n", 
           v1[0], v1[1], v1[2], length);
    CalVector_Normalize(vec1);
    v1 = CalVector_Get(vec1);
    printf("  normalized = (%.2f, %.2f, %.2f), length = %.2f\n", 
           v1[0], v1[1], v1[2], CalVector_Length(vec1));
    
    // Quaternion operations example - returns void* which we cast to struct CalQuaternion*
    struct CalQuaternion* quat1 = (struct CalQuaternion*)CalQuaternion_New();
    struct CalQuaternion* quat2 = (struct CalQuaternion*)CalQuaternion_New();
    struct CalQuaternion* quatResult = (struct CalQuaternion*)CalQuaternion_New();
    
    CalQuaternion_Set(quat1, 0.0f, 0.0f, 0.707f, 0.707f); // 90° around Z
    CalQuaternion_Set(quat2, 0.707f, 0.0f, 0.0f, 0.707f); // 90° around X
    
    float* q1 = CalQuaternion_Get(quat1);
    float* q2 = CalQuaternion_Get(quat2);
    printf("\n✓ Quaternion operations:\n");
    printf("  quat1 = (%.3f, %.3f, %.3f, %.3f)\n", 
           q1[0], q1[1], q1[2], q1[3]);
    printf("  quat2 = (%.3f, %.3f, %.3f, %.3f)\n", 
           q2[0], q2[1], q2[2], q2[3]);
    
    // Multiply quaternions
    CalQuaternion_Op_Multiply(quatResult, quat1, quat2);
    float* qr = CalQuaternion_Get(quatResult);
    printf("  quat1 * quat2 = (%.3f, %.3f, %.3f, %.3f)\n", 
           qr[0], qr[1], qr[2], qr[3]);
    
    // Transform vector by quaternion
    CalVector_Set(vec1, 1.0f, 0.0f, 0.0f);
    v1 = CalVector_Get(vec1);
    printf("\n✓ Vector transformation:\n");
    printf("  vec before = (%.1f, %.1f, %.1f)\n", v1[0], v1[1], v1[2]);
    CalVector_Transform(vec1, quat1);
    v1 = CalVector_Get(vec1);
    printf("  vec after rotation = (%.2f, %.2f, %.2f)\n", 
           v1[0], v1[1], v1[2]);
    
    // Create and manage materials - returns void* which we cast to struct CalCoreMaterial*
    struct CalCoreMaterial* material = (struct CalCoreMaterial*)CalCoreMaterial_New();
    CalCoreModel_AddCoreMaterial(coreModel, material);
    printf("\n✓ Added material to model\n");
    printf("  Total materials: %d\n", CalCoreModel_GetCoreMaterialCount(coreModel));
    
    // Clean up
    CalVector_Delete(vec1);
    CalVector_Delete(vec2);
    CalVector_Delete(result);
    CalQuaternion_Delete(quat1);
    CalQuaternion_Delete(quat2);
    CalQuaternion_Delete(quatResult);
    CalCoreModel_Delete(coreModel);
    
    printf("\n✓ Cleanup complete\n");
    printf("\nExample completed successfully!\n");
    
    // Check for errors
    if (CalError_GetLastErrorCode() != 0) {  // ERROR_CODE_OK = 0
        printf("\nError occurred: %s\n", CalError_GetLastErrorDescription());
        return 1;
    }
    
    return 0;
}
