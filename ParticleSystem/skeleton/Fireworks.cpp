#include "Fireworks.h"

Fireworks::Fireworks(Vector3 _EmitterPos, double _Damping, float _PartSize, Vector3 _Power, float _Age, bool _bHasPayload)
{
	emitter = _EmitterPos;	
	power = _Power;
	age = _Age;
	bPayload = _bHasPayload;
	Size = _PartSize;
	damp = _Damping;

	tempoT = 0;
	bSpawn = false;

	//containerColor = Vector4(0, 1, 1, 1);
	partColor = Vector4(1, 0, 1, 1);

	primigenial = new Particle(emitter, power, Vector3(0, -10, 0) , damp, Size * 0.7, partColor);
	
}

Fireworks::~Fireworks()
{
	//delete Container;
}

void Fireworks::UpdateSys(double t)
{
	tempoT += t;
	if (tempoT > age && !bSpawn)
	{
		if (bPayload)
		{
			physx::PxTransform origin = primigenial->getPos();
			spawnFireworks(100, 1, Vector3(origin.p.x, origin.p.y, origin.p.z));
		}
			
		delete primigenial;
		bSpawn = true;
	}

	primigenial->Update(t);
	
	if (bPayload && bSpawn)
	{
		for (size_t i = 0; i < 100; i++)
		{
			NewFireworks[i]->UpdateSys(t);
		}		
	}
}

void Fireworks::spawnFireworks(int _Amount, unsigned _PartType, Vector3 _Origin)
{	
	for (size_t i = 0; i < _Amount; i++)
	{
		int tempoPower = power.y;
		int rX = rand() % tempoPower * 2 - tempoPower;
		int rZ = rand() % tempoPower * 2 - tempoPower;

		Vector3 newPower(rX, -10, rZ);

		NewFireworks[i] = new Fireworks(_Origin, 0.1, Size * 0.7, newPower, 2, false);
	}
}