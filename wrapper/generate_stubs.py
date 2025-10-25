#!/usr/bin/env python3
"""
Generate stub implementations for all Cal3D wrapper functions
"""

import re

# Functions already implemented in other files - skip these
SKIP_FUNCTIONS = {
    # Error functions
    'CalError_GetLastErrorCode', 'CalError_GetLastErrorDescription',
    'CalError_GetLastErrorFile', 'CalError_GetLastErrorLine',
    'CalError_GetLastErrorText', 'CalError_PrintLastError',
    
    # Vector functions
    'CalVector_New', 'CalVector_Delete', 'CalVector_Get', 'CalVector_Add',
    'CalVector_Subtract', 'CalVector_Scale', 'CalVector_InverseScale',
    'CalVector_Set', 'CalVector_Clear', 'CalVector_Length', 'CalVector_Normalize',
    'CalVector_Equal', 'CalVector_Blend', 'CalVector_Transform',
    'CalVector_Op_Add', 'CalVector_Op_Subtract', 'CalVector_CalVector_Op_Scale',
    'CalVector_CalVector_Op_InverseScale', 'CalVector_Op_Scalar', 'CalVector_Op_Cross',
    
    # Quaternion functions
    'CalQuaternion_New', 'CalQuaternion_Delete', 'CalQuaternion_Get',
    'CalQuaternion_Set', 'CalQuaternion_Clear', 'CalQuaternion_Equal',
    'CalQuaternion_Multiply', 'CalQuaternion_MultiplyVector', 'CalQuaternion_Invert',
    'CalQuaternion_Conjugate', 'CalQuaternion_Blend', 'CalQuaternion_Op_Multiply',
    
    # CoreModel functions
    'CalCoreModel_New', 'CalCoreModel_NewCopy', 'CalCoreModel_Delete',
    'CalCoreModel_GetName', 'CalCoreModel_SetName',
    'CalCoreModel_GetCoreSkeleton', 'CalCoreModel_SetCoreSkeleton',
    'CalCoreModel_AddCoreAnimation', 'CalCoreModel_GetCoreAnimation',
    'CalCoreModel_GetCoreAnimationCount',
    'CalCoreModel_AddCoreMorphAnimation', 'CalCoreModel_GetCoreMorphAnimation',
    'CalCoreModel_GetCoreMorphAnimationCount',
    'CalCoreModel_AddCoreMaterial', 'CalCoreModel_GetCoreMaterial',
    'CalCoreModel_GetCoreMaterialCount', 'CalCoreModel_GetCoreMaterialId',
    'CalCoreModel_SetCoreMaterialId', 'CalCoreModel_CreateCoreMaterialThread',
    'CalCoreModel_GetMaterialThreadCount', 'CalCoreModel_GetMaterialThreadIDArray',
    'CalCoreModel_GetCoreMaterialIDByName',
    'CalCoreModel_AddCoreMesh', 'CalCoreModel_GetCoreMesh',
    'CalCoreModel_GetCoreMeshCount', 'CalCoreModel_GetCoreMeshId',
    'CalCoreModel_LoadCoreAnimation', 'CalCoreModel_LoadCoreMaterial',
    'CalCoreModel_LoadCoreMesh', 'CalCoreModel_LoadCoreSkeleton',
    'CalCoreModel_LoadCoreAnimationFromBuffer', 'CalCoreModel_LoadCoreMaterialFromBuffer',
    'CalCoreModel_LoadCoreMeshFromBuffer', 'CalCoreModel_LoadCoreSkeletonFromBuffer',
    'CalCoreModel_SaveCoreAnimation', 'CalCoreModel_SaveCoreMaterial',
    'CalCoreModel_SaveCoreMesh', 'CalCoreModel_SaveCoreSkeleton',
    'CalCoreModel_GetUserData', 'CalCoreModel_SetUserData',
    'CalCoreModel_ReplaceCoreMesh', 'CalCoreModel_AddMeshName',
    'CalCoreModel_CloneCoreMaterials',
}

# Read the header file line by line to respect comments
with open('../cal3d_wrapper.h', 'r') as f:
    lines = f.readlines()

print("// Auto-generated stub implementations for Cal3D wrapper")
print("#include \"cal3d_wrapper_internal.h\"")
print("#include \"cal3d_wrapper.h\"")
print("#include <cstring>")
print()
print("// Stubs for functions not yet fully implemented")
print("// Functions already implemented in other files:")
print("// - CalError_* in cal3d_wrapper_impl.cpp")
print("// - CalVector_* in cal3d_wrapper_impl.cpp")
print("// - CalQuaternion_* in cal3d_wrapper_impl.cpp")
print("// - CalCoreModel_* in core_model.cpp")
print()

for line in lines:
    # Skip commented lines
    stripped = line.strip()
    if stripped.startswith('//'):
        continue
    
    # Skip lines without CAL3D_WRAPPER_API
    if 'CAL3D_WRAPPER_API' not in line:
        continue
    
    # Skip lines without semicolons (not declarations)
    if ';' not in line:
        continue
    
    # Parse the function declaration
    match = line.strip()
    if not match.endswith(';'):
        continue
    
    # Remove the semicolon
    match = match[:-1].strip()
    
    # Skip #define lines and other non-function declarations
    if '#define' in match or 'typedef' in match or 'enum ' in match and '{' in match:
        continue
    
    # Extract return type and function signature
    parts = match.rsplit('(', 1)
    if len(parts) != 2:
        continue
    
    left_part = parts[0].strip()
    params_part = parts[1].rstrip(')')
    
    # Extract function name using regex (more reliable than split)
    func_pattern = r'(\w+)\s*$'
    func_match = re.search(func_pattern, left_part)
    if not func_match:
        continue
    
    func_name = func_match.group(1)
    
    # Get return type (everything before the function name)
    return_type = left_part[:func_match.start()].strip()
    
    # Remove CAL3D_WRAPPER_API prefix from return type if present
    if return_type.startswith('CAL3D_WRAPPER_API'):
        return_type = return_type[len('CAL3D_WRAPPER_API'):].strip()
    
    if not return_type:
        continue
    
    # Skip already implemented functions
    if func_name in SKIP_FUNCTIONS:
        continue
    
    # Skip C++ vector/reference return types - these shouldn't be in the C API
    if 'std::vector' in return_type or '&' in return_type:
        continue
    
    # Generate stub
    print(f"CAL3D_WRAPPER_API {return_type} {func_name}({params_part}) {{")
    
    # Generate appropriate return statement
    if return_type == 'void':
        print("    // TODO: Implement")
    elif return_type == 'void*':
        print("    // TODO: Implement")
        print("    return nullptr;")
    elif return_type == 'enum CalBoolean' or return_type == 'CalBoolean':
        print("    return False;")
    elif 'float' in return_type and '*' in return_type:
        # float* return type
        print("    return nullptr;")
    elif 'float' in return_type and '*' not in return_type:
        # float return type (not pointer)
        print("    return 0.0f;")
    elif ('int' in return_type or 'CalIndex' in return_type) and '*' not in return_type:
        print("    return 0;")
    elif 'char' in return_type and '*' in return_type:
        print("    return nullptr;")
    elif '*' in return_type or '&' in return_type:
        # Any other pointer or reference type
        print("    return nullptr;")
    elif 'enum' in return_type:
        print(f"    return ({return_type})0;")
    elif 'unsigned' in return_type or 'CalUserData' in return_type or 'CalColorValue' in return_type:
        print("    return 0;")
    else:
        print(f"    return ({return_type})0;")
    
    print("}")
    print()
