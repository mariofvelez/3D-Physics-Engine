#pragma once
#include <vector>
#include <stdlib.h>

#include "Body.h"
#include "geometry/Shape.h"

namespace fiz
{
#define triple_cross(x, y) glm::cross(glm::cross(x, y), x)

	struct Simplex
	{
		glm::vec3 vertices[4];
		unsigned int length;

		Simplex() : vertices(), length(0) {}

		void addVertex(glm::vec3& vertex)
		{
			vertices[length] = vertex;
			++length;
		}
	};

	class World
	{
	public:
		unsigned int iters;

		std::vector<Shape*> shapes;
		std::vector<Body> bodies;

		inline float random()
		{
			return (float)(rand() % 1000) / 1000.0f;
		}

		World() : gravity(0.0f, -9.8f, 0.0f)
		{
			shapes.reserve(100);
			bodies.reserve(100);

			srand(5);

			for (int i = 0; i < 100; ++i)
			{
				if (random() < 0.5f) // sphere
				{
					float r = random() * 0.5f + 0.5f;
					float x = random() * 10.0f - 5.0f;
					float y = random() * 5.0f + 1.0f;
					float z = random() * 10.0f - 5.0f;
					Sphere* sphere = new Sphere(glm::vec3(0.0f, 0.0f, 0.0f), r);
					shapes.push_back((Shape*)sphere);
					bodies.emplace_back(glm::vec3(x, y, z));
					bodies[bodies.size() - 1].addShape((Shape*)sphere);
				}
				else
				{
					float x = random() * 10.0f - 5.0f;
					float y = random() * 5.0f + 1.0f;
					float z = random() * 10.0f - 5.0f;

					float sx = random() * 0.5f + 0.5f;
					float sy = random() * 0.5f + 0.5f;
					float sz = random() * 0.5f + 0.5f;

					AABB* aabb = new AABB(glm::vec3(-sx, -sy, -sz), glm::vec3(sx, sy, sz));
					shapes.push_back((Shape*)aabb);
					bodies.emplace_back(glm::vec3(x, y, z));
					bodies[bodies.size() - 1].addShape((Shape*)aabb);
				}
			}
		}
		~World() {}

		Shape* createShape() {}

		void step(float dt)
		{
			float l = 10.0f;
			float res = 0.9f;

			for (unsigned int i = 0; i < bodies.size(); ++i)
			{
				bodies[i].m_Vel += gravity * dt;
				bodies[i].m_Pos += bodies[i].m_Vel * dt;

				glm::vec3 lowest = bodies[i].shapes[0]->support(glm::vec3(0.0f, -1.0f, 0.0f));
				lowest += bodies[i].m_Pos;

				if (lowest.y < 0.0f)
				{
					bodies[i].m_Pos.y = bodies[i].m_Pos.y - lowest.y;
					bodies[i].m_Vel.y *= -res;
				}
				if (bodies[i].m_Pos.y > 30.0f)
				{
					bodies[i].m_Pos.y = 30.0f;
					bodies[i].m_Vel.y = 0;
				}
				
			}
			for (unsigned int x = 0; x < 8; ++x)
			{
				for (unsigned int i = 0; i < bodies.size(); ++i)
				{
					for (unsigned int j = 0; j < i; ++j)
					{
						glm::vec3 normal = intersectionNormal(&bodies[i], &bodies[j]);
						//bodies[j].m_Pos += normal;

						if (normal != glm::vec3())
						{
							//solveCollision(bodies[i].getMass(), bodies[j].getMass(), bodies[i].m_Vel, bodies[j].m_Vel, normal);
						}
					}
				}
			}
		}
	private:

		glm::vec3 gravity;

