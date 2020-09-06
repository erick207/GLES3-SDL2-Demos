#version 300 es

#ifdef GL_ES
precision highp float;
#endif

in vec2 texCoord;
in vec3 normal;
in vec3 lightVec;

out vec4 fragColour;

uniform vec3 ambientCol; // The light and object's combined ambient colour
uniform vec3 diffuseCol; // The light and object's combined diffuse colour

uniform sampler2D texSampler;

const float invRadiusSq = 0.00001;

void main() {
	// Base colour (from the diffuse texture)
	vec4 colour = texture(texSampler, texCoord);
	
	// Ambient lighting
	vec3 ambient = vec3(ambientCol * colour.xyz);
	
	// Calculate the lighting attenuation, and direction
	float distSq = dot(lightVec, lightVec);
	float attenuation = clamp(1.0 - invRadiusSq * sqrt(distSq), 0.0, 1.0);
	attenuation *= attenuation;
	vec3 lightDir = lightVec * inversesqrt(distSq);
	
	// Diffuse lighting
	vec3 diffuse = max(dot(lightDir, normal), 0.0) * diffuseCol * colour.xyz;
	
	// The final colour
	// NOTE: Alpha channel shouldn't be affected by lights
	vec3 finalColour = (ambient + diffuse) * attenuation;
	fragColour = vec4(finalColour, colour.w);	
}
