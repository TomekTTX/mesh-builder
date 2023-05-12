#include "grid_array.hpp"
#include <bmp.hpp>

#include <array>
#include <bitset>
#include <cmath>

constexpr int64_t realDimension(int64_t imageDim, int64_t spacing) {
	return (imageDim - 2) / (spacing - 1) + 2;
}

constexpr bool duplicated(int64_t imageDim, int64_t spacing) {
	return (imageDim - 1) % (spacing - 1) != 0;
}

GridArray::GridArray(const bmp::BMP& image, int64_t sliceHeight, int64_t spacing) :
	m_sliceHeight(sliceHeight), m_spacing(spacing), m_w(-1), m_h(-1), m_d(-1)
{	
	const int64_t iw = image.width(), ih = sliceHeight, id = image.height() / sliceHeight;
	if (iw < spacing || ih < spacing || id < spacing)
		return;
	finishInit(iw, ih, id);

	const bool dupX = duplicated(iw, m_spacing);
	const bool dupY = duplicated(ih, m_spacing);
	const bool dupZ = duplicated(id, m_spacing);
	ImageIndexer iindex{ ih, spacing - 1 };

	for (int64_t z = 0; z < m_d - dupZ; ++z) {
		for (int64_t y = 0; y < m_h - dupY; ++y) {
			for (int64_t x = 0; x < m_w - dupX; ++x) {
				const auto [ix, iy] = iindex.at(x, y, z);
				m_data[m_di.at(x, y, z)] = (image.pixel(ix, iy) != bmp::colors::black);
			}
		}
	}

	handleDuplication({ dupX, dupY, dupZ });
}

void GridArray::finishInit(int64_t iw, int64_t ih, int64_t id) {
	m_w = realDimension(iw, m_spacing);
	m_h = realDimension(ih, m_spacing);
	m_d = realDimension(id, m_spacing);
	m_data.resize(m_w * m_h * m_d);
	m_di = { m_w, m_h };
}

void GridArray::handleDuplication(std::array<bool, 3> dup) {
	if (dup[0]) {
		for (int64_t z = 0; z < m_d - dup[2]; ++z) {
			for (int64_t y = 0; y < m_h - dup[1]; ++y) {
				m_data[m_di.at(m_w - 1, y, z)] = m_data[m_di.at(m_w - 2, y, z)];
			}
		}
	}

	if (dup[1]) {
		for (int64_t z = 0; z < m_d - dup[2]; ++z) {
			for (int64_t x = 0; x < m_w; ++x) {
				m_data[m_di.at(x, m_h - 1, z)] = m_data[m_di.at(x, m_h - 2, z)];
			}
		}
	}

	if (dup[2]) {
		for (int64_t y = 0; y < m_h; ++y) {
			for (int64_t x = 0; x < m_w; ++x) {
				m_data[m_di.at(x, y, m_d - 1)] = m_data[m_di.at(x, y, m_d - 2)];
			}
		}
	}
}

bool GridArray::at(int64_t x, int64_t y, int64_t z) const {
	return m_data[m_di.at(x, y, z)];
}

std::array<int64_t, 3> GridArray::cubeCount() const {
	return { m_w - 1, m_h - 1, m_d - 1 };
}

std::bitset<8> GridArray::cubeAt(int64_t x, int64_t y, int64_t z) const {
	std::bitset<8> c;

	c[0] = at(x, y + 1, z);
	c[1] = at(x + 1, y + 1, z);
	c[2] = at(x + 1, y + 1, z + 1);
	c[3] = at(x, y + 1, z + 1);
	c[4] = at(x, y, z);
	c[5] = at(x + 1, y, z);
	c[6] = at(x + 1, y, z + 1);
	c[7] = at(x, y, z + 1);

	return c;
}

std::vector<std::bitset<8>> GridArray::allCubes() const {
	const auto [cx, cy, cz] = cubeCount();
	std::vector<std::bitset<8>> cubes(cx * cy * cz);

	for (auto z = 0, i = 0; z < cz; ++z) {
		for (auto y = 0; y < cy; ++y) {
			for (auto x = 0; x < cx; ++x) {
				cubes[i++] = cubeAt(x, y, z);
			}
		}
	}

	return cubes;
}