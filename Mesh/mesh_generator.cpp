#include "mesh_generator.hpp"
#include "cube_processing.hpp"
#include <algorithm>

std::vector<Triangle> MeshGenerator::generateMesh(const Cube& cube, const Point& offset) {
	const auto makeTriangle = [offset](const IndexedTriangle &itri)->Triangle {
		return {
			edgePoint(itri[0]) + offset,
			edgePoint(itri[1]) + offset,
			edgePoint(itri[2]) + offset
		};
	};

	const auto& indexedTris = *cubeIndex[emplaceCube(cube)];
	std::vector<Triangle> result(indexedTris.size());

	std::transform(indexedTris.begin(), indexedTris.end(), result.begin(), makeTriangle);
	return result;
}

auto meshTriangles(const Mesh& mesh) {
	std::vector<std::array<int, 3>> result;

	for (const auto& tri : mesh.faces) {
		result.emplace_back();
		for (int i = 0; i < 3; ++i)
			result.back()[i] = vertexIndex(mesh.vertices[tri[i]]);
	}

	return result;
}

int MeshGenerator::emplaceCube(const Cube& cube) {
	const int index = static_cast<int>(cube.to_ulong());

	if (!cubeIndex[index]) {
		MeshBuilder mb;

		for (const auto& poly : getCubePolygons(cube))
			mb.insertPolygon(poly);

		Mesh mesh = mb.getMesh();
		fixNormals(cube, mesh);
		cubeIndex[index] = meshTriangles(mesh);
	}

	return index;
}

Point vec(int from, int to, const Mesh& mesh) {
	return mesh.vertices[to] - mesh.vertices[from];
}

Point vecToNearest0(const Cube& cube, Point p) {
	constexpr Point cubePoints[] = {
		{ 0, 0, 0 },
		{ 2, 0, 0 },
		{ 2, 0, 2 },
		{ 0, 0, 2 },
		{ 0, 2, 0 },
		{ 2, 2, 0 },
		{ 2, 2, 2 },
		{ 0, 2, 2 },
	};

	float dist = std::numeric_limits<float>::infinity();
	int nearestIndex = -1;

	for (int i = 0; i < 8; ++i) {
		if (cube[i] == 0) {
			const float tempDist = (p - cubePoints[i]).squareLength();
			if (tempDist < dist) {
				dist = tempDist;
				nearestIndex = i;
			}
		}
	}

	return cubePoints[nearestIndex] - p;
}

void MeshGenerator::fixNormals(const Cube& cube, Mesh& mesh) {
	for (auto& [p0, p1, p2] : mesh.faces) {
		Point triangleVec[2] = { vec(p0, p1, mesh), vec(p0, p2, mesh) };
		Point toZeroVec = vecToNearest0(cube, mesh.vertices[p0]);

		if (tripleProduct(triangleVec[0], triangleVec[1], toZeroVec) < 0)
			std::swap(p1, p2);
	}
}