#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

effect eff;
texture tex;
mesh m;
target_camera cam;

bool load_content() {
  // *********************************
  // Load in model, models/teapot.obj
	m = mesh(geometry ("models/orbital/window.obj"));
	mesh s = mesh (geometry ("models/orbital/spoke.obj"));

  // Load in texture, textures/checker.png
	tex = (texture("textures/steel.jpg", true, true)); 
  // *********************************

  // Load in shaders
  eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
  eff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(2000.0f, 2000.0f, 2000.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));

  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 10000.0f);
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
  // Create MVP matrix
  auto M = m.get_transform().get_transform_matrix();
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

  // *********************************
  // Bind texture to renderer
  renderer::bind(tex, 0);
  // Set the texture value for the shader here
  glUniform1i(eff.get_uniform_location("tex"), 0);
  // *********************************

  // Render mesh
  renderer::render(m);

  return true;
}

void main() {
  // Create application
  app application("36_Loading_Models");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}