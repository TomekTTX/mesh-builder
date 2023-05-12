#include "binary_cube_reader.hpp"

template <typename T>
char* bytes(T& x) {
	return reinterpret_cast<char*>(&x);
}

CubeVector readCubes(std::istream& ist) {
	CubeVector cubes;
	int64_t xM, yM, zM;
	char cubeData = 0;

	ist.read(bytes(xM), sizeof(xM));
	ist.read(bytes(yM), sizeof(yM));
	ist.read(bytes(zM), sizeof(zM));

	cubes.reserve(xM * yM * zM);
	for (float z = 0; z < zM; ++z) {
		for (float y = 0; y < yM; ++y) {
			for (float x = 0; x < xM; ++x) {
				ist.read(&cubeData, sizeof(cubeData));
				cubes.push_back({ cubeData, { 2 * x, 2 * (yM - y - 1), 2 * z } });
			}
		}
	}

	return cubes;
}
