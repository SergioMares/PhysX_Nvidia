#include "BodySystem.h"

void BodySystem::addBody() {

	SolidBody* body = nullptr;
	body = new SolidBody;

	//rigid
	PxRigidDynamic* rigid = gPhysics->createRigidDynamic(pSet);

	//shape
	PxShape* shape = CreateShape(PxSphereGeometry(1));
	rigid->attachShape(*shape);

	//Cinetica
	int partVel = 10;
	int rX = rand() % partVel * 2 - partVel;
	int rY = rand() % partVel * 2 - partVel;
	int rZ = rand() % partVel * 2 - partVel;

	Vector3 vel(rX, rY, rZ);

	rigid->setLinearVelocity(vel);
	rigid->setAngularVelocity({ 0,2,0 });
	rigid->setLinearDamping(0.0);
	rigid->setAngularDamping(0.05);
	
	//Dinámica
	rigid->setMass(2);
	rigid->setMassSpaceInertiaTensor(PxVec3(0.f, 0.f, 1.f));

	//PxRigidBodyExt::updateMassAndInertia(*rigid, 1);

	gScene->addActor(*rigid);

	//complete body struct
	body->rigid = rigid;
	body->isNew = true; 
	//body->life = life;
	body->force = { 0.0f, 0.0f, 0.0f };
	body->torque = { 0.0f, 0.0f, 0.0f };
	body->item = new RenderItem(shape, rigid, { 0,0,0,1 });

	bodies.push_back(body);
}

void BodySystem::integrate(double t)
{
	timeSpawn += t;
	if (timeSpawn > rateSpawn)
	{
		addBody();
		timeSpawn = 0;
	}

	for (auto bds : bodies)
	{		
		bds->rigid->addForce(bds->force, PxForceMode::eFORCE);
		bds->force = { 0,0,0 };

		bds->rigid->addTorque(bds->torque);
		bds->torque = { 0,0,0 };
		//bds->life -= t;
	}	

	
}
