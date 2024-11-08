#pragma once
#include "Unit.h"

class Chopper :
    public Unit
{
public:
    Chopper(Scene& scene, Army& belongsTo);
    ~Chopper();
};

