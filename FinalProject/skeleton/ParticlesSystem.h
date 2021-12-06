#pragma once
#include "Particle.h"
#include <vector>

class ParticlesSystem
{
public:
	ParticlesSystem(Vector3 EmitterPos, Vector3 PartVelocity, float PartSize, float PartMass, float SpawnRate, int Amount);
	~ParticlesSystem();

	void UpdateSys(double t);
	std::vector<Particle*> getActors();

private:
	Vector3 emitter;
	Vector3 partVel;
	float mass;
	int amount;
	float rate;
	
	Vector4 containerColor, partColor;

	int counter;
	float tempoT;
	bool bSpawn;

	Particle* Container = NULL;
	std::vector<Particle*> Actorss;
};

