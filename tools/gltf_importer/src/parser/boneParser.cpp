/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "parser.h"

namespace {
  constexpr float MIN_FLOAT_VAL = 0.000001f;
  float safeFloat(float val) {
    if(std::isnan(val))return 0.0f;
    if(fabsf(val) < MIN_FLOAT_VAL)return 0.0f;
    return val;
  }
}

T3DM::Bone T3DM::parseBoneTree(const cgltf_node *rootBone, Bone *parentBone, int &count, cgltf_skin* skin) {
  Bone bone{};
  bone.name = rootBone->name;

  // Read inverse bind matrix
  bool ibm = false;
  if(skin->inverse_bind_matrices != nullptr)
  {
    auto acc = skin->inverse_bind_matrices;
    auto basePtr = ((uint8_t*)acc->buffer_view->buffer->data) + acc->buffer_view->offset + acc->offset;
    basePtr += Gltf::getDataSize(acc->component_type) * cgltf_num_components(acc->type) * count;
    ibm = true;
    bone.inverseBindPose = Gltf::readAsMat4(basePtr, acc->type, acc->component_type);
  }

  bone.pos = rootBone->has_translation ? Vec3(
    safeFloat(rootBone->translation[0]),
    safeFloat(rootBone->translation[1]),
    safeFloat(rootBone->translation[2])
  ) : Vec3();

  bone.rot = rootBone->has_rotation ? Quat(
    safeFloat(rootBone->rotation[0]),
    safeFloat(rootBone->rotation[1]),
    safeFloat(rootBone->rotation[2]),
    safeFloat(rootBone->rotation[3])
  ) : Quat();

  bone.scale = rootBone->has_scale ? Vec3(
    safeFloat(rootBone->scale[0]),
    safeFloat(rootBone->scale[1]),
    safeFloat(rootBone->scale[2])
  ) : Vec3(1,1,1);

  bone.parentMatrix = parseNodeMatrix(rootBone, nullptr);
  bone.parentIndex = parentBone ? parentBone->index : -1;
  bone.index = count;
  count += 1;

  if(T3DM::config.verbose)
  {
    printf("Bone[%d]: %s (parent: %d | %d)\n", count-1, bone.name.c_str(), bone.parentIndex, parentBone ? parentBone->index : -1);
    printf("      t: %.4f %.4f %.4f\n", rootBone->translation[0], rootBone->translation[1], rootBone->translation[2]);
    printf("      s: %.4f %.4f %.4f\n", rootBone->scale[0], rootBone->scale[1], rootBone->scale[2]);
    printf("      r: %.4f %.4f %.4f %.4f\n", rootBone->rotation[3], rootBone->rotation[0], rootBone->rotation[1], rootBone->rotation[2]);
  }

  if(parentBone) {
    bone.modelMatrix = parentBone->modelMatrix * bone.parentMatrix;
  } else {
    bone.modelMatrix = bone.parentMatrix;
  }

  if (!ibm) {
    bone.inverseBindPose = bone.modelMatrix.inverse();
  }

  for(int i=0; i<rootBone->children_count; ++i) {
    bone.children.push_back(std::make_shared<Bone>(
      parseBoneTree(rootBone->children[i], &bone, count, skin)
    ));
  }
  return bone;
}