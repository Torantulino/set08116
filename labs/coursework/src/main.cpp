//--------------------------------
//Author: Toran Bruce Richards
// Spying Earth, Part I
// 15/03/2018
// Controls:
// F - Toggle 'Shuttle Cam'
// WASD - Move 'Shuttle Cam' with sustained velocity
// . - Toggle Constellations
// , - Toggle Stellar Grid
//-------------------------------

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
using namespace std;
using namespace graphics_framework;
using namespace glm;

//Effects
effect eff;
effect light_eff;
effect cloud_eff;
effect space_eff;
effect skybox_overlay_eff;
effect shadow_eff;

shadow_map shadow;

//Cameras
target_camera cam;
free_camera free_cam;
camera* active_cam = &cam;

//Lights
directional_light light;
point_light point;

//textures
texture moon_tex;
texture earth_tex;
texture light_tex;
texture cloud_tex;
texture star_tex;
texture grid_tex;
texture constellation_tex;
texture cockpit_tex;

//Meshes
mesh skybox;
mesh stargrid;
mesh constellations;
map<string, mesh> meshes;

//Global Variables
vec3 velocity = vec3(0.0f);
mat4 LightProjectionMat;
float angularV = 0.0f;
float angularA = 0.1f;
float theta;
double cursor_x = 0.0;
double cursor_y = 0.0;
double mSensitivity = 4.0;
double accel = 1;
bool freecam = false;
bool showGrid = false;
bool showConst = false;
bool newDotPress;
bool newCommaPress;
bool newFPress;

