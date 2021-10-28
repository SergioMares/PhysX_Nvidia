#pragma once
#include "Particle.h"
#include "ParticleForceGenerator.h"

class ParticleForceRegistry
{
protected:
	//strage for generato-particle entry
	struct ParticleForceRegistration
	{
		Particle* particle;
		ParticleForceGenerator* fg;
	};

	typedef	std::vector<ParticleForceRegistration> Registry;
	Registry registrations;

public: 
	//Associate generator with a particle 
	void add(Particle* particle, ParticleForceGenerator* fg);

	//Remove association
	void remove(Particle* particle, ParticleForceGenerator* fg);

	//Removes all associations. Particle and Generators won't be deleted 
	void clear();

	void updateForces(float t);	

	int countRegisters();
};

