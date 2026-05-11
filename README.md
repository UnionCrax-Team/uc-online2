# uc-online2 — xinerqu 分支

> **⚠️ 此仓库是 [UnionCrax-Team/uc-online2](https://github.com/UnionCrax-Team/uc-online2) 的个人维护分支 (fork)。**
>
> 原作者 [veeanti](https://vee-anti.xyz) 的工作是上游的基础。本分支在原 v1.5.0 基础上修复了关键 bug 并补全了功能，如果上游未合并 PR，两条线将各自独立发展。
>
> 英文说明见原文下方，本段为中文摘要。详细变更见 [Changelog](#changelog)。

# uc-online2 (Original README below)

Custom modified Steam API .dll for Steam games to spoof your game as Spacewar (or any other game). Drop-in replacement for `steam_api.dll` / `steam_api64.dll`. Now has its own "client"! (AKA Core)

## Usage

__**If using downloaded .dlls from [Releases](https://github.com/xinerqu/uc-online2/releases) (xinerqu fork, v1.5.1):**__
- 1. Extract the archive downloaded from __**LATEST**__ release (which is now v1.5.0).
- 2. Copy the corresponding .dll to replace your original .dll.
   - 2a. Rename the original .dll before copying it to something else if you feel you must back it up, something like ``steam_api_o.dll`` as Goldberg Emu suggests or ``steam_api64.dll.old``. (It doesn't matter as long as it is just changed.)
- 3. Make sure Steam is running first. Then try running the game as you normally would from the .exe. If it has SteamStub, use Steamless to remove it or use the .dll made to bypass it for games that Steamless cannot unpack. (Dave the Diver is an example.) Or use "GetStubbedLol" in the ini.
   - 3a. If it throws an error related to auth failure, restart Steam and try again. If the error persists, contact me. I'll work with you to figure it out one way or another. 

__**If using self built .dlls:**__
- 1. Run `build.bat` or open the `uc_online2.vcxproj` and `uc_online2_core.vcxproj` files in Visual Studio.
- 2. Copy the output .dll to your game folder:
   - **32-bit:** `build\x86\steam_api.dll`
                 `build\x86\uc_online2_core.dll`
   - **64-bit:** `build\x64\steam_api64.dll`
                 `build\x64\uc_online2_core64.dll`
- 3. Replace your `steam_api(64).dll` with one from here. Back it up if necessary by renaming it to `steam_api(64)_o.dll`. **Make sure you copy the core dll alongside the other one!**

## Configuration

Create `union-crax.ini` next to the game executable to change your AppId as needed. If this file is missing, AppId defaults to `480` and plugins are not loaded. `PluginsFolder` is relative to the game executable or wherever it's set in the .ini. Or should be. I haven't tested it yet. Check the `steam_appid.txt` file that gets created upon running the game to check if your set AppId was accepted. For games that have `480` patched in
the game's code, try setting it to something else free that's multiplayer, like `440`
(Team Fortress 2). Shapes of Dreams did not work using `480`, but worked fine with `440`.
((THANK YOU to deityofsukana for helping figure that out for certain!!!))

```ini
[Settings]
AppId=480 # Spacewar
ogAppId=220 # Half-Life 2
PluginsFolder=plugins # Path relative to the exe
GetStubbedLol=false # true / false, yes / no, 1 / 0
```

## "ogAppId"

This is an attempt to allow the overlay to force use the right game assets even when you very clearly are supposedly running Spacewar. Setting the original AppId here just gets calculated to the 64-bit Game ID string it expects (which I just learned about too...) and is used for the `SteamOverlayGameId` environment variable which could easily be run as a launch arg, but requires you knowing the long string of numbers for your game, so this just makes it way easier to set up. `SteamGameId` is not touched at all by this, as it can cause problems. It uses the `AppId` for that, except it also gets converted to the expected 64-bit Game ID string.

## Plugin Loader / Injector

If `PluginsFolder` is set in the .ini file, all `.dll` files in that folder are loaded at startup in alphabetical order. Use prefixes to control load order:

```
plugins/
  01_first_plugin.dll
  02_second_plugin.dll
  03_another_one_(dj_khaled!!).dll
```

## SteamStubbed

If `GetStubbedLol` is enabled in the .ini file, it will attempt to patch SteamStub on the fly. This is meant for games that Steamless cannot unpack, such as Dave the Diver. However, it can be used to keep from modifying the game files at all, or as little as possible. I'm not responsible for the code, it was used from DenuvoSanctuary's original Rust code, [which can be found here](https://github.com/denuvosanctuary/steamstubbed). I
rewrote it in C++ so I could try integrating it into this project and not need to inject it. I did not ask for permission to use it in any way, so if there are any issues with that, please contact me and I'll remove it or work something out. It's not much of a change anyways, and they're easy to find too.

If the function is disabled, or was never written in the first place, then it simply 
will just ignore the function entirely and continue as it wassn't implemented in the
first place.

## Building

__**Quick way (true Chad way - quick, simple, and easy):**__
- 1. Run `build.bat`.
- 2. ???
- 3. Profit.

__**With Visual Studio (the bum way - requires too much effort):**__
- 1. Open `uc_online2.vcxproj`.
- 2. Select Release | Win32 or Release | x64.
- 3. Build.

Requires Visual Studio 2022 with C / C++ Environment selected (v143 or higher toolset). If MSBuild is not found, `build.bat` will tell you where to get it. The `build.bat` script has a preset path that applies to my personal setup and if it doesn't match yours, you will need to modify it. Otherwise, you'll consistently error out even when you have everything installed. 

## Forking / Modifications 

Okay, so this part I did not cover as of publishing the source files, this will cover personal modifications and forks as well as modifications to this.

- Please feel free to fork this and modify it however you see fit, and if you feel it would benefit the original repo, make a PR and I'll look into it.
- You are allowed to modify this and distribute it to your liking, all I ask is that I'm made aware of this. Read the license for more info. 
   - I only want this so I can be certain that it's not being distributed as a method to spread malware. (I should make the license have something for this, or make it better...)
- Have any ideas but you're not sure how to implement it? Or don't know how to code? Contact me with your ideas. I'll work on it for you and communicate with you throughout the process. 

## Issues?

- No, this will not work with Denuvo protected games. If you think it can, modify it so that it can work like an activated game, but even then I cannot guarantee it will work. It will likely reject you and you will need to get re-activated as your token will be fucked permanently. So basically, __I say just don't even bother. It'll likely waste your time and the activators' time too.__
- As it is right now, DLC you don't own will likely not work - I'll try and add functionality for that in and if it works, then it'll likely work the same as Goldberg does.
- ~~If you're trying this with a game that has the AppId hard coded in (like with Godot games) then you'll need to modify the game to set the AppId to what you need it to be. Though, you won't even need this at all if you do that lol.~~
  **已于 v1.6.0 修复：** 本 fork 通过 `BIsSubscribedApp` hook + 补全 flat API 导出，解决了 Steamworks.NET (C#) 游戏（如 Godot + Steamworks.NET）的兼容性问题。已测试 Slay the Spire 2 正常运行，其他同类游戏应有同样效果。
- You cannot join VAC protected servers or servers hosted using the real AppId in Garry's Mod or other Source games or any other games that have similar protections. (GoldSrc games seemingly do not apply, as CS1.6 let me join any servers.) Please do not message me asking why you can't join any servers in Garry's Mod. Instead, ask me how you can play with your friends if they have legitimate copies. :)
- For any other unexpected or unaccounted for issues, please contact me. I have yet to test this with every game so I will rely on the community to do so. 

## Changelog (xinerqu fork vs upstream v1.5.0)

### v1.6.1 — 存档持久化修复

**修复了部分使用 Steam Remote Storage 的游戏退出后存档丢失的问题**

- **问题**：部分游戏通过文件系统直接写入部分存档文件（而非 Steam Remote Storage API），导致这些文件不在 Steam 的云同步清单（remotecache.vdf）中。重启游戏后 Steam API 无法识别这些文件，返回"文件未找到"。
- **修复**：在 `SteamAPI_ISteamRemoteStorage_FileRead` 中增加了**本地文件系统回退**机制。当 Steam API 返回 0 字节时，自动回退到本地文件系统读取。

### v1.6.0 — Steamworks.NET 兼容性修复 (Godot 游戏支持)

**修复了 Godot + Steamworks.NET 游戏（如 Slay the Spire 2）无法运行的问题**

- **根因**：uc-online2 缺少 `SteamAPI_ISteamUserStats_RequestCurrentStats` flat API 导出
- Steamworks.NET (C#) 在初始化后立即 P/Invoke 此函数加载成就/统计数据，缺失导致 `EntryPointNotFoundException`
- BIsSubscribedApp hook 已经能处理硬编码 AppID 检查，但 flat API 导出不完整导致 C# 层直接抛异常
- **修复**：补全缺失的 flat API 导出。`RequestCurrentStats` 在新版 Steam SDK 中已被移除（stats 由 Steam 客户端自动管理），实现返回 `k_uAPICallInvalid`
- **影响范围**：所有使用 Steamworks.NET 的 Godot / Unity / C# 游戏

### v1.5.1 — Bug fixes & missing API exports

**Bug 1：`InitSteamClient()` 加载路径缺陷**
- 原代码使用 `LoadLibraryA(fullPath)` 加载 `steamclient64.dll`
- Windows 解析 DLL 依赖（tier0_s64.dll, vstdlib_s64.dll）时会在游戏 exe 目录查找，而非 Steam 目录
- **修复**：改用 `LoadLibraryExA` + `LOAD_WITH_ALTERED_SEARCH_PATH`，并在 `uc_loader.h` 中统一修复 core DLL 加载路径

**Bug 2：`LoadGameOverlay()` 硬编码路径**
- `GetModuleHandle` 使用硬编码 `C:\Program Files (x86)\Steam\GameOverlayRenderer64.dll`
- Steam 安装在非默认路径时误判 overlay 已加载
- **修复**：改用 `GetModuleHandleA(模块名)`，三级 Steam 路径回退（注册表 → 硬编码 → API 缓存），PATH 搜索回退

**缺失 flat API 导出补全**
- 34 个 `ISteamMusicRemote` flat API 导出
- 11 个旧版 `ISteamGameSearch` flat API 存根
- `ISteamFriends_SetPersonaName` + `ISteamFriends_GetUserRestrictions`
- 旧版本号接口别名（v001 GameSearch/MusicRemote/Timeline、v017 Friends、v012 UserStats、v020 UGC 等）
- `g_pSteamClientGameServer` 导出（修复 `S_API` 声明位置）

**其他修复**
- Release 模式日志：移除 `#ifdef _DEBUG` 守卫，UCOLOG 现在在所有构建模式下工作
- 更健壮的 DLL 路径解析：`PathFindFileNameA` 确保文件名提取正确