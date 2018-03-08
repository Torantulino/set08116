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
texture star_tex; 

mesh skybox;
map<string, mesh> meshes;

bool load_content() { 
  // - Create the earth -
  meshes["earth"] = mesh(geometry_builder::create_sphere(50, 50));
  meshes["earth"].get_transform().scale = vec3(1275.0f);
  //rotate
  quat erot; erot.x = 0.7; erot.y = 0.0f; erot.z = 0.0f; erot.w = -0.7f;
  meshes["earth"].get_transform().orientation = erot;

  // - Create the clouds -
  meshes["earth_cloud"] = mesh(geometry_builder::create_sphere(50, 50));
  meshes["earth_cloud"].get_transform().scale = vec3(1290.0f);
  //rotate to match earth
  meshes["earth_cloud"].get_transform().orientation = erot;

  // - Create the moon -
  meshes["moon"] = mesh(geometry_builder::create_sphere(50,50));
  meshes["moon"].get_transform().scale = vec3(350.0f);
  meshes["moon"].get_transform().position = vec3(0.0f, 0.0f, 38440.0f);

  // - Create the Space Staion -
  // Load in parts
  //wheel
  meshes["wheel"] = mesh(geometry("models/orbital/wheel.obj"));
  meshes["wheel"].get_transform().scale = vec3(0.1f);
  //pod
  meshes["pod"] = mesh(geometry("models/orbital/pod.obj"));
  //window
  meshes["window"] = mesh(geometry("models/orbital/window.obj"));
  //meshes["window"].get_transform().position += vec3(vec2(0.0f), 100.0f);
  //spokes
  meshes["spoke1"] = mesh(geometry("models/orbital/spoke.obj"));
  meshes["spoke2"] = mesh(geometry("models/orbital/spoke.obj"));
  meshes["spoke2"].get_transform().rotate(quat(-cos(0), vec3(0.0f, 0.0f, 1.0f)));
  meshes["spoke3"] = mesh(geometry("models/orbital/spoke.obj"));
  meshes["spoke3"].get_transform().rotate(quat((0), vec3(0.0f, 0.0f, 1.0f)));
  meshes["spoke4"] = mesh(geometry("models/orbital/spoke.obj"));
  meshes["spoke4"].get_transform().rotate(quat(cos(0), vec3(0.0f, 0.0f, 1.0f)));

  /*// - Create the stars (skybox) -
  skybox = mesh(geometry_builder::create_box());
  skybox.get_transform().scale = vec3(45000.0f, 45000.0f, 45000.0f);
  // Load the cubemap
  array<string, 6> filenames = { "textures/space_box_right1.png", "textures/space_box_left2.png", "textures/space_box_top3.png", 
	  "textures/space_box_bottom4.png", "textures/space_box_front5.png", "textures/space_box_back6.png"};
  // Create cube_map
  cube_map = cubemap(filenames);*/

  // - Create the stars (sky_sphere) - 
  skybox = mesh(geometry_builder::create_sphere(50, 50));
  skybox.get_transform().scale = vec3(45000.0f, 45000.0f, 45000.0f);
   

  //Set material
  mat.set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
  mat.set_emissive(vec4(vec3(0.0f), 1.0f));
  mat.set_shininess(50.0f);
  mat.set_specular(vec4(1.0f));

  //Assign Material 
  //#### IF ALL MATERIAL SAME, ONLY ASSIGN VARIABLE ONCE, IF MOST BUT NOT ALL SAME, BUCKET 'MOST'
  //#### (SET MAT -> RENDER 'MOST' -> SET INDIVIDUAL MATERIALS USING partRenderObject())
  meshes["moon"].set_material(mat);
  meshes["earth"].set_material(mat); 
  meshes["earth_cloud"].set_material(mat);

  material hazeMat;
  hazeMat.set_diffuse(vec4(0.7f, 0.7f, 0.9f, 1.0f));
  hazeMat.set_emissive(vec4(vec3(0.0f), 1.0f));
  hazeMat.set_shininess(500.0f);
  hazeMat.set_specular(vec4(1.0f));
  meshes["earth_cloud"].set_material(hazeMat);

  material glassMat;
  glassMat.set_diffuse(vec4(0.74f, 0.776f, 0.818f, 0.2f));
  glassMat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 0.2f));
  glassMat.set_shininess(25.0f);
  glassMat.set_specular(vec4(1.0f));
  meshes["window"].set_material(glassMat);

  // Load Moon texture
  moon_tex = texture("textures/2k_moon.jpg");
  // Load Earrh texture
  earth_tex = texture("textures/earth.jpg");
  // Load Cloud texture
  cloud_tex = texture("textures/earth_clouds.jpg");
  // Load Star texture
  star_tex = texture("textures/starmap_4k.jpg");


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
  space_eff.add_shader("48_Phong_Shading/skybox.vert", GL_VERTEX_SHADER);
  space_eff.add_shader("48_Phong_Shading/skybox.frag", GL_FRAGMENT_SHADER);
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

  // Set Orbital position
  meshes["wheel"].get_transform().position = cam.get_position() - (400.0f * vec3(normalize(cam.get_position() - cam.get_target())));
  meshes["wheel"].get_transform().rotate( vec3(0.0f, normalize(cam.get_position() - cam.get_target()).y, 0.0f));
  

  // Set skybox position to camera position (camera in centre of skybox)
  skybox.get_transform().position = cam.get_position();

  return true;
}

