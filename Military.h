#pragma once
#include "Entity.h"
#include "DataSuper.h"

class MoveTo;
class Attack;
class Army;
class Projectile;
class Action;

class Military :
    public Entity
{
public:
    Military(Scene& scene, Army& belongsTo, string type);
    virtual ~Military();

    inline const Action* GetAction(uchar actionIndex) const { return actions[actionIndex]; }
    virtual void PerformAction(const Action* action) {};

    virtual void Tick() override;

    virtual void CommandMoveTo(const MoveTo& command) {}
    virtual void CommandAttack(const Attack& command) {}

    virtual void TakeDamage(float amountOfDamage, const Projectile* proj = 0);

    void Destroy();


    const ArmyId GetArmyId() const;

    class Data : 
        public DataSuper<Data>
    {
    private:
        friend DataSuper;
        Data(const string& type);

    public:
        float maxHealth{};

        string explosionType{};
        string spriteType{};
        string turretType{};

        CollisionLayer collisionLayer{};
    };

    inline const Data* GetMilitaryData() const { return militaryData; }

    Army& partOfArmy;

    static constexpr uint numOfActions = 22;
protected:
    array<Action*, numOfActions> actions{};

    Data* militaryData{};
    float health{};

private:

};

