//--------------------------------
//Author: Toran Bruce Richards
// Spying Earth, Part I
// 15/03/2018
// Controls:
// F - Toggle 'Shuttle Cam'
// WASD, RSHIFT, RCTRL- Move 'Shuttle Cam' with sustained velocity
// . - Toggle Constellations
// , - Toggle Stellar Grid
//-------------------------------

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <graphics_framework.h>
#include "planet.h"
#include "atmosphere.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Planets, Stars and Moons
vector<planet> planets; //Planets only.
planet sun;
planet mercury;
planet venus;
planet earth;
planet moon;
planet mars;
planet jupiter;
planet saturn;
planet uranus;
planet neptune;
planet pluto;

// Orbits
vector<mesh> orbits;

//Effects
effect eff;
effect sun_eff;
effect sun_halo_eff; 
effect orbit_eff;
effect Citylight_eff; 
effect earth_cloud_eff;
effect space_eff;
effect skybox_overlay_eff;
effect shadow_eff;
effect flare_eff;

shadow_map shadow;

//Cameras
target_camera target_cam;
free_camera free_cam;
camera* active_cam = &target_cam;

//Lights
point_light light_sun;

//textures
texture moon_tex;
texture earth_tex;
texture Citylight_tex;
texture earth_cloud_tex;
texture mercury_tex;
texture venus_tex;
texture venus_atmos_tex;
texture mars_tex;
texture jupiter_tex;
texture saturn_tex;
texture uranus_tex;
texture neptune_tex;
texture pluto_tex;
texture sun_tex;
texture sun_halo_tex;
texture star_tex;
texture grid_tex;
texture constellation_tex;
texture cockpit_tex;
texture white_tex;

//Meshes
mesh skybox;
mesh stargrid;
mesh constellations;
map<string, mesh> meshes;
mesh haloMesh;

//Global Variables
vec3 velocity = vec3(0.0f);
mat4 LightProjectionMat;
float angularV = 0.0f;
float angularA = 0.1f;
float theta;
float timeM = 1;
float curTime;
float zoomSpeed = 50;
float target_cam_offset = 1000.0f;
float earthRad =  637.8;
double cursor_x = 0.0;
double cursor_y = 0.0;
double mSensitivity = 4.0;
double accel = 1;
bool freecam = false;
bool showGrid = false;
bool showConst = false;
bool showOrbit = false;
bool newDotPress;
bool newUpPress;
bool newDownPress;
bool newCommaPress;
bool newFPress;
bool newSlashPress;
int targetedPlanet = 0;


void loadTextures();
void loadShadersAndEffects();

