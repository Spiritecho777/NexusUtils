#include "KeyboardHook.h"
#include "../Popup/RemoteWindow.h"
#include <algorithm>
#include <mutex>

std::vector<QPointer<RemoteWindow>> KeyboardHook::s_windows;

#ifdef _WIN32
	#include <windows.h>

	HHOOK KeyboardHook::s_hook = nullptr;

	void KeyboardHook::startHook(RemoteWindow* window) 
	{
		if (std::find(s_windows.begin(), s_windows.end(), window) == s_windows.end()) {
			s_windows.push_back(window);
		}
		if (!s_hook) {
			s_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
		}
	}

	void KeyboardHook::stopHook(RemoteWindow* window)
	{
		s_windows.erase(std::remove(s_windows.begin(), s_windows.end(), window), s_windows.end());

		s_windows.erase(std::remove_if(s_windows.begin(), s_windows.end(), 
			[](const QPointer<RemoteWindow>& w) { return w.isNull(); }),
			s_windows.end());

		if (s_windows.empty() && s_hook) {
			UnhookWindowsHookEx(s_hook);
			s_hook = nullptr;
		}
	}

	RemoteWindow* KeyboardHook::getFocusedWindow() 
	{
		HWND fg = GetForegroundWindow();
		if (!fg) return nullptr;

		for (auto& window : s_windows) {
			if (window->isVisible() && (HWND)window->winId() == fg) {
				return window;
			}
		}
		return nullptr;
	}

	LRESULT CALLBACK KeyboardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
	{
		if (nCode < 0 || !lParam) 
			return CallNextHookEx(s_hook, nCode, wParam, lParam);

		auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		if (!kbd) return CallNextHookEx(s_hook, nCode, wParam, lParam);

		int vkCode = kbd->vkCode;

		RemoteWindow* focusedWindow = getFocusedWindow();
		if (focusedWindow && focusedWindow->isHooked()) {
			bool handled = false;

			if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
				handled = focusedWindow->keyboardHookKeyDown(vkCode, (int)wParam);
			else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
				handled = focusedWindow->keyboardHookKeyUp(vkCode);
			
			if (handled)
				return 1;
		}
		return CallNextHookEx(s_hook, nCode, wParam, lParam);
	}
#endif

#if defined(__linux__)
	#include <X11/Xlib.h>
	#include <X11/extensions/record.h>
	#include<cstdlib>

	Display* KeyboardHook::s_display = nullptr;
	static Display* s_queryDisplay = nullptr;
	XRecordContext KeyboardHook::s_context = 0;
	bool KeyboardHook::s_running = false;
	static std::mutex g_xrecordMutex;
	static std::mutex g_windowsMutex;

	bool KeyboardHook::isWayland() 
	{
		const char* platform = getenv("WAYLAND_DISPLAY");
		if (platform && platform[0]) return true;

		const char* session = getenv("XDG_SESSION_TYPE");
		if (session && std::string(session) == "wayland") return true;

		return false;
	}

	bool KeyboardHook::isX11() 
	{
		const char* display = getenv("DISPLAY");
		if (display && display[0]) return true;

		const char* session = getenv("XDG_SESSION_TYPE");
		if (session && std::string(session) == "x11") return true;

		return false;
	}

	void KeyboardHook::ensureXRecordInitialized() 
	{
		std::lock_guard<std::mutex> lock(g_xrecordMutex);
		if (s_running) return;

		s_display = XOpenDisplay(nullptr);
		s_queryDisplay = XOpenDisplay(nullptr);
		if (!s_display || !s_queryDisplay) return;
		
		XRecordRange* range = XRecordAllocRange();
		if(!range)
		{
			XCloseDisplay(s_display);
			s_display = nullptr;
			return;
		}

		range->device_events.first = KeyPress;
		range->device_events.last = KeyRelease;

		XRecordClientSpec clients = XRecordAllClients;

		s_context = XRecordCreateContext(s_display, 0, &clients, 1, &range, 1);
		XFree(range);

		if (!s_context)
		{
			XCloseDisplay(s_display);
			s_display = nullptr;
			return;
		}

		s_running = true;
		XRecordEnableContextAsync(s_display, s_context, keyboardCallback, nullptr);
		XFlush(s_display);
	}

	void KeyboardHook::shutdownXRecord() 
	{
		std::lock_guard<std::mutex> lock(g_xrecordMutex);
		if (!s_running || !s_display) return;

		XRecordDisableContext(s_display, s_context);
		XRecordFreeContext(s_display, s_context);
		s_context = 0;

		XCloseDisplay(s_display);
		s_display = nullptr;

		if(s_queryDisplay) 
		{
			XCloseDisplay(s_queryDisplay);
			s_queryDisplay = nullptr;
		}
		s_running = false;
	}

	void KeyboardHook::startHook(RemoteWindow* window) 
	{
		if (!window) return;
		{
			std::lock_guard<std::mutex> lock(g_windowsMutex);
			if (std::find(s_windows.begin(), s_windows.end(), window) == s_windows.end())
				s_windows.push_back(window);
		}

		if (isWayland())
		{
			qWarning() << "[KeyboardHook] Wayland detected: global keyboard hook disabled.";
			return;
		}

		if (!s_running) ensureXRecordInitialized();
	}

	void KeyboardHook::stopHook(RemoteWindow* window) 
	{
		if (isWayland())
			return;

		bool empty = false;
		{
			std::lock_guard<std::mutex> lock(g_windowsMutex);
			s_windows.erase(std::remove(s_windows.begin(), s_windows.end(), window), s_windows.end());

			s_windows.erase(std::remove_if(s_windows.begin(), s_windows.end(),
				[](const QPointer<RemoteWindow>& w) { return w.isNull(); }),
				s_windows.end());
			empty = s_windows.empty();
		}

		if (empty) shutdownXRecord();
	}	

	RemoteWindow* KeyboardHook::getFocusedWindow() 
	{
		if (!s_queryDisplay) return nullptr;

		Window focus;
		int revert;
		XGetInputFocus(s_queryDisplay, &focus, &revert);

		std::lock_guard<std::mutex> lock(g_windowsMutex);
		for (auto& window : s_windows) 
		{
			if (!window.isNull() && window->isVisible() && (Window)window->winId() == focus) 
			{
				return window;
			}
		}
		return nullptr;
	}

	void KeyboardHook::keyboardCallback(XPointer priv, XRecordInterceptData* data) 
	{
		if (!data) return;

		if (data->category != XRecordFromServer || !data->data || data->data_len < 2)
		{
			XRecordFreeData(data);
			return;
		}

		const unsigned char* raw = data->data;
		int type = raw[0];
		int keycode = raw[1];

		RemoteWindow* focusedWindow = getFocusedWindow();
		if (focusedWindow && focusedWindow->isHooked())
		{
			bool handled = false;

			if (type == KeyPress)
				handled = focusedWindow->keyboardHookKeyDown(keycode, type);
			else if (type == KeyRelease)
				handled = focusedWindow->keyboardHookKeyUp(keycode);

			if (handled)
			{
				XRecordFreeData(data);
				return;
			}
		}
		XRecordFreeData(data);
	}
#endif