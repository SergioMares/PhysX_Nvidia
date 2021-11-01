#include "ParticleForceRegistry.h"

void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* fg)
{
	registrations.push_back(ParticleForceRegistration() = {particle, fg});
}

void ParticleForceRegistry::remove(Particle* particle, ParticleForceGenerator* fg)
{
	for (auto it = registrations.begin(); it != registrations.end(); ++it)
	{
		if ((*it).particle == particle && (*it).fg == fg)
		{
			registrations.erase(it);
			break;
		}
	}		
}

void ParticleForceRegistry::clear()
{
	registrations.clear();
}

void ParticleForceRegistry::updateForces(float t)
{
	for (auto it = registrations.begin(); it != registrations.end(); ++it)
	{
		it->fg->updateForce(it->particle, t);
	}
}

int ParticleForceRegistry::countRegisters()
{
	return registrations.size();
}
