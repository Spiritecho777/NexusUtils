#include "RemoteWindow.h"
#include "../Classe/KeyboardHook.h"
#include <QWebEngineView>

bool RemoteWindow::keyboardHookKeyDown(int vkCode, int msg) 
{
	// Handle key down events here
	// Return true if the event is handled and should not be passed to other applications
	return false;
}

bool RemoteWindow::keyboardHookKeyUp(int vkCode) 
{
	// Handle key up events here
	// Return true if the event is handled and should not be passed to other applications
	return false;
}