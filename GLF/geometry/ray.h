#ifndef GLF_GEOMETRY_RAY
#define GLF_GEOMETRY_RAY
#include "main.h"
#include "arith.h"

class Ray {
public:
	Point src;
	Vector dir;
	mutable float mint, maxt;
	float time;
	int depth;

	Ray() : mint(0.f), maxt(INFINITY), time(0.f), depth(0) { }
	Ray(const Ray &r) {
		src = r.src;
		dir = r.dir;
		return;
	}
	Ray(const Point &origin, const Vector &direction,
		float start, float end = INFINITY, float t = 0.f, int dir = 0)
		: src(origin), dir(direction), mint(start), maxt(end), time(t), depth(dir) { }
	Ray(const Point &origin, const Vector &direction, const Ray &parent,
		float start, float end = INFINITY)
		: src(origin), dir(direction), mint(start), maxt(end),
		time(parent.time), depth(parent.depth + 1) { }
	Point operator()(float t) const { return src + dir * t; }
};

#endif