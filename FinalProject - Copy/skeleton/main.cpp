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
#include "BodySpring.h"



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

//BodySystem*				bodySys		= NULL;
//BodyWind*				windUp		= NULL;
ParticleGravity*		gravUp		= NULL;
ParticleGravity*		gravDown	= NULL;
ParticlesSystem*		fountain	= NULL;

Particle				*Particle_1 = NULL,
						*Particle_2 = NULL,
						*Particle_3 = NULL,
						*Particle_4 = NULL,
						*Particle_5 = NULL;

ParticleForceRegistry*	regiF		= NULL;
ParticleBuoyancy*		Buoyancy	= NULL;
BodySpring				*bridgeSpr1	= NULL,
						*bridgeSpr2	= NULL,
						*bridgeSpr3	= NULL,
						*bridgeSpr4	= NULL,
						*bridgeSpr5	= NULL,
						*bridgeSpr6	= NULL;


PxShape					*plane		= NULL,
						*bridgeSh1  = NULL,
						*baseSh		= NULL;


PxRigidStatic			*ground		= NULL,
						*baseRd1	= NULL,
						*baseRd2	= NULL;

PxRigidDynamic			*bridgeRg1	= NULL,
						*bridgeRg2	= NULL,
						*bridgeRg3	= NULL,
						*bridgeRg4	= NULL,
						*bridgeRg5	= NULL;

RenderItem				*item		= NULL,
						*bridgeItm1 = NULL,
						*bridgeItm2 = NULL,
						*bridgeItm3 = NULL,
						*bridgeItm4 = NULL,
						*bridgeItm5 = NULL,
						*baseItm1	= NULL,
						*baseItm2	= NULL,
						*testItm	= NULL;

PxReal chainZ = 10.0f;

PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}



// revolute joint limited to an angle of at most pi/4 radians (45 degrees)

// fixed, breakable joint
PxJoint* createBreakableFixed(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setBreakForce(1000, 100000);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	return j;
}

// D6 joint with a spring maintaining its position
PxJoint* createDampedD6(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxD6Joint* j = PxD6JointCreate(*gPhysics, a0, t0, a1, t1);
	j->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	j->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	j->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(0, 1000, FLT_MAX, true));
	return j;
}

// spherical joint limited to an angle of at most pi/4 radians (45 degrees)
PxJoint* createLimitedSpherical(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxSphericalJoint* j = PxSphericalJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setLimitCone(PxJointLimitCone(PxPi / 4, PxPi / 4, 0.05f));
	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	return j;
}

typedef PxJoint* (*JointCreateFunction)(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);

// create a chain rooted at the origin and extending along the x-axis, all transformed by the argument t.

