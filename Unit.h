#pragma once
#include "Military.h"
#include "Drawable.h"
#include "DataSuper.h"
#include "NearbyCollider.h"

class Army;
class ColliderComponent;
class Command;
class Investigate;
class Action;

class Unit :
    public Military,
    public Drawable
{
public:
    Unit(Scene& scene, Army& belongsTo, const string& type);
    virtual ~Unit();

    virtual void Tick() override;

    void Draw(Surface* target, const Camera* camera) const override;
    void DebugDraw(Surface* target, const Camera* camera) const;
    
    void CommandMoveTo(const MoveTo& command) override;
    void CommandAttack(const Attack& command) override;
    void CommandInvestigate(const Investigate& command);

    virtual void PerformAction(const Action* action) override;

    void TakeDamage(float amountOfDamage, const Projectile* proj) override;

    enum class State { idle, movement, attacking, investigating, numOfStates };
    inline State GetState() const { return currentState; }

    class Data : 
        public DataSuper<Data>
    {
    private:
        friend DataSuper;
        Data(const string& type);

    public:
        float mass{};
        float maxTurn{};
        float maxForce{};
        float maxSpeed{};
    };

    float2 velocity{};

    Data* unitData{};
    EntityId partOfGroup{};

    static constexpr float desiredSpacing = 25.0f;

private:
    void UpdateMovement();
    void UpdateFrame();

    void TickCurrentState();
    bool AttemptTransition(State toState, bool condition);

    void IdleStateTick();
    void MovementStateTick();
    void AttackStateTick();
    void InvestigateTick();

    [[nodiscard]] float2 Avoidance() const;
    [[nodiscard]] float2 Seek(float2 towardPosition) const;
    [[nodiscard]] float2 FollowPath(list<float2>& path) const;
    [[nodiscard]] float2 Arrival(float2 arriveAt) const;
    [[nodiscard]] float2 Wander() const;

    bool CanMoveTo(float2 position, const ColliderComponent* myCollider, const vector<ColliderComponent*>& possibleCollisions) const;

    void RequestHelpNeighbours(float2 atPosition);
    void AlertNearbyAllies(EntityId ofTarget) const;

    void FormGroupByMyself();

    void ComputeNearbyColliders();
    vector<NearbyCollider> nearbyColliders{};

    unique_ptr<Command> lastReceivedCommand{};
    State currentState = State::idle;

    float2 desiredVel{};
    // Trumps desired velocity, the unit does not move but orientates itself to this rotation instead.
    optional<float> desiredRotation{};

    static constexpr float stateTickTimeStep = 0.1f;
    float timeSinceStateTick = stateTickTimeStep;

    static constexpr double wanderAmplitude = .2;

    static constexpr uint maxTransitionsPerFrame = 25;
    uint transitionsThisFrame{};

    // Multiplied with turret range to determine how closeby the unit should stop.
    static constexpr float attackingApproachTreshold = 0.5f;

    static constexpr float communicationRadius = 500.0f;
    static constexpr float investigationDuration = 30.0f;
    
    float timeInvestigationPosStart{};
    float2 investigatingPos{};
};