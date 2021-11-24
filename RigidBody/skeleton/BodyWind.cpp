#include "BodyWind.h"
#include <iostream>
using namespace std;

void BodyWind::updateForce(SolidBody* body, double t)
{	
	if (isInRange(p, body->rigid->getGlobalPose().p))
		body->force += w;
}

bool BodyWind::isInRange(Vector3 point1, Vector3 point2)
{
	float distance;
	distance = sqrtf(
		powf(point1.x - point2.x, 2) +
		powf(point1.y - point2.y, 2) +
		powf(point1.z - point2.z, 2));

	if (distance < r)
		return true;
	else
		return false;
}
