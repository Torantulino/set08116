#pragma once
#include <glm\glm.hpp>
#include <graphics_framework.h>>
#include <list>
#include "atmosphere.h"
class planet
{

public:
	// Methods
	void createPlanet(double size, double distance, double orbP, double rotP,
			graphics_framework::material mat, graphics_framework::texture tex);	// Create a new celestial body, specifiying orbital elements as a ratio to that of earths.
	void createAtmostphere(float thickness, graphics_framework::texture tex,
			graphics_framework::effect eff, graphics_framework::material mat);	// Create an atmostphere for this celestial body with a specified thickness and add it to a list.
	void createOrbit();															// Create a ring showing the path of the orbit of this celestial body
	void calculateRotation(float t);											// Calculate and update the rotation for this celestial body at the specified time in hours.
	void calculatePos(float t);													// Calculate and update the position of this planet at the specified time in hours.
	void calculatePos(float t, planet parent);									// Calculate and update the position of this moon at the specified time in hours.
	void updateAtmostpheres();													// Updates the position and rotation of the atmosphere's relating to this planet.

																				// Fields
	std::vector<atmosphere> atmospheres;											// The atmosphere's or other secondary meshes associated with this body. TODO: SPECIFY SORTING ORDER
	double size;																// The diametric size of the celestial body as a ratio to Earth.
	double distance;															// The position of the planet in the evenly spaced distance from sun, or proportional ditance to that from it's parent.
	double orbitalPeriod;														// The time taken to complete a full orbit of it's parent.
	double rotationalPeriod;													// The time taken to complete a full rotation.
	float actDist;																// The actual distance of the planet from the sun center.
	glm::vec3 curPos;															// The current position of the celestial body.
	glm::vec3 curOri;															// The current orientation of the celestial body.
	graphics_framework::mesh mesh;												// The associated mesh for the celestial body.
	graphics_framework::texture tex;


private:
	// Properties
	float eSize = 637.8;														// The chosen radial size of the Earth.
	float AU = 30000;															// The chosen distance between each planet.
	float eOrbP = 8764.8;														// The time taken for the Earth to complete an orbit around the Sun in hours.
	float eRotP = 23.9;															// The time taken for the Earth to complete a rotation in hours.
	float sunR = 70000;															// The radius of the sun.

																				// Methods
	float CalculateXPos(float t, bool isMoon);												// Calculate the X position of the celestial body in relation to it's parent at a given time in hours.
	float CalculateZPos(float t, bool isMoon);												// Calculate the Z position of the celestial body in relation to it's parent at a given time in hours.
};

