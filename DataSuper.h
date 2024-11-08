#pragma once

template<typename T>
class DataSuper
{
public:
	static T* GetData(const string& type)
	{
		static unordered_map<string, T*> loadedData;

		auto it = loadedData.find(type);
		if (it == loadedData.end())
		{
			return loadedData.insert({ type, new T(type) }).first->second;
		}

		return it->second;
	}
};

