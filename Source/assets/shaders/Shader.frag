
#version 430 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D Texture;
uniform sampler2D Texture2;
uniform float currentTime;


//lighting
uniform vec3 objectColor;

uniform vec3 cameraPos;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float specularStrength;
uniform float ambientStrength;

void main()
{

//**ambient
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
		
		//** blinn Phong Model
		//vec3 H = normalize( lightDir + viewDir);
		//vec3 specular = pow(max(dot(norm, H), 0.0), 256) * lightColor * specularStrength;

		//** rim lighting
		float f = 1.0 - dot(norm, viewDir);// Calculate the rim factor 
		f = smoothstep(0.0, 1.0, f);// Constrain it to the range 0 tos 1 using a smoothstep function     
		f = pow(f, 16);// Raise it to the rim exponent 
		vec3 rim =  f * vec3(1.0f, 0.0f, 0.0f) * lightColor;// Finally, multiply it by the rim color


		vec3 totalColor = (ambient + diffuse + specular + rim) * objectColor;
		color = vec4(totalColor, 1.0f) * texture(Texture, TexCoord);

	

}

/*
float ambientStrength = 0.5f;
	vec3 ambientValue = ambientStrength * vec3(1, 0, 1); //lightColor;
	vec3 finalObjectLight = ambientValue * outColor;
	color = vec4(finalObjectLight, 1.0f);


    //color = mix(texture(Texture, TexCoord), texture(Texture2, TexCoord), 0.5) * vec4(outColor, 10.f) ; //* abs(sin(currentTime));  
*/