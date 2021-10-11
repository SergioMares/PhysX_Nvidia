#pragma once
#include "core.hpp"
#include "RenderUtils.hpp"

class Particle
{
public:
	Particle(Vector3 Position, Vector3 Velocity, Vector3 Acceleration, double Damping, float Size, Vector4 Color);
	~Particle();
	void Update(double t);

	//getters

	//setters
	void setVel(Vector3 newVel);
	void setAcc(Vector3 newAcc);
	void setPos(Vector3 newPos);

private: 
	Vector3 vel;
	Vector3 acc;
	Vector4 color;
	float size;
	double damp;

	physx::PxTransform pos;

	RenderItem* figure;
};