void partRenderObject(effect eff, mesh m, texture tex);

void renderWithHierarchy(effect eff, mesh m, mesh parent, texture tex) {
	// Bind effect
	renderer::bind(eff);

	// Create MVP matrix
	auto M = parent.get_transform().get_transform_matrix() * m.get_transform().get_transform_matrix();
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

	// Set eye position uniform             #...Not a uniform on every effect...#
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));

	renderer::render(m);

}

bool render() {
  // - Render Skybox -
  /*
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
  */

  // - Render Objects -



  // -Render Moon-
  partRenderObject(eff, meshes["moon"], moon_tex);
  // set tex uniform
  glUniform1i(eff.get_uniform_location("tex"), 0);
  // render mesh
  renderer::render(meshes["moon"]);

  // -Render Earth-
  partRenderObject(eff, meshes["earth"], earth_tex);
  // set tex uniform
  glUniform1i(eff.get_uniform_location("tex"), 0);
  // render mesh
  renderer::render(meshes["earth"]);

  // -Render Earth clouds-
  glDisable(GL_CULL_FACE);
  partRenderObject(cloud_eff, meshes["earth_cloud"], cloud_tex);
  // set tex uniform
  glUniform1i(cloud_eff.get_uniform_location("tex"), 0);
  // render mesh
  renderer::render(meshes["earth_cloud"]);
  glEnable(GL_CULL_FACE);

  // -Render Obital-
  //-Wheel-
  partRenderObject(eff, meshes["wheel"], moon_tex);
  // set tex uniform
  glUniform1i(eff.get_uniform_location("tex"), 0); //##POINTLESS??
  // render mesh
  renderer::render(meshes["wheel"]);
  //-Pod-
  renderWithHierarchy(eff, meshes["pod"], meshes["wheel"], moon_tex);
  //-Window-
  renderWithHierarchy(eff, meshes["window"], meshes["wheel"], moon_tex);
  //-Spoke 1-
  renderWithHierarchy(eff, meshes["spoke1"], meshes["wheel"], moon_tex);
  //-Spoke 2-
  renderWithHierarchy(eff, meshes["spoke2"], meshes["wheel"], moon_tex);
  //-Spoke 3-
  renderWithHierarchy(eff, meshes["spoke3"], meshes["wheel"], moon_tex);
  //-Spoke 4-
  renderWithHierarchy(eff, meshes["spoke4"], meshes["wheel"], moon_tex);

  // -Render Star Sphere-
  // disable depth test & mask, faceculling
  //glDisable(GL_DEPTH_TEST); //This makes everything invisible??? #######
  glDepthMask(GL_FALSE);
  glCullFace(GL_FRONT);

  //part render
  partRenderObject(space_eff, skybox, star_tex);
  // set tex unifrom
  glUniform1i(space_eff.get_uniform_location("tex"), 0);
  // render mesh
  renderer::render(skybox);
  
  // Re-enable faceculling, & depth
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);

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

	// Set eye position uniform             #...Not a uniform on every effect...#
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