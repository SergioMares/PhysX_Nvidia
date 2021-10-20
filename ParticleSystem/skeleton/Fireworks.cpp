#include "Fireworks.h"
#include <iostream>

using namespace std;

Fireworks::Fireworks(Vector3 _EmitterPos, bool _bHasPayload, unsigned _PartType)
{
	emitter = _EmitterPos;			
	bPayload = _bHasPayload;	
	partType = _PartType;

	tempoT = 0;
	bSpawn = false;

	SetFirework();//preset the particle properties depending of their type before create them
	primigenial = new Particle(emitter, vPower, Vector3(0, -10, 0), damp, Size * 0.7, partColor);
}

Fireworks::~Fireworks()
{	
}

void Fireworks::UpdateSys(double t)
{
	tempoT += t;
	//after the primigenial 'explodes' create the new ones, if they have payload
	if (tempoT > age && !bSpawn)
	{
		if (bPayload)
		{
			physx::PxTransform origin = primigenial->getPos();
			SpawnFireworks(100, Vector3(origin.p.x, origin.p.y, origin.p.z));
		}
			
		Clear();
		bSpawn = true;
	}

	primigenial->Update(t);
	
	//update the new ones if they have been spawned
	if (bPayload && bSpawn)
	{
		for (size_t i = 0; i < 100; i++)
		{
			NewFireworks[i]->UpdateSys(t);
		}		
	}
}

void Fireworks::SpawnFireworks(int _Amount, Vector3 _Origin)
{	
	for (size_t i = 0; i < _Amount; i++)
	{		
		NewFireworks[i] = new Fireworks(_Origin, false, partType);
	}
}

void Fireworks::SetFirework()
{
	if (bPayload)//just for the first primigenial that will 'explode'
	{
		partColor = Vector4(1, 1, 0, 1);
		damp = 0.8;
		Size = 0.5;
		vPower = Vector3(0, 50, 0);
		age = 3;
	}
	else
	{
		switch (partType)
		{
		case 1:
			partColor = Vector4(1, 1, 1, 1);
			damp = 0.1;
			Size = 0.2;

			iPower = 50;
			vPower.x = rand() % iPower * 2 - iPower;
			vPower.y = 10;
			vPower.z = rand() % iPower * 2 - iPower;
			age = float(rand() % 30) / 10;
			
			break;

		case 2:
			partColor = Vector4(
				float((rand() % 10)) / 10,
				float((rand() % 10)) / 10,
				float((rand() % 10)) / 10, 
				1);
			damp = 0.3;
			Size = 1;

			iPower = 30;
			vPower.x = rand() % iPower * 2 - iPower;
			vPower.y = rand() % iPower * 2 - iPower;
			vPower.z = rand() % iPower * 2 - iPower;
			age = float(((rand() % 100) + 30)) / 10;
			
			break;

		case 3:
			partColor = Vector4(0, 0, 0, 1);
			damp = 0.1;
			Size = 1;

			iPower = 30;
			vPower.x = rand() % iPower * 2 - iPower;
			vPower.y = rand() % iPower * 2 - iPower;
			vPower.z = rand() % iPower * 2 - iPower;
			age = float(((rand() % 100) + 30)) / 10;
			break;			


		default:
			puts("no such particle");
			break;
		}
	}	
}

void Fireworks::Clear()
{
	//delete the particle if it hasnt already been removed
	if (!bSpawn)
		if (isfinite(primigenial->getPos().p.x))
			delete primigenial;

	//delete the new ones if has payload, if they have already spawned and if doesnt have been removed
	if (bSpawn && bPayload)
	{
		for (size_t i = 0; i < 100; i++)
		{
			if (isfinite(NewFireworks[i]->primigenial->getPos().p.x))
				delete NewFireworks[i]->primigenial;
		}		
	}
	
}