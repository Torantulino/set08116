#version 440

// The model transformation
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;
// The ambient intensity of the scene
uniform vec4 ambient_intensity;
// The light colour of the scene
uniform vec4 light_colour;
// Direction of the light
uniform vec3 light_dir;
// The diffuse reflection colour
uniform vec4 diffuse_reflection;
// The specular reflection colour
uniform vec4 specular_reflection;
// Shininess of the object
uniform float shininess;
// Position of the camera
uniform vec3 eye_pos;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;
// Outgoing vertex colour
layout(location = 0) out vec4 vertex_colour;

void main() {
  // Calculate position
  gl_Position = MVP * vec4(position, 1.0f);
  // Calculate ambient component
  vec4 ambient = diffuse_reflection * ambient_intensity;
  // Transform the normal
  vec3 transformed_normal = N * normal;
  // Calculate k
  float k = max(dot(transformed_normal, light_dir), 0.0f);
  // Calculate diffuse
  vec4 diffuse = k * (diffuse_reflection * light_colour);
  // Calculate world position of vertex
  vec4 world_position = M * vec4(position, 1.0f);
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - vec3(world_position));
  // Calculate half vector between view_dir and light_dir
  vec3 H = normalize(view_dir + light_dir);
  // Calculate k
  float k2 = pow(max(dot(transformed_normal, H), 0.0f), shininess);
  // Calculate specular
  vec4 specular = k2 *(specular_reflection * light_colour);
  // Output combined components
  vertex_colour = ambient + diffuse + specular;
  // *********************************
  // Ensure alpha is 1
  vertex_colour.a = 1.0;
}