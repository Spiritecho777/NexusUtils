using Avalonia;
using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;

namespace NexusUtils.Popup
{
    internal class AlerteWindow : Window
    {
        public AlerteWindow(string message)
        {
            Title = "Alerte";
            Width = 220;
            Height = 75;

            var panel = new StackPanel { Margin = new Thickness(10) };

            panel.Children.Add(new TextBlock
            {
                Text = message,
                TextWrapping = TextWrapping.Wrap
            });

            var okButton = new Button
            {
                Content = "OK",
                HorizontalAlignment = HorizontalAlignment.Right,
                Margin = new Thickness(0, 10, 0, 0)
            };

            okButton.Click += (_, _) => this.Close();

            panel.Children.Add(okButton);

            Content = panel;
        }
    }
}