bool load_content() { 

	// Begin time
	curTime = 0.0f;
	
	// Load Textures
	loadTextures();

	// Load Shaders
	loadShadersAndEffects();
	
	//---------TODO: TIDY UP------------------

	//Assign Material 
	//#### IF ALL MATERIAL SAME, ONLY ASSIGN VARIABLE ONCE, IF MOST BUT NOT ALL SAME, BUCKET 'MOST'
	//#### (SET MAT -> RENDER 'MOST' -> SET INDIVIDUAL MATERIALS USING partRenderObject())
	material mat;
	material glassMat;
	material earth_cloud_mat;
	material gridMat;
	material citylight_mat;

	//Set material
	mat.set_diffuse(vec4(1.0f));
	mat.set_emissive(vec4(vec3(0.0f), 1.0f));
	mat.set_shininess(200.0f);
	mat.set_specular(vec4(1.0f));

	// #### TODO: EXPLORE POSSIBILITIES OF USING MATIERAL FOR lowering ALPHA (RATHER THAN SHADER). ####
	gridMat.set_diffuse(vec4(1.0f));

	citylight_mat.set_diffuse(vec4(1.0f));
	citylight_mat.set_emissive(vec4(vec3(0.0f), 1.0f));
	citylight_mat.set_shininess(0.0f);
	citylight_mat.set_specular(vec4(0.0f));

	//Earth Haze
	earth_cloud_mat.set_diffuse(vec4(0.7f, 0.7f, 0.9f, 1.0f));
	earth_cloud_mat.set_emissive(vec4(vec3(0.0f), 1.0f));
	earth_cloud_mat.set_shininess(500.0f);
	earth_cloud_mat.set_specular(vec4(1.0f));

	//Station Glass
	glassMat.set_diffuse(vec4(0.74f, 0.776f, 0.818f, 0.2f));
	glassMat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 0.2f));
	glassMat.set_shininess(25.0f);
	glassMat.set_specular(vec4(1.0f));

	//-----------------------------

	// Create new shadow map
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	// Set light projection matrix
	LightProjectionMat = infinitePerspective<float>(90.f, renderer::get_screen_aspect(), 0.1f);

	// - Create the Cockpit - 
	meshes["cockpit"] = mesh(geometry_builder::create_plane());

	// - Create the earth -
	earth.createPlanet(1, 3, 1, 1, mat, earth_tex);
	// Set 0 time position
	earth.calculatePos(0);
	


	// - Create the lights -
	earth.createAtmostphere(2.0f, Citylight_tex, Citylight_eff, citylight_mat);		
	// - Create the clouds -   
	earth.createAtmostphere(15, earth_cloud_tex, earth_cloud_eff, earth_cloud_mat);

	// - Create the Sun -
	sun.createPlanet(109, 0, 0, 24.47, mat, sun_tex);

	// - Create the halo -
	vector<vec3> positions;
	positions.push_back(vec3(0.0f));
	geometry haloGeom;
	haloGeom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);	
	// Set geometry type to points
	haloGeom.set_type(GL_POINTS);
	haloMesh = mesh(haloGeom);

	// - Create Mercury - 
	mercury.createPlanet(0.383, 1, 0.241, 58.8, mat, mercury_tex);

	// - Create Venus -
	venus.createPlanet(0.949, 2, 0.615, -244, mat, venus_tex);
	// create atmosphere
	venus.createAtmostphere(15, venus_atmos_tex, earth_cloud_eff, mat);

	// - Create Mars - 
	mars.createPlanet(0.532, 4, 1.88, 1.03, mat, mars_tex);

	// - Create Jupiter -
	jupiter.createPlanet(11.21, 5, 11.9, 0.415, mat, jupiter_tex);
	
	// - Create Saturn -
	saturn.createPlanet(9.45, 6, 29.4, 0.445, mat, saturn_tex);
	//TODO: CREATE SATURN RING

	// - Create Uranus -
	uranus.createPlanet(4.01, 7, 83.7, -0.720, mat, uranus_tex);
	//TODO: CREATE URANUS RING

	// - Create Neptune -
	neptune.createPlanet(3.88, 8, 163.7, 0.673, mat, neptune_tex);

	// Create Pluto -
	pluto.createPlanet(0.186, 9, 247.9, 6.41, mat, pluto_tex);

	// - Create the moon - 
	moon.createPlanet(0.2724, 0.2654, 0.0748, 27.4, mat, moon_tex);

	// Add Planets, Stars and Moons to Collection
	planets = { mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto };

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
	skybox.get_transform().scale = vec3(450000.0f);
	skybox.get_transform().rotate(quat(cos(0), vec3(1.0f, 0.0f, 0.0f)));

	// - Create Star Gird -
	stargrid = mesh(geometry_builder::create_sphere(50, 50));
	stargrid.get_transform().scale = vec3(450000.0f);
	stargrid.get_transform().rotate(quat(cos(0), vec3(1.0f, 0.0f, 0.0f)));

	// - Create Constalations -
	constellations = mesh(geometry_builder::create_sphere(50, 50));
	constellations.get_transform().scale = vec3(450000.0f);
	constellations.get_transform().rotate(quat(cos(0), vec3(1.0f, 0.0f, 0.0f)));

	//Set light
	// Light colour white
	light_sun.set_light_colour(vec4(vec3(1.0f), 1.0f));
	light_sun.set_range(400000.0f);
	light_sun.set_constant_attenuation(0);

	//Shadow for directional light (doesn't move)
	shadow.light_dir = vec3(0.0f, 0.0f, -1.0f);         
	shadow.light_position = vec3(0.0f, 0.0f, 7000.0f);  

	// Set camera properties
	target_cam.set_position(vec3(0.0f, 0.0f, 2000.0f));
	target_cam.set_target(planets[targetedPlanet].curPos);
	target_cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 10.0f, 6100000.0f);

	// Set free camera properties
	free_cam.set_position(vec3(0.0f, 0.0f, 2000.0f));
	free_cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	free_cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 10.0f, 6100000.0f);

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
				active_cam = &target_cam;
				freecam = false;
			}
		}
	}
	else {
		newFPress = true;
	}
	// Freecam Controls
	if (freecam) {
		float deltaV = accel * delta_time;
		//gear 1
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_1)) {
			accel = 1;
		}
		//gear 2
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)) {
			accel = 10;
		}
		//gear 3
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_3)) {
			accel = 100;
		}
		//gear 4
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_4)) {
			accel = 1000;
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_F1)) {
			free_cam.set_position(planets[2].curPos);
		}
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
			velocity += deltaV * -((cross(normalize(free_cam.get_forward()), normalize(free_cam.get_up()))));
		}
		//thrust right
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			velocity += deltaV * ((cross(normalize(free_cam.get_forward()), normalize(free_cam.get_up()))));
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
	// Target_cam Controls
	else {
		// Cycle Planets
		//TODO: SET OFFSET SCALE WITH PLANET SIZE offset * size
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
			if (newUpPress) {
				//Target next planet
				if (targetedPlanet < 8)
					targetedPlanet++;
				else
					targetedPlanet = 0;
				//Scale offset to planet size
				if (!targetedPlanet == 0)
					target_cam_offset *= (planets[targetedPlanet].size / planets[targetedPlanet -1].size);
				else
					target_cam_offset *= (planets[targetedPlanet].size / planets[8].size);
				newUpPress = false;
			}
		}
		else {
			newUpPress = true;
		}
		// Move closer
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			// Do not allow zooming to point of near clippling plane with planet surface (Atmosphere is allowed)
			if (target_cam_offset > planets[targetedPlanet].size * earthRad + 10.5)
				target_cam_offset -= delta_time * zoomSpeed;
		}
		// Move Further
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			target_cam_offset += delta_time * zoomSpeed;
		}
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
	// Toggle Orbit Paths
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SLASH)) {
		if (newSlashPress) {
			//toggle
			showOrbit = !showOrbit;
			newSlashPress = false;
		}
	}
	else {
		newSlashPress = true;
	}
	// Change Clock speed
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_EQUAL)) {
		timeM = 500;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_MINUS)) {
		timeM = 1;
	}


	// Update Orbital position
	//meshes["wheel"].get_transform().position = cam.get_position() - (400.0f * vec3(normalize(cam.get_position() - cam.get_target())));
	//meshes["wheel"].get_transform().rotate(vec3(0.0f, normalize(cam.get_position() - cam.get_target()).y, 0.0f));


	// - Update Positions and rotations of Celestial Objects -
	// update time
	curTime += delta_time * timeM;
	int i = 0;
	for each (planet p in planets)
	{
		p.calculatePos(curTime);
		p.calculateRotation(curTime);
		p.updateAtmostpheres();
		planets[i] = p;
		i++;
	}

	// Sun
	sun.calculateRotation(curTime);
	// Moon
	moon.calculatePos(curTime, planets[2]);
	moon.calculateRotation(curTime);

	// Update target cam (This needs to happen after planet positions are updated)
	target_cam.set_target(planets[targetedPlanet].curPos);
	// Get the normal vector from planet to cam
	vec3 offsetDircet = normalize(target_cam.get_position() - planets[targetedPlanet].curPos);
	target_cam.set_position(planets[targetedPlanet].curPos + offsetDircet * target_cam_offset);
	target_cam.update(delta_time);

	return true;
}

