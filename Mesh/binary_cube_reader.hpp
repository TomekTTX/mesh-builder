#pragma once
#include <array>
#include <bitset>
#include <istream>
#include <utility>
#include <vector>

#include "mesh_builder.hpp"

using CubeVector = std::vector<std::pair<std::bitset<8>, Point>>;

CubeVector readCubes(std::istream& ist);
