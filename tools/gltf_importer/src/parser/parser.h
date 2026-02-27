/**
* @copyright 2024 - Max Bebök
* @license MIT
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#include "../math/mat4.h"
#include "../cgltfHelper.h"
#include "../structs.h"

namespace fs = std::filesystem;

namespace T3DM
{
  void parseMaterial(const fs::path &gltfBasePath, int i, int j, Model &model, cgltf_primitive *prim);
  Mat4 parseNodeMatrix(const cgltf_node *node, const std::unordered_map<std::string, const T3DM::Bone*> *nodeMap);
  Bone parseBoneTree(const cgltf_node *rootBone, Bone *parentBone, int &count, cgltf_skin* skin);
  Anim parseAnimation(const cgltf_animation &anim, const std::unordered_map<std::string, const Bone*> &nodeMap, uint32_t sampleRate);
}