using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace NexusUtils.Classe
{
    public class KeyboardHook
    {
        private const int WH_KEYBOARD_LL = 13;
        private const int WM_KEYDOWN = 0x0100;
        private const int WM_KEYUP = 0x0101;
        private const int VK_SHIFT = 0x10;
        private const int VK_CAPITAL = 0x14;
        private const int VK_ALT = 0x12;
        private const int VK_ALTGR = 0xA5;
        private const int WM_SYSKEYDOWN = 0x0104;
        private const int WM_SYSKEYUP = 0x0105;

        private static LowLevelKeyboardProc _proc = HookCallback;
        private static IntPtr _hookID = IntPtr.Zero;
        private static List<PopupWindow> _popupHandlerInstance = new List<PopupWindow>();
        private static List<PopupWindow> _hookedPopups = new List<PopupWindow>();

        public static IntPtr GetHookID()
        {
            return _hookID;
        }

        public static IntPtr HookKeyboard(PopupWindow popupHandler)
        {
            if (!_hookedPopups.Contains(popupHandler))
                _hookedPopups.Add(popupHandler);

            if (!_popupHandlerInstance.Contains(popupHandler))
                _popupHandlerInstance.Add(popupHandler);

            if (_hookID == IntPtr.Zero)
                _hookID = SetHook(_proc);
            return _hookID;
        }

        public static void UnhookKeyboard(IntPtr hookID, PopupWindow popupWindow)
        {
            _popupHandlerInstance.Remove(popupWindow);

            if(_hookedPopups.Contains(popupWindow))
               _hookedPopups.Remove(popupWindow);

            if (_popupHandlerInstance.Count == 0)
            {
                UnhookWindowsHookEx(hookID);
                _hookID = IntPtr.Zero;
            }
        }

        public static IEnumerable<PopupWindow> GetAllHookedPopups()
        {
            return _hookedPopups.ToArray();
        }

        private static IntPtr SetHook(LowLevelKeyboardProc proc)
        {
            using (Process curProcess = Process.GetCurrentProcess())
            using (ProcessModule curModule = curProcess.MainModule)
            {
                return SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(curModule.ModuleName), 0);
            }
        }

        private delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, IntPtr lParam);

        private static IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
        {
            if (nCode >= 0)
            {
                int vkCode = Marshal.ReadInt32(lParam);
                if (wParam == (IntPtr)WM_KEYDOWN || wParam == (IntPtr)WM_SYSKEYDOWN || wParam == (IntPtr)WM_KEYUP || wParam == (IntPtr)WM_SYSKEYUP)
                {
                    if (_popupHandlerInstance != null)
                    {
                        var focusedWindow = GetFocusedPopupWindow();
                        if (focusedWindow != null)
                        {
                            try
                            {
                                //Debug.WriteLine(wParam + " - " + vkCode);
                                if (wParam == (IntPtr)WM_KEYDOWN || wParam == (IntPtr)WM_SYSKEYDOWN)
                                {
                                    focusedWindow.KeyboardHook_KeyDown(vkCode, (int)wParam);
                                }
                                else if (wParam == (IntPtr)WM_KEYUP || wParam == (IntPtr)WM_SYSKEYUP)
                                    focusedWindow.KeyboardHook_KeyUp(vkCode);
                            }
                            catch (Exception ex)
                            {
                                Debug.WriteLine(ex);
                            }
                        }
                    }

                    if (!GetAllHookedPopups().Any(p => p.allowShiftAndCaps))
                    { 
                        if (vkCode != VK_SHIFT && vkCode != VK_CAPITAL && vkCode != VK_ALT && vkCode != VK_ALTGR)
                        {
                            return (IntPtr)1;
                        }
                    }
                }
            }
            return CallNextHookEx(_hookID, nCode, wParam, lParam);
        }

        private static PopupWindow GetFocusedPopupWindow()
        {
            return _popupHandlerInstance.FirstOrDefault(w =>
        w != null && w.IsVisible && w.IsActive);

            /*var foreground = GetForegroundWindow();
            return _popupHandlerInstance.FirstOrDefault(w =>
                w != null && w.IsVisible && w.GetHandle() == foreground);*/
        }

        #region import
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr SetWindowsHookEx(int idHook, LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool UnhookWindowsHookEx(IntPtr hhk);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("user32.dll")]
        private static extern IntPtr GetForegroundWindow(); 
        #endregion
    }
}
