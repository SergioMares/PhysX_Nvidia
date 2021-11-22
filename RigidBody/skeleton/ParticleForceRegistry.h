#pragma once
#include "Particle.h"
#include "ParticleForceGenerator.h"
#include "BodySystem.h"
#include "BodyForceGenerator.h"

class ParticleForceRegistry
{
protected:
	//strage for generato-particle entry
	struct ParticleForceRegistration
	{
		Particle* particle;
		ParticleForceGenerator* fg;
	};

	struct BodyForceRegistration
	{
		SolidBody* body;
		BodyForceGenerator* fg;
	};

	typedef	std::vector<ParticleForceRegistration> Registry;
	Registry registrations;

	typedef std::vector<BodyForceRegistration> Registry_rigid;
	Registry_rigid registrations_rigid;

public: 
	//Associate generator with a particle 
	void add(Particle* particle, ParticleForceGenerator* fg);
	void addB(SolidBody* body, BodyForceGenerator* fg);

	//Remove association
	void remove(Particle* particle, ParticleForceGenerator* fg);

	//Removes all associations. Particle and Generators won't be deleted 
	void clear();

	void updateForces(float t);	

	int countRegisters();
};

