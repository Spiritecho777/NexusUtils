using NexusUtils.Classe;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using System;

namespace NexusUtils;

public partial class PopupWindow : Window
{
    #region Variable
    private IntPtr _hookID;
    public bool allowShiftAndCaps;
    #endregion

    public PopupWindow()
    {
        InitializeComponent();
    }

    public void KeyboardHook_KeyDown(int keyCode, int etat)
    {

    }

    public void KeyboardHook_KeyUp(int keyCode)
    {

    }
}