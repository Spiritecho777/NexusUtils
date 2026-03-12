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

	if (kbd->flags & LLKHF_INJECTED)
		return CallNextHookEx(s_hook, nCode, wParam, lParam);

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
#include <X11/keysym.h>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <unistd.h>

#undef KeyPress
#undef KeyRelease
#undef False
#undef True
#undef None
#undef Bool
#undef Status
#undef FontChange
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

static const int X11_KeyPress = 2;
static const int X11_KeyRelease = 3;
static const int X11_False = 0;
static const int X11_True = 1;
static const long X11_None = 0L;

static const int KC_SUPER_L = 133;
static const int KC_SUPER_R = 134;
static const int KC_ALT = 64;
static const int KC_ALTGR = 108;

static Display* s_grabDisplay = nullptr;
static Display* s_queryDisplay = nullptr;
static std::thread s_eventThread;
static std::atomic<bool> s_running{ false };
static std::atomic<bool> s_stopRequested{ false };
static int s_wakePipe[2] = { -1, -1 };
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

static void grabTargetKeys(Display* dpy) 
{
	Window root = DefaultRootWindow(dpy);
	XSetErrorHandler([](Display*, XErrorEvent*) -> int { return 0; });

	XGrabKey(dpy, KC_SUPER_L, AnyModifier, root, X11_False, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, KC_SUPER_R, AnyModifier, root, X11_False, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, KC_ALT, AnyModifier, root, X11_False, GrabModeAsync, GrabModeAsync);
	XGrabKey(dpy, KC_ALTGR, AnyModifier, root, X11_False, GrabModeAsync, GrabModeAsync);

	XFlush(dpy);
}

static void ungrabTargetKeys(Display* dpy)
{
	Window root = DefaultRootWindow(dpy);
	XUngrabKey(dpy, KC_SUPER_L, AnyModifier, root);
	XUngrabKey(dpy, KC_SUPER_R, AnyModifier, root);
	XUngrabKey(dpy, KC_ALT, AnyModifier, root);
	XUngrabKey(dpy, KC_ALTGR, AnyModifier, root);
	XFlush(dpy);
}

void KeyboardHook::eventLoop()
{
	int xfd = ConnectionNumber(s_grabDisplay);

	while (!s_stopRequested)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(xfd, &fds);
		FD_SET(s_wakePipe[0], &fds);
		int maxfd = std::max(xfd, s_wakePipe[0]) + 1;

		if (select(maxfd, &fds, nullptr, nullptr, nullptr) < 0) break;
		if (s_stopRequested) break;

		while (XPending(s_grabDisplay))
		{
			XEvent ev;
			XNextEvent(s_grabDisplay, &ev);

			if (ev.type != X11_KeyPress && ev.type != X11_KeyRelease)
				continue;

			int  keycode = ev.xkey.keycode;
			bool isPress = (ev.type == X11_KeyPress);

			// Grab clavier complet quand Super ou AltGr s'enfonce
			if (isPress && (keycode == KC_SUPER_L || keycode == KC_SUPER_R || keycode == KC_ALTGR))
			{
				XGrabKeyboard(s_grabDisplay, DefaultRootWindow(s_grabDisplay),
					X11_False, GrabModeAsync, GrabModeAsync, CurrentTime);
				XFlush(s_grabDisplay);
			}

			// Relâcher le grab quand Super ou AltGr se relâche
			if (!isPress && (keycode == KC_SUPER_L || keycode == KC_SUPER_R || keycode == KC_ALTGR))
			{
				XUngrabKeyboard(s_grabDisplay, CurrentTime);
				XFlush(s_grabDisplay);
			}

			// Chercher la RemoteWindow qui a le focus
			RemoteWindow* focused = getFocusedWindow();

			bool handled = false;
			if (focused && focused->isHooked())
			{
				QMetaObject::invokeMethod(focused, [focused, keycode, isPress, evType = ev.type, &handled]() {
					if (isPress)
						handled = focused->keyboardHookKeyDown(keycode, evType);
					else
						handled = focused->keyboardHookKeyUp(keycode);
				}, Qt::BlockingQueuedConnection);
			}

			// Si non géré → relayer à la fenêtre focalisée
			if (!handled)
			{
				Window targetWin;
				int revert;
				XGetInputFocus(s_grabDisplay, &targetWin, &revert);
				if (targetWin != X11_None && targetWin != PointerRoot)
				{
					XEvent relay = ev;
					relay.xkey.window = targetWin;
					relay.xkey.subwindow = X11_None;
					XSendEvent(s_grabDisplay, targetWin, X11_True,
						isPress ? KeyPressMask : KeyReleaseMask, &relay);
					XFlush(s_grabDisplay);
				}
			}
		}
	}
}

void KeyboardHook::ensureInitialized()
{
	if (s_running) return;

	s_grabDisplay = XOpenDisplay(nullptr);
	s_queryDisplay = XOpenDisplay(nullptr);
	if (!s_grabDisplay || !s_queryDisplay) return;

	if (pipe(s_wakePipe) != 0) return;

	grabTargetKeys(s_grabDisplay);

	s_stopRequested = false;
	s_running = true;
	s_eventThread = std::thread(&KeyboardHook::eventLoop);
}

void KeyboardHook::shutdown()
{
	if (!s_running) return;
	s_stopRequested = true;

	// Débloquer le select()
	char c = 1;
	write(s_wakePipe[1], &c, 1);

	if (s_eventThread.joinable())
		s_eventThread.join();

	close(s_wakePipe[0]);
	close(s_wakePipe[1]);
	s_wakePipe[0] = s_wakePipe[1] = -1;

	ungrabTargetKeys(s_grabDisplay);

	XCloseDisplay(s_grabDisplay);  s_grabDisplay = nullptr;
	XCloseDisplay(s_queryDisplay); s_queryDisplay = nullptr;

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

	if (!s_running) ensureInitialized();
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

	if (empty) shutdown();
}

RemoteWindow* KeyboardHook::getFocusedWindow()
{
	if (!s_queryDisplay) return nullptr;

	Window focus;
	int revert;
	XGetInputFocus(s_queryDisplay, &focus, &revert);
	qDebug() << "Focused window ID:" << focus;

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
#endif