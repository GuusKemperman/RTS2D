#pragma once
#include "enums.h"

class Military;

enum class CommandType { null = -1, moveTo, attack, idle, investigate };
enum class AggroLevel { passive, defensive, aggressive };

class Command
{
public:
    Command(CommandType type, AggroLevel aggroLevel);
    virtual ~Command();

    CommandType type;
    AggroLevel aggroLevel;
};

class Idle : public Command
{
public:
    Idle(AggroLevel aggroLevel);
};

class MoveTo : public Command
{
public:
    MoveTo();
    MoveTo(float2 destination, AggroLevel aggroLevel, optional<float> rotationAtEnd);


    float2 destination{};
    optional<float> rotationAtEnd{};
};

class Investigate : public Command
{
public:
    Investigate(float timeInvestigationStart, float2 position);

    float timeInvestigationStart{};
    float2 position;
};

class Attack : public Command
{
public:
    Attack(optional<EntityId> target);
    optional<EntityId> target;
};