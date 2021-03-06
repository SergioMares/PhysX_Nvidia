#include "Particle.h"

Particle::Particle(Vector3 _Position, Vector3 _Velocity, Vector3 _Acceleration, double _Damping, float _Size, Vector4 _Color)
{
	//header variables get constructor variables values
	vel = _Velocity;
	acc = _Acceleration;
	damp = _Damping;
	size = _Size;

	pos = physx::PxTransform(_Position.x, _Position.y, _Position.z);

	figure = new RenderItem(CreateShape(physx::PxSphereGeometry(_Size)), &pos, _Color);	
}

Particle::~Particle()
{
	DeregisterRenderItem(figure);
	delete figure;
}

void Particle::Update(double t)
{
	//apply acceleration
	vel += acc * t;

	//apply drag
	vel *= powf(damp, t);

	//MRUA
	pos = physx::PxTransform(pos.p.x + vel.x * t, pos.p.y + vel.y * t, pos.p.z + vel.z * t);
}

//Getters
physx::PxTransform Particle::getPos()
{
	return pos;
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