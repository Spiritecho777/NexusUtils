NexusUtils - README

Overview
NexusUtils is a lightweight, privacy-focused desktop application that combines a secure site manager with a custom Chromium-based browser. It allows you to store website credentials encrypted on disk and open sites in dedicated browser windows with automatic credential injection and advanced keyboard hooking support.
Designed for power users, gamers, and anyone who needs reliable auto-login + special keyboard handling (especially useful for browser-based games or applications using canvas/WebGL).

Key Features

Secure Credential Storage
AES-256-CBC encryption (OpenSSL)
PBKDF2-HMAC-SHA256 key derivation (100,000 iterations)
Optional Master Password protection
Machine-bound encryption fallback

Smart Browser
Built with Qt6 WebEngine (Chromium)
Automatic credential injection on page load
Tabbed browsing
Popup window support

Advanced Keyboard Hooking
Windows: Low-level keyboard hook (WH_KEYBOARD_LL)
Linux: X11 global key grabbing + event relay
Special handling for Alt, AltGr, Super/Meta keys
Improved input compatibility for canvas-based applications and games

User Experience
System tray integration
Single instance lock
Clean, simple UI
Cross-platform (Windows + Linux)



Screenshots
(Add screenshots here when available)

Building the Project
Prerequisites

Qt 6.10+ (Widgets + WebEngine)
CMake 3.16+
OpenSSL (via vcpkg or system)
Visual Studio 2022 (Windows)
Inno Setup (for Windows installer)

Windows
PowerShell# Standard build with WebEngine
.\Build.ps1 -Target all_web -ProjectName NexusUtils

# With static OpenSSL
.\Build.ps1 -Target all_web_secure -ProjectName NexusUtils
Linux
The build script supports both static builds and AppImage generation.
Bash# Run via PowerShell (WSL) or adapt for native
./Build.ps1 -Target all_web -ProjectName NexusUtils

Usage

Launch NexusUtils
Add a site (Name + URL)
Click the key icon (🔑) to add credentials and enable keyboard hook if needed
Click the site name to open it in a dedicated browser window
Credentials are automatically injected after page load

Master Password: You can enable a master password via the lock button for extra protection.

Security Notes

All credentials are encrypted at rest.
Master password uses strong PBKDF2 hashing.
Keyboard hooks are only active for windows where the feature is enabled.
Linux hook currently requires X11 (not Wayland).


Known Limitations

Linux keyboard hook does not work on Wayland (X11 only).
Some complex login forms may require manual adjustment of selectors in TabView.cpp / RemoteWindow.cpp.
The Windows installer (Inno Setup) includes all necessary Qt libraries.


License
This project is under the MIT license.
Do what you want

Author
Built with ❤️ using Qt6 and C++17.

Enjoy secure and seamless browsing!