bool load_content() { 

	// Create new shadow map
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	// Set light projection matrix
	LightProjectionMat = infinitePerspective<float>(90.f, renderer::get_screen_aspect(), 0.1f);

	// - Create the earth -
	meshes["earth"] = mesh(geometry_builder::create_sphere(50, 50));
	meshes["earth"].get_transform().scale = vec3(1275.0f);
	//rotate
	quat erot; erot.x = 0.7; erot.y = 0.0f; erot.z = 0.0f; erot.w = -0.7f;
	meshes["earth"].get_transform().orientation = erot;

	// - Create the lights -
	meshes["earth_lights"] = mesh(geometry_builder::create_sphere(50, 50));
	meshes["earth_lights"].get_transform().scale = vec3(1276.0f);
	//rotate
	meshes["earth_lights"].get_transform().orientation = erot;

	// - Create the clouds -   
	meshes["earth_cloud"] = mesh(geometry_builder::create_sphere(50, 50));
	meshes["earth_cloud"].get_transform().scale = vec3(1290.0f);
	//rotate to match earth
	meshes["earth_cloud"].get_transform().orientation = erot;

	// - Create the moon - 
	meshes["moon"] = mesh(geometry_builder::create_sphere(50, 50));
	meshes["moon"].get_transform().scale = vec3(350.0f);
	meshes["moon"].get_transform().position = vec3(0.0f, 0.0f, 10000.0f); //Realisticly 38440.0f

	// - Create the Space Staion -
    // Load in parts
	//wheel 
	meshes["wheel"] = mesh(geometry("res/models/orbital/wheel.obj"));   
	meshes["wheel"].get_transform().scale = vec3(0.1f);
	//pod
	meshes["pod"] = mesh(geometry("res/models/orbital/pod.obj"));
	//window
	meshes["window"] = mesh(geometry("res/models/orbital/window.obj"));
	//spokes
	meshes["spoke1"] = mesh(geometry("res/models/orbital/spoke.obj"));
	meshes["spoke2"] = mesh(geometry("res/models/orbital/spoke.obj"));
	meshes["spoke2"].get_transform().rotate(quat(-cos(0), vec3(0.0f, 0.0f, 1.0f)));
	meshes["spoke3"] = mesh(geometry("res/models/orbital/spoke.obj"));
	meshes["spoke3"].get_transform().rotate(quat((0), vec3(0.0f, 0.0f, 1.0f)));
	meshes["spoke4"] = mesh(geometry("res/models/orbital/spoke.obj"));
	meshes["spoke4"].get_transform().rotate(quat(cos(0), vec3(0.0f, 0.0f, 1.0f)));

	// - Create the stars (sky_sphere) - 
	skybox = mesh(geometry_builder::create_sphere(50, 50));
	skybox.get_transform().scale = vec3(450000.0f, 450000.0f, 450000.0f);
	skybox.get_transform().rotate(quat(cos(0), vec3(1.0f, 0.0f, 0.0f)));

	// - Create Star Gird -
	stargrid = mesh(geometry_builder::create_sphere(50, 50));
	stargrid.get_transform().scale = vec3(449999.99f, 449999.99f, 449999.99f);
	stargrid.get_transform().rotate(quat(cos(0), vec3(1.0f, 0.0f, 0.0f)));

	// - Create Constalations -
	constellations = mesh(geometry_builder::create_sphere(50, 50));
	constellations.get_transform().scale = vec3(449999.5f, 449999.5f, 449999.5f);
	constellations.get_transform().rotate(quat(cos(0), vec3(1.0f, 0.0f, 0.0f)));
	
	material mat;
	material glassMat;
	material hazeMat;
	material gridMat;
	material lightMat;

	//Set material
	mat.set_diffuse(vec4(1.0f));
	mat.set_emissive(vec4(vec3(0.0f), 1.0f));
	mat.set_shininess(50.0f);
	mat.set_specular(vec4(1.0f)); 

	// #### TODO: EXPLORE POSSIBILITIES OF USING MATIERAL FOR SETTING ALPHA (RATHER THAN SHADER). ####
	gridMat.set_diffuse(vec4(1.0f));

	lightMat.set_diffuse(vec4(1.0f));
	lightMat.set_emissive(vec4(vec3(0.0f), 1.0f)); 
	lightMat.set_shininess(0.0f); 
	lightMat.set_specular(vec4(0.0f));   

	//Earth Haze
	hazeMat.set_diffuse(vec4(0.7f, 0.7f, 0.9f, 1.0f));
	hazeMat.set_emissive(vec4(vec3(0.0f), 1.0f));
	hazeMat.set_shininess(500.0f);
	hazeMat.set_specular(vec4(1.0f));

	//Station Glass
	glassMat.set_diffuse(vec4(0.74f, 0.776f, 0.818f, 0.2f));
	glassMat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 0.2f));
	glassMat.set_shininess(25.0f);
	glassMat.set_specular(vec4(1.0f));


	//Assign Material 
	//#### IF ALL MATERIAL SAME, ONLY ASSIGN VARIABLE ONCE, IF MOST BUT NOT ALL SAME, BUCKET 'MOST'
	//#### (SET MAT -> RENDER 'MOST' -> SET INDIVIDUAL MATERIALS USING partRenderObject())
	meshes["moon"].set_material(mat);
	meshes["earth"].set_material(mat);
	meshes["earth_lights"].set_material(lightMat);
	meshes["earth_cloud"].set_material(hazeMat);
	meshes["window"].set_material(glassMat);
	   
	// Load Moon texture
	moon_tex = texture("res/textures/2k_moon.jpg");
	// Load Earrh texture
	earth_tex = texture("res/textures/earth.jpg");
	// Load Citylight texture
	light_tex = texture("res/textures/earth_lights.gif");
	// Load Cloud texture
	cloud_tex = texture("res/textures/earth_clouds.jpg");
	// Load Star texture
	star_tex = texture("res/textures/starmap_4k.jpg");
	// Load StarGrid texture
	grid_tex = texture("res/textures/stargrid.jpg");
	// Load constellation texture
	constellation_tex = texture("res/textures/constellation_chart.jpg");
	// Load cockpit texture
	cockpit_tex = texture("res/textures/cockpit.png");
	
	//Set light
	// ambient intensity
	light.set_ambient_intensity(vec4(vec3(0.01f), 1.0f)); 
	// Light colour white
	light.set_light_colour(vec4(vec3(1.0f), 1.0f));
	// Light direction
	light.set_direction(vec3(0.0f, 0.0f, 1.0f)); 

	//Shadow for directional light (doesn't move)
	shadow.light_dir = vec3(0.0f, 0.0f, -1.0f);         
	shadow.light_position = vec3(0.0f, 0.0f, 7000.0f);   

	//TODO: REMOVE SINGLE CITY LIGHT 
	// Set properties of City light   
	point.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	point.set_range(100.0f);  
	point.set_position(vec3(0.0f, 0.0, -1257.0f));   

	// Load in city light shaders
	light_eff.add_shader("res/shaders/phong.vert", GL_VERTEX_SHADER);
	vector<string> cityLightFrags{
		"res/shaders/city_lights.frag", "res/shaders/phong.frag" };
	light_eff.add_shader(cityLightFrags, GL_FRAGMENT_SHADER);
	light_eff.build();
	 
	// Load in cloud shaders 
	cloud_eff.add_shader("res/shaders/phong.vert", GL_VERTEX_SHADER);
	vector<string> cloudFrags {
		 "res/shaders/cloud.frag", "res/shaders/phong.frag" };
	cloud_eff.add_shader(cloudFrags, GL_FRAGMENT_SHADER);   
	cloud_eff.build();

	// Load in main shaders 
	eff.add_shader("res/shaders/phong.vert", GL_VERTEX_SHADER);
	vector<string> planetFrags{
		"res/shaders/planet.frag" ,"res/shaders/phong.frag", "res/shaders/shadow.frag" };
	eff.add_shader(planetFrags, GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();   

	// Load in skybox effect  
	space_eff.add_shader("res/shaders/skybox.vert", GL_VERTEX_SHADER); 
	space_eff.add_shader("res/shaders/skybox.frag", GL_FRAGMENT_SHADER);
	// Build effect
	space_eff.build();

	// Load in Skybox Overlay effect
	skybox_overlay_eff.add_shader("res/shaders/skybox.vert", GL_VERTEX_SHADER);
	skybox_overlay_eff.add_shader("res/shaders/trans_skybox.frag", GL_FRAGMENT_SHADER);   
	// Build effect
	skybox_overlay_eff.build();

	// Load in Shadow effect
	shadow_eff.add_shader("res/shaders/plainPhong.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("res/shaders/plainPhong.frag", GL_FRAGMENT_SHADER); 
	// Build effect
	shadow_eff.build();

	// Set camera properties
	cam.set_position(vec3(0.0f, 0.0f, 2000.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 6000000.0f);

	// Set free camera properties
	free_cam.set_position(vec3(0.0f, 0.0f, 2000.0f));
	free_cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	free_cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 6000000.0f);

	return true;
}

void updateFreeCam(float delta_time) {
	// Ratio of pixels to rotation
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height =
		(quarter_pi<float>() *
		(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
		static_cast<float>(renderer::get_screen_height());

	double current_x;
	double current_y;
	double delta_x;
	double delta_y;

	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// Calculate delta of cursor positions from last frame
	delta_x = cursor_x - current_x;
	delta_y = cursor_y - current_y;

	// Multiply deltas by ratios - gets actual change in orientation
	delta_x = delta_x * ratio_width;
	delta_y = delta_y * ratio_height;

	// Rotate cameras by delta 
	// delta_y - x-axis rotation
	// delta_x - y-axis rotation
	free_cam.rotate(-delta_x * mSensitivity, delta_y * mSensitivity);

	// Update the camera
	free_cam.update(delta_time);

	// Update cursor pos
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
}

bool update(float delta_time) {
	// - Player input -
	// Toggle freecam
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_F)) {
		if (newFPress) {
			newFPress = false;
			if (!freecam) {
				// - Activate free_cam -
				// Set input mode, hide cursor
				glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				// Capture mouse position
				glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
				// Set active cam
				active_cam = &free_cam;
				freecam = true;
			}
			else {
				active_cam = &cam;
				freecam = false;
			}
		}
	}
	else {
		newFPress = true;
	}
	// Move and update cam
	if (freecam) {
		float deltaV = accel * delta_time;
		//thrust forward
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			velocity += deltaV * free_cam.get_forward();
		}
		//thrust backward
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			velocity += -deltaV * free_cam.get_forward();
		}
		//thrust left
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			velocity += -((cross(normalize(free_cam.get_forward()), normalize(free_cam.get_up())) * delta_time));
		}
		//thrust right
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			velocity += ((cross(normalize(free_cam.get_forward()), normalize(free_cam.get_up())) * delta_time));
		}
		//thrust up
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT)) {
			velocity += deltaV * free_cam.get_up();
		}
		//thrust down
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL)) {
			velocity += -deltaV * free_cam.get_up();
		}
		//rotate left
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q)) {
			angularV += (angularA * delta_time);
		}
		// Roll # in progress #
		//free_cam.roll(angularV);
		free_cam.set_position(free_cam.get_position() + velocity);
		updateFreeCam(delta_time);
	}
	// Toggle StarGrid
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_COMMA)) {
		if (newCommaPress) {
			showGrid = !showGrid;
			newCommaPress = false;
		}
	}
	else {
		newCommaPress = true;
	}
	// Toggle Constalations
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_PERIOD)) {
		if (newDotPress) {
			showConst = !showConst;
			newDotPress = false;
		}
	}
	else {
		newDotPress = true;
	}

	// Update target cam even if not in use
	theta += pi<float>() * delta_time;
	cam.set_position(rotate(vec3(2000.0f, 5.0f, 2000.0f), theta * 0.05f, vec3(0, 1.0f, 0)));
	cam.update(delta_time);

	// Update Orbital position
	meshes["wheel"].get_transform().position = cam.get_position() - (400.0f * vec3(normalize(cam.get_position() - cam.get_target())));
	meshes["wheel"].get_transform().rotate(vec3(0.0f, normalize(cam.get_position() - cam.get_target()).y, 0.0f));

	return true;
}

