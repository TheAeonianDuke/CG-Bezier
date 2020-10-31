//sphere_dielectric.h
#ifndef _SPHERE_DIELECTRIC_H_
#define _SPHERE_DIELECTRIC_H_

#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "color.h"

class Sphere_2 : public Object
{
private:
	Vector3D position;
	double radius;

public:
	Sphere_2(const Vector3D& _pos, double _rad, Material* mat):
		Object(mat), position(_pos), radius(_rad)
	{
		isSolid = false;
	}
	
	virtual bool intersect(Ray& r) const;
};
#endif