void createChain(const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, JointCreateFunction createJoint)
{
	PxVec3 offset(separation / 2, 0, 0);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;

	for (PxU32 i = 0; i < length; i++)
	{	
		PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t * localTm, g, *gMaterial, 1.0f);
		if (i == 0)
		{
			puts("masa cero");
			current->setMass(0);
			current->setMassSpaceInertiaTensor(Vector3(0));
			current->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		}
		(*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
		gScene->addActor(*current);
		testItm = new RenderItem(CreateShape(g), current, { 0.5,0.5,0.5,1 });
		prev = current;
		localTm.p.x += separation;

		if (i == length-1)
		{
			PxRigidStatic* current = PxCreateStatic(*gPhysics, t * localTm, g, *gMaterial);
			(*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
			gScene->addActor(*current);
			testItm = new RenderItem(CreateShape(g), current, { 0.5,0.5,0.5,1 });
		}
	}
}

float	cubeSize;

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

	createChain(PxTransform(PxVec3(-10.0f, 0.0f, -10.0f)), 6, PxBoxGeometry(2.0f, 0.5f, 0.5f), 6.0f, createLimitedSpherical);
	//createChain(PxTransform(PxVec3(20.0f, 0.0f, -10.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, createBreakableFixed);
	//createChain(PxTransform(PxVec3(30.0f, 0.0f, -20.0f)), 5, PxBoxGeometry(2.0f, 0.5f, 0.5f), 4.0f, createDampedD6);


	cubeSize = 50;
	
	//ground
	/*plane = CreateShape(PxBoxGeometry(cubeSize, cubeSize, cubeSize));
	ground = gPhysics->createRigidStatic({ 0,-cubeSize,0 });
	ground->attachShape(*plane);
	gScene->addActor(*ground);
	item = new RenderItem(plane, ground, { 0,1,1,0 });*/

	/*bridgeSh1 = CreateShape(PxBoxGeometry(5, 0.5, 1.5));
	bridgeRg1 = gPhysics->createRigidDynamic({ 25,10,8 });
	bridgeRg1->attachShape(*bridgeSh1);
	gScene->addActor(*bridgeRg1);
	bridgeItm1 = new RenderItem(bridgeSh1, bridgeRg1, { 0.5,0.5,0.5,1 });

	bridgeRg2 = gPhysics->createRigidDynamic({ 25,10,4 });
	bridgeRg2->attachShape(*bridgeSh1);
	gScene->addActor(*bridgeRg2);
	bridgeItm2 = new RenderItem(bridgeSh1, bridgeRg2, { 0.5,0.5,0.5,1 });

	bridgeRg3 = gPhysics->createRigidDynamic({ 25,10,0 });
	bridgeRg3->attachShape(*bridgeSh1);
	gScene->addActor(*bridgeRg3);
	bridgeItm3 = new RenderItem(bridgeSh1, bridgeRg3, { 0.5,0.5,0.5,1 });

	bridgeRg4 = gPhysics->createRigidDynamic({ 25,10,-4 });
	bridgeRg4->attachShape(*bridgeSh1);
	gScene->addActor(*bridgeRg4);
	bridgeItm4 = new RenderItem(bridgeSh1, bridgeRg4, { 0.5,0.5,0.5,1 });

	bridgeRg5 = gPhysics->createRigidDynamic({ 25,10,-8 });
	bridgeRg5->attachShape(*bridgeSh1);
	gScene->addActor(*bridgeRg5);
	bridgeItm4 = new RenderItem(bridgeSh1, bridgeRg5, { 0.5,0.5,0.5,1 });

	baseSh = CreateShape(PxBoxGeometry(5, 5, 1));
	baseRd1 = gPhysics->createRigidStatic({ 25,30,-12 });
	baseRd1->attachShape(*baseSh);
	gScene->addActor(*baseRd1);
	baseItm1 = new RenderItem(baseSh, baseRd1, { 0.5,0.5,0.5,1 });

	baseRd2 = gPhysics->createRigidStatic({ 25,5,12 });
	baseRd2->attachShape(*baseSh);
	gScene->addActor(*baseRd2);
	baseItm1 = new RenderItem(baseSh, baseRd2, { 0.5,0.5,0.5,1 });

	PxDistanceJointCreate(*gPhysics, 
		baseRd1, bridgeRg5->getGlobalPose(),
		bridgeRg5, baseRd1->getGlobalPose());*/

	

	/*PxDistanceJoint *joint1 = PxDistanceJointCreate(*gPhysics, 
		baseRd1, baseRd1->getGlobalPose(), 
		bridgeRg5, bridgeRg5->getGlobalPose());*/

	//joint1->setMaxDistance(1.0f);
	//joint->setDistanceJointFlag(physx::PxDistanceJointFlag::Enum::eMAX_DISTANCE_ENABLED, true);	

	/*PxRevoluteJointCreate(*gPhysics, 
		baseRd1, baseRd1->getGlobalPose(), 
		bridgeRg5, bridgeRg5->getGlobalPose());*/


	//solid rigid and particles system
	//bodySys = new BodySystem(gPhysics, gScene, { 0,40,0 });
	fountain = new ParticlesSystem({ 0,-1,0 }, {cubeSize,1,cubeSize}, 0.05, 1, 0.5, 100);

	Particle_1 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_2 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_3 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_4 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);
	Particle_5 = new Particle({ float((rand() % 100) - 50), 10, float((rand() % 20) - 10) }, Vector3(0), 1, rand() % 10, 1, 0.5);

	Particle_1->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_2->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_3->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_4->setNewColor({ 0.4,0.6,0.1,1 });
	Particle_5->setNewColor({ 0.4,0.6,0.1,1 });

	//forces 
	//windUp = new BodyWind({ 100.f,0.f,0.f }, 30, { 0,0,0 });
	gravUp	 = new ParticleGravity({ 0,1,0 });
	gravDown = new ParticleGravity({ 0,-10,0 });
	
	Buoyancy = new ParticleBuoyancy(6, 0.1, -5, 1000.0);

	//registry
	regiF = new ParticleForceRegistry();
	for (auto prt : fountain->getActors())
		regiF->add(prt, gravUp);
	
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
	//bodySys->integrate(t);
	regiF->updateForces(t);
	/*for (auto bds : bodySys->bodies)
		regiF->addB(bds, windUp);*/
	//bodySys->deleteDeads();//aguas con esto

	Particle_1->Update(t);
	Particle_2->Update(t);
	Particle_3->Update(t);
	Particle_4->Update(t);
	Particle_5->Update(t);	
}

// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{//bodysys windup regif
	//clean ground
	item->release();
	plane->release();
	ground->release();

	//delete bodySys;//listo
	//delete windUp;
	delete regiF;
	delete fountain;
	delete gravUp;

	delete Particle_1;
	delete Particle_2;
	delete Particle_3;
	delete Particle_4;
	delete Particle_5;


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
- partículas con distinta masa

- un ejemplo de muelle (con rígidos pls)
- un sistema de sólidos rígidos
- sólidos rígidos con distinto tamaño, masa y momento de inercia

- 2 generadores de fuerzas diferentes (sin contar muelles), cada uno con su fórmula y sus restricciones de aplicación
- activación de efectos por teclado


- fuegos artificiales
- gestión de creación y destrucción de instancias (las partículas no pueden estar indefinidamente en escena, tampoco pueden estar desde el principio, ha de existir un mecanismo que cree las partículas)
- destrucción de todos los elementos al salir de la escena


nenúfares en el agua. Ponles masa diferente
a cada uno para cumplir con otro punto

puente para muelle. De ser posible, con
sólidos rígidos para que se mueva cuando
le cae un objeto. Aquí ya iría el sistema
de sólidos rígidos. Añade a este sistema
masa, tamaño y momentos de inercia
aleatorios para cumplir otro punto

1 generadores de fuerza por definir.
Puedes hacer una fuente y te quitas
de broncas (viento y gravedad)
Una idea puede ser lo del espacio. Esto está
cool porque puedes manejar distintas
gravedades y quedaría mamalón. A parte de
que sería un completo extra bb

ojo para esto de arriba contempla usar un
gestor de escenas

usa los fuegos artificiales que ya tienes y
si puedes, corrige el sistema
*/