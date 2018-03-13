#version 410

// Star texture
uniform sampler2D tex;

// Incoming 2D texture coordinate
layout (location = 0) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main() {
	// Sample texture
	colour = texture(tex, tex_coord);
}