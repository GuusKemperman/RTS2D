#include "precomp.h"
#include "AssetManager.h"

auto AssetManager::LoadSurface(const std::string& name)
{
	std::shared_ptr<Surface> newSurface = std::make_shared<Surface>(static_cast<string>(name).c_str());
	return surfaces.insert({ name, move(newSurface) });;
}

shared_ptr<Surface> AssetManager::GetCopy(const std::string& name)
{
	return move(make_shared<Surface>(name.c_str()));
}

shared_ptr<Surface> AssetManager::GetShared(const std::string& name)
{
	auto it = surfaces.find(name);
	if (it == surfaces.end())
	{
		return LoadSurface(name).first->second;
	}
	return it->second;
}

