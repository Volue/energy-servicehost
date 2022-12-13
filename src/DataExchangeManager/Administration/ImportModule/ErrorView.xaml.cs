using System.Windows;
using System.Windows.Input;

namespace DataExchange.Administration.ImportModule
{
    public partial class ErrorView : Window
    {
        public ErrorView()
        {
            InitializeComponent();
        }

        private void CloseCommandHandler(object sender, ExecutedRoutedEventArgs e)
        {
            Close();
        }
    }
}
