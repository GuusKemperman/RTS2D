#include "precomp.h"
#include "Action.h"
#include "PlayerData.h"

Action::Action(const string& actionType) :
	type(actionType)
{
	PlayerData playerData("actions." + actionType);

	index = stoi(playerData.GetVariableValue("index"));
	frame = stoi(playerData.GetVariableValue("frame"));
}
