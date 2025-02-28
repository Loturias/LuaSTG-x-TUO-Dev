#pragma once
#include "base/ccConfig.h"
#include "LuaBindings.h"

extern int luaReg_SteamAchievement_lstgSteamAchievement(lua_State* lua_S);

inline int luaReg_SteamAchievement(lua_State* lua_S)
{
	luaReg_SteamAchievement_lstgSteamAchievement(lua_S);
	return 0;
}