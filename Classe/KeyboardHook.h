#pragma once
#include <vector>
#include <QPointer>

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__linux__)
#include <X11/Xlib.h>

#undef KeyPress
#undef KeyRelease
#undef FontChange
#undef Status
#undef Bool
#undef None
#undef True
#undef False
#undef Complex
#undef FocusIn
#undef FocusOut
#undef Expose
#undef DestroyNotify
#undef CursorShape
#undef Unsorted
#undef Below
#undef Above
#undef Success
#endif

class RemoteWindow;

class KeyboardHook
{
public:
	static void startHook(RemoteWindow* window);
	static void stopHook(RemoteWindow* window);

private:
	static std::vector<QPointer<RemoteWindow>> s_windows;

	static RemoteWindow* getFocusedWindow();

#ifdef _WIN32
	static HHOOK s_hook;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
#endif

#if defined(__linux__)
	static void ensureInitialized();
	static void shutdown();
	static void eventLoop();
	static bool isWayland();
	static bool isX11();
#endif
};