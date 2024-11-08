#pragma once
#include "Singleton.h"

class AssetManager : 
	public Singleton<AssetManager>
{
public:
	shared_ptr<Surface> GetCopy(const std::string& fileLocation);
	shared_ptr<Surface> GetShared(const std::string& fileLocation);

private:
	auto LoadSurface(const std::string& name);

	unordered_map<string, shared_ptr<Surface>> surfaces;
};

