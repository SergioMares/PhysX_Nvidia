#pragma once
#include "ParticleForceGenerator.h"
class ParticleWind : public ParticleForceGenerator
{
	Vector3 w, pos;	
	float r;	
public:
	ParticleWind(const Vector3& wind, float radius, Vector3 position) : w(wind), r(radius), pos(position) {}

	virtual void updateForce(Particle* particle, float t);

	bool isInRange(Vector3 point1, Vector3 point2);
};

