#include "ParticleWind.h"
#include <iostream>

using namespace std;

void ParticleWind::updateForce(Particle* particle, float t)
{
	if (1 / particle->getMass() == 0) return;
		
	if (isInRange(pos, particle->getPos().p))
		particle->addForce(w / particle->getMass());
}

bool ParticleWind::isInRange(Vector3 point1, Vector3 point2)
{
	float distance;
	distance = sqrtf(
		powf(point1.x - point2.x, 2) +
		powf(point1.y - point2.y, 2) +
		powf(point1.z - point2.z, 2));	

	if (distance < r)
		return true;
	else
		return false;		
}

