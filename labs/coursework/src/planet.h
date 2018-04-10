#pragma once
#include <glm\glm.hpp>
#include <graphics_framework.h>>
#include <list>

class planet
{
	// Fields
public:
	void createPlanet(double size, double distance, double orbP, double rotP);
	void createAtmostphere(float thickness); 	// Create an atmostphere for this planetary body with a specified thickness and add it to a list
	void calculateRotation(float t);
	glm::vec3 calculatePos(float t);
	glm::vec3 calculatePos(float t, planet parent);


	double size; // radial size in earths
	double distance; // semi-major axis / radius (since orbits here are circular) in AU
	double orbitalPeriod;
	double rotationalPeriod;
	glm::vec3 curPos; // the current position of the planet
	graphics_framework::mesh mesh; //The associated mesh

			   // Properties
private:
	// Orbital Properties of earth for Quick System Scaling
	float eSize = 1276;
	float AU = 2000000;
	float eOrbP = 8764.8; //in hours
	float eRotP = 23.9; // hours
	float CalculateXPos(float t);
	float CalculateZPos(float t);
	std::list<graphics_framework::mesh> atmostpheres;
	bool ordering(graphics_framework::mesh& a, graphics_framework::mesh& b); //Ordering rules to sort atmospheres by thickness for rendering order.
};

