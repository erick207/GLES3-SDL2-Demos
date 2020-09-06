#version 300 es

#ifdef GL_ES
precision highp float;
#endif

in vec2 texCoord;

out vec4 fragColour;

uniform sampler2D texSampler;

void main() {
	fragColour = texture(texSampler, texCoord);
}
