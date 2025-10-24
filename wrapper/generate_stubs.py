#!/usr/bin/env python3
"""
Generate stub implementations for all Cal3D wrapper functions
"""

import re

# Read the header file
with open('../cal3d_wrapper.h', 'r') as f:
    content = f.read()

# Extract function declarations
pattern = r'CAL3D_WRAPPER_API\s+([^;]+);'
matches = re.findall(pattern, content)

print("// Auto-generated stub implementations")
print()

for match in matches:
    # Skip #define lines
    if '#define' in match or 'typedef' in match:
        continue
    
    # Parse the function declaration
    match = match.strip()
    
    # Extract return type and function signature
    parts = match.rsplit('(', 1)
    if len(parts) != 2:
        continue
    
    left_part = parts[0].strip()
    params_part = parts[1].rstrip(')')
    
    # Split left part to get return type and function name
    tokens = left_part.split()
    if len(tokens) < 2:
        continue
    
    return_type = ' '.join(tokens[:-1])
    func_name = tokens[-1]
    
    # Generate stub
    print(f"CAL3D_WRAPPER_API {return_type} {func_name}({params_part}) {{")
    
    # Generate appropriate return statement
    if return_type == 'void':
        print("    // TODO: Implement")
    elif return_type == 'enum CalBoolean' or return_type == 'CalBoolean':
        print("    // TODO: Implement")
        print("    return False;")
    elif 'float' in return_type:
        print("    // TODO: Implement")
        print("    return 0.0f;")
    elif 'int' in return_type:
        print("    // TODO: Implement")
        print("    return 0;")
    elif '*' in return_type:
        print("    // TODO: Implement")
        print("    return nullptr;")
    else:
        print("    // TODO: Implement")
        print(f"    return ({return_type})0;")
    
    print("}")
    print()
