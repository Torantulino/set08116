#version 440

// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};
#endif

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

vec4 calculatePoint(in point_light point, in vec3 position, in vec3 normal, in material mat, in vec3 view_dir, in vec4 tex_colour) {

  // Get distance between point light and vertex
  float d = distance(point.position, position);
  // Calculate light colour
  vec4 light_col = (1 / (point.constant + (point.linear * d) + (point.quadratic * (pow(d, 2))))) * point.light_colour;
  // Calculate light dir
  vec3 L = normalize(point.position - position);
  // Diffuse
  vec4 diffuse = (max(dot(normal, L), 0.0f)) * (mat.diffuse_reflection * light_col); 
  // Calculate half vector using pre-calculated view_dir
  vec3 H = normalize(L + view_dir);
  // Specular
  vec4 specular = (pow(max(dot(normal, H), 0.0f), mat.shininess)) * (mat.specular_reflection * light_col);
  // Sum colours
  vec4 primary = mat.emissive + diffuse;
  vec4 secondary = specular;
  // Ensure alpha is 1
  primary.a = 1.0f;
  secondary.a = 1.0f;
  // Calculate final colour
  vec4 pointCol = primary * tex_colour + secondary;
  return pointCol;
}

vec4 calculateDirectional(in directional_light direct, in vec3 position, in vec3 normal, in material mat, in vec3 view_dir, in vec4 tex_colour) {
  // Calculate ambient component
  vec4 ambient = mat.diffuse_reflection * direct.ambient_intensity; 
  // Calculate diffuse component
  vec4 diffuse = (max(dot(normal, direct.light_dir), 0.0f)) * (mat.diffuse_reflection * direct.light_colour);
  // Calculate half vector
  vec3 H = normalize(direct.light_dir + view_dir);
  // Calculate specular component
  vec4 specular = (pow(max(dot(normal, H), 0.0f), mat.shininess)) * (mat.specular_reflection * direct.light_colour);  
  // Calculate primary and secondary colour components
  vec4 primary = mat.emissive + ambient + diffuse;
  vec4 secondary = specular;
  // Ensure alpha is 0
  primary.a = 1.0f;
  secondary.a = 1.0f;
  // Calculate final colour
  vec4 directCol = primary * tex_colour + secondary;
  return directCol;
}

void main() {
 
  // View Direction
  vec3 view_dir = normalize(eye_pos - position); 
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);
  // Calculate Point
  colour += calculatePoint(point, position, normal, mat, view_dir, tex_colour);
  // Calculate Directional
  colour += calculateDirectional(light, position, normal, mat, view_dir, tex_colour);
}