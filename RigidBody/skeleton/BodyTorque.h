#pragma once
#include "BodyForceGenerator.h"
class BodyTorque : public BodyForceGenerator
{
	Vector3 torque;
public:
	BodyTorque(const Vector3 _torque) :
		torque(_torque) {}

	virtual void updateForce(SolidBody* body, float t);
};

