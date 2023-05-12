#pragma once
#include <array>
#include <bitset>
#include <cstdint>
#include <utility>
#include <vector>

namespace bmp { class BMP; }

class GridArray {
	struct DataIndexer {
		int64_t w, h;
		constexpr int64_t at(int64_t x, int64_t y, int64_t z) const {
			return x + w * (y + h * z);
		}
	};

	struct ImageIndexer {
		int64_t h, interv;
		constexpr std::pair<int64_t, int64_t> at(int64_t x, int64_t y, int64_t z) const {
			return { x * interv, (y + z * h) * interv };
		}
	};
private:
	std::vector<bool> m_data;
	int64_t m_w = -1, m_h = -1, m_d = -1, m_sliceHeight, m_spacing;
	DataIndexer m_di;
public:
	GridArray(const bmp::BMP& image, int64_t sliceHeight, int64_t spacing);

	bool at(int64_t x, int64_t y, int64_t z) const;
	std::array<int64_t, 3> cubeCount() const;
	std::bitset<8> cubeAt(int64_t x, int64_t y, int64_t z) const;
	std::vector<std::bitset<8>> allCubes() const;
private:
	void finishInit(int64_t iw, int64_t ih, int64_t id);
	void handleDuplication(std::array<bool, 3> dup);
};