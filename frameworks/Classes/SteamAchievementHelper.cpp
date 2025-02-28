#include "SteamAchievementHelper.hpp"

lstg::SteamAchievementHelper::SteamAchievementHelper()
{
	// 根据SDK的注释，现在Steam客户端将自动完成成就数据的获取
	// 程序内无需手动调用RequestCurrentStats拉取
}

/// @brief 从Steam客户端拉取成就信息
/// @return 
std::vector<SteamAchievementStatus> lstg::SteamAchievementHelper::getSteamAchievementList()
{
	auto res = std::vector<SteamAchievementStatus>();
	
	for (auto ach_name : AchievementList)
	{
		bool status = false;
		if (SteamUserStats()->GetAchievement(ach_name.c_str(), &status))
		{
			res.push_back({ ach_name,status });
		}
		else
		{
			CC_LOG_ERROR("Achievement %s not exists,Please connect your Boss.",ach_name.c_str());
		}
	}

	return res;
}

void lstg::SteamAchievementHelper::unlockAchievement(std::string ach_name)
{
	// 校验成就是否已经解锁
	if (getAchievementStatus(ach_name))
	{
		CC_LOG_INFO("[Steam] Achievement %s already unlock.Do nothing.",ach_name.c_str());
		return;
	}
	
	if (SteamUserStats()->SetAchievement(ach_name.c_str()))
	{
		//TODO 异步提交直到提交成功
		SteamUserStats()->StoreStats();
	}
	else
		CC_LOG_ERROR("Achievement %s not exists,Please connect your Boss.", ach_name.c_str());
}

void lstg::SteamAchievementHelper::resetAchievement(std::string ach_name)
{
	SteamUserStats()->ClearAchievement(ach_name.c_str());
	//TODO 异步提交直到提交成功
	SteamUserStats()->StoreStats();
}

bool lstg::SteamAchievementHelper::getAchievementStatus(std::string ach_name)
{
	bool status = false;
	
	if (SteamUserStats()->GetAchievement(ach_name.c_str(), &status))
	{
		return status;
	}
	else
	{
		CC_LOG_ERROR("Achievement %s not exists,Please connect your Boss.", ach_name.c_str());
		return false;
	}
}

