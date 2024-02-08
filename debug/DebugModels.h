#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../stb_image.h"

class DebugModels
{
public:
	unsigned int sphereVAO;
	unsigned int aabbVAO;
	std::vector<unsigned int> polyhedronVAO;

	unsigned int sphere_texture;
	unsigned int aabb_texture;

	DebugModels(int polyhedron_capacity) : sphereVAO(0), aabbVAO(0)
	{
		polyhedronVAO.reserve(polyhedron_capacity);

		// generate models
		{
			const int sphere_width = 20;
			const int sphere_height = 20;

			float sphere_vertices[8 * (sphere_width + 1) * (sphere_height + 1)];
			int sphere_indices[6 * sphere_width * sphere_height];

			createSphere(sphere_width, sphere_height, 1.0f, sphere_vertices, sphere_indices);

			unsigned int sphereVBO;
			glGenBuffers(1, &sphereVBO);
			glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);

			glGenVertexArrays(1, &sphereVAO);
			glBindVertexArray(sphereVAO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

			// vertex positions
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// vertex normals
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// vertex texture coordinates
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			unsigned int sphereEBO;
			glGenBuffers(1, &sphereEBO);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_indices), sphere_indices, GL_STATIC_DRAW);
		}

		{
			float aabb_vertices[288];

			createAABB(aabb_vertices);

			unsigned int aabbVBO;
			glGenBuffers(1, &aabbVBO);
			glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);

			glGenVertexArrays(1, &aabbVAO);
			glBindVertexArray(aabbVAO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(aabb_vertices), aabb_vertices, GL_STATIC_DRAW);

			// vertex positions
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			// normals
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			// vertex texture coordinates
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
		}

		sphere_texture = textureFromFile("debug/debug_res/sphere_texture.png");

		aabb_texture = textureFromFile("debug/debug_res/aabb_texture.png");
	}

private:
	void createSphere(int width, int height, float rho, float* vertices, int* indices)
	{
		int vertex_index = 0;
		int indices_index = 0;
		for (int h = 0; h <= height; ++h)
		{
			for (int w = 0; w <= width; ++w)
			{
				float u = ((float)w / (float)width);
				float v = ((float)h / (float)height);
				float theta = u * glm::two_pi<float>();
				float phi = v * glm::pi<float>();

				float x = glm::sin(phi) * glm::cos(theta);
				float z = glm::sin(phi) * glm::sin(theta);
				float y = glm::cos(phi);

				vertices[vertex_index * 8 + 0] = x * rho;
				vertices[vertex_index * 8 + 1] = -y * rho;
				vertices[vertex_index * 8 + 2] = z * rho;
				vertices[vertex_index * 8 + 3] = x;
				vertices[vertex_index * 8 + 4] = -y;
				vertices[vertex_index * 8 + 5] = z;
				vertices[vertex_index * 8 + 6] = u;
				vertices[vertex_index * 8 + 7] = v;

				++vertex_index;
			}
		}
		for (int h = 0; h < height; ++h)
		{
			for (int w = 0; w < width; ++w)
			{
				int top_left = w * (width + 1) + h;
				int top_right = top_left + 1;
				int bot_left = top_left + width + 1;
				int bot_right = top_right + width + 1;
				indices[indices_index * 6 + 0] = top_left;
				indices[indices_index * 6 + 1] = bot_right;
				indices[indices_index * 6 + 2] = top_right;
				indices[indices_index * 6 + 3] = bot_right;
				indices[indices_index * 6 + 4] = top_left;
				indices[indices_index * 6 + 5] = bot_left;

				++indices_index;
			}
		}
	}

	void createAABB(float* vertices)
	{
		static float aabb_vertices[] = {
			 0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

			 0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f
		};

		for (unsigned int i = 0; i < 288; ++i)
		{
			vertices[i] = aabb_vertices[i];
		}
	}

	unsigned int textureFromFile(const std::string& filename, unsigned int texture_type = GL_TEXTURE_2D)
	{
		stbi_set_flip_vertically_on_load(true);

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, num_components;
		std::cout << "filename: " << filename << std::endl;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &num_components, 0);
		std::cout << "stbi loaded texture: " << textureID << std::endl;
		if (data)
		{
			glBindTexture(texture_type, textureID);
			glTexImage2D(texture_type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(texture_type);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << filename << std::endl;
			stbi_image_free(data);
		}
		return textureID;
	}
};