#include "BodyWind.h"
#include <iostream>
using namespace std;

void BodyWind::updateForce(SolidBody* body, double t) 
{	
	body->force += w;
}
