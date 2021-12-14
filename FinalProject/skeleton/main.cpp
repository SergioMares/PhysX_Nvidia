#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include "Particle.h"
#include "ParticleWind.h"

#include <iostream>
#include "ParticleGravity.h"
#include "ParticleForceRegistry.h"
#include "ParticleSpring.h"
#include "ParticleAnchoredSpring.h"
#include "ParticleBuoyancy.h"
#include "BodySystem.h"
#include "BodyWind.h"
#include "ParticlesSystem.h"
#include "BodySpring.h"
#include "Fireworks.h"

using namespace physx;
using namespace std;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;


PxMaterial*				gMaterial	= NULL,
						*iceMaterial= NULL;

PxPvd*                  gPvd        = NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene      = NULL;
ContactReportCallback	gContactReportCallback;

#pragma region Pointers and classes used

ParticleGravity*		gravUp		= NULL;
ParticleGravity*		gravDown	= NULL;
ParticleWind*			windUp		= NULL;

ParticlesSystem			*fountain	= NULL,
						*fountain2	= NULL;

Particle				*Particle_1 = NULL,
						*Particle_2 = NULL,
						*Particle_3 = NULL,
						*Particle_4 = NULL,
						*Particle_5 = NULL,
						*Particle_6 = NULL;

ParticleForceRegistry*	regiF		= NULL;
ParticleBuoyancy*		Buoyancy	= NULL;

PxShape					*plane		= NULL,
						*landSh		= NULL,
						*snowSh		= NULL;


PxRigidStatic			*landRS1	= NULL,
						*landRS2	= NULL,
						*snowRS		= NULL;

RenderItem				*item		= NULL,
						*testItm	= NULL,
						*landItm1	= NULL,
						*landItm2	= NULL,
						*snowItm	= NULL,
						*skyBox		= NULL;

BodySystem*				bodySys		= NULL;

Fireworks				*FW1		= NULL;


bool fwFlag1 = false;

vector<PxRigidDynamic*> spheres;
vector<RenderItem*> sphereItems;

PxReal density = 25;
#pragma endregion

#pragma region joints
// spherical joint limited to an angle of at most pi/4 radians (45 degrees)
PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxSphericalJoint* j = PxSphericalJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f));
	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	j->setBreakForce(100000.0f, 100000.0f);
	return j;
}

typedef PxJoint* (*JointCreateFunction)(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);

// create a chain rooted at the origin and extending along the x-axis, all transformed by the argument t.
void createChain(const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, JointCreateFunction createJoint)
{
	PxVec3 offset(0, 0, separation / 2);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;

	for (PxU32 i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t * localTm, g, *gMaterial, 1.0f);
		current->setLinearDamping(4);
		
		if (i == 0)
		{
			puts("masa cero");
			current->setMass(0);
			current->setMassSpaceInertiaTensor(Vector3(0));
			current->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		}
		(*createJoint)(prev, prev ? PxTransform(offset + PxVec3(2, 0, 0)) : t, current, PxTransform(-offset + PxVec3(2, 0, 0)));
		(*createJoint)(prev, prev ? PxTransform(offset + PxVec3(-2, 0, 0)) : t, current, PxTransform(-offset + PxVec3(-2, 0, 0)));
		gScene->addActor(*current);
		testItm = new RenderItem(CreateShape(g), current, { 0.5,0.5,0.5,1 });
		prev = current;
		localTm.p.z += separation;

		if (i == length - 1)
		{
			PxRigidStatic* current = PxCreateStatic(*gPhysics, t * localTm, g, *gMaterial);
			(*createJoint)(prev, prev ? PxTransform(offset + PxVec3(2, 0, 0)) : t, current, PxTransform(-offset + PxVec3(2, 0, 0)));
			(*createJoint)(prev, prev ? PxTransform(offset + PxVec3(-2, 0, 0)) : t, current, PxTransform(-offset + PxVec3(-2, 0, 0)));
			gScene->addActor(*current);
			testItm = new RenderItem(CreateShape(g), current, { 0.5,0.5,0.5,1 });
		}
	}
}
#pragma endregion

float	cubeSize;

