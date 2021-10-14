#pragma once
#include "Particle.h"

class Fireworks
{
public:
	Fireworks(Vector3 EmitterPos, bool bHasPayload, unsigned PartType);
	~Fireworks();

	void UpdateSys(double t);
	void SpawnFireworks(const int Amount, Vector3 Origin);
	void SetFirework();
	void Clear();

private:
	Vector3 emitter;
	Vector4 partColor;
	//int amount = 100;
	//float rate;
	Vector3 vPower;

	float age;
	float tempoT;
	bool bSpawn, bPayload;
	float Size;
	double damp;
	unsigned partType;
	int iPower;

	Particle* primigenial = NULL;
	Fireworks* NewFireworks[100];
	//Particle* Actors[100];
};



