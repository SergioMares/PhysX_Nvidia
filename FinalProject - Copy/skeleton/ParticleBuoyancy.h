#pragma once
#include "ParticleForceGenerator.h"
class ParticleBuoyancy :
    public ParticleForceGenerator
{
    float maxDepth;

    float volume;

    //consifering a water plane parallel to XZ
    float waterHeight;

    //Density of the liquid the object is suberged in
    float liquidDensity;

public:
    ParticleBuoyancy(float _maxDepth, float _volume, float _waterHeight, float _liquidDensity = 1000.0f) 
        : maxDepth(_maxDepth), volume(_volume), waterHeight(_waterHeight), liquidDensity(_liquidDensity) {};

    virtual void updateForce(Particle* particle, float t);
};

