#version 300 es

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTexCoord;
layout(location = 2) in vec3 vertNormal;

out vec2 texCoord;
out vec3 normal;
out vec3 lightVec;

uniform mat4 mvMat;
uniform mat4 normalMat;
uniform mat4 projMat;
uniform vec3 lightPos; // NOTE: position in view space ( so after being
					   //transformed by its own MV matrix)

void main() {
	
	// Pass the texture coordinate
	texCoord = vertTexCoord;
	
	// Calc. the position in view space
	vec4 viewPos = mvMat * vec4(vertPos, 1.0);
	
	// Calc. the position
	gl_Position = projMat * viewPos;
	
	// Transform the normal
	normal = normalize((normalMat * vec4(vertNormal, 1.0)).xyz);
	
	// Calc. the light vector
	lightVec = lightPos - viewPos.xyz;
}
