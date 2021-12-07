#include "BodySpring.h"

void BodySpring::updateForce(SolidBody* body, double t)
{
	if (body->rigid->getMass() == 0) return;

	//calculate disctance vector
	Vector3 f = body->rigid->getGlobalPose().p;
	f -= other->rigid->getGlobalPose().p;

	//length
	float length = f.normalize();

	//resulting force
	float deltaL = (length - restLength);
	float forceMagnitude = -k * deltaL;
	f *= forceMagnitude;
	body->force += f;
}
