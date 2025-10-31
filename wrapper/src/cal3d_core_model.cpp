//****************************************************************************//
// core_model.cpp - CalCoreModel wrapper functions
//****************************************************************************//

#include "cal3d_wrapper_internal.h"
#include "cal3d_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

CAL3D_WRAPPER_API void* CalCoreModel_New(const char* name) {
  auto model = new CalCoreModel();
  if (name) {
    model->name = name;
  }
  model->coreSkeleton = nullptr;
  model->userData = nullptr;
  return model;
}

CAL3D_WRAPPER_API void* CalCoreModel_NewCopy(struct CalCoreModel* other) {
  if (!other) return nullptr;

  auto model = new CalCoreModel();
  model->name = other->name;
  model->userData = other->userData;

  // Deep copy skeleton
  if (other->coreSkeleton) {
    model->coreSkeleton = new CalCoreSkeleton();
    // TODO: Deep copy skeleton data
  }

  // Deep copy other data...
  return model;
}

CAL3D_WRAPPER_API void CalCoreModel_Delete(struct CalCoreModel* self) {
  if (!self) return;

  // Clean up all core animations
  for (auto anim : self->coreAnimations) {
    delete anim;
  }

  // Clean up morph animations
  for (auto morphAnim : self->coreMorphAnimations) {
    delete morphAnim;
  }

  // Clean up meshes
  for (auto mesh : self->coreMeshes) {
    if (mesh) {
      for (auto submesh : mesh->submeshes) {
        delete submesh;
      }
      delete mesh;
    }
  }

  // Clean up materials
  for (auto material : self->coreMaterials) {
    delete material;
  }

  // Clean up skeleton
  if (self->coreSkeleton) {
    for (auto bone : self->coreSkeleton->bones) {
      delete bone;
    }
    delete self->coreSkeleton;
  }

  delete self;
}

CAL3D_WRAPPER_API const char* CalCoreModel_GetName(struct CalCoreModel* self) {
  return self ? self->name.c_str() : "";
}

CAL3D_WRAPPER_API void CalCoreModel_SetName(struct CalCoreModel* self,
                                            const char* inName) {
  if (self && inName) {
    self->name = inName;
  }
}

CAL3D_WRAPPER_API void* CalCoreModel_GetCoreSkeleton(
    struct CalCoreModel* self) {
  return self ? self->coreSkeleton : nullptr;
}

CAL3D_WRAPPER_API void CalCoreModel_SetCoreSkeleton(
    struct CalCoreModel* self, struct CalCoreSkeleton* pCoreSkeleton) {
  if (self) {
    self->coreSkeleton = pCoreSkeleton;
  }
}

CAL3D_WRAPPER_API int CalCoreModel_AddCoreAnimation(
    struct CalCoreModel* self, struct CalCoreAnimation* pCoreAnimation) {
  if (!self || !pCoreAnimation) return -1;

  int id = self->coreAnimations.size();
  self->coreAnimations.push_back(pCoreAnimation);
  return id;
}

CAL3D_WRAPPER_API void* CalCoreModel_GetCoreAnimation(struct CalCoreModel* self,
                                                      int coreAnimationId) {
  if (!self || coreAnimationId < 0 ||
      coreAnimationId >= (int)self->coreAnimations.size()) {
    return nullptr;
  }
  return self->coreAnimations[coreAnimationId];
}

CAL3D_WRAPPER_API int CalCoreModel_GetCoreAnimationCount(
    struct CalCoreModel* self) {
  return self ? self->coreAnimations.size() : 0;
}

CAL3D_WRAPPER_API int CalCoreModel_AddCoreMorphAnimation(
    struct CalCoreModel* self, struct CalCoreMorphAnimation* pCoreAnimation) {
  if (!self || !pCoreAnimation) return -1;

  int id = self->coreMorphAnimations.size();
  self->coreMorphAnimations.push_back(pCoreAnimation);
  return id;
}

CAL3D_WRAPPER_API void* CalCoreModel_GetCoreMorphAnimation(
    struct CalCoreModel* self, int morphAnimID) {
  if (!self || morphAnimID < 0 ||
      morphAnimID >= (int)self->coreMorphAnimations.size()) {
    return nullptr;
  }
  return self->coreMorphAnimations[morphAnimID];
}