//Render Object with Hierarchy
void renderObject(effect eff, mesh m, mesh parent, texture tex, camera* activeCam) {
	// Bind effect
	renderer::bind(eff);

	// Create MVP matrix
	auto M = parent.get_transform().get_transform_matrix() * m.get_transform().get_transform_matrix();
	auto V = activeCam->get_view();
	auto P = activeCam->get_projection();
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
	renderer::bind(point, "point");
	// Bind texture
	renderer::bind(tex, 0);

	// Set eye position uniform
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(activeCam->get_position()));

	renderer::render(m);

}

//Render Object
void renderObject(effect eff, mesh m, texture tex, camera* activeCam) {
	// Bind effect
	renderer::bind(eff);

	// Create MVP matrix
	auto M = m.get_transform().get_transform_matrix();
	auto V = activeCam->get_view();
	auto P = activeCam->get_projection();
	auto MVP = P * V * M;

	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Set M matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	// Set N matrix uniform
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
	  
	// Set lightMVP uniform, using:
	//Model matrix from m
	// viewmatrix from the shadow map
	auto viewM = shadow.get_view();
	// Multiply together with LightProjectionMat
	auto lightMVP = LightProjectionMat * viewM * M;
	// Set uniform
	glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(lightMVP));

	// Bind material
	renderer::bind(m.get_material(), "mat");
	// Bind light
	renderer::bind(light, "light");
	// Bind texture
	renderer::bind(tex, 0);

	// Set eye position uniform     
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(activeCam->get_position()));

	// Bind shadow map texture
	renderer::bind(shadow.buffer->get_depth(), 2);
	// Set the shadow_map uniform
	glUniform1i(eff.get_uniform_location("shadow_map"), 2);

	renderer::render(m);

}