		inline glm::vec3 intersectionNormal(Body* a, Body* b)
		{
			Sphere* s_a = (Sphere*)a->shapes[0];
			Sphere* s_b = (Sphere*)b->shapes[0];
			glm::vec3 dist = b->m_Pos - a->m_Pos;
			glm::vec3 normal = glm::normalize(dist);
			float dist2 = dist.x * dist.x + dist.y * dist.y + dist.z * dist.z;
			float r2 = s_a->rad + s_b->rad;
			if (dist2 < r2 * r2 && a->m_Pos != b->m_Pos)
			{
				float len = r2 - std::sqrtf(dist2);
				normal *= len;
				return normal;
			}
			return glm::vec3(0.0f, 0.0f, 0.0f);
		}
		inline void solveCollision(float m1, float m2, glm::vec3& v1, glm::vec3& v2, glm::vec3& normal)
		{
			static float friction = 0.0f;
			static float restitution = 0.0f;

			float fr = 1 - friction;

			normal = normalize(normal);
			glm::vec3 lat1 = glm::normalize(glm::cross(normal, glm::vec3(0.57f, 0.51f, 0.13f)));
			glm::vec3 lat2 = glm::normalize(glm::cross(normal, lat1));
			
			float m1m2 = m1 + m2;

			float u1 = glm::dot(normal, v1);
			float u2 = glm::dot(normal, v2);
			float l1_1 = glm::dot(lat1, v1);
			float l2_1 = glm::dot(lat1, v2);
			float l1_2 = glm::dot(lat2, v1);
			float l2_2 = glm::dot(lat2, v2);

			float v_1 = ((m1 - m2) / m1m2) * u1 + ((m2 * 2) / m1m2) * u2;
			float v_2 = ((m1 * 2) / m1m2) * u1 + ((m2 - m1) / m1m2) * u2;

			v1 = (normal * v_1 * restitution) + (lat1 * l1_1 * fr) + (lat2 * l1_2 * fr);
			v2 = (normal * v_1 * restitution) + (lat1 * l2_1 * fr) + (lat2 * l2_2 * fr);
		}

		bool gjk(Shape* a, Shape* b)
		{
			glm::vec3 A = support(a, b, glm::vec3(1.0f, 1.0f, 1.0f));

			Simplex s;
			s.addVertex(A);

			glm::vec3 D = -A;

			for (unsigned int i = 0; i < 32; ++i)
			{
				A = support(a, b, D);
				if (glm::dot(A, D) < 0.0f)
					return false;
				s.addVertex(A);

				if (doSimplex(s, D))
					return true;
			}
		}
		glm::vec3 support(Shape* a, Shape* b, glm::vec3 axis)
		{
			return a->support(axis) - b->support(-axis);
		}
		bool doSimplex(Simplex& s, glm::vec3& d)
		{
			switch (s.length)
			{
			case 1:
				return false;
			case 2:
			{
				glm::vec3 ab = s.vertices[1] - s.vertices[0];
				glm::vec3 ao = -s.vertices[1];
				d = triple_cross(ab, ao);
				return false;
			}
			case 3:
			{
				glm::vec3 ab = s.vertices[2] - s.vertices[1];
				glm::vec3 ac = s.vertices[2] - s.vertices[0];
				glm::vec3 ao = -s.vertices[2];
				glm::vec3 abc = glm::cross(ab, ac);
				if (glm::dot(glm::cross(abc, ac), ao) > 0.0f)
				{
					if (glm::dot(ac, ao) > 0.0f) // edge AC was closest
					{
						s.vertices[1] = s.vertices[2];
						s.length = 2;
						d = triple_cross(ac, ao);
					}
					else
					{
						if (glm::dot(ab, ao) > 0.0f) // edge AB was closest
						{
							s.vertices[0] = s.vertices[1]; // [C, B, A] -> [B, A]
							s.vertices[1] = s.vertices[2];
							s.length = 2;
							d = triple_cross(ab, ao);
						}
						else
						{
							s.vertices[0] = s.vertices[2];
							s.length = 1;
							d = ao;
						}
					}
				}
				else
				{
					if (glm::dot(glm::cross(ab, abc), ao) > 0.0f)
					{
						if (glm::dot(ab, ao) > 0.0f) // edge AB was closest
						{
							s.vertices[0] = s.vertices[1]; // [C, B, A] -> [B, A]
							s.vertices[1] = s.vertices[2];
							s.length = 2;
							d = triple_cross(ab, ao);
						}
						else
						{
							s.vertices[0] = s.vertices[2];
							s.length = 1;
							d = ao;
						}
					}
					else
					{
						if (glm::dot(abc, ao) > 0.0f)
						{
							d = abc;
						}
						else
						{
							glm::vec3 temp = s.vertices[1];
							s.vertices[1] = s.vertices[2];
							s.vertices[2] = temp;
							d = -abc;
						}
					}
				}
				return false;
			}
			case 4:
			{
				// regions: ABCD, ABC, ACD, ADB, AB, AC, AD
			}
			}
		}
	};
}