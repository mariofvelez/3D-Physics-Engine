#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "geometry/Shape.h"

#include <vector>

namespace fiz
{
	class Body
	{
	public:
		glm::vec3 m_Pos;
		glm::vec3 m_Vel;

		float m_Friction;
		float m_Restitutiton;

		std::vector<Shape*> shapes;

		Body() : m_Pos(0.0f, 0.0f, 0.0f), m_Vel(0.0f, 0.0f, 0.0f), m_Friction(0.0f), m_Restitutiton(0.0f), m_Density(1.0f), m_Mass(0.0f), m_InertiaTensor(1.0f)
		{

		}
		Body(glm::vec3 pos) : m_Pos(pos), m_Vel(0.0f, 0.0f, 0.0f), m_Friction(0.0f), m_Restitutiton(0.0f), m_Density(1.0f), m_Mass(0.0f), m_InertiaTensor(1.0f)
		{

		}

		void addShape(Shape* shape)
		{
			shapes.push_back(shape);

			updateMass();
		}

		float setDensity(float density) // returns mass
		{
			m_Density = density;

			updateMass();
		}

		float getMass() const
		{
			return m_Mass;
		}

	private:

		glm::mat3 m_InertiaTensor;

		float m_Density;
		float m_Mass;

		void updateMass()
		{
			m_Mass = 0.0f;
			for (unsigned int i = 0; i < shapes.size(); ++i)
			{
				m_Mass += shapes[i]->computeVolume();
			}
			m_Mass *= m_Density;
		}
	};
}