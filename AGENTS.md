# LuaSTG-x-TUO-Dev 项目默认记忆

## 项目定位

本项目是一个基于 Cocos2d-x/C++ 与 Lua 的跨平台弹幕射击游戏引擎。当前维护重点是 C++ 层和 Lua 层之间的 API 接口，使其他游戏项目可以复用引擎的渲染、资源、输入、音视频及平台工具能力，而不必直接依赖底层 C++ 实现。

仓库的主要引擎代码位于 `frameworks`，游戏/示例层代码位于 `src`，资源位于 `res`。工程由 CMake 组织，核心源文件和头文件集中列在根目录 `CMakeSources.cmake`；可选模块（FairyGUI、Video、Live2D、YueScript 等）通过编译宏控制。

## 任务开始前检查

执行任何任务操作前，必须先检查当前 Git 分支和工作区状态。

1. 检查当前分支是否为 `TUO_Dev_Codex`。
2. 检查当前工作区（包括子模块）是否存在任何未提交变更或未跟踪文件。
3. 如果工作区存在任何未提交变更，立即中止任务并通知用户处理；不得自行暂存、提交、丢弃或隐藏这些变更，也不得切换分支。
4. 如果工作区干净但当前分支不是 `TUO_Dev_Codex`，切换到 `TUO_Dev_Codex`；切换失败则中止任务并通知用户。
5. 确认位于 `TUO_Dev_Codex` 且工作区干净后，才能继续执行任务。

## 维护约定

- 优先保持现有 Cocos2d-x Lua tolua 风格、`lstg` 命名空间和 C++17/CMake 组织方式。
- 修改公共 C++ API 时同步检查 Lua 绑定、`CMakeSources.cmake` 和对应平台的条件编译。
- 自动生成绑定与手写绑定应分开维护；手写修复放在 `*_manual.cpp` 或 `lua_cc_fix.cpp`，不要直接改生成代码后忘记生成源。
- 涉及线程、Steam SDK 或平台 API 的实现，需要同时记录生命周期、失败重试和关闭阶段行为。

## 专题记忆索引
维护流程文件的快速索引。

| 文件 | 功能 |
| --- | --- |
| [LuaBindings 维护记忆](AiMemory/LUA_BINDINGS_MEMORY.md) | 说明 `frameworks/LuaBindings` 的模块注册、自动/手写绑定、类型转换、错误处理、条件编译和构建清单维护方法。 |
| [工具 C++ 类维护记忆](AiMemory/CPP_TOOLS_MEMORY.md) | 说明 `frameworks/Classes` 工具类的接口/实现组织，并以 `SteamAchievementHelper` 为例记录单例、Steam API、异步提交、资源生命周期和扩展步骤。 |
