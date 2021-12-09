#include "BodySystem.h"

BodySystem::~BodySystem()//bodies<solidbody*> && body rigid shape
{
	for (auto bds : bodies)
	{
		bds->item->shape->release();
		bds->item->release();
		bds->rigid->release();

		delete bds;
		puts("se borro body");
	}
	bodies.clear();
	puts("se limpo bodies");
}

void BodySystem::addBody() {
	
	SolidBody* body = nullptr;
	body = new SolidBody;

	//rigid
	PxRigidDynamic* rigid = gPhysics->createRigidDynamic(pSet);

	//shape
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(1,1,1), *Material);
	rigid->attachShape(*shape);

	//Cinetica
	int partVel = 5;
	int rX = rand() % partVel * 2 - partVel;
	int rY = rand() % partVel * 2 - partVel;
	int rZ = rand() % partVel * 2 - partVel;

	Vector3 vel(rX, rY, rZ);

	rigid->setLinearVelocity(vel);
	rigid->setAngularVelocity({ 0,0,0 });
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
	body->life = 5;
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
		bds->life -= t;

		/*if (bds->life <= 0)
		{
			delete bds;
			bodies
		}*/
	}	

	/*for (auto i = bodies.begin(); i != bodies.end(); ++i)
	{
		if ((*i)->life <= 0)
		{
			delete* i;
			bodies.erase(i);
			break;
		}
	}*/
	
}

void BodySystem::deleteDeads()
{
	for (auto i = bodies.begin(); i != bodies.end(); ++i)
	{
		if ((*i)->life <= 0)
		{
			//(*i)->rigid->release();
			(*i)->item->shape->release();
			(*i)->item->release();
			(*i)->rigid->release();
			
			delete (*i);

			bodies.erase(i);
			break;

			/*item->release();
			plane->release();
			ground->release();*/
		}
	}
}
