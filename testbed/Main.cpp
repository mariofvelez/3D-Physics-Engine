#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gl_util/Shader.h>
#include <gl_util/Camera.h>

#include "../physics/World.h"
#include "../debug/DebugRenderer.h"

#include <vector>
#include <ctime>

#define DEBUG_LOG
#define DEBIG_TIME

#ifdef DEBUG_LOG
#define print(x) std::cout << x << std::endl
#endif
#ifndef DEBUG_LOG
#define print(x)
#endif
#ifdef DEBIG_TIME
#define time(x) std::time_t t1 = std::time(nullptr); x; std::time_t t2 = std::time(nullptr); std::cout << "time: " << (t2 - t1) << std::endl
#endif
#ifndef DEBIG_TIME
#define time(x)
#endif

Camera* camera;

void createSphere(int width, int height, float rho, float* vertices, int* indices)
{
	int index = 0;
	for (int h = 0; h <= height; ++h)
	{
		for (int w = 0; w < width; ++w)
		{
			float u = ((float)w / (float)width);
			float v = ((float)h / (float)height);
			float theta = u * glm::two_pi<float>();
			float phi = v * glm::pi<float>();

			float x = glm::sin(phi) * glm::cos(theta);
			float z = glm::sin(phi) * glm::sin(theta);
			float y = glm::cos(phi);

			vertices[index * 6 + 0] = x * rho;
			vertices[index * 6 + 1] = -y * rho;
			vertices[index * 6 + 2] = z * rho;
			vertices[index * 6 + 3] = x;
			vertices[index * 6 + 4] = -y;
			vertices[index * 6 + 5] = z;

			if (h < height)
			{
				int top_left = index;
				int top_right = (w < width - 1) ? index + 1 : index - width + 1;
				int bot_left = index + width;
				int bot_right = top_right + width;
				indices[index * 6 + 0] = top_left;
				indices[index * 6 + 1] = top_right;
				indices[index * 6 + 2] = bot_right;
				indices[index * 6 + 3] = bot_right;
				indices[index * 6 + 4] = bot_left;
				indices[index * 6 + 5] = top_left;
			}

			++index;
		}
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	static float last_X = 450;
	static float last_Y = 300;

	static bool firstMouse = true;
	if (firstMouse)
	{
		last_X = xpos;
		last_Y = ypos;
		firstMouse = false;
	}

	float offsetX = xpos - last_X;
	float offsetY = last_Y - ypos;

	last_X = xpos;
	last_Y = ypos;

	static float sensitivity = 0.001f;
	offsetX *= sensitivity;
	offsetY *= sensitivity;

	camera->onMouseCallback(offsetX, offsetY);
}
void scrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	camera->onScrollCallback(offsetX, offsetY);
}
void processInput(GLFWwindow* window, DebugRenderer& renderer, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	renderer.processInput(window, deltaTime);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(900, 600, "Physics Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 900, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ground
	const float ground_width = 10;
	const float ground_height = 10;

	float ground_vertices[] = {
		-ground_width, 0.0f,  ground_height, 0.0f, 1.0f, 0.0f,
		 ground_width, 0.0f,  ground_height, 0.0f, 1.0f, 0.0f,
		 ground_width, 0.0f, -ground_height, 0.0f, 1.0f, 0.0f,
		-ground_width, 0.0f, -ground_height, 0.0f, 1.0f, 0.0f
	};
	int ground_indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);

	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ground_indices), ground_indices, GL_STATIC_DRAW);

	fiz::World* world = new fiz::World();

	DebugRenderer* renderer = new DebugRenderer(world);
	camera = &renderer->camera;

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// render loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input
		processInput(window, *renderer, deltaTime);

		world->step(deltaTime);

		//rendering commands here
		glm::vec3 clear_col = glm::vec3(152.0f, 229.0f, 237.0f);
		clear_col /= 255.0f;
		glClearColor(clear_col.x, clear_col.y, clear_col.z, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer->render();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete(renderer);
	delete(world);

	glfwTerminate();

	return 0;
}