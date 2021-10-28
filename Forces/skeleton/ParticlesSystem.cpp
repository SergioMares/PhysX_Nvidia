#include "ParticlesSystem.h"
#include <iostream>

using namespace std;

ParticlesSystem::ParticlesSystem(Vector3 _EmitterPos, int _PartVel, float _PartSize, float _PartMass, float _SpawnRate,  int _Amount)
{
	emitter = _EmitterPos;
	mass = _PartMass;
	partVel = _PartVel;
	rate = _SpawnRate;
	amount = _Amount;
	

	counter = 0;
	tempoT = 0;
	bSpawn = false;

	containerColor = Vector4(0, 1, 1, 1);

	
	//initialize all particles
	for (size_t i = 0; i < amount; i++)
	{
		Actorss.push_back(new Particle(_EmitterPos, Vector3(0), _PartSize, 10));
	}
	
	Container = new Particle(emitter, Vector3(0), _PartSize * 2, INFINITY);
}

ParticlesSystem::~ParticlesSystem()
{
	for (size_t i = 0; i < amount; i++)
	{
		//Actors[i]->~Particle();
		//delete Actors[i];		
	}

	Container->~Particle();
	//delete Container;
}

void ParticlesSystem::UpdateSys(double t)
{
	tempoT += t;
	if (tempoT >= rate)
	{
		tempoT = 0;//reset the timer
		bSpawn = true; //set the flag on to spawn a particle
	}

	for (size_t i = 0; i < amount; i++)
	{
		//all actors are updating at every frame
		Actorss[i]->Update(t);

		//actor by actor is getting a new velocity and gravity acceleration
		if (counter == i && bSpawn)
		{
			//reset the particle vectors. Usefull to reutilize the vectors already spawned
			Actorss[i]->setPos(emitter);
			Actorss[i]->setAcc(Vector3(0));
			Actorss[i]->setVel(Vector3(0));
			
			int rX = rand() % partVel * 2 - partVel;
			int rY = rand() % partVel * 2 - partVel;
			int rZ = rand() % partVel * 2 - partVel;

			Vector3 partVel(rX, rY, rZ);

			//giving the new values
			Actorss[i]->setVel(partVel);
			//Actors[i]->setAcc(Vector3(0, -10, 0));

			//counter to check to wich actor should give the new values
			counter++;
			//to avoid give the values again
			bSpawn = false;
		}

	}

	//start using again the already spawned particles
	if (counter >= amount)
	{
		counter = 0;
	}
}

std::vector<Particle*> ParticlesSystem::getActors()
{
	return Actorss;
}
