#include "precomp.h"
#include "Location.h"
#include "Variable.h"

using namespace std;

namespace Data
{
	Location::Location(ifstream& dataFile, const string& arg_name, Location* arg_parent) : name(arg_name), parent(arg_parent)
	{
		string line;
		while (getline(dataFile, line))
		{
			// If line contains {, consider it the start of a new location.
			if (line.find('{') != string::npos)
			{
				string childName = line;
				childName.erase(std::remove_if(childName.begin(), childName.end(), [](unsigned char x) { return std::isspace(x); }), childName.end());// Remove whitespace
				childName.pop_back(); // Remove '{'
				children.push_back(new Location(dataFile, childName, this));
				continue;
			}

			// If line contains a =, consider it a variable.
			size_t equalSignPos = line.find('=');
			if (equalSignPos != string::npos)
			{
				string varName = line.substr(0, equalSignPos - 1);
				varName.erase(std::remove_if(varName.begin(), varName.end(), [](unsigned char x) { return std::isspace(x); }), varName.end());// Remove whitespace
				string varVal = line.substr(equalSignPos + 2);

				variables.push_back(new Variable(varName, varVal));
				continue;
			}

			// If a line contains }, consider it the end of this location.
			else if (line.find('}') != string::npos)
			{
				return;
			}
		}
	}

	Location::~Location()
	{
		for (Location* child : children)
		{
			delete child;
		}

		for (Variable* var : variables)
		{
			delete var;
		}
	}

	Location* Location::FindLocation(const string& path)
	{
		if (path.empty())
		{
			return this;
		}

		size_t firstPeriod = path.find_first_of('.');
		bool endOfPath = firstPeriod == string::npos;
		string lookingFor = endOfPath ? path : path.substr(0, firstPeriod);

		list<Location*>::iterator child = find_if(children.begin(), children.end(), [lookingFor](const Location* s) { return s->name == lookingFor; });

		if (child == children.end())
		{
			throw out_of_range("This location does not contain " + lookingFor);
		}

		if (endOfPath)
		{
			return *child;
		}
		else
		{
			string newPath = path;
			newPath.erase(0, firstPeriod + 1);
			return (*child)->FindLocation(newPath); // Continue searching with the first part of the path cut off.
		}
	}

	Variable* Location::FindVariable(const string& path)
	{
		size_t lastPeriod = path.find_last_of('.');

		if (lastPeriod == string::npos)
		{
			const list<Variable*>::iterator variable = find_if(variables.begin(), variables.end(), 
				[path](const Variable* v) 
				{ 
					return v->name == path; 
				});

			if (variable == variables.end())
			{
				LOGWARNING("This location does not contain " + path);
				return nullptr;
			}

			return *variable;
		}

		// If this location does not contain the variable, find the location that does.
		string varName = path.substr(lastPeriod + 1, path.size());
		string locationPath = path.substr(0, lastPeriod - 1);

		Location* variableLocation = FindLocation(locationPath);
		return variableLocation->FindVariable(varName);
	}

	void Location::Save(std::ofstream& toFile, uint8_t numOfIndentations) const
	{
		string indentation = "";

		// Do not add location or brackets for the very first location, the first location is just
		// used to group all the parentless locations in the .txt together and should not actually be
		// written to the file.
		if (parent != nullptr)
		{
			for (uint8_t i = 0; i < numOfIndentations; i++)
			{
				indentation += '\t';
			}

			toFile << indentation << name << " {" << endl;
			++numOfIndentations;
		}


		for (Location* child : children)
		{
			child->Save(toFile, numOfIndentations);
		}

		for (Variable* var : variables)
		{
			toFile << indentation << '\t' << var->name << " = " << var->value << endl;
		}

		if (parent != nullptr)
		{
			toFile << indentation << '}' << endl;
		}
	}
}
