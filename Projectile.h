#pragma once
#include "Entity.h"
#include "DataSuper.h"

class Projectile : 
    public Entity
{
public:
    Projectile(Scene& scene, string type);
    ~Projectile();

    void Tick() override;

    void Fire(float2 position, float rotation, ArmyId firedBy, CollisionLayer layer, float startingVel, float turretRange);
    void Hit();

    inline float2 GetOrigin() const { return origin; }

    class Data : 
        public DataSuper<Data>
    {
    private:
        friend DataSuper;
        Data(const string& type);

    public:
        float damage{};
        string explosionType{};
        uint spriteRow{};
    };


private:
    Data* projData{};

    ArmyId firedBy{};
    CollisionLayer layer{};

    float2 origin{};
    float vel{};
    float distanceLeft{};

};