// Load Texures
void loadTextures() {
	// Load Sun texture
	sun_tex = texture("res/textures/2k_sun.jpg");
	// Load sun halo texture
	sun_halo_tex = texture("res/textures/sun_halo2.png");
	// Load Moon texture
	moon_tex = texture("res/textures/2k_moon.jpg");
	// Load Earth texture
	earth_tex = texture("res/textures/earth.jpg");
	// Load Citylight texture
	Citylight_tex = texture("res/textures/earth_lights.gif");
	// Load Earth Cloud texture
	earth_cloud_tex = texture("res/textures/earth_clouds.jpg");
	// Load Mercury Texture
	mercury_tex = texture("res/textures/8k_mercury.jpg");
	// Load Venus Texture
	venus_tex = texture("res/textures/8k_venus_surface.jpg");
	// Load Venus Atmosphere Texture
	venus_atmos_tex = texture("res/textures/8k_venus_atmosphere.jpg");
	// Load Mars Texture
	mars_tex = texture("res/textures/8k_mars.jpg");
	// Load Jupiter Texture
	jupiter_tex = texture("res/textures/4k_jupiter.jpg");
	// Load Saturn Texture
	saturn_tex = texture("res/textures/8k_saturn.jpg");
	// Load Uranus Texture
	uranus_tex = texture("res/textures/2k_uranus.jpg");
	// Load Neptune Texture
	neptune_tex = texture("res/textures/2k_neptune.jpg");
	// Load Pluto Texture
	pluto_tex = texture("res/textures/2k_pluto.jpg");
	// Load Star texture
	star_tex = texture("res/textures/starmap_4k.jpg");
	// Load StarGrid texture
	grid_tex = texture("res/textures/stargrid.jpg");
	// Load constellation texture
	constellation_tex = texture("res/textures/constellation_chart.jpg");
	// Load cockpit texture
	cockpit_tex = texture("res/textures/cockpit.png");
	// Load white tex
	white_tex = texture("res/textures/white_texel.png");
}

