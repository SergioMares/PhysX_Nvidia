#pragma once
#include "core.hpp"
#include "RenderUtils.hpp"

class Particle
{
public:
	Particle(Vector3 Position, Vector3 Velocity, float Size);
	~Particle();
	void Update(double t);
	void clearForce();
	void addForce(const Vector3& f);

private: 
	Vector3 vel;
	Vector3 acc;
	Vector3 force;
	float size;
	double damp;

	physx::PxTransform pos;

	RenderItem* figure;
};

