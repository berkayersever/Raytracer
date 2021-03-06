#include <iostream>

#include "raytracer.h"

#include "sphere.h"
#include "plane.h"

constexpr int width  = 640;
constexpr int height = 480;

int main (){
	std::cout << "rendering ..." << '\n';

	using raytracer::Color;

	// Camera eye
	using raytracer::Camera;

	const Camera &camera = Camera::create(width, height, { 3.00, 1.50, -4.00 } );

	// Lights
	raytracer::Light scene_light	( { -7.00, 10.00, -10.00 }, Color::WHITE	);

	raytracer::SceneLights lights = {
		&scene_light
	};

	// Scene objects

	using raytracer::Sphere;

	Color color1{ 0.25, 1.00, 0.25, 0.3 };
	Color color2{ 0.25, 0.25, 1.00, 0.3 };

	const Sphere scene_sphere	( {  0.00,  0.00, 0.00 },  1.00, color1	);
	const Sphere scene_sphere1	( { -0.80, -0.80, 0.00 },  0.80, color2	);
	const Sphere scene_sphere2	( {  0.80, -0.80, 0.00 },  0.80, color2	);

	using raytracer::Plane;

	const Plane  scene_plane	( -1.8, { 0.1, 0.1, 0.1, 0.2 }, { 0.9, 0.9, 0.9, 0.2 }	);

	using raytracer::iObject;

	const raytracer::SceneObjects scene_objects = {
		dynamic_cast<const iObject*>(&scene_sphere	)
	,	dynamic_cast<const iObject*>(&scene_sphere1	)
	,	dynamic_cast<const iObject*>(&scene_sphere2	)
	,	dynamic_cast<const iObject*>(&scene_plane	)
	};

	// Render

	raytracer::render("data/water.ppm", width, height, scene_objects, lights, camera);
}

