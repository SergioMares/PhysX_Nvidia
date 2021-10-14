#pragma once
#include "Particle.h"

class Fireworks
{
public:
	Fireworks(Vector3 EmitterPos, double Damping, float PartSize, Vector3 Power, float Age, bool bHasPayload);
	~Fireworks();

	void UpdateSys(double t);
	void spawnFireworks(const int Amount, unsigned PartType, Vector3 Origin);

private:
	Vector3 emitter;
	Vector4 partColor;
	//int amount = 100;
	//float rate;
	Vector3 power;

	float age;
	float tempoT;
	bool bSpawn, bPayload;
	float Size;
	double damp;

	Particle* primigenial = NULL;
	Fireworks* NewFireworks[100];
	//Particle* Actors[100];
};



