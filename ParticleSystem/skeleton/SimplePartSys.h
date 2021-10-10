#pragma once
#include "Particle.h"
#include <vector>

using namespace std;

class SimplePartSys
{
public:
	SimplePartSys(Vector3 EmitterPos, int PartAmount, float PartSize, float SpawnRate, int Power);
	~SimplePartSys();

	void UpdateSys(double t, float CurrentTime);

private: 
	Vector3 emitter;
	Vector4 containerColor, partColor;
	int amount;
	float rate;
	int power;
	
	int counter;
	float tempoT;
	bool bSpawn;

	Particle	*Actor = NULL, 
				*Container = NULL;
	Particle	*Actors[100];
	
};

