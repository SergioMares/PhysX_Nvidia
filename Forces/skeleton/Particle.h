#pragma once
#include "core.hpp"
#include "RenderUtils.hpp"

class Particle
{
public:
	Particle(Vector3 Position, Vector3 Velocity, float Size, float Mass, float Alpha);
	~Particle();
	void Update(double t);
	void clearForce();
	void addForce(const Vector3& f);

	//getters
	physx::PxTransform getPos();
	float getMass();

	//setters
	void setVel(Vector3 newVel);
	void setAcc(Vector3 newAcc);
	void setPos(Vector3 newPos);

private: 
	Vector3 vel;
	Vector3 acc;
	Vector3 force;
	float size;
	double damp;
	float mass;

	physx::PxTransform pos;

	RenderItem* figure;
};

