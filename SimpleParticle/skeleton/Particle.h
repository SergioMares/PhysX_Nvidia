#pragma once
#include "core.hpp"
#include "RenderUtils.hpp"

class Particle
{
public:
	Particle(Vector3 Position, Vector3 Velocity, Vector3 Acceleration, double Damping);
	~Particle();
	void Update(double t);

private: 
	Vector3 vel;
	Vector3 acc;
	double damp;

	physx::PxTransform pos;

	RenderItem* figure;
};

