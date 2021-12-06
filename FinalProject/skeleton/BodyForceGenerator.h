#pragma once
#include "BodySystem.h"
struct SolidBody;

class BodyForceGenerator
{
public:
	virtual void updateForce(SolidBody* body, double t) = 0;
};

