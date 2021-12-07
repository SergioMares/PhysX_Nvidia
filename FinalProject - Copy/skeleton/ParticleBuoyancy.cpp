#include "ParticleBuoyancy.h"

void ParticleBuoyancy::updateForce(Particle* p, float t)
{
	
	float depth;
	depth = p->getPos().p.y;
	Vector3 f(0.0f, 0.0f, 0.0f);
	if (depth > (waterHeight + maxDepth)) {
		//p->setColor({ 1,0,0,1 });		
		return;
	}
	if (depth < (waterHeight - maxDepth)) {
		//p->setColor({ 1,0,1,1 });
		f.y = liquidDensity * volume;
	}
	else {
		float depthExt = waterHeight + maxDepth;
		float volFactor = (depthExt - depth) / (2 * maxDepth);
		f.y = liquidDensity * volume * volFactor;		
		//p->setColor({ 0.5,0,1,1 });
	}
	p->addForce(f);
}
