#include "Particle.h"
#include <iostream>
using namespace std;

Particle::Particle(Vector3 _Position, Vector3 _Velocity, float _Size, float _Mass, float _Alpha, double _Damping)
{
	//header variables get constructor variables
	vel = _Velocity;	
	size = _Size;
	mass = _Mass;

	pos = physx::PxTransform(_Position.x, _Position.y, _Position.z);

	acc = Vector3(0);
	force = Vector3(0);
	damp = _Damping;

	Vector4(
		float((rand() % 10)) / 10,
		float((rand() % 10)) / 10,
		float((rand() % 10)) / 10,
		1);

	//float((rand() % 10)) / 10
	figure = new RenderItem(CreateShape(physx::PxSphereGeometry(_Size)), &pos, Vector4(
		0.9,
		0.8,
		0,
		_Alpha));
}

Particle::~Particle()
{
	DeregisterRenderItem(figure);
}

void Particle::Update(double t)
{	
	//cout << pos.p.y << endl;

	Vector3 totalAcc = acc;
	acc = force;/////////////////////

	//apply acceleration
	vel += totalAcc * t;

	//apply drag
	vel *= powf(damp, t);

	//MRUA
	pos = physx::PxTransform(pos.p.x + vel.x * t, pos.p.y + vel.y * t, pos.p.z + vel.z * t);

	clearForce();
}

void Particle::clearForce()
{
	force = Vector3(0);		
}

void Particle::addForce(const Vector3& f)
{
	force += f;
}

//Getters
physx::PxTransform Particle::getPos()
{
	return pos;
}

float Particle::getMass()
{
	return mass;
}

//Setters
void Particle::setVel(Vector3 _newVel)
{
	vel = _newVel;
}

void Particle::setAcc(Vector3 _newAcc)
{
	acc = _newAcc;
}

void Particle::setPos(Vector3 _newPos)
{
	pos = physx::PxTransform(_newPos.x, _newPos.y, _newPos.z);
}

void Particle::setMass(float newMass)
{
	mass = newMass;
}

void Particle::setDamp(double newDamp)
{
	damp = newDamp;
}
