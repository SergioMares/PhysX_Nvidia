#include "Particle.h"

Particle::Particle(Vector3 _Position, Vector3 _Velocity, Vector3 _Acceleration, double _Damping, float Size)
{
	//header variables get constructor variables
	vel = _Velocity;
	acc = _Acceleration;
	damp = _Damping;
	size = Size;

	pos = physx::PxTransform(_Position.x, _Position.y, _Position.z);

	figure = new RenderItem(CreateShape(physx::PxSphereGeometry(Size)), &pos, { 0, 0, 0, 0 });
}

Particle::~Particle()
{
	DeregisterRenderItem(figure);
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