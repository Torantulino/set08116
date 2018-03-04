#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Material information
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light for the scene
uniform point_light point;
// Material for the object
uniform material mat;
// Eye position
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
  // *********************************
  // Get distance between point light and vertex
  float d = distance(point.position, position);
  // Calculate light colour
  vec4 light_col = (1 / (point.constant + (point.linear * d) + (point.quadratic * (pow(d, 2))))) * point.light_colour;
  // Calculate light dir
  vec3 L = normalize(point.position - position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  // Diffuse
  vec4 diffuse = (max(dot(normal, L), 0.0f)) * (mat.diffuse_reflection * light_col); 
  // View Direction
  vec3 view_dir = normalize(eye_pos - position);
  // Half Vector
  vec3 H = normalize(L + view_dir);
  // Specular
  vec4 specular = (pow(max(dot(normal, H), 0.0f), mat.shininess)) * (mat.specular_reflection * light_col);
  vec4 primary = mat.emissive + diffuse;
  vec4 secondary = specular;
  // Sample Texture colour
  vec4 tex_colour = texture(tex, tex_coord);
  colour = primary * tex_colour + secondary;
  // *********************************
}