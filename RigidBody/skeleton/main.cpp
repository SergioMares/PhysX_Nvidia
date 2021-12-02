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
#include "ParticleSpring.h"
#include "ParticleAnchoredSpring.h"
#include "ParticleBuoyancy.h"
#include "BodySystem.h"
#include "BodyWind.h"
#include "ParticlesSystem.h"



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
ContactReportCallback	gContactReportCallback;

BodySystem*				bodySys		= NULL;
BodyWind*				windUp		= NULL;
ParticleGravity*		gravUp		= NULL;
ParticlesSystem*		fountain	= NULL;

ParticleForceRegistry*	regiF		= NULL;

PxShape* plane = NULL;
PxRigidStatic* ground = NULL;
RenderItem* item = NULL;

// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	gMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);//static, dynamic, bounce

	//actorPos(30, 40, 40);		
	srand(time(NULL));

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);
	// ------------------------------------------------------

	//ground
	plane = CreateShape(PxBoxGeometry(100, 1, 100));
	ground = gPhysics->createRigidStatic({ 0,0,0 });
	ground->attachShape(*plane);
	gScene->addActor(*ground);
	item = new RenderItem(plane, ground, { 0.6,0.2,1,1 });

	//solid rigid and particles system
	bodySys = new BodySystem(gPhysics, gScene, { 0,40,0 });
	fountain = new ParticlesSystem({ 0,0,0 }, 30, 0.25, 1, 0.1, 100);


	//forces 
	windUp = new BodyWind({ 100.f,0.f,0.f }, 30, { 0,0,0 });
	gravUp = new ParticleGravity({ 5,0,0 });

	//registry
	regiF = new ParticleForceRegistry();
	for (auto prt : fountain->getActors())
		regiF->add(prt, gravUp);
	
	
}


// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{	
	PX_UNUSED(interactive);

	gScene->simulate(t);
	gScene->fetchResults(true);	
	
	fountain->UpdateSys(t);
	bodySys->integrate(t);
	regiF->updateForces(t);
	for (auto bds : bodySys->bodies)
		regiF->addB(bds, windUp);
	//bodySys->deleteDeads();//aguas con esto
}

// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{//bodysys windup regif
	//clean ground
	item->release();
	plane->release();
	ground->release();

	delete bodySys;//listo
	delete windUp;
	delete regiF;
	delete fountain;
	delete gravUp;

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
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
	case 'C': break;
	case 'U': break;
	case 'J': break;
	case 'G': break;
	case '+': break;
	case '-': break;
	case '1': break;
	case '2': break;
	case '3': break;
	case '4': break;
	case ' ': break;	
	default : break;
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
* 
* 
* DOUBTS
* italy hehehe 
* PxRigidBodyExt::updateMassAndInertia(*rigid, 1);
* 
* TO DO
* create a scene manager
*/