#ifndef _SKETCHY_SHAPE_
#define _SKETCHY_SHAPE_

#include <utils.hpp>
#include <geometry/geometry.hpp>

class Shape {
public:
	Shape(
		unsigned subSampling,
		const std::vector<glm::vec2> & points
	): subSampling(subSampling), points(points) {
		for(unsigned i=0; i<points.size(); i++) {
			if(i%subSampling == 0) {
				subPoints.push_back(points[i]);
			}
		}
	}

	inline const std::vector<glm::vec2> & getSubSampledPoints() const {
		return subPoints;
	}

	inline const std::vector<glm::vec2> & getFullPoints() const {
		return points;
	}

	inline unsigned fromSubSampleIndexToFullIndex(unsigned index) const {
		return index * subSampling;
	}

	inline std::vector<Geometry::Edge> convertToFullEdgeSet(
		const std::vector<Geometry::Edge> & edges
	) const {
		std::vector<Geometry::Edge> res;
		for(auto edge : edges) {
			res.push_back(Geometry::Edge(
				fromSubSampleIndexToFullIndex(edge.a),
				fromSubSampleIndexToFullIndex(edge.b)
			));
		}
		return res;
	}

	inline std::vector<glm::uvec3> convertToFullTriangleSet(
		const std::vector<glm::uvec3> & triangles
	) const {
		std::vector<glm::uvec3> res;
		for(auto triangle : triangles) {
			res.push_back(glm::uvec3(
				fromSubSampleIndexToFullIndex(triangle.x),
				fromSubSampleIndexToFullIndex(triangle.y),
				fromSubSampleIndexToFullIndex(triangle.z)
			));
		}
		return res;
	}

private:
	std::vector<glm::vec2> points;
	std::vector<glm::vec2> subPoints;
	unsigned subSampling;
};

#endif
