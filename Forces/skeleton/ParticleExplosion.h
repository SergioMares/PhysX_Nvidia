#pragma once
#include "ParticleForceGenerator.h"
class ParticleExplosion : public ParticleForceGenerator
{
	Vector3 e, pos;
	float r;
	int TNT;
	float exlpoTime = 100.0;
	float tempoTime;

public:
	ParticleExplosion(int _TNT, float radius, Vector3 position, float _tempoTime = 0) : TNT(_TNT), r(radius), pos(position), tempoTime(_tempoTime)  {}

	virtual void updateForce(Particle* particle, float t);

	bool isInRange(Vector3 point1, Vector3 point2);
	float getDistance(Vector3 point1, Vector3 point2);
};

