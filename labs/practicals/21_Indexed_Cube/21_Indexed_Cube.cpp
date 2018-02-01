#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

geometry geom;
effect eff;
target_camera cam;
float theta = 0.0f;
float rho = 0.0f;

bool load_content() {
  // Create cube data - eight corners
  // Positions
  vector<vec3> positions{
      // *********************************
      // Add the position data for cube corners here (8 total)
	  vec3(0,0,4),
	  vec3(0,0,0),
	  vec3(4,0,0),
	  vec3(4,0,4),

	  vec3(0,4,4),
	  vec3(0,4,0),
	  vec3(4,4,0),
	  vec3(4,4,4)
      // *********************************
  };
  // Colours
  vector<vec4> colours;
  for (auto i = 0; i < positions.size(); ++i) {
    colours.push_back(vec4((i + 1) % 2, 0.0f, i % 2, 1.0f));
  }
  // Create the index buffer
  vector<GLuint> indices{
		// *********************************
		// Add index information here - 3 per triangle, 6 per face, 12 triangles
		// Front
		7, 3, 6,
		3, 2, 6,
		// Back
		0, 4, 5,
		0, 5, 1,
		// Right
		1, 5, 6,
		1, 6, 2,
		// Left
		0, 7, 4,
		0, 3, 7,
		// Top
		7, 6, 5,
		7, 5, 4,
		// Bottom
		0, 1, 2,
		0, 2, 3
		// *********************************
  };
  // Add to the geometry
  geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
  geom.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);
  // ****************************
  // Add index buffer to geometry
  // ****************************
  geom.add_index_buffer(indices);

  // Load in shaders
  eff.add_shader("shaders/basic.vert", GL_VERTEX_SHADER);
  eff.add_shader("shaders/basic.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(10.0f, 10.0f, 10.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
  cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
    theta -= pi<float>() * delta_time;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
    theta += pi<float>() * delta_time;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT)) {
    rho -= pi<float>() * delta_time;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT)) {
    rho += pi<float>() * delta_time;
  }
  // Update the camera
  cam.update(delta_time);
  return true;
}

bool render() {
  // Bind effect
  renderer::bind(eff);
  // Create MVP matrix
  mat4 M = eulerAngleXZ(theta, rho);
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Render geometry
  renderer::render(geom);
  return true;
}

void main() {
  // Create application
  app application("21_Indexed_Cube");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}