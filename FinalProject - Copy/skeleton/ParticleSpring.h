#pragma once
#include "ParticleForceGenerator.h"
class ParticleSpring : public ParticleForceGenerator
{
	//the other particle the spring is attached to
	Particle* other;

	//Elasticity constant
	float k;

	float restLength;

public:
	ParticleSpring(Particle* _other, float _k, float _restLength) : other(_other), k(_k), restLength(_restLength) {};

	virtual void updateForce(Particle* particle, float t);
};

