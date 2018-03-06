#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
using namespace std;
using namespace graphics_framework;
using namespace glm;

mesh sphere;
mesh skybox;
effect eff;
effect sky_eff;
cubemap cube_map;
target_camera cam;

bool load_content() {
  // Create a sphere
  sphere = mesh(geometry_builder::create_sphere(25, 25));
  // *********************************
  // Create box geometry for skybox
  skybox = mesh(geometry_builder::create_box());
  // Scale box by 100
  skybox.get_transform().scale = vec3(100.0f, 100.0f, 100.0f);
  // Load the cubemap
  array<string, 6> filenames = { "textures/space_box_right1.png", "textures/space_box_left2.png", "textures/space_box_top3.png", 
	  "textures/space_box_bottom4.png", "textures/space_box_front5.png", "textures/space_box_back6.png"};
  // Create cube_map
  cube_map = cubemap(filenames);
  // *********************************
  // Load in shaders
  eff.add_shader("57_Skybox/shader.vert", GL_VERTEX_SHADER);
  eff.add_shader("57_Skybox/shader.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // *********************************
  // Load in skybox effect
  sky_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
  sky_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);
  // Build effect
  sky_eff.build();
  // *********************************

  // Set camera properties
  cam.set_position(vec3(0.0f, 0.0f, 10.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}
float theta;
bool update(float delta_time) {
  theta += pi<float>() * delta_time;
  cam.set_position(rotate(vec3(15.0f, 5.0f, 15.0f), theta * 0.05f, vec3(0, 1.0f, 0)));
  cam.update(delta_time);
  // *********************************
  // Set skybox position to camera position (camera in centre of skybox)
  skybox.get_transform().position = cam.get_position();
  // *********************************
  return true;
}

bool render() {
  // *********************************
  // Disable depth test,depth mask,face culling
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDisable(GL_CULL_FACE);
  // Bind skybox effect
  renderer::bind(sky_eff);
  // Calculate MVP for the skybox
  auto M = skybox.get_transform().get_transform_matrix();
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Set cubemap uniform
  renderer::bind(cube_map, 0);
  glUniform1i(eff.get_uniform_location("cubemap"), 0);
  // Render skybox
  renderer::render(skybox);
  // Enable depth test,depth mask,face culling
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  // *********************************

  // Bind effect
  renderer::bind(eff);
  // Create MVP matrix
  M = sphere.get_transform().get_transform_matrix();
  V = cam.get_view();
  P = cam.get_projection();
  MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Render mesh
  renderer::render(sphere);

  return true;
}

void main() {
  // Create application
  app application("57_Skybox");
  // Set methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}