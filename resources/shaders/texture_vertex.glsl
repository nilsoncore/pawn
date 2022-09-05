#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texture_coords;

uniform mat4 projection;

out vec3 our_color;
out vec2 TexCoord;

void main () {
	gl_Position = projection * vec4(position, 0.0, 1.0);
	our_color = color;
	TexCoord = texture_coords;
}