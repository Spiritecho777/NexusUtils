#pragma once

#include <QWidget>
#include <vector>

#ifdef _WIN32
	#include <windows.h>
#endif

//class RemoteWindow;

class KeyboardHook 
{
public:
	static void startHook(RemoteWindow* window);
	static void stopHook(RemoteWindow* window);

private:
	static std::vector<RemoteWindow*> s_windows;

	static RemoteWindow* getFocusedWindow();

#ifdef _WIN32
	static HHOOK s_hook;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
#endif

#ifdef DEBUG
	static Display* s_display; 
	static XRecordContext s_context;
	static void keyboardCallback(XPointer priv, XRecordInterceptData* data);
#endif
};