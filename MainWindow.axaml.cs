using Avalonia.Controls;
using Avalonia.VisualTree;
using NexusUtils.Classe;
using NexusUtils.Popup;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.ConstrainedExecution;
using System.Threading.Tasks;

namespace NexusUtils
{
    public partial class MainWindow : Window
    {
        internal ObservableCollection<SiteItem> Sites { get; } = new();
        internal SiteManager siteManager = new SiteManager();
        private BrowserWindow ? browserWindow;

        public MainWindow()
        {
            InitializeComponent();

            ListSite.LayoutUpdated += (_, _) => ListSite_Loaded();

            UpdateSiteList();
        }

        private void UpdateSiteList()
        {
            ListSite.Items.Clear();

            if (siteManager.FileExists())
            {
                foreach (var site in siteManager.GetAllSites())
                {
                    ListSite.Items.Add(site);
                }
            }
        }

        private void ListSite_Loaded()
        {
            for (int i = 0; i < ListSite.ItemCount; i++)
            {
                var container = ListSite.ItemContainerGenerator.ContainerFromIndex(i);
                if (container is ListBoxItem listBoxItem)
                {
                    var buttons = listBoxItem.GetVisualDescendants().OfType<Button>().ToList();
                    if (buttons.Count == 3)
                    {
                        buttons[0].Click -= OpenSite_Click;    // Open button
                        buttons[1].Click -= AddCreds_Click;   // Add Credentials button
                        buttons[2].Click -= DeleteSite_Click; // Delete button

                        buttons[0].Click += OpenSite_Click;    // Open button
                        buttons[1].Click += AddCreds_Click;   // Add Credentials button
                        buttons[2].Click += DeleteSite_Click; // Delete button
                    }
                }
            }
        }

        private async void AddSite_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
        {
            try
            {
                var popup = new AddSitePopup();
                popup.Closed += (_,_) => UpdateSiteList();
                popup.Show();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error showing AddSitePopup: {ex.Message}");
                Debug.WriteLine(ex.StackTrace);
            }
        }

        private void Popup_Closed(object? sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        #region Interactions Site
        private void AddCreds_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
        {
            if (sender is Button button && button.Tag is SiteItem site)
            {
                var popup = new CredSitePopup(site);
                popup.Show();
            }
            UpdateSiteList();
        }

        private void DeleteSite_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
        {
            if (sender is Button button && button.Tag is SiteItem site)
            {
                siteManager.DeleteSite(site);
                UpdateSiteList();
            }
            UpdateSiteList();
        }

        private void OpenSite_Click(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
        {
            if (sender is Button button && button.Tag is SiteItem site)
            {
                if (browserWindow == null || !browserWindow.IsVisible)
                {
                    browserWindow = new BrowserWindow(site);
                    browserWindow.OpenTab(site);
                    browserWindow.Closed += (_, _) => browserWindow = null;
                    browserWindow.Show();
                }
                else
                {
                    browserWindow.OpenTab(site);
                }
            }
        }
        #endregion
    }
}