// Initialize physics engine
void initPhysics(bool interactive)
{
	puts("use [C] to create a rigid body above the bridge");
	puts("you can use [+/-] to increase or decrease the density of the next rigid body to create");
	puts("with [1] yo can fire some fireworks");
	puts("you can navigate through the scene with [A/S/D/W] and moving the coursor");

	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	gMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);//static, dynamic, bounce
	iceMaterial = gPhysics->createMaterial(0, 0, 0);

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

	cubeSize = 50;
	
	PxTransform* posss = new PxTransform(0, -cubeSize, 0); 
	plane = CreateShape(PxBoxGeometry(cubeSize, cubeSize, cubeSize));
	item = new RenderItem(plane, posss, { 0,1,1,0 });
	
	skyBox = new RenderItem(CreateShape(PxBoxGeometry(cubeSize * 10, cubeSize * 10, cubeSize * 10)), posss, { 0,0,0.3,0 });


	//solid rigid and particles system
	fountain = new ParticlesSystem({ 0,-1,0 }, {cubeSize*2,1,cubeSize*2}, 0.05, 1, 0.5, 100,0.1);
	fountain2 = new ParticlesSystem({ 30, 1, -30 }, Vector3(3), 0.1, 1, 0.1, 100,0.9);

	Particle_1 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_2 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_3 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_4 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_5 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_6 = new Particle({ 30,1,-30 }, Vector3(0), 1, rand() % 10, 1, 0.5);

	Particle_1->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_2->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_3->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_4->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_5->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_6->setNewColor({ 0.9,0.9,0.9,1 });

	//forces 
	windUp = new ParticleWind({ 0.f,800.f,0.f }, 2, { 30,0,-30 });
	gravUp	 = new ParticleGravity({ 0,1,0 });
	gravDown = new ParticleGravity({ 0,-10,0 });
	
	Buoyancy = new ParticleBuoyancy(6, 0.1, -5, 1000.0);

	//registry
	regiF = new ParticleForceRegistry();
	for (auto prt : fountain->getActors())
		regiF->add(prt, gravUp);
	for (auto prt : fountain2->getActors())
	{
		regiF->add(prt, gravDown);
		regiF->add(prt, windUp);
		prt->setNewColor(Vector4(0, 1, 1, 1));
	}

	regiF->add(Particle_1, gravDown);
	regiF->add(Particle_2, gravDown);
	regiF->add(Particle_3, gravDown);
	regiF->add(Particle_4, gravDown);
	regiF->add(Particle_5, gravDown);

	regiF->add(Particle_1, Buoyancy);
	regiF->add(Particle_2, Buoyancy);
	regiF->add(Particle_3, Buoyancy);
	regiF->add(Particle_4, Buoyancy);
	regiF->add(Particle_5, Buoyancy);

	createChain(PxTransform(PxVec3(30.0f, 2.0f, -10.0f)), 10, PxBoxGeometry(2.0f, 0.5f, 0.5f), 2.0f, createLimitedSpherical);	

	landSh = CreateShape(PxBoxGeometry(cubeSize, 1, cubeSize/2));
	landRS1 = gPhysics->createRigidStatic({ 0,0,35 });
	landRS1->attachShape(*landSh);
	gScene->addActor(*landRS1);
	landItm1 = new RenderItem(landSh, landRS1, { 0.6,0.4,0.3,1 });
		
	landRS2 = gPhysics->createRigidStatic({ 0,0,-33 });
	landRS2->attachShape(*landSh);
	gScene->addActor(*landRS2);
	landItm2 = new RenderItem(landSh, landRS2, { 0.6,0.4,0.3,1 });

	bodySys = new BodySystem(gPhysics, gScene, { -35,10,35 }, iceMaterial);

	//snowSh = CreateShape(PxBoxGeometry(cubeSize / 4, 0.5, cubeSize / 4)); 
	snowSh = gPhysics->createShape(PxBoxGeometry(cubeSize / 4, 0.5, cubeSize / 4), *iceMaterial);
	snowRS = PxCreateStatic(*gPhysics, PxTransform(Vector3(-35,1,35)), PxBoxGeometry(cubeSize / 4, 0.5, cubeSize / 4), *iceMaterial);
	gScene->addActor(*snowRS);
	snowItm = new RenderItem(snowSh, snowRS, { 0.9,0.9,0.9,1 });
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
	fountain2->UpdateSys(t);
	bodySys->integrate(t);
	regiF->updateForces(t);
	bodySys->deleteDeads();//aguas con esto
	Particle_1->Update(t);
	Particle_2->Update(t);
	Particle_3->Update(t);
	Particle_4->Update(t);
	Particle_5->Update(t);	

	if (fwFlag1)
		FW1->UpdateSys(t);
}

// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{//bodysys windup regif
	//clean RG and SH

#pragma region primal instances delete
	item->release();
	plane->release();

	landSh->release();		
	snowSh->release();

	testItm->release();
	landItm1->release();
	landItm2->release();
	snowItm->release();
	skyBox->release();


	delete regiF;
	delete bodySys;
	delete gravUp;
	delete gravDown;
	delete Buoyancy;
	delete windUp;

	delete fountain;
	delete fountain2;
	delete FW1;

	delete Particle_1;
	delete Particle_2;
	delete Particle_3;
	delete Particle_4;
	delete Particle_5;
	delete Particle_6;

	for (auto a : spheres)
		a->release();
	
	for (auto a : sphereItems)
		a->release();
#pragma endregion
	
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
	case 'C':
	{//*gPhysics, t * localTm, g, *gMaterial, 1.0f PxTransform(PxVec3(30.0f, 2.0f, -10.0f))
		PxRigidDynamic* sphere = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(30.0f, 20.0f, 0.0f)), PxSphereGeometry(1), *gMaterial, density);
		gScene->addActor(*sphere);

		sphereItems.push_back(new RenderItem(CreateShape(PxSphereGeometry(1)), sphere, { 0.5,0.5,0.5,1 }));
		spheres.push_back(sphere);
		break;
	}
	case 'U': break;
	case 'J': break;
	case 'G': break;
	case '+': 
		density++;
		cout << "actual density: " << density << endl;
		break;
	case '-': 
		density--;
		cout << "actual density: " << density << endl;
		break;
	case '1': 
		FW1 = new Fireworks({100,-10,100}, true, 1);
		fwFlag1 = true;
		break;
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