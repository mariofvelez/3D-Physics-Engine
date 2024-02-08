#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace fiz
{
	enum ShapeType
	{
		SPHERE_TYPE,
		AABB_TYPE,
		POLYHEDRON_TYPE
	};

	class Shape
	{
	public:
		ShapeType shape_type;

		virtual bool intersects(glm::vec3 point) { return false; }

		virtual glm::vec3 support(glm::vec3 axis) { return glm::vec3(0.0f); }

		virtual float computeVolume() { return 0.0f; }
	};

	class Sphere : Shape
	{
	public:
		glm::vec3 pos;
		float rad;

		Sphere(glm::vec3 pos, float rad) : pos(pos), rad(rad), rad2(rad * rad)
		{
			shape_type = SPHERE_TYPE;
		}

		bool intersects(glm::vec3 point)
		{
			return glm::distance(point, pos) <= rad;
		}

		glm::vec3 support(glm::vec3 axis)
		{
			return pos + axis * rad;
		}

		float computeVolume()
		{
			return (4.0f / 3.0f) * glm::pi<float>() * rad2 * rad;
		}
	private:
		float rad2;
	};

	class AABB : Shape
	{
	public:
		glm::vec3 min;
		glm::vec3 max;

		AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max)
		{
			shape_type = AABB_TYPE;
		}

		bool intersects(glm::vec3 point)
		{
			return point.x >= min.x && point.x <= max.x &&
				   point.y >= min.y && point.y <= max.y &&
				   point.z >= min.z && point.z <= max.z;
		}

		glm::vec3 support(glm::vec3 axis)
		{
			float x = axis.x < 0.0f ? min.x : max.x;
			float y = axis.y < 0.0f ? min.y : max.y;
			float z = axis.z < 0.0f ? min.z : max.z;
			return glm::vec3(x, y, z);
		}

		float computeVolume()
		{
			return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
		}
	};

	class Polyhedron : Shape
	{
	public:
		std::vector<glm::vec3> vertices;

		Polyhedron(int vertex_count)
		{
			shape_type = POLYHEDRON_TYPE;
			vertices.reserve(vertex_count);
		}

		void setVertex(unsigned int index, glm::vec3 vec)
		{
			vertices[index] = vec;
		}
	};
}