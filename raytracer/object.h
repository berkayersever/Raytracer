#ifndef OBJECT_H_
#define OBJECT_H_

#include "ray.h"
#include "color.h"

namespace raytracer{

class Object {
private:
	static constexpr double ACCURACY = 0.00000001;

public:
	virtual const char *getName() const = 0;

	virtual const Color &getColor() const = 0;
	virtual Vect normalAt(const Vect &point) const = 0;

	// find only positive intersections
	bool intersection(const Ray &ray, double &t) const{
		bool const result = intersection_(ray, t);

		return result && t < ACCURACY ? false : result;
	}

private:
	virtual bool intersection_(const Ray &ray, double &t) const = 0;

};

} //namespace raytracer

#endif

