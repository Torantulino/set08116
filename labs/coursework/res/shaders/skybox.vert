#version 410

// MVP transformation matrix
uniform mat4 MVP;

// Incoming position
layout (location = 0) in vec3 position;
//Incoming tex_coord
layout (location = 10) in vec2 tex_coord_in;

// Outgoing 3D texture coordinate
layout (location = 0) out vec2 tex_coord_out;

void main()
{
	// Calculate screen space position
	gl_Position = MVP * vec4(position, 1.0);

	// Set outgoing texture coordinate
	tex_coord_out = tex_coord_in;
}