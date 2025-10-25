#!/bin/bash
# Generate stub files for all wrapper functions

cat > src/all_stubs.cpp << 'STUBEOF'
//****************************************************************************//
// all_stubs.cpp - Stub implementations for all Cal3D wrapper functions
// This file contains placeholder implementations that return sensible defaults
// TODO: Implement each function properly with ozz-animation backend
//****************************************************************************//

#include "cal3d_wrapper.h"
#include "cal3d_wrapper_internal.h"

// NOTE: This file contains AUTO-GENERATED STUBS
// Many functions are implemented in separate files:
// - cal3d_wrapper_impl.cpp (Vector, Quaternion, Error)
// - core_model.cpp (CoreModel functions)
// Remaining stubs are here as placeholders

STUBEOF

chmod +x src/stubs_generator.sh
