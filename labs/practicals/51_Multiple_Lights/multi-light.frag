#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

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

// Point lights being used in the scene
uniform point_light points[4];
// Spot lights being used in the scene
uniform spot_light spots[5];
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

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour) {
  // *********************************
  // Get distance between point light and vertex
  float d = distance(point.position, position);
  // Calculate light colour
  vec4 light_col = (1 / (point.constant) + (point.linear * d) + (point.quadratic * (pow(d, 2)))) * point.light_colour;
  // Calculate light dir
  vec3 L = normalize(point.position - position);

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  // Diffuse
  vec4 diffuse = (max(dot(normal, L), 0.0f)) * (mat.diffuse_reflection * light_col); 
  // Half Vector
  vec3 H = normalize(L + view_dir);
  // Specular
  vec4 specular = (pow(max(dot(normal, H), 0.0f), mat.shininess)) * (mat.specular_reflection * light_col);
  vec4 primary = mat.emissive + diffuse;
  vec4 secondary = specular;
  vec4 pColour = primary * tex_colour + secondary;
  sColour.a = 1.0f;
  // *********************************
  return pColour;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // *********************************
  // Calculate direction to the light
  vec3 L = normalize(spot.position - position);
  // Calculate distance to light
  float d = distance(spot.position, position);
  // Calculate attenuation value
  float attenuation = (spot.constant + (spot.linear * d) + (spot.quadratic * (pow(d, 2))));
  // Calculate spot light intensity
  float intensity pow(max((-spot.direction * L), 0.0f), spot.power);
  // Calculate light colour
  vec4 light_col = (intensity/attenuation) * spot.light_colour;
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  // Diffuse
  vec4 diffuse = (max(dot(normal, L), 0.0f)) * (mat.diffuse_reflection * light_col); 
  // Half Vector
  vec3 H = normalize(L + view_dir);
  // Specular
  vec4 specular = (pow(max(dot(normal, H), 0.0f), mat.shininess)) * (mat.specular_reflection * light_col);
  vec4 primary = mat.emissive + diffuse;
  vec4 secondary = specular;
  vec4 sColour = primary * tex_colour + secondary;
  sColour.a = 1.0f;
  // *********************************
  return sColour;
}

void main() {

  colour = vec4(0.0, 0.0, 0.0, 1.0);
  // *********************************
  // Calculate view direction
  vec3 view_direction = normalize(eye_pos - position);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);
  // Sum point lights
  for (int i = 0; i < 5; ++i){
	colour += calculate_point(points[i], mat, position, normal, view_direction, tex_colour);
  }
  // Sum spot lights
  for (int i = 0; i < 6; ++i){
	colour += calculate_spot(spots[i], mat, position, normal, view_direction, tex_colour);
  }
  // *********************************
}