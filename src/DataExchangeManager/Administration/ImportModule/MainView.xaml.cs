using System;
using System.Collections;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace DataExchange.Administration.ImportModule
{
    /// <summary>
    /// Interaction logic for MainView.xaml
    /// </summary>
    public partial class MainView : UserControl
    {
        public MainView(MainViewModel mainViewModel)
        {
            InitializeComponent();

            mainViewModel.Update();

            DataContext = mainViewModel;
            mainViewModel.PropertyChanged += OnMainViewModelPropertyChanged;
        }

        private void OnMainViewModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if(e.PropertyName != "Error")
            {
                return;
            }

            MainViewModel mainViewModel = sender as MainViewModel;
            if (mainViewModel != null && mainViewModel.Error != null)
            {
                ErrorView errorView = new ErrorView();
                errorView.DataContext = mainViewModel.Error;
                errorView.ShowDialog();
            }
        }

        private void CloseCommandHandler(object sender, ExecutedRoutedEventArgs e)
        {
            Application.Current.MainWindow.Close();
        }

        private IList GetItemsSelectedBeforeSelectionChange(SelectionChangedEventArgs e)
        {
            ArrayList selectedItemsBeforeChange = new ArrayList();
            selectedItemsBeforeChange.AddRange(FailedImportsDataGrid.SelectedItems);
            selectedItemsBeforeChange.AddRange(e.RemovedItems);
            foreach (object item in e.AddedItems)
            {
                selectedItemsBeforeChange.Remove(item);
            }
            return selectedItemsBeforeChange;
        }   

        private void FailedImportsDataGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            IList selectedItemsBeforeChange = GetItemsSelectedBeforeSelectionChange(e);
            MainViewModel model = (MainViewModel)DataContext;

            model.FailedImportsSelectionChanged(selectedItemsBeforeChange);
        }


    }
}