CAL3D_WRAPPER_API int CalCoreModel_GetCoreMorphAnimationCount(
    struct CalCoreModel* self) {
  return self ? self->coreMorphAnimations.size() : 0;
}

CAL3D_WRAPPER_API int CalCoreModel_AddCoreMaterial(
    struct CalCoreModel* self, struct CalCoreMaterial* pCoreMaterial) {
  if (!self || !pCoreMaterial) return -1;

  int id = self->coreMaterials.size();
  self->coreMaterials.push_back(pCoreMaterial);
  return id;
}

CAL3D_WRAPPER_API void* CalCoreModel_GetCoreMaterial(struct CalCoreModel* self,
                                                     int coreMaterialId) {
  if (!self || coreMaterialId < 0 ||
      coreMaterialId >= (int)self->coreMaterials.size()) {
    return nullptr;
  }
  return self->coreMaterials[coreMaterialId];
}

CAL3D_WRAPPER_API int CalCoreModel_GetCoreMaterialCount(
    struct CalCoreModel* self) {
  return self ? self->coreMaterials.size() : 0;
}

CAL3D_WRAPPER_API int CalCoreModel_AddCoreMesh(struct CalCoreModel* self,
                                               struct CalCoreMesh* pCoreMesh) {
  if (!self || !pCoreMesh) return -1;

  int id = self->coreMeshes.size();
  self->coreMeshes.push_back(pCoreMesh);
  return id;
}

CAL3D_WRAPPER_API void* CalCoreModel_GetCoreMesh(struct CalCoreModel* self,
                                                 int coreMeshId) {
  if (!self || coreMeshId < 0 || coreMeshId >= (int)self->coreMeshes.size()) {
    return nullptr;
  }
  return self->coreMeshes[coreMeshId];
}

CAL3D_WRAPPER_API int CalCoreModel_GetCoreMeshCount(struct CalCoreModel* self) {
  return self ? self->coreMeshes.size() : 0;
}

