#version 300 es

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTexCoord;

out vec2 texCoord;

uniform mat4 mvpMatrix;

void main() {
	texCoord = vertTexCoord;
	gl_Position = mvpMatrix * vec4(vertPos, 1.0);
}

