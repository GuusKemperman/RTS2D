#include "precomp.h"
#include "HugeTank.h"
#include "SpriteComponent.h"
#include "TurretComponent.h"
#include "SelectingComponent.h"
#include "Army.h"


HugeTank::HugeTank(Scene& scene, Army& belongsTo) : 
	Unit(scene, belongsTo, "hugetank")
{

}

HugeTank::~HugeTank()
{
}