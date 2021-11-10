#pragma once
#include "core.hpp"
#include "RenderUtils.hpp"

class Particle
{
public:
	Particle(Vector3 Position, Vector3 Velocity, float Size, float Mass, float Alpha, double Damping = 0.9);
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
	void setMass(float newMass);
	void setDamp(double newDamp);

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

