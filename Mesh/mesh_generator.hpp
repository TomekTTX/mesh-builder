#pragma once
#include "mesh_builder.hpp"
#include "cube_processing.hpp"
#include <optional>

class MeshGenerator {
private:
	std::array<std::optional<std::vector<std::array<int, 3>>>, 256> cubeIndex;
public:
	MeshGenerator() = default;

	std::vector<Triangle> generateMesh(const Cube& cube, const Point& offset = {});
private:
	int emplaceCube(const Cube& cube);
	static void fixNormals(const Cube& cube, Mesh &mesh);
};