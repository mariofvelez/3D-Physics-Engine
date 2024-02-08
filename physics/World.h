#pragma once
#include <vector>
#include <stdlib.h>

#include "Body.h"
#include "geometry/Shape.h"

namespace fiz
{
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
	};
}