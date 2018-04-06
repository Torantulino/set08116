#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect shadow_eff;
texture tex;
target_camera cam;
spot_light spot;
shadow_map shadow;

bool load_content() {
  // *********************************
  // Create shadow map- use screen size
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());
  // Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane());
  // Create "teapot" mesh by loading in models/teapot.obj
	meshes["teapot"] = mesh(geometry("models/teapot.obj"));
  // Need to rotate the teapot on x by negative pi/2
	meshes["teapot"].get_transform().rotate(eulerAngleX(half_pi<float>()));
  // Scale the teapot - (0.1, 0.1, 0.1)
	meshes["teapot"].get_transform().scale = vec3(0.1f);
	// ***********************
  // Set materials
  // - all emissive is black
  // - all specular is white
  // - all shininess is 25
  // ***********************
  // White plane
	material planeMat;
	planeMat.set_diffuse(vec4(1.0f));
	planeMat.set_emissive(vec4(vec3(0.0f), 0.1f));
	planeMat.set_specular(vec4(1.0f));
	planeMat.set_shininess(25);
  // Red teapot
	material teapotMat;
	planeMat.set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	planeMat.set_emissive(vec4(vec3(0.0f), 0.1f));
	planeMat.set_specular(vec4(1.0f));
	planeMat.set_shininess(25);
  // *********************************

  // Set spot properties
  // Pos (20, 30, 0), White
  // Direction (-1, -1, 0) normalized
  // 50 range, 10 power
  spot.set_position(vec3(20.0f, 30.0f, 0.0f));
  spot.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
  spot.set_direction(normalize(vec3(-1.0f, -1.0f, 0.0f)));
  spot.set_range(50.0f);
  spot.set_power(10.0f);

  // Load in shaders
  shadow_eff.add_shader("50_Spot_Light/spot.vert", GL_VERTEX_SHADER);
  shadow_eff.add_shader("50_Spot_Light/spot.frag", GL_FRAGMENT_SHADER);
  // Build effect
  shadow_eff.build();

  // Set camera properties
  cam.set_position(vec3(0.0f, 20.0f, -30.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  // Rotate the teapot
  meshes["teapot"].get_transform().rotate(vec3(0.0f, 0.0f, half_pi<float>()) * delta_time);

  // *********************************
  // Update the shadow map light_position from the spot light
  shadow.light_position = spot.get_position();
  // do the same for light_dir property
  shadow.light_dir = spot.get_direction();
  // *********************************

  // Press s to save
  if (glfwGetKey(renderer::get_window(), 'S') == GLFW_PRESS) {
    shadow.buffer->save("test.png");
  }

  cam.update(delta_time);

  return true;
}

bool render() {
  // *********************************
  // Set render target to shadow map
	renderer::set_render_target(shadow);
  // Clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);
  // Set face cull mode to front
	glCullFace(GL_FRONT);
  // *********************************

  // Bind shader
  renderer::bind(shadow_eff);

  // Render meshes
  for (auto &e : meshes) {
    auto m = e.second;
    // Create MVP matrix
    auto M = m.get_transform().get_transform_matrix();
    // *********************************
    // View matrix taken from shadow map
	auto V = shadow.get_view();
    // *********************************

    auto P = cam.get_projection();
    auto MVP = P * V * M;
    // Set MVP matrix uniform
    glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
    // Render mesh
    renderer::render(m);
  }

  // *********************************
  // Set render target back to the screen
  renderer::set_render_target();
  // Set face cull mode to back
  glCullFace(GL_BACK);
  // *********************************

  return true;
}

void main() {
  // Create application
  app application("53_Shadow_Mapping");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}