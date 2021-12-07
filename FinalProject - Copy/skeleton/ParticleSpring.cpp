#include "ParticleSpring.h"

void ParticleSpring::updateForce(Particle* particle, float t)
{
	if (particle->getMass() == 0) return;

	//calculate distance vector
	Vector3 f = particle->getPos().p;
	f -= other->getPos().p;

	//length
	float length = f.normalize();

	//resulting force
	float deltaL = (length - restLength);
	float forceMagnitude = -k * deltaL;
	f *= forceMagnitude;
	particle->addForce(f);
}
