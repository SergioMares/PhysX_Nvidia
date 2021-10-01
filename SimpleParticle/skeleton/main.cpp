#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "Particle.h"

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

Particle*				Actor		= NULL;
PxTransform*			CamTrans	= NULL;

float					camX,
						camY,
						camZ,
						actorSize;

Vector3					actorPos, 
						actorVel, 
						actorAcc;

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

	Vector3
		actorPos(30, 40, 40),
		actorVel(0, 20, -20),
		//actorVel(0),
		actorAcc(0, -10, 0);
		//actorAcc(0);

	actorSize = (rand() % 10 + 1)/10.0f;
	cout << actorSize;

	Actor = new Particle(actorPos, actorVel, actorAcc, 0.111, actorSize);

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

	Actor->Update(t);
	//cout << "camera: " << GetCamera() << endl;
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
	Actor->~Particle();
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
	case 'B': 
		//use delete instead 
		//deregister previous particle
		delete Actor;

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