bool render() { 
	/*
	## Currently causing everything to 'shadow' so removed for now. Code left in for review if desired. ##
	//-------------------------------------------------------	
	// - Render shadow_map -
	// Set target
	renderer::set_render_target(shadow);
	// Clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	// Change culling to front face
	glCullFace(GL_FRONT);

	// Bind shadow effect
	renderer::bind(shadow_eff);
	
	// Set mesh
	auto mm = meshes["moon"];
	// Create MVP matrix
	auto Mm = mm.get_transform().get_transform_matrix();
	// Take view matrix from shadow map
	auto Vm = shadow.get_view();
	auto MVPm = LightProjectionMat * Vm * Mm;
	// Set MVP matrix uniform
	glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVPm));
	// Render mesh
	renderer::render(mm);

	// Set render target back to the screen
	renderer::set_render_target();
	// Set face cull mode to back
	glCullFace(GL_BACK);
	*/

	//-----------------------------------------------------

	//-Render Stars-
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);
	//part render
	renderObject(space_eff, skybox, star_tex, active_cam);
	// set tex unifrom
	glUniform1i(space_eff.get_uniform_location("tex"), 0);
	// render mesh
	renderer::render(skybox);

	//-Render StarGird-
	if (showGrid) {
		renderObject(skybox_overlay_eff, stargrid, grid_tex, active_cam);
		// set tex unifrom
		glUniform1i(skybox_overlay_eff.get_uniform_location("tex"), 0);
		// render mesh
		renderer::render(stargrid);
	}

	// -Render Constellation-
	if (showConst) {
		renderObject(skybox_overlay_eff, constellations, constellation_tex, active_cam);
		// set tex unifrom
		glUniform1i(skybox_overlay_eff.get_uniform_location("tex"), 0);
		// render mesh
		renderer::render(constellations);
	}

	// Re-enable faceculling, & depth
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	// - Render Objects -

	// -Render Moon-
	renderObject(eff, meshes["moon"], moon_tex, active_cam);
	// set tex uniform
	glUniform1i(eff.get_uniform_location("tex"), 0);
	// render mesh
	renderer::render(meshes["moon"]);

	// -Render Earth-
	renderObject(eff, meshes["earth"], earth_tex, active_cam);
	// set tex uniform
	glUniform1i(eff.get_uniform_location("tex"), 0);
	// render mesh
	renderer::render(meshes["earth"]);

	// -Render Citylight-
	renderObject(light_eff, meshes["earth_lights"], light_tex, active_cam);
	// set texture uniform
	glUniform1i(light_eff.get_uniform_location("tex"), 0); 
	// render mesh
	renderer::render(meshes["earth_lights"]);

	// -Render Earth clouds-
	glDisable(GL_CULL_FACE);
	renderObject(cloud_eff, meshes["earth_cloud"], cloud_tex, active_cam);
	// set tex uniform  
	glUniform1i(cloud_eff.get_uniform_location("tex"), 0);
	// render mesh
	renderer::render(meshes["earth_cloud"]);
	glEnable(GL_CULL_FACE);

	// -Render Obital-
	//-Wheel-
	renderObject(eff, meshes["wheel"], moon_tex, active_cam);
	// set tex uniform
	glUniform1i(eff.get_uniform_location("tex"), 0); 
	// render mesh
	renderer::render(meshes["wheel"]);
	//-Pod-
	renderObject(eff, meshes["pod"], meshes["wheel"], moon_tex, active_cam);
	//-Window-
	renderObject(eff, meshes["window"], meshes["wheel"], moon_tex, active_cam);
	//-Spoke 1-
	renderObject(eff, meshes["spoke1"], meshes["wheel"], moon_tex, active_cam);
	//-Spoke 2-
	renderObject(eff, meshes["spoke2"], meshes["wheel"], moon_tex, active_cam);
	//-Spoke 3-
	renderObject(eff, meshes["spoke3"], meshes["wheel"], moon_tex, active_cam);
	//-Spoke 4-
	renderObject(eff, meshes["spoke4"], meshes["wheel"], moon_tex, active_cam);

	return true;
}

void main() {
	// Create application
	app application("Spying Earth");
	// Set methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}