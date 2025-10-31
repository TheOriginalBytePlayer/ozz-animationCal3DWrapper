//****************************************************************************//
// cal3d_wrapper_impl.cpp                                                     //
// Main implementation of Cal3D wrapper with ozz-animation backend            //
//****************************************************************************//

#ifndef CAL3D_WRAPPER_EXPORTS
#define CAL3D_WRAPPER_EXPORTS
#endif


#include "cal3d_wrapper_internal.h"
#include "cal3d_wrapper.h"
#include <cstring>
#include <cmath>
#include <algorithm>

// Error handling
static std::string g_lastError = "";
static int g_lastErrorLine = 0;
static std::string g_lastErrorFile = "";
static CalErrorCode g_lastErrorCode = ERROR_CODE_OK;

[[maybe_unused]] static void SetError(CalErrorCode code, const char* file, int line, const char* text) {
    g_lastErrorCode = code;
    g_lastErrorFile = file ? file : "";
    g_lastErrorLine = line;
    g_lastError = text ? text : "";
}

//****************************************************************************//
// CalError wrapper functions implementation                                  //
//****************************************************************************//

#ifdef __cplusplus
extern "C" {
#endif

CAL3D_WRAPPER_API enum CalErrorCode CalError_GetLastErrorCode() {
  return g_lastErrorCode;
}

CAL3D_WRAPPER_API const char* CalError_GetLastErrorDescription() {
  return g_lastError.c_str();
}

CAL3D_WRAPPER_API const char* CalError_GetLastErrorFile() {
  return g_lastErrorFile.c_str();
}

CAL3D_WRAPPER_API int CalError_GetLastErrorLine() { return g_lastErrorLine; }

CAL3D_WRAPPER_API const char* CalError_GetLastErrorText() {
  return g_lastError.c_str();
}

CAL3D_WRAPPER_API void CalError_PrintLastError() {
  if (g_lastErrorCode != ERROR_CODE_OK) {
    fprintf(stderr, "Cal3D Error: %s (code %d) at %s:%d\n", g_lastError.c_str(),
            g_lastErrorCode, g_lastErrorFile.c_str(), g_lastErrorLine);
  }
}

//****************************************************************************//
// CalVector wrapper functions implementation                                 //
//****************************************************************************//

CAL3D_WRAPPER_API void* CalVector_New() { return new CalVector(); };

CAL3D_WRAPPER_API void CalVector_Delete(struct CalVector* self) { delete self; }

CAL3D_WRAPPER_API void CalVector_Set(struct CalVector* self, float vx, float vy,
                                     float vz) {
  if (self) {
    self->x = vx;
    self->y = vy;
    self->z = vz;
  }
}

CAL3D_WRAPPER_API float* CalVector_Get(struct CalVector* self) {
  return self ? &self->x : nullptr;
}

CAL3D_WRAPPER_API void CalVector_Add(struct CalVector* self,
                                     struct CalVector* pV) {
  if (self && pV) {
    self->x += pV->x;
    self->y += pV->y;
    self->z += pV->z;
  }
}

CAL3D_WRAPPER_API void CalVector_Subtract(struct CalVector* self,
                                          struct CalVector* pV) {
  if (self && pV) {
    self->x -= pV->x;
    self->y -= pV->y;
    self->z -= pV->z;
  }
}

CAL3D_WRAPPER_API void CalVector_Scale(struct CalVector* self, float d) {
  if (self) {
    self->x *= d;
    self->y *= d;
    self->z *= d;
  }
}

CAL3D_WRAPPER_API void CalVector_InverseScale(struct CalVector* self, float d) {
  if (self && d != 0.0f) {
    self->x /= d;
    self->y /= d;
    self->z /= d;
  }
}

CAL3D_WRAPPER_API float CalVector_Length(struct CalVector* self) {
  if (!self) return 0.0f;
  return std::sqrt(self->x * self->x + self->y * self->y + self->z * self->z);
}

CAL3D_WRAPPER_API float CalVector_Normalize(struct CalVector* self) {
  if (!self) return 0.0f;
  float len = CalVector_Length(self);
  if (len > 0.0f) {
    CalVector_InverseScale(self, len);
  }
  return len;
}

CAL3D_WRAPPER_API void CalVector_Clear(struct CalVector* self) {
  if (self) {
    self->x = self->y = self->z = 0.0f;
  }
}

CAL3D_WRAPPER_API void CalVector_Equal(struct CalVector* self,
                                       struct CalVector* pV) {
  if (self && pV) {
    self->x = pV->x;
    self->y = pV->y;
    self->z = pV->z;
  }
}

CAL3D_WRAPPER_API void CalVector_Blend(struct CalVector* self, float d,
                                       struct CalVector* pV) {
  if (self && pV) {
    self->x += d * pV->x;
    self->y += d * pV->y;
    self->z += d * pV->z;
  }
}

CAL3D_WRAPPER_API void CalVector_Op_Add(struct CalVector* pResult,
                                        struct CalVector* pV,
                                        struct CalVector* pU) {
  if (pResult && pV && pU) {
    pResult->x = pV->x + pU->x;
    pResult->y = pV->y + pU->y;
    pResult->z = pV->z + pU->z;
  }
}

CAL3D_WRAPPER_API void CalVector_Op_Subtract(struct CalVector* pResult,
                                             struct CalVector* pV,
                                             struct CalVector* pU) {
  if (pResult && pV && pU) {
    pResult->x = pV->x - pU->x;
    pResult->y = pV->y - pU->y;
    pResult->z = pV->z - pU->z;
  }
}

CAL3D_WRAPPER_API void CalVector_CalVector_Op_Scale(struct CalVector* pResult,
                                                    struct CalVector* pV,
                                                    float d) {
  if (pResult && pV) {
    pResult->x = pV->x * d;
    pResult->y = pV->y * d;
    pResult->z = pV->z * d;
  }
}

CAL3D_WRAPPER_API void CalVector_CalVector_Op_InverseScale(
    struct CalVector* pResult, struct CalVector* pV, float d) {
  if (pResult && pV && d != 0.0f) {
    pResult->x = pV->x / d;
    pResult->y = pV->y / d;
    pResult->z = pV->z / d;
  }
}

CAL3D_WRAPPER_API float CalVector_Op_Scalar(struct CalVector* pV,
                                            struct CalVector* pU) {
  if (!pV || !pU) return 0.0f;
  return pV->x * pU->x + pV->y * pU->y + pV->z * pU->z;
}

CAL3D_WRAPPER_API void CalVector_Op_Cross(struct CalVector* pResult,
                                          struct CalVector* pV,
                                          struct CalVector* pU) {
  if (pResult && pV && pU) {
    pResult->x = pV->y * pU->z - pV->z * pU->y;
    pResult->y = pV->z * pU->x - pV->x * pU->z;
    pResult->z = pV->x * pU->y - pV->y * pU->x;
  }
}

CAL3D_WRAPPER_API void CalVector_Transform(struct CalVector* self,
                                           struct CalQuaternion* pQ) {
  if (!self || !pQ) return;

  // Convert to ozz types
  auto ozzV = self->ToOzz();
  auto ozzQ = pQ->ToOzz();

  // Transform: v' = q * v * q^(-1)
  auto result = ozz::math::TransformVector(ozzQ, ozzV);

  // Convert back
  self->x = result.x;
  self->y = result.y;
  self->z = result.z;
}

//****************************************************************************//
// CalQuaternion wrapper functions implementation                             //
//****************************************************************************//

CAL3D_WRAPPER_API void* CalQuaternion_New() { return new CalQuaternion(); }

CAL3D_WRAPPER_API void CalQuaternion_Delete(struct CalQuaternion* self) {
  delete self;
}

CAL3D_WRAPPER_API void CalQuaternion_Set(struct CalQuaternion* self, float qx,
                                         float qy, float qz, float qw) {
  if (self) {
    self->x = qx;
    self->y = qy;
    self->z = qz;
    self->w = qw;
  }
}

CAL3D_WRAPPER_API float* CalQuaternion_Get(struct CalQuaternion* self) {
  return self ? &self->x : nullptr;
}

CAL3D_WRAPPER_API void CalQuaternion_Clear(struct CalQuaternion* self) {
  if (self) {
    self->x = self->y = self->z = 0.0f;
    self->w = 1.0f;
  }
}

CAL3D_WRAPPER_API void CalQuaternion_Equal(struct CalQuaternion* self,
                                           const struct CalQuaternion* pQ) {
  if (self && pQ) {
    self->x = pQ->x;
    self->y = pQ->y;
    self->z = pQ->z;
    self->w = pQ->w;
  }
}

CAL3D_WRAPPER_API void CalQuaternion_Multiply(struct CalQuaternion* self,
                                              const struct CalQuaternion* pQ) {
  if (!self || !pQ) return;

  auto ozzSelf = self->ToOzz();
  auto ozzQ = pQ->ToOzz();
  auto result = ozzSelf * ozzQ;

  self->FromOzz(result);
}

CAL3D_WRAPPER_API void CalQuaternion_Op_Multiply(
    struct CalQuaternion* pResult, const struct CalQuaternion* pQ,
    const struct CalQuaternion* pR) {
  if (!pResult || !pQ || !pR) return;

  auto ozzQ = pQ->ToOzz();
  auto ozzR = pR->ToOzz();
  auto result = ozzQ * ozzR;

  pResult->FromOzz(result);
}

CAL3D_WRAPPER_API void CalQuaternion_Blend(struct CalQuaternion* self, float d,
                                           const struct CalQuaternion* pQ) {
  if (!self || !pQ) return;

  // Simple linear interpolation (LERP), could use SLERP for better quality
  self->x += d * pQ->x;
  self->y += d * pQ->y;
  self->z += d * pQ->z;
  self->w += d * pQ->w;

  // Normalize
  float len = std::sqrt(self->x * self->x + self->y * self->y +
                        self->z * self->z + self->w * self->w);
  if (len > 0.0f) {
    self->x /= len;
    self->y /= len;
    self->z /= len;
    self->w /= len;
  }
}

CAL3D_WRAPPER_API void CalQuaternion_Conjugate(struct CalQuaternion* self) {
  if (self) {
    self->x = -self->x;
    self->y = -self->y;
    self->z = -self->z;
  }
}

CAL3D_WRAPPER_API void CalQuaternion_Invert(struct CalQuaternion* self) {
  if (!self) return;

  auto ozzQ = self->ToOzz();
  auto result = ozz::math::Conjugate(ozzQ);

  // Normalize for proper inverse
  float lengthSq = result.x * result.x + result.y * result.y +
                   result.z * result.z + result.w * result.w;
  if (lengthSq > 0.0f) {
    float invLength = 1.0f / lengthSq;
    result.x *= invLength;
    result.y *= invLength;
    result.z *= invLength;
    result.w *= invLength;
  }

  self->FromOzz(result);
}

CAL3D_WRAPPER_API void CalQuaternion_MultiplyVector(
    struct CalQuaternion* self, const struct CalVector* pV) {
  // This is not standard quaternion operation, implementing as no-op
  // or could transform the vector
}

#ifdef __cplusplus
}
#endif

// This file is getting large, will continue in separate files for each major component
