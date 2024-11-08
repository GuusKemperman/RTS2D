#include "precomp.h"
#include "PlayerData.h"
#include "Variable.h"
#include "Location.h"

using namespace std;
using namespace Data;

static Location* loadedData = nullptr;

// Needed for updating all the classes after reloading the Data.
static list<PlayerData*> allInstances{};

PlayerData::PlayerData(const std::string& arg_focusedLocationName) : focusedLocationName(arg_focusedLocationName)
{
	allInstances.push_back(this);

	if (loadedData == nullptr)
	{
		Load();
	}
	else
	{
		UpdateFocusedLocation();
	}
}

PlayerData::~PlayerData()
{
	allInstances.remove(this);
}

void PlayerData::Save() const
{
	ofstream dataFile(fileName);

	loadedData->Save(dataFile);

	dataFile.close();
}

void PlayerData::Load()
{
	if (loadedData != nullptr)
	{
		delete loadedData;
	}

	ifstream dataFile(fileName);
	assert(dataFile.is_open());

	loadedData = new Location(dataFile, mainLocationName);

	dataFile.close();

	// After loading the Data, the focused locations of all the instances will be pointing to the wrong Data, which is why all of them need to be updated.
	for (PlayerData* pd : allInstances)
	{
		pd->UpdateFocusedLocation();
	}
}

list<Data::Variable*> PlayerData::GetVariables(const string& ofLocation)
{
	Location* loc = focusedLocation->FindLocation(ofLocation);
	return loc->variables;
}

std::list<Data::Location*> PlayerData::GetChildren(const string& ofLocation)
{
	Location* loc = focusedLocation->FindLocation(ofLocation);
	return loc->children;
}

Data::Location* PlayerData::FindLocation(const string& path)
{
	Location* loc = focusedLocation->FindLocation(path);
	return loc;
}

Data::Variable* PlayerData::FindVariable(const string& path)
{
	return focusedLocation->FindVariable(path);
}

std::string PlayerData::GetVariableValue(const string& path)
{
	return FindVariable(path)->value;
}

void PlayerData::UpdateFocusedLocation()
{
	focusedLocation = focusedLocationName == mainLocationName ? loadedData : loadedData->FindLocation(focusedLocationName);

	if (focusedLocation == nullptr)
	{
		throw std::out_of_range("The focusedLocationName of this instance is invalid.");
	}
}
