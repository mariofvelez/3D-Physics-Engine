#version 420 core

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LocalPos;
in vec2 TexCoord;

uniform sampler2D uTexture;

uniform vec3 viewPos;

uniform DirLight dirlight;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction.xyz);

	// ambient
	vec3 ambient = light.ambient;

	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse;

	return (ambient + diffuse);
}

void main()
{
	//FragColor = vec4(vertexColor, 1.0); // colors based on vertex
	//FragColor = texture(uTexture, TexCoord) * vec4(vertexColor, 1.0); // colors texture mixed with vertex colors

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = calcDirLight(dirlight, norm, viewDir);
	result *= texture(uTexture, TexCoord).rgb;

	FragColor = vec4(result, 1.0);
}