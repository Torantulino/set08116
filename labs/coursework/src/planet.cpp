#include "planet.h"
#include <glm/glm.hpp>
#include <graphics_framework.h>
using namespace std;
using namespace graphics_framework;


	// Calculate X position given t and t0 in days
	float planet::CalculateXPos(float t) {
		float x;
		float actDist = distance * AU;
		float actOrbP = orbitalPeriod * eOrbP;

		x = actDist * cos(2 * glm::pi<float>() * t  / actOrbP); // x(t) = (a cos(2pi(t-t0)/T)
		return x;
	}

	// Calculate Y position given t and t0
	float planet::CalculateZPos(float t) {
		float z;
		float actDist = distance * AU;
		float actOrbP = orbitalPeriod * eOrbP;

		z = actDist * sin(2 * glm::pi<float>() * t / actOrbP); // z(t) = (a sin(2pi(t-t0)/T)
		return z;
	}

	// Create the planetary body
	void planet::createPlanet(double pSize, double pDistance, double orbP, double rotP) {

		// Set Properties
		size = pSize;
		distance = pDistance;
		orbitalPeriod = orbP;
		rotationalPeriod = rotP;

		// Create planetary body
		geometry geom = geometry_builder::create_sphere(50, 50);
		mesh = mesh::mesh(geom);
		mesh.get_transform().scale = glm::vec3(size * eSize);
		mesh.get_transform().position = glm::vec3(distance * AU, 0.0f, 0.0f);
	}

	bool planet::ordering(graphics_framework::mesh& a, graphics_framework::mesh& b)
	{
		return a.get_transform().scale.x < b.get_transform().scale.x;
	}

	// Create an atmostphere for this planetary body with a specified thickness and add it to a list.
	void planet::createAtmostphere(float thickness) {
		// Create the mesh
		graphics_framework::mesh atMesh;
		atMesh = mesh::mesh(geometry_builder::create_sphere(50, 50));
		// Scale to desired thickness above planetary body
		mesh.get_transform().scale = glm::vec3((size * eSize) + thickness);

		// Add to collection of atmospheres for this planetary body
		atmostpheres.push_back(atMesh);

		// Sort to ensure correct rendering order
		atmostpheres.sort(ordering); 
	}

	// Calculate overall position
	glm::vec3 planet::calculatePos(float t) {
		float x;
		float y = 0;
		float z;
		glm::vec3 pos;

		x = CalculateXPos(t);
		z = CalculateZPos(t);

		pos = glm::vec3(x, y, z);
		return pos;
	}

	// Calculate overall position
	glm::vec3 planet::calculatePos(float t, planet parent) {
		float x;
		float y = 0;
		float z;

		x = CalculateXPos(t);
		z = CalculateZPos(t);

		curPos = parent.curPos + glm::vec3(x, y, z);
		return curPos;
	}

	void planet::calculateRotation(float t) {
		float rot = (t / (rotationalPeriod*eRotP)) * (glm::two_pi<float>());
		mesh.get_transform().orientation = glm::vec3(0.0f, -rot, 0.0f);
	}

