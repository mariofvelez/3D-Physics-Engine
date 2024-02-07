#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gl_util/Shader.h>
#include <gl_util/Camera.h>

#include "DebugModels.h"
#include "../physics/World.h"
#include "../physics/Body.h"
#include "../physics/geometry/Shape.h"

class DebugRenderer
{
public:
	fiz::World* world;

	Camera camera;

	DebugModels models;

	DebugRenderer(fiz::World* world) : world(world), light_direction(0.0f, 0.0f, -1.0f), camera(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, -0.2f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)), models(100)
	{
		// generate shaders
		shader = new Shader("shaders/Vertex.shader", "shaders/Fragment.shader");
		shadow_shader = new Shader("shaders/Vertex.shader", "shaders/Fragment.shader");

		shader->use();

		shader->setVec3("dirlight.direction", light_direction);

		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		float ambient_strength = 0.5f;
		shader->setVec3("dirlight.ambient", color.x * ambient_strength, color.y * ambient_strength, color.z * ambient_strength);
		shader->setVec3("dirlight.diffuse", color.x, color.y, color.z);
		shader->setVec3("dirlight.specular", color.x, color.y, color.z);

		model_loc = shader->uniformLoc("model");
		view_loc = shader->uniformLoc("view");
		proj_loc = shader->uniformLoc("projection");

		shader->setInt("uTexture", 0);

		// camera transform
		proj = glm::perspective(camera.getFOV(), 900.0f / 600.0f, 0.1f, 5000.0f);
		view = camera.createView();
	}

	void render()
	{
		// draw all shapes to shadow map
		// draw all shapes to the scene

		shader->use();

		shader->setVec3("viewPos", camera.m_Pos);

		view = camera.createView();
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

		for (unsigned int i = 0; i < world->bodies.size(); ++i)
		{
			fiz::Body& body = world->bodies[i];

			glm::mat4 model(1.0f);
			model = glm::translate(model, body.m_Pos);
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

			for (unsigned int j = 0; j < body.shapes.size(); ++j)
			{
				fiz::Shape* shape = body.shapes[j];

				switch (shape->shape_type)
				{
				case fiz::SPHERE_TYPE:
				{
					glBindTexture(GL_TEXTURE_2D, models.sphere_texture);
					glBindVertexArray(models.sphereVAO);

					fiz::Sphere* sphere = (fiz::Sphere*)shape;
					shader->setVec3("scale", sphere->rad, sphere->rad, sphere->rad);

					glDrawElements(GL_TRIANGLES, 6 * 20 * 20, GL_UNSIGNED_INT, 0);
					break;
				}
				case fiz::AABB_TYPE:
					break;
				}
			}
		}

		// reset uniforms
		glm::mat4 model(1.0f);
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
		shader->setVec3("scale", 1.0f, 1.0f, 1.0f);
	}

	void processInput(GLFWwindow* window, float delta_time)
	{
		camera.processCameraInput(window, delta_time);
	}

private:
	Shader* shader;
	Shader* shadow_shader;

	glm::vec3 light_direction; // directional light

	glm::mat4 view;
	glm::mat4 proj;

	unsigned int model_loc;
	unsigned int view_loc;
	unsigned int proj_loc;

	unsigned int scale_loc;
};