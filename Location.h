#pragma once
#include <list>
#include <string>
#include <fstream>

namespace Data
{
	class Variable;
	/// <summary>
	/// A container for the variables, stored with the following syntax:
	/// "name" {
	/// var1 = null
	/// }
	/// </summary>
	class Location
	{
	public:
		Location(std::ifstream& dataFile, const string& name, Location* parent = nullptr);
		~Location();

		Location* FindLocation(const string& path);
		Variable* FindVariable(const string& path);

		/// <summary>
		/// Saves the Data in memory to file. 
		/// </summary>
		/// <param name="toFile"></param>
		/// <param name="numOfIndentations"></param>
		void Save(std::ofstream& toFile, uint8_t numOfIndentations = 0) const;

		const std::string name;
		const Location* parent;
		std::list<Location*> children;
		std::list<Variable*> variables;
	};
}

