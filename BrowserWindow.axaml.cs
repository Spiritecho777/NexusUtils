using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using NexusUtils.Classe;
using NexusUtils.Control;
using System;
using System.Diagnostics;
using System.Linq;
using Xilium.CefGlue;

namespace NexusUtils;

public partial class BrowserWindow : Window
{
    //private readonly TabControl _tabControl = new();

    public BrowserWindow(SiteItem site)
    {
        InitializeComponent();

        Title = site.Name;
        Width = 1024;
        Height = 768;

        Content = _tabControl;
    }

    public void OpenTab(SiteItem site)
    {
        var browser = new BrowserTab(site);
        var tabItem = new TabItem
        {
            Header = site.Name,
            Content = browser
        };

        _tabControl.Items.Add(tabItem);
        _tabControl.SelectedItem = tabItem;
    }
}

public class BrowserTab : UserControl
{
    public BrowserTab(SiteItem site)
    {
        Debug.WriteLine("Opening URL: " + site.Url);
        var browserView = new CefBrowserView(site.Url);

        Content = browserView;
    }
}