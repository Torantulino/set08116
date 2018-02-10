#include <glm\glm.hpp>
#include <graphics_framework.h>
#include <memory>

using namespace std;
using namespace graphics_framework;
using namespace glm;

mesh m;
mesh m1;
effect eff;
target_camera cam;
texture tex;
texture tex2;

bool load_content() {
  // Construct geometry objects
  geometry geom;
  geometry geom1;

  // --Positions--
  //Right Quad
  vector<vec3> positions{	vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), 
							vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)
  };
  //Left Quad
  vector<vec3> positions1{ vec3(-1.0f, 0.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec3(-2.0f, 1.0f, 0.0f),
							vec3(-2.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(-2.0f, 1.0f, 0.0f)
  };

  //--Texture Coords--
  //Right Quad
  vector<vec2> tex_coords{	vec2(0.0f, 1.0f), vec2(0.0f, 0.0f),vec2(1.0f, 0.0f), 
							vec2(1.0f, 1.0f), vec2(0.0f, 1.0f), vec2(1.0f, 0.0f)
  };
  //Left Quad
  vector<vec2> tex_coords1{ vec2(-1.0f, 0.0f), vec2(-1.0f, 1.0f), vec2(-2.0f, 1.0f),
							vec2(-2.0f, 0.0f), vec2(-1.0f, 0.0f), vec2(-2.0f, 1.0f)
  };

  // Add to the geometrys
  geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
  geom1.add_buffer(positions1, BUFFER_INDEXES::POSITION_BUFFER);

  // Add texture coordinate buffer to geometrys
  geom.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
  geom1.add_buffer(tex_coords1, BUFFER_INDEXES::TEXTURE_COORDS_0);

  // Create mesh objects
  m = mesh(geom);
  m1 = mesh(geom1);

  // Load in texture shaders, !Note that are pulling in shader file from previous project!
  eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
  eff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);

  // Build effect
  eff.build();
  // Load texture "textures/sign.jpg"
  tex = texture("textures/sign.jpg", true, false);
  tex2 = texture("textures/sign.jpg", false, false);

  // Set camera properties
  cam.set_position(vec3(-0.5f, 0.5f, 2.5f));
  cam.set_target(vec3(-0.5f, 0.5f, 0.0f));
  auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
  cam.set_projection(quarter_pi<float>(), aspect, 1.0f, 1000.0f);

  //Move Meshes closer together
  m.get_transform().position -= vec3(0.5f, vec2(0.0f));
  m1.get_transform().position += vec3(0.5, vec2(0.0f));

  return true;
}

bool update(float delta_time) {
  // Update the camera
  cam.update(delta_time);
  return true;
}

bool render() {
  // Bind effect
  renderer::bind(eff);
  //------------------QUAD 1---------------
  // Create MVP matrix
  auto M = m.get_transform().get_transform_matrix();
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;

  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), // Location of uniform
                     1,                               // Number of values - 1 mat4
                     GL_FALSE,                        // Transpose the matrix?
                     value_ptr(MVP));                 // Pointer to matrix data


  // Bind texture to renderer
  renderer::bind(tex, 0);

  // Set the texture value for the shader here
  glUniform1i(eff.get_uniform_location("tex"), 0);

  // Render the mesh
  renderer::render(m);

  //------------------QUAD 2---------------
  // Create MVP matrix
  M = m1.get_transform().get_transform_matrix();
  V = cam.get_view();
  P = cam.get_projection();
  MVP = P * V * M;

  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), // Location of uniform
	  1,                               // Number of values - 1 mat4
	  GL_FALSE,                        // Transpose the matrix?
	  value_ptr(MVP));                 // Pointer to matrix data

  renderer::bind(tex2, 0);

  // Render second the mesh
  renderer::render(m1);

  // Set the texture value for the shader here
  glUniform1i(eff.get_uniform_location("tex"), 0);

  return true;
}

void main() {
  // Create application
  app application("28_Repeat_Textures");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}