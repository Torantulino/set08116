#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
using namespace std;
using namespace graphics_framework;
using namespace glm;

effect eff;
effect cloud_eff;
effect space_eff;


cubemap cube_map;
target_camera cam;
directional_light light;
material mat;

texture moon_tex;
texture earth_tex;
texture cloud_tex;

mesh skybox;
map<string, mesh> meshes;

bool load_content() { 
  // Create the earth
  meshes["earth"] = mesh(geometry_builder::create_sphere(50, 50));
  meshes["earth"].get_transform().scale = vec3(1275.0f);
  quat erot; erot.x = 0.7; erot.y = 0.0f; erot.z = 0.0f; erot.w = -0.7f;
  meshes["earth"].get_transform().orientation = erot;

  meshes["earth_cloud"] = mesh(geometry_builder::create_sphere(50, 50));
  meshes["earth_cloud"].get_transform().scale = vec3(1290.0f);
  meshes["earth_cloud"].get_transform().orientation = erot;

  // Create the moon
  meshes["moon"] = mesh(geometry_builder::create_sphere(50,50));
  meshes["moon"].get_transform().scale = vec3(350.0f);
  meshes["moon"].get_transform().position = vec3(0.0f, 0.0f, 38440.0f);
 
  // Create the stars (skybox)
  skybox = mesh(geometry_builder::create_box());
  // Scale box by 100
  skybox.get_transform().scale = vec3(45000.0f, 45000.0f, 45000.0f);
  // Load the cubemap
  array<string, 6> filenames = { "textures/space_box_right1.png", "textures/space_box_left2.png", "textures/space_box_top3.png", 
	  "textures/space_box_bottom4.png", "textures/space_box_front5.png", "textures/space_box_back6.png"};
  // Create cube_map
  cube_map = cubemap(filenames);

  //Set material
  mat.set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
  mat.set_emissive(vec4(vec3(0.0f), 1.0f));
  mat.set_shininess(50.0f);
  mat.set_specular(vec4(1.0f));

  //Assign Material
  meshes["moon"].set_material(mat);
  meshes["earth"].set_material(mat); 

  material hazeMat;
  hazeMat.set_diffuse(vec4(0.7f, 0.7f, 0.9f, 1.0f));
  hazeMat.set_emissive(vec4(vec3(0.0f), 1.0f));
  hazeMat.set_shininess(500.0f);
  hazeMat.set_specular(vec4(1.0f));
  meshes["earth_cloud"].set_material(hazeMat);


  // Load Moon texture
  moon_tex = texture("textures/2k_moon.jpg");
  // Load Earrh texture
  earth_tex = texture("textures/earth.jpg");
  // Load Clour texture
  cloud_tex = texture("textures/earth_clouds.jpg");

  //Set light
  // ambient intensity (0.3, 0.3, 0.3)
  light.set_ambient_intensity(vec4(vec3(0.01f), 1.0f));
  // Light colour white
  light.set_light_colour(vec4(vec3(0.8f), 1.0f));
  // Light direction (1.0, 1.0, -1.0)
  light.set_direction(vec3(1.0f, 1.0f, -1.0f));

  // Load in cloud shaders
  cloud_eff.add_shader("48_Phong_Shading/phong.vert", GL_VERTEX_SHADER);
  cloud_eff.add_shader("57_Skybox/shader.frag", GL_FRAGMENT_SHADER);
  cloud_eff.build();

  // Load in Phong shaders
  eff.add_shader("48_Phong_Shading/phong.vert", GL_VERTEX_SHADER);
  eff.add_shader("48_Phong_Shading/phong.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Load in skybox effect
  space_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
  space_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);
  // Build effect
  space_eff.build();  

  // Set camera properties
  cam.set_position(vec3(0.0f, 0.0f, 2000.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 50000.0f);
  return true;
}
float theta;
bool update(float delta_time) { 
  theta += pi<float>() * delta_time;
  cam.set_position(rotate(vec3(2000.0f, 5.0f, 2000.0f), theta * 0.05f, vec3(0, 1.0f, 0)));
  cam.update(delta_time);
  // Set skybox position to camera position (camera in centre of skybox)
  skybox.get_transform().position = cam.get_position();

  if(glfwGetKey(renderer::get_window(), GLFW_KEY_W)){
  meshes["earth"].get_transform().rotate(vec3(5.0f, 0.0f, 0.0f) * delta_time);
  quat currentRot = meshes["earth"].get_transform().orientation;
  std::cout << currentRot.x << ", " << currentRot.y << ", " << currentRot.z << ", " << currentRot.w << endl;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
	  meshes["earth"].get_transform().rotate(vec3(0.0f, 5.0f, 0.0f) * delta_time);
	  quat currentRot = meshes["earth"].get_transform().orientation;
	  std::cout << currentRot.x << ", " << currentRot.y << ", " << currentRot.z << ", " << currentRot.w << endl;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q)) {
	  meshes["earth"].get_transform().rotate(vec3(0.0f, 0.0f, 5.0f) * delta_time);
	  quat currentRot = meshes["earth"].get_transform().orientation;
	  std::cout << currentRot.x << ", " << currentRot.y << ", " << currentRot.z << ", " << currentRot.w << endl; 
  }


  return true;
}

void partRenderObject(effect eff, mesh m, texture tex);

bool render() {
  // - Render Skybox -

  // Disable depth test, depth mask & face culling
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDisable(GL_CULL_FACE);

  // Bind skybox effect
  renderer::bind(space_eff);

  // Calculate MVP for the skybox
  auto M = skybox.get_transform().get_transform_matrix();
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;

  // Set MVP matrix uniform
  glUniformMatrix4fv(space_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Set cubemap uniform
  renderer::bind(cube_map, 0);
  glUniform1i(space_eff.get_uniform_location("cubemap"), 0);
  // Render skybox
  renderer::render(skybox);

  // Re-enable depth test,depth mask & face culling
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);


  // - Render Objects -

  //Render Moon
  partRenderObject(eff, meshes["moon"], moon_tex);
  // Set tex uniform
  glUniform1i(eff.get_uniform_location("tex"), 0);
  // Render mesh
  renderer::render(meshes["moon"]);

  //Render Earth
  partRenderObject(eff, meshes["earth"], earth_tex);
  // Set tex uniform
  glUniform1i(eff.get_uniform_location("tex"), 0);
  // Render mesh
  renderer::render(meshes["earth"]);

  //Render Earth clouds
  glDisable(GL_CULL_FACE);
  partRenderObject(cloud_eff, meshes["earth_cloud"], cloud_tex);
  // Set tex uniform
  glUniform1i(cloud_eff.get_uniform_location("tex"), 0);
  //Render mesh
  renderer::render(meshes["earth_cloud"]);
  glEnable(GL_CULL_FACE);
  return true;
}

void partRenderObject(effect eff, mesh m, texture tex) {
	// Bind effect
	renderer::bind(eff);

	// Create MVP matrix
	auto M = m.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;

	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Set M matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	// Set N matrix uniform
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));

	// Bind material
	renderer::bind(m.get_material(), "mat");
	// Bind light
	renderer::bind(light, "light");
	// Bind texture
	renderer::bind(tex, 0);

	// Set eye position uniform
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));

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