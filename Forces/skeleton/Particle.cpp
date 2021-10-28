#include "Particle.h"
#include <iostream>
using namespace std;

Particle::Particle(Vector3 _Position, Vector3 _Velocity, float Size)
{
	//header variables get constructor variables
	vel = _Velocity;	
	size = Size;

	pos = physx::PxTransform(_Position.x, _Position.y, _Position.z);

	acc = Vector3(0);
	force = Vector3(0);
	damp = 0.9;
	figure = new RenderItem(CreateShape(physx::PxSphereGeometry(Size)), &pos, { 0, 0, 0, 1 });
}

Particle::~Particle()
{
	DeregisterRenderItem(figure);
}

void Particle::Update(double t)
{	
	cout << pos.p.y << endl;

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