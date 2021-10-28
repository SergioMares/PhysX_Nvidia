#include "ParticleGravity.h"

void ParticleGravity::updateForce(Particle* particle, float t)
{
	particle->addForce(g);
}