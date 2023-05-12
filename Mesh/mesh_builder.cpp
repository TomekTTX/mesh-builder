#include "mesh_builder.hpp"
#include <functional>
#include <ostream>
#include <stdexcept>
#include <string>

template <int32_t maxVal>
struct BoundedValue {
    int32_t v = 0;

    constexpr BoundedValue() = default;
    constexpr BoundedValue(int32_t v) : v(v) {}

    constexpr static int32_t normalize(int32_t x) {
        if (x >= maxVal)
            return x - maxVal;
        if (x < 0)
            return x + maxVal;
        return x;
    }

    constexpr BoundedValue operator+(int32_t x) const {
        return { normalize(v + x) };
    }
    constexpr BoundedValue operator-(int32_t x) const {
        return { normalize(v - x) };
    }
    constexpr BoundedValue operator++() {
        return { v = normalize(v + 1) };
    }

    constexpr operator int32_t() const { return v; }
};

bool Point::operator==(const Point& other) const {
    const Point diff = *this - other;
    return floatIsZero(diff.x) && floatIsZero(diff.y) && floatIsZero(diff.z);
}

std::string getPLYHeader(std::size_t vertexCount, std::size_t faceCount) {
    return
        "ply\n"
        "format ascii 1.0\n"
        "element vertex "
        + std::to_string(vertexCount) + '\n' +
        "property float x\n"
        "property float y\n"
        "property float z\n"
        //"property material_index int\n"
        "element face "
        + std::to_string(faceCount) + '\n' +
        "property list uchar int vertex_index\n"
        //"element material 1\n"
        //"property ambient_red uchar\n"
        //"property ambient_green uchar\n"
        //"property ambient_blue uchar\n"
        //"property ambient_coeff float\n"
        //"property diffuse_red uchar\n"
        //"property diffuse_green uchar\n"
        //"property diffuse_blue uchar\n"
        //"property diffuse_coeff float\n"
        //"property specular_red uchar\n"
        //"property specular_green uchar\n"
        //"property specular_blue uchar\n"
        //"property specular_coeff float\n"
        //"property specular_power float\n"
        "end_header\n";
}

constexpr std::string_view materialString() {
    return
        "255 255 255 0.4 "
        "255 255 255 0.4 "
        "255 255 255 0.2 64\n";
};

void MeshBuilder::writePLY(std::ostream& ost) {
    ost << getPLYHeader(m_vertices.size(), m_faces.size());

    for (const auto& [x, y, z] : m_vertices)
        ost << x << ' ' << y << ' ' << z << /*' ' << '0' <<*/ '\n';
    for (const auto& [p0, p1, p2] : m_faces)
        ost << "3 " << p0 << ' ' << p1 << ' ' << p2 << '\n';
    //ost << materialString();
}

int32_t MeshBuilder::insertVertex(const Point &p) {
	if (m_index.contains(p))
        return m_index.at(p);

	m_vertices.push_back(p);
	return m_index[p] = static_cast<int32_t>(m_vertices.size()) - 1;
	
}

void MeshBuilder::insertTriangle(const Triangle& tri) {
	m_faces.emplace_back();
	for (int32_t i = 0; i < tri.size(); ++i)
		m_faces.back()[i] = insertVertex(tri[i]);
}

void MeshBuilder::insertPolygon(const Polygon& poly) {
    for (const Triangle& tri : dividePolygon(poly))
        insertTriangle(tri);
}

void MeshBuilder::clear() {
    m_faces.clear();
    m_vertices.clear();
    m_index.clear();
}

Mesh MeshBuilder::getMesh() const {
	return { m_vertices, m_faces };
}

bool floatIsZero(float x) {
    return std::fpclassify(x) == FP_ZERO;
}

float tripleProduct(const Point& a, const Point& b, const Point& c) {
    return
        (a.y * b.z - a.z * b.y) * c.x +
        (a.z * b.x - a.x * b.z) * c.y +
        (a.x * b.y - a.y * b.x) * c.z;
}

bool coplanar(const std::array<Point, 4>& pts) {
    return floatIsZero(tripleProduct(
        pts[0] - pts[1],
        pts[0] - pts[2],
        pts[0] - pts[3]
    ));
}

// pentagon only
int32_t noncoplanarVertex(const Polygon& poly) {
    constexpr int32_t outsideIndex[] = { 4,0,1,2,3 };
    std::array<Point, 4> pts{ poly[0], poly[1], poly[2], poly[3] };

    for (int i = 0; i <= 4; ++i) {
        if (coplanar(pts))
            return outsideIndex[i];
        if (i != 4)
            pts[i] = poly[outsideIndex[i]];
    }

    return -1;
}

// heptagon only
int32_t singlePlaneVertex(const Polygon& poly) {
    static const std::function<bool(const Point&)> pred[] = {
        [](const Point& p)->bool { return p.x == 1; },
        [](const Point& p)->bool { return p.y == 1; },
        [](const Point& p)->bool { return p.z == 1; }
    };

    int32_t oneCounts[3] = { 0,0,0 };
    auto tipPointIter = poly.end();

    for (const Point& point : poly) {
        oneCounts[0] += (point.x == 1);
        oneCounts[1] += (point.y == 1);
        oneCounts[2] += (point.z == 1);
    }

    for (int i = 0; i < 3; ++i) {
        if (oneCounts[i] == 1) {
            tipPointIter = std::find_if(poly.begin(), poly.end(), pred[i]);
            break;
        }
    }

    if (tipPointIter != poly.end())
        return static_cast<int32_t>(tipPointIter - poly.begin());

    return -1;
}

std::vector<Triangle> dividePolygon(const Polygon& poly) {
    switch (poly.size()) {
    case 3:
        return { { poly[0], poly[1], poly[2] } };
    case 4:
        return { { poly[0], poly[1], poly[2] },{ poly[2], poly[3], poly[0] } };
    case 5:
        if (BoundedValue<5> ncv = noncoplanarVertex(poly); ncv != -1) {
            return {
                { poly[ncv], poly[ncv + 1], poly[ncv - 1] },
                { poly[ncv + 1], poly[ncv + 2], poly[ncv - 2] },
                { poly[ncv + 1], poly[ncv - 2], poly[ncv - 1] },
            };
        }
        break;
    case 6:
        return {
            { poly[0], poly[1], poly[2] },
            { poly[3], poly[4], poly[5] },
            { poly[0], poly[3], poly[5] },
            { poly[0], poly[2], poly[3] },
        };
    case 7:
        if (BoundedValue<7> spv = singlePlaneVertex(poly); spv != -1) {
            return {
                { poly[spv], poly[spv - 1], poly[spv + 1] },
                { poly[spv - 1], poly[spv + 2], poly[spv + 1] },
                { poly[spv - 1], poly[spv - 2], poly[spv + 2] },
                { poly[spv - 2], poly[spv - 3], poly[spv + 2] },
                { poly[spv - 3], poly[spv + 3], poly[spv + 2] },
            };
        }
        break;
    default:
        break;
    }

    throw std::domain_error("Invalid polygon");
}
