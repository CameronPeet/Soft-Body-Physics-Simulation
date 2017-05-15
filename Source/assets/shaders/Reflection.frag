#version 430 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float specularStrength;
uniform float ambientStrength;

void main()
{
	vec3 ambient = ambientStrength * lightColor;

	//**diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//**specular 
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectionDir = reflect(-lightDir, norm);

	//** Phong Model
	float spec = pow(max(dot(viewDir, reflectionDir),0.0),256);
	vec3 specular = specularStrength * spec * lightColor;
		






    vec3 I = normalize(FragPos - cameraPos);

    vec3 R = reflect(I, normalize(Normal));

	vec3 totalColor = (ambient + diffuse + specular) * objectColor;
    color = vec4(totalColor, 1.0) * texture(skybox, R);
}