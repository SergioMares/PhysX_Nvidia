#pragma once
#include "Particle.h"
class ParticleForceGenerator
{
public:
	//overload to provide functionality
	virtual void updateForce(Particle* particle, float t) = 0;
};

