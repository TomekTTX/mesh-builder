#include <bmp.hpp>
#include <bitset>
#include <cmath>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

#include "grid_array.hpp"

template <typename T>
const char* bytes(const T& data) {
	return reinterpret_cast<const char*>(&data);
}

void process(std::string_view sourceName, std::string_view targetName, int cubeSize) {
	constexpr int sliceHeight = 32;
	std::ofstream output{ targetName.data(), std::ios::out | std::ios::binary };

	GridArray grid{ sourceName, sliceHeight, cubeSize };

	for (int64_t dimCount : grid.cubeCount())
		output.write(bytes(dimCount), sizeof(dimCount));
	for (const auto& cube : grid.allCubes()) {
		const char cubeBits = static_cast<char>(cube.to_ulong());
		output.write(&cubeBits, sizeof(cubeBits));
	}
}

int main() {
	process("testimg.bmp", "..\\Mesh\\cubes.bin", 2);
}

/*



auto pixelLocations(const std::bitset<3>& off, int increment, int x, int y, int zDist) {
	const int dx = off[0] * increment;
	const int dy = off[1] * increment;
	const int dz = off[2] * increment * zDist;
	std::array<std::pair<int, int>, 8> locs;

	locs[0] = { x,		y + dy };
	locs[1] = { x + dx, y + dy };
	locs[2] = { x + dx, y + dy + dz };
	locs[3] = { x,		y + dy + dz };
	locs[4] = { x,		y };
	locs[5] = { x + dx, y };
	locs[6] = { x + dx,	y + dz };
	locs[7] = { x,		y + dz };

	return locs;
}
	const int increment = cubeSize - 1;
	const int zDist = sliceHeight * image.width();
	const auto [countX, lastX] = divide(image.width(), increment);
	const auto [countY, lastY] = divide(sliceHeight, increment);
	const auto [countZ, lastZ] = divide(image.height() / sliceHeight, increment);

	output.write(bytes(countX + lastX), sizeof(countX));
	output.write(bytes(countY + lastY), sizeof(countY));
	output.write(bytes(countZ + lastZ), sizeof(countZ));

	std::bitset<3> off = 0b111;
	for (int z = 0; z < countZ - 1 + lastZ; ++z) {
		off[1] = true;
		off[2] = (z != countZ - 1);

		for (int y = 0; y < countY - 1 + lastY; ++y) {
			const int baseY = z * sliceHeight + y;
			off[0] = true;
			off[1] = (y != countY - 1);

			for (int x = 0, i; x < countX - 1 + lastX; ++x) {
				off[0] = (x != countX - 1);
				i = 0;
				for (auto& [px, py] : pixelLocations(off, increment, x, baseY, sliceHeight))
					cube[i++] = (image.pixel(px, py) != bmp::colors::black);

				const char cubeBits = static_cast<char>(cube.to_ulong());
				output.write(&cubeBits, sizeof(cubeBits));
			}
		}
	}
*/