#include "SimplePartSys.h"

SimplePartSys::SimplePartSys(Vector3 _EmitterPos, float _PartSize, float _SpawnRate, int _Power)
{	
	emitter = _EmitterPos;
	rate = _SpawnRate;
	power = _Power;
	
	counter = 0;
	tempoT = 0;
	bSpawn = false;
	
	containerColor = Vector4(0, 1, 1, 1);	

	//initialize all particles
	for (size_t i = 0; i < amount; i++)
	{
		partColor = Vector4(
			float((rand() % 10)) / 10,
			float((rand() % 10)) / 10,
			float((rand() % 10)) / 10,
			1);

		//intialize without vel and acc to add them later
		Actors[i] = new Particle(_EmitterPos, Vector3(0), Vector3(0), 0.9, _PartSize, partColor);
	}
			
	Container = new Particle(emitter, Vector3(0), Vector3(0),0.9, _PartSize*2, containerColor);
}

SimplePartSys::~SimplePartSys()
{	
	for (size_t i = 0; i < amount; i++)
	{
		Actors[i]->~Particle();
		//delete Actors[i];		
	}
		
	Container->~Particle();
	//delete Container;
}

void SimplePartSys::UpdateSys(double t)
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
		Actors[i]->Update(t);

		//actor by actor is getting a new velocity and gravity acceleration
		if (counter == i && bSpawn)
		{
			//reset the particle vectors. Usefull to reutilize the vectors already spawned
			Actors[i]->setPos(emitter);
			Actors[i]->setAcc(Vector3(0));
			Actors[i]->setVel(Vector3(0));

			int rX = rand() % power * 2 - power;
			int rZ = rand() % power * 2 - power;

			Vector3 partVel(rX, power, rZ);

			//giving the new values
			Actors[i]->setVel(partVel);
			Actors[i]->setAcc(Vector3(0, -10, 0));
			
			//counter to check to wich actor should give the new values
			counter++;
			//to avoid give the values again
			bSpawn = false;
		}

	}

	//start using again the already spawned particles
	if (counter >= 100)
	{
		counter = 0;
	}
}