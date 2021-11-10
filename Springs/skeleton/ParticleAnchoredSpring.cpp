#include "ParticleAnchoredSpring.h"

void ParticleAnchoredSpring::updateForce(Particle* particle, float t)
{
	Vector3 f = particle->getPos().p;
	f -= *anchor;

	//length
	float length = f.normalize();

	//resulting force
	f *= -((length - restLength) * k);
	particle->addForce(f);
}
