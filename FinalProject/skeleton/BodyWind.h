#pragma once
#include "BodyForceGenerator.h"
class BodyWind : public BodyForceGenerator
{
	Vector3 w, p;
	float r;

public:
	BodyWind(const Vector3 wind, float radius, Vector3 position) 
		: w(wind), r(radius), p(position) {}

	virtual void updateForce(SolidBody* body, double t);
	bool isInRange(Vector3 point1, Vector3 point2);
};

