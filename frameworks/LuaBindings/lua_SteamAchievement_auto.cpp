#include "lua_SteamAchievement_auto.hpp"
#include "SteamAchievementHelper.hpp"

int lua_lstg_SteamHelper_getInstance(lua_State* lua_S)
{
    LUA_SINVOKE_HEADER("lstg.SteamHelper", "lstg.SteamHelper:getInstance");
    LUA_TRY_INVOKE_R(0, &lstg::SteamAchievementHelper::getInstance);
    LUA_SINVOKE_FOOTER("0");
}

int lua_lstg_SteamHelper_getSteamAchievementList(lua_State* lua_S)
{
    LUA_INVOKE_HEADER("lstg.SteamHelper", "lstg.SteamHelper:getSteamAchievementList");
    LUA_TRY_INVOKE_R(0, &lstg::SteamAchievementHelper::getSteamAchievementList);
    LUA_INVOKE_FOOTER("0");
}

int lua_lstg_SteamHelper_unlockAchievement(lua_State* lua_S)
{
    LUA_INVOKE_HEADER("lstg.SteamHelper", "lstg.SteamHelper:unlockAchievement");
    LUA_TRY_INVOKE(1, &lstg::SteamAchievementHelper::unlockAchievement);
    LUA_INVOKE_FOOTER("0");
}

int lua_lstg_SteamHelper_resetAchievement(lua_State* lua_S)
{
    LUA_INVOKE_HEADER("lstg.SteamHelper", "lstg.SteamHelper:resetAchievement");
    LUA_TRY_INVOKE(1, &lstg::SteamAchievementHelper::resetAchievement);
    LUA_INVOKE_FOOTER("0");
}

int lua_lstg_SteamHelper_getAchievementStatus(lua_State* lua_S)
{
    LUA_INVOKE_HEADER("lstg.SteamHelper", "lstg.SteamHelper:resetAchievement");
    LUA_TRY_INVOKE_R(1, &lstg::SteamAchievementHelper::resetAchievement);
    LUA_INVOKE_FOOTER("0");
}

int luaReg_SteamAchievement_lstgSteamAchievement(lua_State* lua_S)
{
    LUA_ENTRY("lstg");
    LUA_CLS_DEF_BASE(lstg::SteamAchievementHelper, "SteamHelper", "lstg.SteamHelper",
        cocos2d::Ref, "cc.Ref",
        nullptr, nullptr);
    LUA_METHOD("getInstance", lua_lstg_SteamHelper_getInstance);
	LUA_METHOD("getSteamAchievementList", lua_lstg_SteamHelper_getSteamAchievementList);
	LUA_METHOD("unlockAchievement", lua_lstg_SteamHelper_unlockAchievement);
	LUA_METHOD("resetAchievement", lua_lstg_SteamHelper_resetAchievement);
	LUA_METHOD("getAchievementStatus", lua_lstg_SteamHelper_getAchievementStatus);
    LUA_CLS_END();
    LUA_ENTRY_END(1);
    return 0;
}
