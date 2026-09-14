# C++ 层 Lua API 绑定维护记忆

## 代码范围

目标目录为 `frameworks/LuaBindings`。入口是 `LuaBindings.cpp`，声明和自定义转换位于 `LuaBindings.h`；通用转换模板和绑定宏位于 `lua_conversion/lua_conversion.hpp` 及 `lua_conversion_tolua.hpp`。自动绑定文件命名为 `lua_<类名>_auto.hpp/.cpp`，手写补充通常命名为 `*_manual.cpp` 或放在 `lua_cc_fix.cpp`、`lua_cc_ext.cpp`。

## 注册流程

`XLuaModuleRegistry.h` 提供 `LUA_REGISTER_MODULE(Name, Entry)` 静态注册宏。`LuaBindings.cpp` 通过该宏把入口函数加入全局 `LuaModuleRegistry`，运行时由注册表统一调用。模块名决定 Lua 侧的命名空间，例如 `x_Resource`、`cc_audio`、`x_SteamHelper`。外部 Lua 库（lfs、lpeg、cjson、imgui，以及可选 yue）也采用同一注册表。

典型类入口使用以下模式：

1. `LUA_ENTRY("lstg")` 打开命名空间。
2. `LUA_CLS_DEF` 或 `LUA_CLS_DEF_BASE` 声明 tolua 用户类型、继承关系和析构策略。
3. `LUA_METHOD` 映射 C++ 方法包装函数。
4. `LUA_CLS_END` 结束类并调用 `registerLuaType`，最后用 `LUA_ENTRY_END` 清理 Lua 栈。

自动绑定文件可拆出多个 `luaReg_<模块>_<类型>`，总入口按顺序调用它们（资源绑定是现成示例）。

## 包装函数与类型转换

成员调用通常以 `LUA_INVOKE_HEADER` 开始，用 `LUA_TRY_INVOKE`（无返回值）或 `LUA_TRY_INVOKE_R`（有返回值）尝试签名，失败时由 `LUA_INVOKE_FOOTER` 生成参数错误。静态/全局函数使用 `LUA_SINVOKE_*` 或 `LUA_GINVOKE_*`。这些宏依赖约定的局部变量 `lua_S`、`argc`、`ok` 和 `arg_cur`，手写包装不要破坏 Lua 栈布局。

`lua::to_native`/`lua::to_lua` 提供 C++ 与 Lua 值转换：Cocos `Ref` 子类通过 tolua userdata 和 `g_luaType` 注册名转换；非 `Ref` 指针走普通 userdata；`Vec2`、颜色、矩形、容器等结构有专门模板。`Color4B` 同时支持 `lstgColor` userdata 和带 `r/g/b/a` 字段的 Lua table。新增类型优先增加模板特化，而不是在多个包装函数中重复解析。

## 自动与手写绑定的边界

- `*_auto.cpp/.hpp`：由生成器产生的稳定 API 映射。重新生成后可能被覆盖，避免手工改动。
- `*_manual.cpp`：补充生成器无法表达的重载、回调、生命周期或事件逻辑；例如 Controller/FairyGUI 的回调注册和对象克隆。
- `lua_cc_fix.cpp`：针对 Cocos 原生绑定的签名修复/兼容层，包含参数检查和返回值重组。
- `LuaBindings.cpp`：只负责 include 与模块挂接，以及少量跨模块转换/手写函数；新增绑定必须在此注册。

绑定对象必须与 C++ 所有权一致。Cocos `Ref` 对象通常使用 `toluafix_pushusertype_ccobject` 并带 `_ID/_luaID`，创建对象时注册 GC；借用指针不得在 Lua 侧重复释放。

## 构建与排查清单

- 同时把新 `.cpp/.hpp` 加入根 `CMakeSources.cmake` 的 `GAME_SOURCE/GAME_HEADER`，并确认 include 目录。
- 检查模块是否受 `LSTGX_NO_FAIRY_GUI`、`LSTGX_NO_VIDEO`、`LSTGX_NO_LIVE2D` 或平台宏保护。
- Lua 栈错误应通过统一 footer 返回；不要静默吞掉类型转换失败。
- 变更后至少编译受影响平台，并用 Lua 脚本覆盖：构造/析构、继承转换、nil、错误参数、回调触发和返回容器。
