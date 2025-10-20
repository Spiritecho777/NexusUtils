using Avalonia;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using NexusUtils.Classe;

namespace NexusUtils.Popup;

public partial class CredSitePopup : Window
{
    public SiteItem Site = new SiteItem();
    public CredSitePopup(SiteItem site)
    {
        InitializeComponent();
        Site = site;
    }

    private void Add_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        string identifiant = Identifiant?.Text?.Trim() ?? "";
        string password = Password?.Text?.Trim() ?? "";

        SiteManager siteManager = new SiteManager();
        siteManager.AddCreds(identifiant,password,Site);

        this.Close();
    }

    private void Cancel_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        this.Close();
    }
}