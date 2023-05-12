#include <iostream>
#include <fstream>

#include "binary_cube_reader.hpp"
#include "mesh_generator.hpp"

int main() {
	MeshGenerator mgen;
	MeshBuilder mb;
	CubeVector cv;

	{
		std::fstream ifs{ "cubes.bin", std::ios::in | std::ios::binary };
		cv = readCubes(ifs);
	}

	for (auto& [cube, offset] : cv)
		for (auto& tri : mgen.generateMesh(cube, offset))
			mb.insertTriangle(tri);

	{
		std::ofstream ofs{ "out.ply", std::ios::out };
		mb.writePLY(ofs);
	}

	return 0;
}
