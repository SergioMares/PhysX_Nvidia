#pragma once
#include <PxPhysicsAPI.h>
#include "RenderUtils.hpp"
using namespace physx;
using namespace std;

struct SolidBody
{
	PxRigidDynamic* rigid = nullptr;
	bool isNew;
	float life;
	RenderItem* item = nullptr;
	PxVec3 force;
	PxVec3 torque;
};

class BodySystem
{
public: 
	BodySystem(PxPhysics* _gPhysics, PxScene* _gScene, PxTransform _pSet, PxMaterial *_gMaterial) :
		gPhysics(_gPhysics), gScene(_gScene), pSet(_pSet), Material(_gMaterial)
	{
		rateSpawn = 1;
		timeSpawn = 0;		
	}

	~BodySystem();
	void addBody();
	void integrate(double t);
	void deleteDeads();
	vector<SolidBody*> bodies;

private:
	PxPhysics*	gPhysics	= nullptr;
	PxScene*	gScene		= nullptr;
	PxTransform	pSet;
	PxMaterial* Material = nullptr;

	double timeSpawn, rateSpawn;


};

