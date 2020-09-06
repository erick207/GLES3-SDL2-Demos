#version 300 es

layout(location = 0) in vec2 vertPos;
layout(location = 1) in vec2 vertTexCoord;

out vec2 texCoord;

void main() {
	texCoord = vertTexCoord;
	gl_Position = vec4(vertPos, 0. , 1. );
		
}

