#include "ParticleExplosion.h"
#include <iostream>
using namespace std;

void ParticleExplosion::updateForce(Particle* particle, float t)
{
	tempoTime += t;
	if (1 / particle->getMass() == 0) return;

	if (isInRange(pos, particle->getPos().p))
	{
		e = particle->getPos().p - pos;
		
		particle->addForce(e*TNT / particle->getMass());		
		//e *= TNT;
	}

	if (tempoTime > exlpoTime)
	{
		if (isInRange(pos, particle->getPos().p))
		{
			e = particle->getPos().p - pos;

			particle->addForce((-e * TNT / particle->getMass()));
			//e *= TNT;
		}
		//cout << tempoTime << endl;
		
	}
}

bool ParticleExplosion::isInRange(Vector3 point1, Vector3 point2)
{
	float distance;
	distance = getDistance(point1, point2);

	if (distance < r)
		return true;
	else
		return false;
}

float ParticleExplosion::getDistance(Vector3 point1, Vector3 point2)
{
	return sqrtf(
		powf(point1.x - point2.x, 2) +
		powf(point1.y - point2.y, 2) +
		powf(point1.z - point2.z, 2));
}
