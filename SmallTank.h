#pragma once
#include "Unit.h"

class Army;

class SmallTank :
    public Unit
{
public:
    SmallTank(Scene& scene, Army& belongsTo);
    ~SmallTank();
};

