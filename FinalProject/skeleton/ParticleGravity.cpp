#include "ParticleGravity.h"

void ParticleGravity::updateForce(Particle* particle, float t)
{
	if (1/particle->getMass() == 0) return;
	
	particle->addForce(g);
}