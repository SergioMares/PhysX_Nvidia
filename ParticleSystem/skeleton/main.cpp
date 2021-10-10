#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "Particle.h"
#include "SimplePartSys.h"

#include <iostream>

using namespace physx;
using namespace std;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;


PxMaterial*				gMaterial	= NULL;

PxPvd*                  gPvd        = NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene      = NULL;
ContactReportCallback gContactReportCallback;

//Particle*				Actor		= NULL;
//PxTransform*			CamTrans	= NULL;
SimplePartSys*			Fountain	= NULL;

float					actorSize;
double					curTime;

/*
Vector3					actorPos, 
						actorVel, 
						actorAcc;


Particle* tempoArr[100];
*/


// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	//initialize variables
	srand(time(NULL));
	curTime = 0;

	Vector3 sysPos(32, 40, 30);

	Fountain = new SimplePartSys(sysPos, 100, 0.5, 0.1, 10);
	
	/*
	for (size_t i = 0; i < 100; i++)
	{
		int particleH = rand() % 20 + 1;
		int particleX = rand() % 5 + 1;
		int particleZ = rand() % 5 + 1;

		Vector3
			actorPos(32, 40, 30),
			actorVel(particleX, particleH, particleZ),
			//actorVel(0),
			actorAcc(0, -10, 0);
		//actorAcc(0);

	//actorSize = (rand() % 10 + 1)/10.0f;

		Actor = new Particle(actorPos, actorVel, actorAcc, 0.5, 0.1);
		tempoArr[i] = Actor;

	}
	*/
	
	

	//CamTrans = &GetCamera()->getTransform();
	

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);
	// ------------------------------------------------------
}


// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	gScene->fetchResults(true);
	curTime += t;
	
	Fountain->UpdateSys(t, curTime);
	

	/*
	for (size_t i = 0; i < 100; i++)
	{
		tempoArr[i]->Update(t);
	}
	*/
	
	//Actor->Update(t);
}

// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	// Rigid Body ++++++++++++++++++++++++++++++++++++++++++
	gScene->release();
	gDispatcher->release();
	// -----------------------------------------------------
	gPhysics->release();	
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();	
	gFoundation->release();

	//cleaning the actor from memory
	//dont forget to do this tho the array
	//Actor->~Particle();
	Fountain->~SimplePartSys();

}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
	/*
	case 'B': 
		
		//deregister previous particle
		Actor->~Particle();

		//get cam position
		CamTrans = &GetCamera()->getTransform();
		actorPos.x = CamTrans->p.x;
		actorPos.y = CamTrans->p.y;
		actorPos.z = CamTrans->p.z;

		//Add new acceleration a velocity accordingly to the camera
		//cout << GetCamera()->getDir().z;
		actorVel.x = GetCamera()->getDir().x*20;
		actorVel.y = GetCamera()->getDir().y*20;
		actorVel.z = GetCamera()->getDir().z*20;

		//add acceleration
		actorAcc.y = -10;
		
		//render new particle
		Actor = new Particle(actorPos, actorVel, actorAcc, 0.999, actorSize);

		break;
	*/
	//case 'V': Actor->~Particle();	break;
	case ' ':
	{
		break;
	}
	default:
		break;
	}
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
	PX_UNUSED(actor1);
	PX_UNUSED(actor2);
}


int main(int, const char*const*)
{
#ifndef OFFLINE_EXECUTION 
	extern void renderLoop();
	renderLoop();	
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}