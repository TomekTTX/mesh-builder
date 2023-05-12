#include "mesh_builder.hpp"
#include "cube_processing.hpp"

// points laying in the middle of cube edges
constexpr static Point edgePoints[] = {
	{ 1,0,0 },
	{ 2,0,1 },
	{ 1,0,2 },
	{ 0,0,1 },
	{ 1,2,0 },
	{ 2,2,1 },
	{ 1,2,2 },
	{ 0,2,1 },
	{ 0,1,0 },
	{ 2,1,0 },
	{ 2,1,2 },
	{ 0,1,2 },
};

// indices of cube vertices corresponding to each edge
constexpr static std::pair<int, int> edgeBitIndices[] = {
	{ 0,1 },
	{ 1,2 },
	{ 2,3 },
	{ 3,0 },
	{ 4,5 },
	{ 5,6 },
	{ 6,7 },
	{ 7,4 },
	{ 0,4 },
	{ 1,5 },
	{ 2,6 },
	{ 3,7 },
};

// indices of cube vertices corresponding to each face
constexpr static std::array<int, 4> faceBitIndices[] = {
	{ 0,1,2,3 },
	{ 0,1,5,4 },
	{ 1,2,6,5 },
	{ 2,3,7,6 },
	{ 3,0,4,7 },
	{ 4,5,6,7 },
};

// indices of cube faces corresponding to each edge
constexpr static std::pair<int, int> pointToFaceMap[] = {
	{ 0,1 },
	{ 0,2 },
	{ 0,3 },
	{ 0,4 },
	{ 5,1 },
	{ 5,2 },
	{ 5,3 },
	{ 5,4 },
	{ 4,1 },
	{ 1,2 },
	{ 2,3 },
	{ 3,4 },
};

std::vector<int32_t> edgePointIndices(const Cube& c) {
	int ctr = 0;
	std::vector<int32_t> indices;

	for (auto [a, b] : edgeBitIndices) {
		if (c[a] ^ c[b])
			indices.push_back(ctr);
		++ctr;
	}

	return indices;
}

int commonFace(int ind1, int ind2) {
	const auto [f11, f12] = pointToFaceMap[ind1];
	const auto [f21, f22] = pointToFaceMap[ind2];

	if (f11 == f21 || f11 == f22)
		return f11;
	if (f12 == f21 || f12 == f22)
		return f12;
	return -1;
}

int commonVertex(int edge1, int edge2) {
	const auto [v11, v12] = edgeBitIndices[edge1];
	const auto [v21, v22] = edgeBitIndices[edge2];

	if (v11 == v21 || v11 == v22)
		return v11;
	if (v12 == v21 || v12 == v22)
		return v12;
	return -1;
}

FaceLayout faceVertexLayout(const Cube& c, int faceId) {
	auto& vertices = faceBitIndices[faceId];
	FaceLayout layout;

	for (int i = 0; i < 4; ++i)
		layout[i] = c[vertices[i]];

	return layout;
}

int faceBitCount(const Cube& c, int faceId) {
	auto& vertices = faceBitIndices[faceId];
	int count = 0;

	for (int i = 0; i < 4; ++i)
		count += c[vertices[i]];

	return count;
}

bool validContinuation(int from, int to, int faceId, const Cube& cube) {
	switch (faceBitCount(cube, faceId)) {
	case 1:
	case 3:
		return true;
	case 2:
		if (int vertId = commonVertex(from, to); vertId == -1) {
			const FaceLayout layout = faceVertexLayout(cube, faceId);
			if (layout == 0b0101 || layout == 0b1010)
				return false;
			return true;
		}
		else return cube[vertId];
	default:
		return false;
	}
}

std::vector<Polygon> getCubePolygons(const Cube& c) {
	const std::vector<int32_t> edge = edgePointIndices(c);
	std::vector<Polygon> polys;
	std::vector<bool> processed(edge.size(), false);
	auto pointsLeft = processed.size();

	while (pointsLeft) {
		int cur = -1, pointsAdded = -1;
		for (int i = 0; i < processed.size(); ++i) {
			if (!processed[i]) {
				--pointsLeft;
				cur = i;
				processed[i] = true;
				polys.emplace_back(Polygon{ edgePoints[edge[i]] });
				break;
			}
		}
		while (pointsAdded) {
			pointsAdded = 0;
			for (int i = 0; i < processed.size(); ++i) {
				int faceId = -1;
				if (processed[i] || (faceId = commonFace(edge[cur], edge[i])) == -1)
					continue;
				if (validContinuation(edge[cur], edge[i], faceId, c)) {
					polys.back().push_back(edgePoints[edge[i]]);
					--pointsLeft;
					++pointsAdded;
					cur = i;
					processed[i] = true;
				}
			}
		}
	}

	return polys;
}

int vertexIndex(const Point& p) {
	const auto res = std::find(std::begin(edgePoints), std::end(edgePoints), p);
	if (!res)
		return -1;
	return static_cast<int>(res - std::begin(edgePoints));
}

Point edgePoint(int i) {
	return edgePoints[i];
}
