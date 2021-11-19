#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "Particle.h"

#include <iostream>
#include "ParticleGravity.h"
#include "ParticleForceRegistry.h"
#include "ParticlesSystem.h"
#include "ParticleWind.h"
#include "ParticleExplosion.h"

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

ParticleGravity			*gravityDown= NULL,
						*gravityUp	= NULL;

ParticleWind			*windRight	= NULL,
						*windLeft	= NULL,
						*windForward= NULL, 
						*windBackward= NULL,
						*windUp		= NULL;

ParticleExplosion		*explosion	= NULL;


Particle				*windVolume1	= NULL,
						*windVolume2	= NULL,
						*windVolume3	= NULL,
						*windVolume4	= NULL,
						*windVolume5	= NULL,
						*exploVolume	= NULL;

ParticleForceRegistry*	regiF		= NULL;
ParticlesSystem*		fountaint	= NULL;

float					actorSize,
						actorMass,
						actorVel,
						fountainRate,
						fountainAmount,
						windRadio,
						explRadio,
						TNT;

Vector3					fountainPos, 
						wVolumePos1, 
						wVolumePos2, 
						wVolumePos3, 
						wVolumePos4,
						wVolumePos5,
						exVolumePos;

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
		//actorPos(30, 40, 40);		
		fountainPos(0, 0, 0);

	wVolumePos1 = Vector3(0, 25, 0);
	wVolumePos2 = Vector3(0, 50, 0);
	wVolumePos3 = Vector3(0, 75, 0);
	wVolumePos4 = Vector3(0, 100, 0);
	wVolumePos5 = Vector3(0, 0, 0);

	exVolumePos = Vector3(0, 20, 0);

	actorVel = 30;
	actorSize = 1;	
	actorMass = 10;	
	fountainRate = 0.01;
	fountainAmount = 1000;
	windRadio = 20;
	explRadio = 20;
	TNT = 10000;

	fountaint	= new ParticlesSystem(fountainPos, actorVel, actorSize, actorMass, fountainRate, fountainAmount);

	gravityDown	= new ParticleGravity(Vector3(0, -10, 0));
	gravityUp	= new ParticleGravity(Vector3(0, 10, 0));

	windRight		= new ParticleWind(Vector3(2000, 0, 0), windRadio, wVolumePos1);
	windLeft		= new ParticleWind(Vector3(-2000, 0, 0), windRadio, wVolumePos2);
	windForward		= new ParticleWind(Vector3(0, 0, 2000), windRadio, wVolumePos3);
	windBackward	= new ParticleWind(Vector3(0, 0, -2000), windRadio, wVolumePos4);
	windUp			= new ParticleWind(Vector3(0, 4000, 0), windRadio, wVolumePos5);	

	explosion = new ParticleExplosion(TNT, explRadio, exVolumePos);

	regiF		= new ParticleForceRegistry();	

	
				
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

	fountaint->UpdateSys(t);	
	if (regiF->countRegisters() > 0)
		regiF->updateForces(t);	
	//gravG->updateForce(Actor, t);
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
	//Actor->~Particle();
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
	case 'P': 
		for (auto a : fountaint->getActors())
			regiF->add(a, windRight);
		windVolume1 = new Particle(wVolumePos1, Vector3(0), windRadio, 0, 0);
		break;
	case 'O':
		for (auto a : fountaint->getActors())
			regiF->add(a, windLeft);
		windVolume2 = new Particle(wVolumePos2, Vector3(0), windRadio, 0, 0);
		break;
	case 'I':
		for (auto a : fountaint->getActors())
			regiF->add(a, windForward);
		windVolume3 = new Particle(wVolumePos3, Vector3(0), windRadio, 0, 0);
		break;
	case 'U':
		for (auto a : fountaint->getActors())
			regiF->add(a, windBackward);
		windVolume4 = new Particle(wVolumePos4, Vector3(0), windRadio, 0, 0);
		break;
	case 'Y':
		for (auto a : fountaint->getActors())
			regiF->add(a, windUp);
		windVolume5 = new Particle(wVolumePos5, Vector3(0), windRadio, 0, 0);
		break;
	case 'E':	
		for (auto a : fountaint->getActors())
			regiF->add(a, explosion);		
		exploVolume = new Particle(exVolumePos, Vector3(0), explRadio, 0, 0);
		break;
	case 'C': 
		regiF->clear();
		break;
	case '8': //8 is up in numbers keyboard
		for (auto a : fountaint->getActors())
			regiF->add(a, gravityUp);
		break;
	case '2': //2 is down in numbers keyboard
		for (auto a : fountaint->getActors())
			regiF->add(a, gravityDown);
		break;
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
* NEW IDEAS
* magnets and metal materials
* planets gravity
* 
* DOUBTS
* why inverse mass? and uses 
* italy hehehe 
* 
* corregir masa y fuerza
*/