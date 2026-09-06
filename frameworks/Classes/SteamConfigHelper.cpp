#include "SteamConfigHelper.hpp"

lstg::SteamConfigHelper* lstg::SteamConfigHelper::getInstance()
{
	static SteamConfigHelper instance;
	return &instance;
}

std::string lstg::SteamConfigHelper::getSteamLanguage()
{
	return std::string(SteamApps()->GetCurrentGameLanguage());
}
