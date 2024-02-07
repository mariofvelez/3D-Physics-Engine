#version 420 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec3 FragPos;
out vec3 LocalPos;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 scale;

void main()
{
	vec3 pos = aPos * scale;
	vec4 pmodel = model * vec4(pos, 1.0);

	gl_Position = projection * view * pmodel;
	FragPos = vec3(model * vec4(aPos, 1.0));

	Normal = mat3(model) * aNormal;
	LocalPos = aPos;

	TexCoord = aTexCoord;
}