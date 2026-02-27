/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#include "parser.h"

Mat4 T3DM::parseNodeMatrix(const cgltf_node *node, const std::unordered_map<std::string, const T3DM::Bone*> *nodeMap)
{
  Mat4 matScale{};
  if(node->has_scale)matScale.setScale({node->scale[0], node->scale[1], node->scale[2]});

  Mat4 matRot{};
  if(node->has_rotation)matRot.setRot({
    node->rotation[0],
    node->rotation[1],
    node->rotation[2],
    node->rotation[3]
  });

  Mat4 matTrans{};
  if(node->has_translation) {
    matTrans.setPos({node->translation[0], node->translation[1], node->translation[2]});
  };

  Mat4 res = matTrans * matRot * matScale;

  if(nodeMap != nullptr && node->parent) {
    // Do not recurse above the skin's root bone
    auto it = nodeMap->find(node->parent->name);
    if(it != nodeMap->end()) {
      auto parentMat = parseNodeMatrix(node->parent, nodeMap);
      res = parentMat * res;
    }
  }

  // remove very small values (underflow issues & '-0' values)
  for(int i=0; i<4; ++i) {
    for(int j=0; j<4; ++j) {
      if(fabs(res.data[i][j]) < 0.0001f)res.data[i][j] = 0.0f;
    }
  }

  return res;
}