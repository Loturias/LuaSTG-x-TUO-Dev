# 工具 C++ 类实现维护记忆

## 代码范围与惯例

工具类主要位于 `frameworks/Classes`，头文件集中列在 `CMakeSources.cmake` 的 `GAME_HEADER`，实现列在 `GAME_SOURCE`。常见类别包括窗口/显示器（`WindowHelper`）、控制器（`ControllerHelper`）、文件/压缩、流、随机数、性能计时以及 Steam 辅助类。公共类型放在 `lstg` 或全局项目命名空间，接口声明保持轻量，平台和第三方 SDK 细节放到 `.cpp`。

新增工具类时建议遵循：头文件声明最小依赖和所有权；实现文件负责平台宏、日志和第三方调用；需要 Lua 使用时，再增加对应 `lua_*_auto`/manual 文件并在 `LuaBindings.cpp` 注册；最后更新 CMake 清单。

## SteamAchievementHelper 参考实现

文件：`frameworks/Classes/SteamAchievementHelper.hpp/.cpp`。

- `SteamAchievementStatus` 是 `<string, bool>`，全局 `AchievementList` 是待查询成就 ID 列表（当前只有 `ACH_1ST_ENTER_1_0`）。新增成就需同步修改该列表及 Steam 后台配置。
- `getInstance()` 使用函数内静态对象，返回单例指针；构造函数不主动调用 `RequestCurrentStats`，依赖 Steam 客户端自动获取数据。
- `getSteamAchievementList()` 遍历列表并调用 `SteamUserStats()->GetAchievement`；失败写错误日志并跳过该项。
- `unlockAchievement()` 先查询状态，已解锁则直接返回；成功 `SetAchievement` 后异步提交。
- `resetAchievement()` 调用 `ClearAchievement` 后异步提交；当前未检查 Clear 返回值，若扩展应补充错误处理。
- `getAchievementStatus()` 查询单个成就，Steam 不可用/ID 无效时返回 `false` 并记录日志。
- `getSteamLanguage()` 实际通过 `SteamConfigHelper::getSteamLanguage` 调用 `SteamApps()->GetCurrentGameLanguage()`，Lua 入口虽位于 `lua_SteamHelper_auto.cpp`，但绑定的是另一个帮助类。

## 异步提交与生命周期

`SubmitChangeAsync()` 使用 `isSubmitting`、`submitMutex`、`submitCondition` 和 detached `std::thread` 调用 `StoreStats()`。失败时每 200ms 重试，最多 5 次，失败把 `isDataCached` 设为 `true`。析构函数先等待提交线程结束，再对缓存数据做一次同步提交。

维护时必须保证：线程访问的对象在析构前仍存活；等待条件始终在状态变更后 `notify_all`；不要把 Steam SDK 调用迁移到不允许的线程；重试次数和缓存语义要与日志保持一致。当前实现的 `isSubmitting`/`isDataCached` 是普通 `bool`，若未来允许多线程并发调用，应改用受同一互斥量保护或原子变量，并重新审视 detached 线程模型。

## Lua 暴露接口

`lua_SteamHelper_auto.cpp` 将 C++ 类以 `lstg.SteamHelper` 暴露，继承声明为 `cc.Ref`，方法包括 `getInstance`、`getSteamAchievementList`、`unlockAchievement`、`resetAchievement`、`getAchievementStatus` 和 `getSteamLanguage`。包装使用统一 `LUA_*INVOKE_*` 宏；修改 C++ 签名时必须同步包装函数和 Lua 类型注册名。

## 新工具类检查表

- 明确单例还是普通实例，禁止意外复制；析构时释放监听器、线程、句柄或 SDK 资源。
- 对平台不可用、SDK 未初始化、空指针和失败返回值记录项目统一日志（`CC_LOG_*`/`LINFO`）。
- 回调应保存可清理的句柄或 `std::function`，并确认回调发生线程符合 Cocos/Lua 要求。
- 将公共 API、实现、Lua 绑定和 CMake 清单作为一个变更单元检查。
