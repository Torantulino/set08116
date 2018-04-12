#include "planet.h"
#include "atmosphere.h"
#include <glm/glm.hpp>
#include <graphics_framework.h>
#include <algorithm>
using namespace std;
using namespace graphics_framework;


// Create a celestial body, specifiying orbital elements as a ratio to that of earths.
void planet::createPlanet(double pSize, double pDistance, double orbP, double rotP, graphics_framework::material mat, graphics_framework::texture texIn) {

	// Set Properties
	size = pSize;
	distance = pDistance;
	orbitalPeriod = orbP;
	rotationalPeriod = rotP;
	tex = texIn;

	// Create celestial body
	geometry geom = geometry_builder::create_sphere(50, 50);
	mesh = mesh::mesh(geom);
	mesh.get_transform().scale = glm::vec3(size * eSize);
	mesh.set_material(mat);
}


// Calculate the X position of the celestial body in relation to it's parent at a given time in hours.
float planet::CalculateXPos(float t, bool isMoon) {
	float x;
	actDist = distance * AU;
	// Add sun radius displacement if not a moon.
	if(!isMoon)
		actDist += sunR;
	float actOrbP = orbitalPeriod * eOrbP;

	x = actDist * cos(2 * glm::pi<float>() * t / actOrbP); // x(t) = (a cos(2pi(t-t0)/T)
	return x;
}

// Calculate the Y position of the celestial body in relation to it's parent at a given time in hours.
float planet::CalculateZPos(float t, bool isMoon) {
	float z;
	actDist = distance * AU;
	// Add sun radius displacement if not a moon.
	if (!isMoon)
		actDist += sunR;
	float actOrbP = orbitalPeriod * eOrbP;

	z = actDist * sin(2 * glm::pi<float>() * t / actOrbP); // z(t) = (a sin(2pi(t-t0)/T)
	return z;
}


// Calculate and update the overall position of this planet at the specified time in hours.
void planet::calculatePos(float t) {
	float x;
	float y = 0.0f;
	float z;

	x = CalculateXPos(t, false);
	z = CalculateZPos(t, false);

	curPos = glm::vec3(x, y, z);
	mesh.get_transform().position = curPos;
}

// Calculate and update the position of this moon at the specified time in hours.
void planet::calculatePos(float t, planet parent) {
	float x;
	float y = 0;
	float z;

	x = CalculateXPos(t, true);
	z = CalculateZPos(t, true);

	curPos = parent.curPos + glm::vec3(x, y, z);
	// Ensure Y is 0
	curPos.y = 0.0f;
	mesh.get_transform().position = curPos;
}

// Calculate and update the rotation for this celestial body at the specified time in hours.
void planet::calculateRotation(float t) {
	float rot = (t / (rotationalPeriod*eRotP)) * (glm::two_pi<float>());
	// Update current orientation property and rotate
	curOri = glm::vec3(-glm::half_pi<float>(), -rot, 0.0f);
	mesh.get_transform().orientation = curOri;
}


// Create an atmostphere for this celestial body with a specified thickness and add it to a list.
void planet::createAtmostphere(float thickness, graphics_framework::texture tex, graphics_framework::effect eff, graphics_framework::material mat) {
	// Create the atmosphere
	atmosphere atmos;
	// Create the mesh
	graphics_framework::mesh atMesh;
	// Create the geometry
	atMesh = mesh::mesh(geometry_builder::create_sphere(50, 50));
	// Scale to desired thickness above celestial body
	atmos.size = (size * eSize) + thickness;
	atMesh.get_transform().scale = glm::vec3(atmos.size);
	// Assign atmosphere properties
	atmos.mesh = atMesh;
	atmos.thickness = thickness;
	atmos.tex = tex;
	atmos.eff = eff;
	atmos.mesh.set_material(mat);

	// Add to collection of atmospheres for this celestial body
	atmospheres.push_back(atmos);

	// Sort to ensure correct rendering order
	sort(atmospheres.begin(), atmospheres.end(),[](const atmosphere &a, const atmosphere &b)
	{
		return a.size < b.size;
	});
}


void planet::updateAtmostpheres() {
	int i = 0;
	for each (atmosphere atmos in atmospheres)
	{
		atmos.mesh.get_transform().position = curPos;
		atmos.mesh.get_transform().orientation = curOri;
		atmospheres[i] = atmos;
		i++;
	}
}

