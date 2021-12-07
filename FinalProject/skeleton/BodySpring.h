#pragma once
#include "BodyForceGenerator.h"
class BodySpring : public BodyForceGenerator
{
	SolidBody* other;
	float k;
	float restLength;

public:
	BodySpring(SolidBody* _other, float _k, float _restLength) 
		: other(_other), k(_k), restLength(_restLength) {};

	virtual void updateForce(SolidBody* body, double t);
};

