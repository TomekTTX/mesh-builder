#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>

struct Point {
    float x, y, z;

    bool operator==(const Point& other) const;

    Point operator-() const { return { -x, -y, -z }; }
    Point operator+(const Point& other) const {
        return { x + other.x, y + other.y, z + other.z };
    };
    Point operator-(const Point& other) const {
        return operator+(-other);
    };

    float squareLength() const { return x * x + y * y + z * z; }
};

using Triangle = std::array<Point, 3>;
using IndexedTriangle = std::array<int32_t, 3>;
using Polygon = std::vector<Point>;

bool floatIsZero(float x);
float tripleProduct(const Point& a, const Point& b, const Point& c);
bool coplanar(const std::array<Point, 4>& pts);
std::vector<Triangle> dividePolygon(const Polygon& poly);

namespace std {
    template <>
    struct hash<Point> {
        std::size_t operator()(const Point& p) const {
            return
                std::hash<float>()(p.x) ^
                (std::hash<float>()(p.y) << 1) ^
                (std::hash<float>()(p.z) << 2);
        }
    };
}

struct Mesh {
    std::vector<Point> vertices;
    std::vector<IndexedTriangle> faces;
};

class MeshBuilder {
private:
    std::vector<Point> m_vertices;
    std::vector<IndexedTriangle> m_faces;
    std::unordered_map<Point, int32_t> m_index;
public:
    MeshBuilder() = default;

    int32_t insertVertex(const Point& p);
    void insertTriangle(const Triangle& tri);
    void insertPolygon(const Polygon& poly);
    void clear();

    Mesh getMesh() const;
    void writePLY(std::ostream& ost);
};