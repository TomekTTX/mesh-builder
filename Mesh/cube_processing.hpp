#pragma once
#include <bitset>
#include <vector>

using Cube = std::bitset<8>;
using FaceLayout = std::bitset<4>;

std::vector<Polygon> getCubePolygons(const Cube& c);
int vertexIndex(const Point& p);
Point edgePoint(int i);



