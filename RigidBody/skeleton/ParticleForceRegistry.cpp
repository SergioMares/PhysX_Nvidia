#include "ParticleForceRegistry.h"

void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* fg)
{
	registrations.push_back(ParticleForceRegistration() = {particle, fg});
}

void ParticleForceRegistry::addB(SolidBody* body, BodyForceGenerator* fg)
{	
	if (body->isNew)
	{		
		registrations_rigid.push_back(BodyForceRegistration() = { body, fg });
		body->isNew = false;
	}
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

	for (auto it = registrations_rigid.begin(); it != registrations_rigid.end(); ++it)
	{
		it->fg->updateForce(it->body, t);
	}		
}

int ParticleForceRegistry::countRegisters()
{
	return registrations.size();
}
