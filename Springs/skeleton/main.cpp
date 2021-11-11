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

ParticleSpring			*spring1	= NULL,
						*spring2	= NULL,

						*bridgeSpr1	= NULL,
						*bridgeSpr2	= NULL,
						*bridgeSpr3	= NULL,
						*bridgeSpr4	= NULL;

ParticleAnchoredSpring	*AnchorSpr = NULL,
						*bridgeAnch1 = NULL,
						*bridgeAnch2 = NULL;

ParticleBuoyancy		*Buoyancy	= NULL;



ParticleForceRegistry	*regiF		= NULL;

Particle				*Particle_1 = NULL,
						*Particle_2 = NULL,
						*Particle_3 = NULL,
						*Particle_4 = NULL,
						*Particle_5 = NULL;

RenderItem				*waterSurface;

Vector3					PartVel,
						Partpos1,
						Partpos2,
						PartPos5;

physx::PxTransform		Surfpos;

float					partSize1,	partSize2,
						partMass1,	PartMass2,
						rstSpring,	kSpring,
						wHeight,	wVolume,
						wDepth;

bool					bP1, bP2, bP3;

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
	bP1 = false;
	bP2 = false;
	bP3 = false;
	
	//actorPos(30, 40, 40);		

	PartVel		= Vector3(0);

	rstSpring	= 5.0; //bridge to 1
	kSpring		= 20.0;//bridge to +50

	wHeight = 0;
	wVolume = 0.1;
	wDepth = 8;
	Surfpos.p = Vector3(30, wHeight, 40);

	waterSurface = new RenderItem(CreateShape(physx::PxBoxGeometry(50, 0.1, 50)), &Surfpos, { 0,1,1,0.1 });

	Partpos1	= Vector3(30, 40, 40);
	partSize1	= 1.0;

	Partpos2	= Vector3(40, 40, 40);
	partSize2	= 1.0;

	PartPos5 = Vector3(70, 40, 40);

	Particle_1 = new Particle(Partpos1, PartVel, partSize1, 0, 1);
	Particle_2 = new Particle(Partpos2, PartVel, partSize2, 0, 1);
	Particle_3 = new Particle(Vector3(0), PartVel, partSize2, 0, 1);
	Particle_4 = new Particle(Vector3(0), PartVel, partSize2, 0, 1);
	Particle_5 = new Particle(Vector3(0), PartVel, partSize2, 0, 1);
	
	gravityDown	= new ParticleGravity(Vector3(0, -10, 0));
	gravityUp	= new ParticleGravity(Vector3(0, 10, 0));

	spring1 = new ParticleSpring(Particle_2, kSpring, rstSpring);
	spring2 = new ParticleSpring(Particle_1, kSpring, rstSpring);

	bridgeSpr1 = new ParticleSpring(Particle_3, kSpring, rstSpring);
	bridgeSpr2 = new ParticleSpring(Particle_2, kSpring, rstSpring);
	bridgeSpr3 = new ParticleSpring(Particle_4, kSpring, rstSpring);
	bridgeSpr4 = new ParticleSpring(Particle_3, kSpring, rstSpring);

	AnchorSpr = new ParticleAnchoredSpring(&Partpos2, kSpring, rstSpring);
	
	bridgeAnch1 = new ParticleAnchoredSpring(&Partpos1, kSpring, rstSpring);
	bridgeAnch2 = new ParticleAnchoredSpring(&PartPos5, kSpring, rstSpring);

	puts("to see the differet examples, press 1, 2, 3 & 4 numbers");
	puts("you can add gravity by pressing the U (gravity up) and J (gravity down) keys");
	puts("press the G key to remove gravity");
	puts("you can also press the '+' and '-' to modify the spring tension");

	Buoyancy = new ParticleBuoyancy(wDepth, wVolume, wHeight);

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

	Particle_1->Update(t);
	Particle_2->Update(t);
	Particle_3->Update(t);
	Particle_4->Update(t);
	Particle_5->Update(t);

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

	delete waterSurface;
	delete Particle_1;
	delete Particle_2;
	delete Particle_3;
	delete Particle_4;
	delete Particle_5;
	delete gravityDown;
	delete gravityUp;
	delete AnchorSpr;
	
	delete Buoyancy;
	delete spring1;
	delete spring2;
	delete regiF;
}

