#pragma once
#include <vector>
#include <QPointer>

#ifdef _WIN32
	#include <windows.h>
#endif

#if defined(__linux__)
	#include <X11/Xlib.h>
	#include <X11/extensions/record.h>

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
	static Display* s_display; 
	static XRecordContext s_context;
	static bool s_running;

	static void keyboardCallback(XPointer priv, XRecordInterceptData* data);
	static void ensureXRecordInitialized();
	static void shutdownXRecord();

	static bool isWayland();
	static bool isX11();
#endif
};