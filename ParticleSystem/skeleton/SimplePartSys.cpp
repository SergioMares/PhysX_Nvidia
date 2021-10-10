#include "SimplePartSys.h"
#include<iostream>

SimplePartSys::SimplePartSys(Vector3 _EmitterPos, int _PartAmount, float _PartSize, float _SpawnRate, int _Power)
{	
	emitter = _EmitterPos;
	amount = _PartAmount;
	rate = _SpawnRate;
	power = _Power;
	
	counter = 0;
	tempoT = 0;
	bSpawn = false;
	
	containerColor = Vector4(0, 1, 1, 1);
	partColor = Vector4(1, 1, 0, 0);

	for (size_t i = 0; i < amount; i++)
	{
		Actors[i] = new Particle(_EmitterPos, Vector3(0), Vector3(0), 0.9, _PartSize, partColor);
	}

	//Actor = new Particle(_EmitterPos, partVel, Vector3(0, -10, 0), 0.9, _PartSize, partColor);
	Container = new Particle(emitter, Vector3(0), Vector3(0),0.9, _PartSize*2, containerColor);
	
	//Actors[0] = Actor;

	cout << sizeof(*Actors) << endl;
}

SimplePartSys::~SimplePartSys()
{
	
	for (size_t i = 0; i < amount; i++)
	{
		Actors[i]->~Particle();
	}
	
	/*Actors[0]->~Particle();
	Actors[8]->~Particle();*/
}

void SimplePartSys::UpdateSys(double t, float time)
{
	/*
	for (size_t i = 0; i < amount; i++)
	{
		Actors[i]->Update(t);

		if (int(time) == i && counter == i)
		{
			cout << "just once please" << endl;

			int rX = rand() % power * 2 - power;
			int rZ = rand() % power * 2 - power;

			Vector3 partVel(rX, power, rZ);

			Actors[i]->setVel(partVel);
			Actors[i]->setAcc(Vector3(0, -10, 0));
			counter++;
		}
		
	}*/

	tempoT += t;
	if (tempoT >= rate)
	{
		tempoT = 0;
		bSpawn = true;		
	}

	cout << counter << endl;

	for (size_t i = 0; i < amount; i++)
	{
		//all actors are updating at every frame
		Actors[i]->Update(t);

		//actor  by actor is getting a new velocity and gravity acceleration
		if (counter == i && bSpawn)
		{		
			Actors[i]->setPos(emitter);
			Actors[i]->setAcc(Vector3(0));
			Actors[i]->setVel(Vector3(0));

			int rX = rand() % power * 2 - power;
			int rZ = rand() % power * 2 - power;

			Vector3 partVel(rX, power, rZ);

			Actors[i]->setVel(partVel);
			Actors[i]->setAcc(Vector3(0, -10, 0));
			
			//counter to check wich actor should give the new values
			counter++;
			//to avoid give the values again
			bSpawn = false;
		}

	}

	if (counter >= 100)
	{
		counter = 0;

	}
		
	
	 /*
	Actors[0]->Update(t);
	Actors[99]->Update(t);
	*/
	//Actors.begin()->Update(t);

	//Actors[0].Update(t);
	//Actor->Update(t);
}