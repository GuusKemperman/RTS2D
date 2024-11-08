#pragma once

namespace Data
{
	class Location;
	class Variable;
}

/// <summary>
/// The file location where the Data is stored.
/// </summary>
constexpr const char* fileName = "Data/playerdata.txt";

/// <summary>
/// The name of the main location, all locations and variables without a parent assigned will, by default, be assigned as a child of the main location.
/// </summary>
constexpr const char* mainLocationName = "playerdata";

/// <summary>
/// A class intended to help you save and load settings stored in a human-readable way, sectioned of like namespaces. Loading the Data happens the first time this function is called, 
/// after which the Data will be stored on the heap and is quick to acces. Any changes to that Data will not be saved unless the SaveAll() function is called.
/// </summary>
class PlayerData
{
public:
	/// <param name="focusedLocationName:">The name of the focusedLocation that this class will use. See the focusedLocation variable for more details.</param>
	PlayerData(const std::string& focusedLocationName = mainLocationName);

	~PlayerData();

	PlayerData(const PlayerData& pd) = delete;

	/// <summary>
	/// Saves ALL the Data to file, including Data outside of the focused scope.
	/// </summary>
	void Save() const;

	/// <summary>
	/// Loads ALL the Data from file, and updates each instance of this class.
	/// </summary>
	void Load();

	/// <summary>
	/// Returns all the variables of the given location.
	/// </summary>
	/// <param name="ofLocation">Searching starts from the focused location; exclude the name of the focused location.
	/// Will throw an out_of_range exception if the location is not found.</param>
	std::list<Data::Variable*> GetVariables(const string& ofLocation = "");
	/// <summary>
	/// Returns all the children of the given location.
	/// </summary>
	/// <param name="ofLocation">Searching starts from the focused location; exclude the name of the focused location.
	/// Will throw an out_of_range exception if the location is not found.</param>
	std::list<Data::Location*> GetChildren(const string& ofLocation = "");

	/// <summary>
	/// Returns the given location.
	/// </summary>
	/// <param name="ofLocation">Searching starts from the focused location; exclude the name of the focused location.
	/// Will throw an out_of_range exception if the location is not found.</param>
	Data::Location* FindLocation(const string& path);

	Data::Variable* FindVariable(const string& path);
	std::string GetVariableValue(const string& path);

private:
	friend class PlayerData;

	/// <summary>
	/// After loading/reloading the Data, this function will reset the focusedLocation of this instance.
	/// </summary>
	void UpdateFocusedLocation();

	/// <summary>
	/// Every instance of playerdata will only be able to find and access the Data on their focused location when looking for variables.
	/// An inputmanager for example could be focused on "playerdata/input", and will only be able to access the variables of that location and it's children.
	/// </summary>
	Data::Location* focusedLocation = nullptr;

	/// <summary>
	/// Should always look like: mainLocationName/restOfPath
	/// </summary>
	std::string focusedLocationName;
};

