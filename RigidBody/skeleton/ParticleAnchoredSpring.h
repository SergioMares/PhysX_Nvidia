#pragma once
#include "ParticleForceGenerator.h"
class ParticleAnchoredSpring : public ParticleForceGenerator
{
	//positionof the other end of the spring
	Vector3* anchor;
	
	//elasticuty constant
	float k;

	float restLength;

public:
	ParticleAnchoredSpring(Vector3* _anchor, float _k, float _restLength) : anchor(_anchor), k(_k), restLength(_restLength) {};

	virtual void updateForce(Particle* particle, float t);
};

