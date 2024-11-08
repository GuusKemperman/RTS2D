#include "precomp.h"
#include "Commands.h"

Command::Command(CommandType a_type, AggroLevel a_aggroLevel) :
	type(a_type),
	aggroLevel(a_aggroLevel)
{
}

Command::~Command()
{
}

Attack::Attack(optional<EntityId> a_target) :
	Command(CommandType::attack, AggroLevel::aggressive),
	target(a_target)
{

}

Investigate::Investigate(float a_timeInvestigationStart, float2 a_position) :
	Command(CommandType::investigate, AggroLevel::aggressive),
	position(a_position),
	timeInvestigationStart(a_timeInvestigationStart)
{

}

Idle::Idle(AggroLevel aggroLevel) :
	Command(CommandType::idle, aggroLevel)
{
}

MoveTo::MoveTo() :
	Command(CommandType::moveTo, AggroLevel::passive)
{
}

MoveTo::MoveTo(float2 a_destination, AggroLevel aggroLevel, optional<float> a_rotationAtEnd) :
	Command(CommandType::moveTo, aggroLevel),
	destination(a_destination),
	rotationAtEnd(a_rotationAtEnd)
{

}
