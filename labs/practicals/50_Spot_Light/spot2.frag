#version 440

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
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
  // Calculate direction to the light
  vec3 L = normalize(spot.position - position);
  // Calculate distance to light
  float d = distance(spot.position, position);
  // Calculate attenuation value
  float attenuation = (spot.constant + (spot.linear * d) + (spot.quadratic * (pow(d, 2))));
  // Calculate spot light intensity :  
  float intensity pow(max((-spot.direction * L), 0.0f), spot.power);
  // Calculate light colour:
  vec4 light_col = (intensity/attenuation) * spot.light_colour;
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - position);
  // Half Vector
  vec3 H = normalize(L + view_dir);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec4 specular = (pow(max(dot(normal, H), 0.0f), mat.shininess)) * (mat.specular_reflection * light_col);
  vec4 primary = mat.emissive + diffuse;
  vec4 secondary = specular;
  // Sample Texture colour
  vec4 tex_colour = texture(tex, tex_coord);
  colour = primary * tex_colour + secondary;
  // *********************************
}