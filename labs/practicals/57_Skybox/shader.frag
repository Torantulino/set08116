#version 440

// Point light structure
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};
#endif

// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Point light for the scene
uniform point_light point;
// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // Get distance between point light and vertex
  float d = distance(point.position, position);
  // Calculate light colour
  vec4 light_col = (1 / (point.constant + (point.linear * d) + (point.quadratic * (pow(d, 2))))) * point.light_colour;
  // Calculate light dir
  vec3 L = normalize(point.position - position);
  // Diffuse
  vec4 diffuse = (max(dot(normal, L), 0.0f)) * (mat.diffuse_reflection * light_col); 
  // View Direction
  vec3 view_dir = normalize(eye_pos - position);
  // Half Vector
  vec3 H = normalize(L + view_dir);
  
  // Calculate specular component
  vec4 specular = (pow(max(dot(normal, H), 0.0f), mat.shininess)) * (mat.specular_reflection * light_col);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);
  // Calculate primary colour component
  vec4 primary = mat.emissive + diffuse;
  // Calculate final colour
  vec4 secondary = specular;



  primary.a = 1.0f;
  secondary.a = 1.0f;
  colour = primary * tex_colour + secondary;

  // set black to transparent
  colour.a = tex_colour.x;
}