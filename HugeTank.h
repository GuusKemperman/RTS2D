#pragma once
#include "Unit.h"

class Army;

class HugeTank :
    public Unit
{
public:
    HugeTank(Scene& scene, Army& belongsTo);
    ~HugeTank();
};
