using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using AvaloniaWebView;
using NexusUtils.Classe;
using Pango;
using System;
using System.Diagnostics;
using System.Linq;


namespace NexusUtils;

public partial class BrowserWindow : Window
{
    public BrowserWindow(SiteItem site)
    {
        App.InitializeCef();

        InitializeComponent();

        Title = site.Name;
        Width = 1024;
        Height = 768;

        _WebBrowser.Url = new Uri (site.Url);
    }

    public void OpenTab(SiteItem site)
    {
        var tabItem = new TabItem
        {
            Header = site.Name,
        };

        #region Injection javascript pour auto-login
        _WebBrowser.NavigationCompleted += (sender, args) =>
        {
                string login = site.Credentials;
                string password = site.Password;

                string script = $@"
                (function() 
                {{
                    var loginInput = document.querySelector('input[name=usermail], input[id=username]');
                    if(loginInput) 
                    {{
                        loginInput.value = '{login.Replace("'", "\\'")}';
                    }}
                            
                    var passwordInput = document.querySelector('input[type=password], input[name=password], input[id=password]');
                    if(passwordInput)
                    {{
                        passwordInput.value = '{password.Replace("'", "\\'")}';
                    }}
                }})();
                ";

                _WebBrowser.ExecuteScriptAsync(script);
        };
    }
    #endregion
}