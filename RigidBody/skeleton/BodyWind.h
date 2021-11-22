#pragma once
#include "BodyForceGenerator.h"
class BodyWind : public BodyForceGenerator
{
	PxVec3 w;

public:
	BodyWind(const Vector3 wind) : w(wind){}

	virtual void updateForce(SolidBody* body, double t);
};

