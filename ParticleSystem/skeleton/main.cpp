#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "Particle.h"
#include "SimplePartSys.h"
#include "Fireworks.h"

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

//my variables
SimplePartSys*			Fountain	= NULL;

Fireworks				*FW1		= NULL,
						*FW2		= NULL,
						*FW3		= NULL;

float					PartSize, fwAge;
bool					render1 = false, 
						render2 = false, 
						render3 = false;
Vector3					*sysPos;



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
	PartSize = 0.5;
	fwAge = 5;

	sysPos = new Vector3(32, 40, 30);


	Fountain = new SimplePartSys(*sysPos, PartSize, 0.1, 10);	

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
	
	Fountain->UpdateSys(t);	
	if (render1)
		FW1->UpdateSys(t);
	if (render2)
		FW2->UpdateSys(t);
	if (render3)
		FW3->UpdateSys(t);
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

	//cleaning the particle system from memory
	Fountain->~SimplePartSys();
	FW1->~Fireworks();
	FW2->~Fireworks();
	FW3->~Fireworks();

	delete sysPos;
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
		case '1': 
			if (render1)
				FW1->Clear();

			FW1 = new Fireworks(*sysPos, true, 1);
			render1 = true;			
			break;

		case '2':
			if (render2)
				FW2->Clear();

			FW2 = new Fireworks(*sysPos, true, 2);
			render2 = true;
			break;

		case '3':
			if (render3)
				FW3->Clear();

			FW3 = new Fireworks(*sysPos, true, 3);
			render3 = true;
			break;


		case 'V': break;
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
/*
* SymplePartSys, Particle & Firework classes made by Sergio Mares. MIT License on complete repository
* Check this full proyect and an updated version on https://github.com/SergioMares/PhysX_Nvidia 
*/