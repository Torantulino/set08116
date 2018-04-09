#version 440

// The projection transformation
uniform mat4 P;

// Point size for the billboards
uniform float point_size;

// Incoming data
layout(points) in;
// Outgoing data
layout(triangle_strip, max_vertices = 4) out;

// Outgoing texture coordinate
layout(location = 0) out vec2 tex_coord;

void main() {
  // Incoming position
  vec4 position = gl_in[0].gl_Position;

  // Vertex 1 is bottom left
  vec2 va = position.xy + vec2(-0.5, -0.5) * point_size;
  gl_Position = P * vec4(va, position.zw);
  tex_coord = vec2(0.0, 0.0);
  EmitVertex();

  // Vertex 2 is bottom right
  vec2 vb = position.xy + vec2(0.5, -0.5) * point_size;
  gl_Position = P * vec4(vb, position.zw);
  tex_coord = vec2(1.0, 0.0);
  EmitVertex();

  // Vertex 3 is top left
  vec2 vc = position.xy + vec2(-0.5, 0.5) * point_size;
  gl_Position = P * vec4(vc, position.zw);
  tex_coord = vec2(0.0, 1.0);
  EmitVertex();  

  // Vertex 4 it top right
  vec2 vd = position.xy + vec2(0.5, 0.5) * point_size;
  gl_Position = P * vec4(vd, position.zw);
  tex_coord = vec2(1.0, 1.0);
  EmitVertex();

  // End Primitive
  EndPrimitive();
}