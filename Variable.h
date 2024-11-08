#pragma once


namespace Data
{
	class Location;

	/// <summary>
	/// Stored as a string on a single line: "name" = "value".
	/// </summary>
	class Variable
	{
	public:
		Variable(const string& name, const string& value);

		const std::string name;

		/// <summary>
		/// The value stored on file for this variable will be set to this value at the time of saving.
		/// </summary>
		std::string value;
	};
}

