#include <iostream>

#include <utility>	// pair
#include <algorithm>	// min_element

#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Plane.h"

#include "RGB.h"

using SceneLights  = std::vector<const Light*>;
using SceneObjects = std::vector<const Object*>;

using IntersectionPair = std::pair<double, const Object *>;

namespace{

void intersectVector(const Ray &ray, const SceneObjects &objects, std::vector<IntersectionPair> &intersections){
	intersections.clear();

	for(const auto *obj : objects){
		double t;
		if ( obj->intersection(ray, t) )
			intersections.push_back( std::make_pair(t, obj) );
	}
}

Color traceColor(const Object *obj, const Vect &intersection_position, const Vect &intersecting_ray_direction,
	const SceneObjects &scene_objects, const SceneLights &light_sources) {

	double const ambientlight = 0.2;
	double const accuracy     = 0.00000001;

	const Color &obj_color  = obj->getColor();
	const Vect  &obj_normal = obj->normalAt(intersection_position);

	Color final_color = obj_color * ambientlight;


	if(0)
	if (obj_color.special > 0 && obj_color.special <= 1) {
		// reflection from objects with specular intensity
		double const dot1 = obj_normal.dotProduct(intersecting_ray_direction.negative());

		const Vect &scalar1 = obj_normal * dot1;
		const Vect &add1    = scalar1 + intersecting_ray_direction;
		const Vect &scalar2 = add1 * 2;
		const Vect &add2    = intersecting_ray_direction.negative() + scalar2;
		const Vect &reflection_direction = add2.normalize();

		Ray reflection_ray (intersection_position, reflection_direction);

		// determine what the ray intersects with first
		std::vector<IntersectionPair> reflection_intersections;

		intersectVector(reflection_ray, scene_objects, reflection_intersections);

		if (! reflection_intersections.empty()){
			const auto &p = *std::min_element(reflection_intersections.begin(), reflection_intersections.end());
			double const t = p.first;
			const auto *obj = p.second;

			// reflection ray missed everthing else
			if (t > accuracy){
				// determine the position and direction at the point of intersection with the reflection ray
				// the ray only affects the color if it reflected off something

				Vect reflection_intersection_position =
					intersection_position +
					reflection_direction * t;

				Vect reflection_intersection_ray_direction = reflection_direction;

				Color reflection_intersection_color = traceColor(
						obj, reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, light_sources);

				final_color = final_color + reflection_intersection_color * obj_color.special;
			}
		}
	}



	std::vector<IntersectionPair> secondary_intersections;

	for (const Light *light : light_sources){

		const Vect &light_direction =
					( light->pos + intersection_position.negative() ).normalize();

		double const cosine_angle = obj_normal.dotProduct(light_direction);

		if (cosine_angle > 0) {
			// test for shadows
			const Vect &distance_to_light = light_direction;

			double const distance_to_light_magnitude = distance_to_light.magnitude();

			const Ray shadow_ray( intersection_position, light_direction);

			intersectVector(shadow_ray, scene_objects, secondary_intersections);

			bool shadowed = false;
			for (const auto &p : secondary_intersections){
				if (p.first > accuracy && p.first <= distance_to_light_magnitude){
					shadowed = true;
					break;
				}
			}

			if (shadowed == false) {
				final_color = final_color + (obj_color * light->color * cosine_angle);

				if (0)
				if (obj_color.special > 0 && obj_color.special <= 1) {
					double dot1 = obj_normal.dotProduct(intersecting_ray_direction.negative());
					const Vect &scalar1 = obj_normal * dot1;
					const Vect &add1 = scalar1 + intersecting_ray_direction;
					const Vect &scalar2 = add1 * 2;
					const Vect &add2 = intersecting_ray_direction.negative() + scalar2;
					const Vect &reflection_direction = add2.normalize();

					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color + light->color * (specular * obj_color.special);
					}
				}


			}

		}
	}

	return final_color.clip();
}

void render(int const width, int const height, RGBVector &pixels,
		const SceneObjects &scene_objects, const SceneLights &scene_lights,
		const Camera &camera){

	std::vector<IntersectionPair> intersections;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			const Ray &camera_ray = camera.getRay(x, y);

			intersectVector(camera_ray, scene_objects, intersections);

			if (intersections.empty()){
				// set the backgroung black
				pixels.push_back( { 0, 0, 0 } );

				continue;
			}

			const auto &p = *std::min_element(intersections.begin(), intersections.end());
			double const t = p.first;
			const auto *obj = p.second;

		#if 1
			const Vect &intersection_position      = camera_ray.origin + camera_ray.dir * t;
			const Vect &intersecting_ray_direction = camera_ray.dir;

			const Color &color = traceColor(
						obj,
						intersection_position, intersecting_ray_direction,
						scene_objects, scene_lights);
		#else
			(void)t;
			const Color &color = obj->getColor();
		#endif

			pixels.push_back( { color.r, color.g, color.b } );

		}
	}
}

} // anonymous namespace

int main (){
	std::cout << "rendering ..." << '\n';

	int const width  = 640;
	int const height = 480;

	RGBVector pixels;
	pixels.reserve(width * height);

	// Camera eye
	const Camera &camera = Camera::create(width, height, { 3.00, 1.50, -4.00 } );

	// lights
	Light scene_light	( { -7.00, 10.00, -10.00 }, Color::WHITE	);

	SceneLights lights = {
		&scene_light
	};

	// scene objects
	const Color tileFloor(1, 1, 1, 2);

	const Sphere scene_sphere	( {  0.00,  0.00, 0.00 },  1.00, { 0.50, 1.00, 0.50, 0.3 }	);
	const Sphere scene_sphere1	( { -0.80, -0.80, 0.00 },  0.80, Color::RED			);
	const Sphere scene_sphere2	( {  0.80, -0.80, 0.00 },  0.80, Color::RED			);

	const Plane  scene_plane	( -1.8, Color::GREY	);

	const SceneObjects scene_objects = {
		dynamic_cast<const Object*>(&scene_sphere	)
	,	dynamic_cast<const Object*>(&scene_sphere1	)
	,	dynamic_cast<const Object*>(&scene_sphere2	)
	,	dynamic_cast<const Object*>(&scene_plane	)
	};

	render(width, height, pixels, scene_objects, lights, camera);

	saveRGB("scene_anti-aliased.ppm", width, height, pixels);
}