CAL3D_WRAPPER_API int CalCoreModel_GetCoreMeshId(struct CalCoreModel* self,
                                                 const char* coreMeshName) {
  if (!self || !coreMeshName) return -1;

  auto it = self->meshNameToId.find(coreMeshName);
  if (it != self->meshNameToId.end()) {
    return it->second;
  }
  return -1;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_AddMeshName(
    struct CalCoreModel* self, const char* name, int coreMeshId) {
  if (!self || !name || coreMeshId < 0 ||
      coreMeshId >= (int)self->coreMeshes.size()) {
    return False;
  }

  self->meshNameToId[name] = coreMeshId;
  return True;
}

CAL3D_WRAPPER_API CalUserData
CalCoreModel_GetUserData(struct CalCoreModel* self) {
  return self ? self->userData : nullptr;
}

CAL3D_WRAPPER_API void CalCoreModel_SetUserData(struct CalCoreModel* self,
                                                CalUserData userData) {
  if (self) {
    self->userData = userData;
  }
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_CreateCoreMaterialThread(
    struct CalCoreModel* self, int coreMaterialThreadId) {
  if (!self) return False;

  // Create empty material thread
  self->materialThreads[coreMaterialThreadId] = std::map<int, int>();
  return True;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_SetCoreMaterialId(
    struct CalCoreModel* self, int coreMaterialThreadId, int coreMaterialSetId,
    int coreMaterialId) {
  if (!self) return False;

  self->materialThreads[coreMaterialThreadId][coreMaterialSetId] =
      coreMaterialId;
  return True;
}

CAL3D_WRAPPER_API int CalCoreModel_GetCoreMaterialId(struct CalCoreModel* self,
                                                     int coreMaterialThreadId,
                                                     int coreMaterialSetId) {
  if (!self) return -1;

  auto threadIt = self->materialThreads.find(coreMaterialThreadId);
  if (threadIt == self->materialThreads.end()) return -1;

  auto setIt = threadIt->second.find(coreMaterialSetId);
  if (setIt == threadIt->second.end()) return -1;

  return setIt->second;
}

CAL3D_WRAPPER_API int CalCoreModel_GetMaterialThreadCount(
    struct CalCoreModel* self) {
  return self ? self->materialThreads.size() : 0;
}

CAL3D_WRAPPER_API void CalCoreModel_GetMaterialThreadIDArray(
    struct CalCoreModel* self, int* outThreadIDs) {
  if (!self || !outThreadIDs) return;

  int i = 0;
  for (const auto& pair : self->materialThreads) {
    outThreadIDs[i++] = pair.first;
  }
}

CAL3D_WRAPPER_API int CalCoreModel_GetCoreMaterialIDByName(
    struct CalCoreModel* self, const char* matName) {
  if (!self || !matName) return -1;

  // Search for material by name
  for (size_t i = 0; i < self->coreMaterials.size(); ++i) {
    if (self->coreMaterials[i] && self->coreMaterials[i]->name == matName) {
      return i;
    }
  }
  return -1;
}

CAL3D_WRAPPER_API void CalCoreModel_ReplaceCoreMesh(
    struct CalCoreModel* self, int coreMeshId, struct CalCoreMesh* pCoreMesh) {
  if (!self || coreMeshId < 0 || coreMeshId >= (int)self->coreMeshes.size() ||
      !pCoreMesh) {
    return;
  }

  // Delete old mesh
  auto oldMesh = self->coreMeshes[coreMeshId];
  if (oldMesh) {
    for (auto submesh : oldMesh->submeshes) {
      delete submesh;
    }
    delete oldMesh;
  }

  self->coreMeshes[coreMeshId] = pCoreMesh;
}

CAL3D_WRAPPER_API void CalCoreModel_CloneCoreMaterials(
    struct CalCoreModel* self) {
  if (!self) return;

  // Clone all materials
  std::vector<CalCoreMaterial*> newMaterials;
  for (auto mat : self->coreMaterials) {
    if (mat) {
      auto newMat = new CalCoreMaterial(*mat);
      newMaterials.push_back(newMat);
    }
  }

  // Replace with clones
  for (auto mat : self->coreMaterials) {
    delete mat;
  }
  self->coreMaterials = newMaterials;
}

// Load/Save functions - these need file format parsers
CAL3D_WRAPPER_API int CalCoreModel_LoadCoreAnimation(struct CalCoreModel* self,
                                                     const char* strFilename) {
  // TODO: Implement Cal3D file format parser
  return -1;
}

CAL3D_WRAPPER_API int CalCoreModel_LoadCoreMaterial(struct CalCoreModel* self,
                                                    const char* strFilename) {
  // TODO: Implement Cal3D file format parser
  return -1;
}

CAL3D_WRAPPER_API int CalCoreModel_LoadCoreMesh(struct CalCoreModel* self,
                                                const char* strFilename) {
  // TODO: Implement Cal3D file format parser
  return -1;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_LoadCoreSkeleton(
    struct CalCoreModel* self, const char* strFilename) {
  // TODO: Implement Cal3D file format parser
  return False;
}

CAL3D_WRAPPER_API int CalCoreModel_LoadCoreAnimationFromBuffer(
    struct CalCoreModel* self, const void* buffer) {
  // TODO: Implement Cal3D format parser
  return -1;
}

CAL3D_WRAPPER_API int CalCoreModel_LoadCoreMaterialFromBuffer(
    struct CalCoreModel* self, const void* buffer) {
  // TODO: Implement Cal3D format parser
  return -1;
}

CAL3D_WRAPPER_API int CalCoreModel_LoadCoreMeshFromBuffer(
    struct CalCoreModel* self, const void* buffer) {
  // TODO: Implement Cal3D format parser
  return -1;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_LoadCoreSkeletonFromBuffer(
    struct CalCoreModel* self, const void* buffer) {
  // TODO: Implement Cal3D format parser
  return False;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_SaveCoreAnimation(
    struct CalCoreModel* self, const char* strFilename, int coreAnimtionId) {
  // TODO: Implement Cal3D file format writer
  return False;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_SaveCoreMaterial(
    struct CalCoreModel* self, const char* strFilename, int coreMaterialId) {
  // TODO: Implement Cal3D file format writer
  return False;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_SaveCoreMesh(
    struct CalCoreModel* self, const char* strFilename, int coreMeshId) {
  // TODO: Implement Cal3D file format writer
  return False;
}

CAL3D_WRAPPER_API enum CalBoolean CalCoreModel_SaveCoreSkeleton(
    struct CalCoreModel* self, const char* strFilename) {
  // TODO: Implement Cal3D file format writer
  return False;
}

#ifdef __cplusplus
}
#endif