// Load Shaders, Build Effects
void loadShadersAndEffects() {
	// Load in sun shaders
	sun_eff.add_shader("res/shaders/skybox.vert", GL_VERTEX_SHADER);
	sun_eff.add_shader("res/shaders/skybox.frag", GL_FRAGMENT_SHADER);
	sun_eff.build();

	// Load in flare shaders  TODO: RESEARCH LENSEFLARES THROUGH SHADERTOY TO BETTER UNDERSTAND HOW THEY WORK
	flare_eff.add_shader("res/shaders/flare.vert", GL_VERTEX_SHADER);
	flare_eff.add_shader("res/shaders/flare.frag", GL_FRAGMENT_SHADER);
	flare_eff.build();

	// Load in sun halo shader
	sun_halo_eff.add_shader("res/shaders/shader.vert", GL_VERTEX_SHADER);
	sun_halo_eff.add_shader("res/shaders/billboard.geom", GL_GEOMETRY_SHADER);
	sun_halo_eff.add_shader("res/shaders/shader.frag", GL_FRAGMENT_SHADER);
	sun_halo_eff.build();

	// Load in orbit shader
	orbit_eff.add_shader("res/shaders/shader.vert", GL_VERTEX_SHADER);
	orbit_eff.add_shader("res/shaders/orbit.frag", GL_FRAGMENT_SHADER);
	orbit_eff.build();

	// Load in city light shaders
	Citylight_eff.add_shader("res/shaders/phong.vert", GL_VERTEX_SHADER);
	vector<string> cityLightFrags{
		"res/shaders/city_lights.frag", "res/shaders/phong.frag" };
	Citylight_eff.add_shader(cityLightFrags, GL_FRAGMENT_SHADER);
	Citylight_eff.build();

	// Load in cloud shaders 
	earth_cloud_eff.add_shader("res/shaders/phong.vert", GL_VERTEX_SHADER);
	vector<string> cloudFrags{
		"res/shaders/cloud.frag", "res/shaders/phong.frag" };
	earth_cloud_eff.add_shader(cloudFrags, GL_FRAGMENT_SHADER);
	earth_cloud_eff.build();

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
}

// Create orbit path
void createOrbitPath(geometry geom, int quality) {
	// Set geometry type to line loop
	geom.set_type(GL_LINE_LOOP);
	
	// Positions
	vector<vec3> positions;

	for (int i = 0; i<quality; ++i)
		positions.push_back(vec3(cos(2 * i*pi<float>() / quality), 0.0f, sin(2 * i*pi<float>() / quality)));

	// Add to the geometry
	geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
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
	renderer::bind(light_sun, "light");
	// Bind texture
	renderer::bind(tex, 0);

	// Set eye position uniform
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(activeCam->get_position()));
	// Set texture uniform
	glUniform1i(eff.get_uniform_location("tex"), 0);

	//Render Mesh
	renderer::render(m);
}

