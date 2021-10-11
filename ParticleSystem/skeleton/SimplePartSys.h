#pragma once
#include "Particle.h"

class SimplePartSys
{
public:
	SimplePartSys(Vector3 EmitterPos, float PartSize, float SpawnRate, int Power);
	~SimplePartSys();

	void UpdateSys(double t);

private: 
	Vector3 emitter;
	Vector4 containerColor, partColor;
	int amount = 100;
	float rate;
	int power;
	
	int counter;
	float tempoT;
	bool bSpawn;

	Particle	*Container = NULL;
	Particle	*Actors[100];	
};

