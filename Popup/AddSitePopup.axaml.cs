using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using NexusUtils.Classe;
using System;
using System.Windows;
using Tmds.DBus.Protocol;

namespace NexusUtils.Popup;

public partial class AddSitePopup : Window
{
    public AddSitePopup()
    {
        InitializeComponent();
    }

    private void Add_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        string siteName = SiteName?.Text?.Trim() ?? "";
        string siteUrl = SiteUrl?.Text?.Trim() ?? "";
        
        if (string.IsNullOrWhiteSpace(siteName) || string.IsNullOrEmpty(siteUrl))
        {
            new AlerteWindow("Les champs sont obligatoires.").ShowDialog(this);
            return;
        }
        else
        {
            SiteManager siteManager = new SiteManager();
            siteManager.AddSite(siteName, siteUrl);
        }

            this.Close();
    }

    private void Cancel_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        this.Close();
    }
}