// Render halo object
void renderObject(effect eff, mesh geo, texture tex, camera * activeCam, int i) {

	renderer::bind(eff);
	// Create MVP matrix
	auto V = activeCam->get_view();
	auto P = activeCam->get_projection();
	auto MVP = P * V;

	glUniformMatrix4fv(
		eff.get_uniform_location("MV"),
		1,
		GL_FALSE,
		value_ptr(V));
	glUniformMatrix4fv(
		eff.get_uniform_location("P"),
		1, 
		GL_FALSE,
		value_ptr(P));
	glUniform1f(eff.get_uniform_location("point_size"), 30000.0f);   
	renderer::bind(tex, 0);
	// Set eye position uniform     
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(activeCam->get_position()));
	glUniform1i(eff.get_uniform_location("tex"), 0);
	//Render Geometry
	renderer::render(geo);
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
	renderer::bind(light_sun, "light");
	// Bind texture
	renderer::bind(tex, 0);

	// Set eye position uniform     
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(activeCam->get_position()));

	// Bind shadow map texture
	renderer::bind(shadow.buffer->get_depth(), 2);
	// Set the shadow_map uniform
	glUniform1i(eff.get_uniform_location("shadow_map"), 2);
	// set texture uniform
	glUniform1i(eff.get_uniform_location("tex"), 0);

	//Render Mesh
	renderer::render(m);
}

//Render Object without texture
void renderObject(effect eff, geometry geom, camera* activeCam) {
	// Bind effect
	renderer::bind(eff);

	// Create MVP matrix
	mat4 M(1.0f);
	auto V = activeCam->get_view();
	auto P = activeCam->get_projection();
	auto MVP = P * V * M;

	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	//Render Mesh
	renderer::render(geom);
}

// Render line showing orbit given the orbital radius
void renderOrbit(float orbR) {
	geometry orbit;
	// Set geometry type to line loop
	orbit.set_type(GL_LINE_LOOP);

	// Positions
	vector<vec3> positions;

	for (int i = 0; i < 100; ++i)
		positions.push_back(orbR * vec3(cos(2 * i*pi<float>() / 100), 0.0f, sin(2 * i*pi<float>() / 100)));

	// Add to the geometry
	orbit.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	mesh orb = mesh(orbit);
	renderObject(space_eff, orb, white_tex, active_cam);
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

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);
	//render
	renderObject(space_eff, skybox, star_tex, active_cam);


	//-Render StarGird-
	if (showGrid) {
		renderObject(skybox_overlay_eff, stargrid, grid_tex, active_cam);
	}

	// -Render Constellation-
	if (showConst) {
		renderObject(skybox_overlay_eff, constellations, constellation_tex, active_cam);
	}

	// Re-enable faceculling, & depth   
	glDepthMask(GL_TRUE);  
	glEnable(GL_DEPTH_TEST);  
	glCullFace(GL_BACK); 

	// - Render Objects -  
	 
	// -Render Sun- 
	renderObject(sun_eff, sun.mesh, sun_tex, active_cam);  

	// Render halo
	renderObject(sun_halo_eff, haloMesh, sun_halo_tex, active_cam, 1); 





	//Render planets
	for each (planet p in planets)
	{
		// Render planet
		renderObject(eff, p.mesh, p.tex, active_cam);
		// Render atmospheres
		glDisable(GL_CULL_FACE);
		for each (atmosphere atmos in p.atmospheres)
		{
			renderObject(atmos.eff, atmos.mesh, atmos.tex, active_cam);
		}
		glEnable(GL_CULL_FACE);
	}


	// -Render Moon-
	renderObject(eff, moon.mesh, moon_tex, active_cam);


	// -Render Obital-
	//-Wheel-
	renderObject(eff, meshes["wheel"], moon_tex, active_cam);
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

	// Render Orbits
	if (showOrbit) {
		for each (planet p in planets)
		{
			renderOrbit(p.actDist);
		}
	}

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
