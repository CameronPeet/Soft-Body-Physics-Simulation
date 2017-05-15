#version 450 core

in vec2 TexCoord;
in vec3 worldPos;
in vec3 FragPos;
in vec3 Normal;

out vec4 color;

// texture
uniform sampler2D texture;
uniform sampler2D texture1;
uniform sampler2D texture2;

float fRenderHeight = 70.0f;

//lighting
uniform float specularStrength;
uniform float ambientStrength;

uniform vec3 objectColor;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 lightColor;


void main(){

	float fScale = worldPos.y/fRenderHeight;

	vec4 vTexColor = vec4(0.0);
	
	const float fRange1 = 0.2f;
	const float fRange2 = 0.3f;
	const float fRange3 = 0.65f;
    const float fRange4 = 0.85f;
	

	if(fScale >= 0.0 && fScale <= fRange1)
		vTexColor = texture2D(texture, TexCoord);
	
	else if(fScale <= fRange2){
		fScale -= fRange1;
		fScale /= (fRange2-fRange1);
		
		float fScale2 = fScale;
		fScale = 1.0-fScale; 
		
		vTexColor += texture2D(texture, TexCoord)*fScale;
		vTexColor += texture2D(texture1, TexCoord)*fScale2;
	}
	
	else if(fScale <= fRange3)
		vTexColor = texture2D(texture1, TexCoord);
	
	else if(fScale <= fRange4){
		fScale -= fRange3;
		fScale /= (fRange4-fRange3);
		
		float fScale2 = fScale;
		fScale = 1.0-fScale; 
		
		vTexColor += texture2D(texture1, TexCoord)*fScale;
		vTexColor += texture2D(texture2, TexCoord)*fScale2;		
	}	
	
	else vTexColor = texture2D(texture2, TexCoord);
	
	//lighting
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
		//float spec = pow(max(dot(viewDir, reflectionDir),0.0),256);
		//vec3 specular = specularStrength * spec * lightColor;
		
		//** blinn Phong Model
		vec3 H = normalize( lightDir + viewDir);
		vec3 specular = pow(max(dot(norm, H), 0.0), 256) * lightColor * specularStrength;

		//** rim lighting
		float f = 1.0 - dot(norm, viewDir);// Calculate the rim factor 
		f = smoothstep(0.0, 1.0, f);// Constrain it to the range 0 to 1 using a smoothstep function     
		f = pow(f, 16);// Raise it to the rim exponent 
		vec3 rim =  f * vec3(1.0f, 1.0f, 0.0f) * lightColor;// Finally, multiply it by the rim color

		vec3 totalColor = (ambient + diffuse + specular + rim) * objectColor;
	
		color = vec4(totalColor, 1.0f) * vTexColor;

		float d = distance(worldPos.xyz, cameraPos);
		float lerp = (d) / 200.0f;
		lerp = clamp(lerp, 0.0, 1.0);

		vec4 vFogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

		color = mix(color, vFogColor, lerp);
		
		//color =  vTexColor;
}