void updateSpring()
{
	if (bP1)
	{
		regiF->remove(Particle_1, AnchorSpr);
		delete AnchorSpr;		
		AnchorSpr = new ParticleAnchoredSpring(&Partpos2, kSpring, rstSpring);
		regiF->add(Particle_1, AnchorSpr);

		/*regiF->remove(Particle_2, AnchorSpr2);
		delete AnchorSpr2;
		AnchorSpr2 = new ParticleAnchoredSpring(&Particle_1->getPos().p, kSpring, rstSpring);
		regiF->add(Particle_2, AnchorSpr2);*/
	}
	else if (bP2)
	{
		regiF->remove(Particle_1, spring1);
		delete spring1;
		spring1 = new ParticleSpring(Particle_2, kSpring, rstSpring);
		regiF->add(Particle_1, spring1);

		regiF->remove(Particle_2, spring2);
		delete spring2;
		spring2 = new ParticleSpring(Particle_1, kSpring, rstSpring);
		regiF->add(Particle_2, spring2);
	}
	else if (bP3)
	{
		regiF->remove(Particle_2, bridgeAnch1);
		regiF->remove(Particle_2, bridgeSpr1);
		regiF->remove(Particle_3, bridgeSpr2);
		regiF->remove(Particle_3, bridgeSpr3);
		regiF->remove(Particle_4, bridgeSpr4);
		regiF->remove(Particle_4, bridgeAnch2);
		
		delete bridgeAnch1;
		delete bridgeAnch2;
		delete bridgeSpr1;
		delete bridgeSpr2;
		delete bridgeSpr3;
		delete bridgeSpr4;		

		bridgeAnch1 = new ParticleAnchoredSpring(&Partpos1, kSpring, rstSpring);
		bridgeAnch2 = new ParticleAnchoredSpring(&PartPos5, kSpring, rstSpring);
		bridgeSpr1 = new ParticleSpring(Particle_3, kSpring, rstSpring);
		bridgeSpr2 = new ParticleSpring(Particle_2, kSpring, rstSpring);
		bridgeSpr3 = new ParticleSpring(Particle_4, kSpring, rstSpring);
		bridgeSpr4 = new ParticleSpring(Particle_3, kSpring, rstSpring);

		regiF->add(Particle_2, bridgeAnch1);
		regiF->add(Particle_2, bridgeSpr1);
		regiF->add(Particle_3, bridgeSpr2);
		regiF->add(Particle_3, bridgeSpr3);
		regiF->add(Particle_4, bridgeSpr4);
		regiF->add(Particle_4, bridgeAnch2);
	}
	

	cout << kSpring << endl;
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
	case 'C': 
		regiF->clear();
		break;
	case 'U': //U stands for Up
		regiF->add(Particle_1, gravityUp);
		regiF->add(Particle_2, gravityUp);
		regiF->add(Particle_3, gravityUp);
		regiF->add(Particle_4, gravityUp);
		regiF->add(Particle_5, gravityUp);
		break;
	case 'J': //gravity down
		regiF->add(Particle_1, gravityDown);
		regiF->add(Particle_2, gravityDown);
		regiF->add(Particle_3, gravityDown);
		regiF->add(Particle_4, gravityDown);
		regiF->add(Particle_5, gravityDown);
		break;
	case 'G':
		regiF->remove(Particle_1, gravityDown);
		regiF->remove(Particle_2, gravityDown);
		regiF->remove(Particle_3, gravityDown);
		regiF->remove(Particle_4, gravityDown);
		regiF->remove(Particle_5, gravityDown);

		regiF->remove(Particle_1, gravityUp);
		regiF->remove(Particle_2, gravityUp);
		regiF->remove(Particle_3, gravityUp);
		regiF->remove(Particle_4, gravityUp);
		regiF->remove(Particle_5, gravityUp);
		break;
	case '+':
	{
		kSpring += 1.0;
		updateSpring();		
		break;
	}
	case '-':
		if (kSpring > 1)
		{
			kSpring -= 1.0;
			updateSpring();
		}
		break;
	case '1':
		bP1 = true;
		bP2 = false;
		bP3 = false;

		regiF->clear();
		
		Particle_1->setPos(Partpos1);
		Particle_1->setVel(PartVel);
		Particle_1->setMass(10);
		Particle_1->setDamp(0.5);

		Particle_2->setPos(Partpos2);
		Particle_2->setVel(PartVel);
		Particle_2->setMass(0);
		//Particle_2->setDamp(0.5);

		regiF->add(Particle_1, AnchorSpr);
		//regiF->add(Particle_2, AnchorSpr2);
		break;

	case '2':
		bP1 = false;
		bP2 = true;
		bP3 = false;

		regiF->clear();
		
		Particle_1->setPos(Partpos1);
		Particle_1->setVel(PartVel);
		Particle_1->setMass(10);
		Particle_1->setDamp(0.5);

		Particle_2->setPos(Partpos2);
		Particle_2->setVel(PartVel);
		Particle_2->setMass(10);
		Particle_2->setDamp(0.5);

		
		regiF->add(Particle_1, spring1);
		regiF->add(Particle_2, spring2);
		break;
	case '3':
		bP1 = false;
		bP2 = false;
		bP3 = false;

		regiF->clear();

		Particle_1->setPos(Partpos1);
		Particle_1->setVel(PartVel);
		Particle_1->setMass(10);
		Particle_1->setDamp(0.5);

		Particle_2->setPos(Partpos2);
		Particle_2->setVel(PartVel);
		Particle_2->setMass(5);
		Particle_2->setDamp(0.5);

		regiF->add(Particle_1, Buoyancy);
		regiF->add(Particle_2, Buoyancy);
		break;
	case '4':
		bP1 = false;
		bP2 = false;
		bP3 = true;

		regiF->clear();

		Particle_1->setPos(Vector3(30, 40, 40));
		Particle_1->setVel(PartVel);
		Particle_1->setMass(0);
		Particle_1->setDamp(0.5);

		Particle_2->setPos(Vector3(40, 40, 40));
		Particle_2->setVel(PartVel);
		Particle_2->setMass(10);
		Particle_2->setDamp(0.5);

		Particle_3->setPos(Vector3(50, 40, 40));
		Particle_3->setVel(PartVel);
		Particle_3->setMass(10);
		Particle_3->setDamp(0.5);

		Particle_4->setPos(Vector3(60, 40, 40));
		Particle_4->setVel(PartVel);
		Particle_4->setMass(10);
		Particle_4->setDamp(0.5);

		Particle_5->setPos(PartPos5);
		Particle_5->setVel(PartVel);
		Particle_5->setMass(0);
		Particle_5->setDamp(0.5);

		regiF->add(Particle_2, bridgeAnch1);
		regiF->add(Particle_2, bridgeSpr1);
		regiF->add(Particle_3, bridgeSpr2);
		regiF->add(Particle_3, bridgeSpr3);
		regiF->add(Particle_4, bridgeSpr4);
		regiF->add(Particle_4, bridgeAnch2);

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
* 
* 
* DOUBTS
* why inverse mass? and uses 
* does springs has to have the same length and k values?
* distance or orientation?
* 
* italy hehehe 
* 
* TO DO
* manage the different springs with booleans and inputs
* create a scene manager
*/