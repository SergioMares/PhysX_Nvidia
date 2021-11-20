#include "RBGenerator.h"

RBGenerator::~RBGenerator()
{
	delete item;
}

void RBGenerator::update(double t)
{
	time += t;
	if (time >= rate)
	{
		//create basics
		SolidBody* body = NULL;
		body = new SolidBody;
		
		
		//rigid
		PxRigidDynamic* RigidD = gPhysics->createRigidDynamic({ 0,50,0 });

		//shape
		PxShape* sphere = CreateShape(PxSphereGeometry(1));
		RigidD->attachShape(*sphere);

		//cinetica
		RigidD->setLinearVelocity(Velocity);		
		RigidD->setAngularVelocity(AngularVel);
		RigidD->setLinearDamping(0.0);
		RigidD->setAngularDamping(0.0);
		
		//dinámica
		RigidD->setMass(Mass);
		PxRigidBodyExt::updateMassAndInertia(*RigidD, 1);

		//add to scene
		gScene->addActor(*RigidD);
		
		body->item = new RenderItem(sphere, RigidD, { 0,0,0,1 });
		body->rigidDyn = RigidD;

		RDbodies.push_back(body);

		time = 0;
	}	
}
