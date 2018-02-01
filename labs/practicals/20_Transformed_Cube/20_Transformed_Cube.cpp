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
vec3 pos(0.0f, 0.0f, 0.0f);
float s = 1.0f;

bool load_content() {
	// Create cube data - twelve triangles triangles
	// Positions
	vector<vec3> positions{
		// *********************************
		// Add the position data for triangles here, (6 verts per side)
		// Front
		vec3(4,0,4),
		vec3(4,0,0),
		vec3(4,4,0),

		vec3(4,0,4),
		vec3(4,4,0),
		vec3(4,4,4),

		// Back
		vec3(0,0,4),
		vec3(0,4,4),
		vec3(0,4,0),

		vec3(0,0,4),
		vec3(0,4,0),
		vec3(0,0,0),

		// Right
		vec3(0,0,0),
		vec3(4,4,0),
		vec3(4,0,0),

		vec3(0,0,0),
		vec3(0,4,0),
		vec3(4,4,0),
		// Left
		vec3(0,0,4),
		vec3(4,4,4),
		vec3(0,4,4),

		vec3(0,0,4),
		vec3(4,0,4),
		vec3(4,4,4),

		// Top
		vec3(4,4,4),
		vec3(0,4,0),
		vec3(0,4,4),

		vec3(4,4,4),
		vec3(4,4,0),
		vec3(0,4,0),
		// Bottom
		vec3(0,0,4),
		vec3(0,0,0),
		vec3(4,0,0),

		vec3(4,0,0),
		vec3(4,0,4),
		vec3(0,0,4)
		// *********************************
	};
	// Colours
	vector<vec4> colours;
	for (auto i = 0; i < positions.size(); ++i) {
		colours.push_back(vec4(i % 2, 0.6, 0.0f, 1.0f)); // Notice how I got those Rad colours?
	}
	// Add to the geometry
	geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);

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
	// *********************************
	// Use keys to update transform values
	// WSAD - movement
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		pos += vec3(0.0f, 4.0f, 0.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		pos += vec3(0.0f, -4.0f, 0.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		pos += vec3(4.0f, 0.0f, 0.0f) * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		pos += vec3(-4.0f, 0.0f, 0.0f) * delta_time;
	}
  // Arrow Keys - rotation
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
  // O decrease scale, P increase scale
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_P)) {
		s += 1.0f * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_O)) {
		s -= +1.0f * delta_time;
	}
  // *********************************
  // Update the camera
  cam.update(delta_time);
  return true;
}

bool render() {
  // Bind effect
  renderer::bind(eff);
  mat4 T, R, S, M;
  // *********************************
  // Create transformation matrix
  R = eulerAngleXZ(theta, rho);
  T = translate(mat4(1.0f), pos);
  S = scale(mat4(1.0f), s*(vec3(1.0f,1.0f,1.0f)));
  M = T * (R * S);
  // *********************************
  // Create MVP matrix
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
  app application("20_Transformed_Cube");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}