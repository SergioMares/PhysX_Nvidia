#pragma once
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>

using namespace physx;
using namespace std;

class RBGenerator
{
private: 
	struct SolidBody
	{
		PxRigidDynamic* rigidDyn = NULL;
		RenderItem* item = NULL;
	};

	PxPhysics* gPhysics = NULL;
	PxScene* gScene = NULL;
	RenderItem* item = NULL;
	vector<SolidBody*> RDbodies;	

	Vector3 
		Velocity = Vector3(0), 
		Force = Vector3(0), 
		AngularVel = Vector3(0);

	PxReal Mass = 1;
	double rate = 1;
	double life = 10;
	double time = 0;


public:
	RBGenerator(PxPhysics *_gPhysics, PxScene* _gScene) : gPhysics(_gPhysics), gScene(_gScene) {}

	~RBGenerator();
	void update(double t);

};

