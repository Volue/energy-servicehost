using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using DataExchange.Administration.ImportModule.Annotations;
using Microsoft.Practices.Prism.Commands;
using Powel.Wpf.Common;

namespace DataExchange.Administration.ImportModule
{
    public class MainViewModel : INotifyPropertyChanged
    {
        private DelegateCommand _refreshListCommand;
        private DelegateCommand _deleteSelectedCommand;
        private DelegateCommand _saveAndImportCommand;

        private ExtendedObservableCollection<FailedImportModel> _failedImports;
        private readonly IErroneousImportsRepository _erroneousImportsRepository;
        private readonly TaskScheduler _guiThreadTaskScheduler;
        private bool _isLoading;

        private ErrorViewModel _error;
        private DelegateCommand _selectAllCommand;

        public MainViewModel(IErroneousImportsRepository erroneousImportsRepository, TaskScheduler guiThreadTaskScheduler)
        {
            _erroneousImportsRepository = erroneousImportsRepository;
            _guiThreadTaskScheduler = guiThreadTaskScheduler;
        }

        public void Update()
        {
            UpdateFailedImports();
        }

        public bool IsLoading
        {
            get
            {
                return _isLoading;
            }
            private set
            {
                if (_isLoading == value)
                {
                    return;
                }

                _isLoading = value;
                OnPropertyChanged("IsLoading");
                RaiseCanExecuteChangedOnCommands();
            }
        }

        public ExtendedObservableCollection<FailedImportModel> FailedImports
        {
            get
            {
                if (_failedImports == null)
                {
                    _failedImports = new ExtendedObservableCollection<FailedImportModel>();
                }

                return _failedImports;
            }
        }

        public ErrorViewModel Error
        {
            get
            {
                return _error;
            }
            private set
            {
                _error = value;
                OnPropertyChanged("Error");
            }
        }

        public DelegateCommand RefreshListCommand
        {
            get
            {
                if (_refreshListCommand == null)
                {
                    _refreshListCommand = new DelegateCommand(UpdateFailedImports, CanUpdateFailedImports);
                }

                return _refreshListCommand;
            }
        }

        public DelegateCommand DeleteSelectedCommand
        {
            get
            {
                if (_deleteSelectedCommand == null)
                {
                    _deleteSelectedCommand = new DelegateCommand(DeleteFailedImports, CanDeleteFailedImports);
                }

                return _deleteSelectedCommand;
            }
        }

        public DelegateCommand SaveAndImportCommand
        {
            get
            {
                if (_saveAndImportCommand == null)
                {
                    _saveAndImportCommand = new DelegateCommand(SaveAndImport, CanSaveAndImport);
                }

                return _saveAndImportCommand;
            }
        }

        public DelegateCommand SelectAllCommand
        {
            get
            {
                if(_selectAllCommand == null)
                {
                    _selectAllCommand = new DelegateCommand(SelectAll, CanSelectAll);
                }

                return _selectAllCommand;
            }
        }

        private bool CanSelectAll()
        {
            if(IsLoading)
            {
                return false;
            }

            if(FailedImports.All(failedImportModel => failedImportModel.IsSelected))
            {
                return false;
            }

            return FailedImports.Count > 0;
        }

        private void SelectAll()
        {
            foreach (var failedImportModel in FailedImports)
            {
                failedImportModel.IsSelected = true;
            }
        }

        public void FailedImportsSelectionChanged(IList selectedItemsBeforeChange)
        {
            if (selectedItemsBeforeChange.Count == 1)
            {
                // One previously selected item means we've been in edit-mode and that we now should revert any changes made to that item
                FailedImportModel failedImportModel = (FailedImportModel)selectedItemsBeforeChange[0];
                if (failedImportModel.IsAnyPropertyModified)
                {
                    // Revert changes to item when the selection changes
                    failedImportModel.RevertPropertyModifications();
                }
            }

            RaiseCanExecuteChangedOnCommands();
            OnPropertyChanged("FailedImportsDataGrid");
        }

        private void DeleteFailedImports()
        {
            var failedImportsToDelete = FailedImports.Where(failedImportModel => failedImportModel.IsSelected).ToList();

            IsLoading = true;
            var task = new TaskFactory().StartNew(() => _erroneousImportsRepository.Delete(failedImportsToDelete));
            task.ContinueWith(t =>
            {
                if (t.Exception != null)
                {
                    CreateError(t.Exception);
                    IsLoading = false;
                }
                else
                {
                    UpdateFailedImports();
                }

            }, _guiThreadTaskScheduler);
        }

        private bool CanDeleteFailedImports()
        {
            if (IsLoading)
            {
                return false;
            }

            return FailedImports.Count(failedImportModel => failedImportModel.IsSelected) > 0;
        }

        private void SaveAndImport()
        {
            var selectedFailedImports = FailedImports.Where(failedImportModel => failedImportModel.IsSelected).Select(failedImportModel => failedImportModel).ToList();

            if (selectedFailedImports.Count == 1)
            {
                var failedImport = selectedFailedImports[0];

                IsLoading = true;
                var task = new TaskFactory().StartNew(() => _erroneousImportsRepository.SaveAndImport(failedImport));
                task.ContinueWith(t =>
                {
                    if (t.Exception != null)
                    {
                        CreateError(t.Exception);
                        IsLoading = false;
                    }
                    else
                    {
                        UpdateFailedImports();
                    }
                }, _guiThreadTaskScheduler);
            }
        }

        private void CreateError(Exception e)
        {
            Error = new ErrorViewModel(e);
        }

        private bool CanSaveAndImport()
        {
            if (IsLoading)
            {
                return false;
            }

            return FailedImports.Count(failedImportModel => failedImportModel.IsSelected) == 1;
        }

        private void UpdateFailedImports()
        {
            IsLoading = true;
            var task = new TaskFactory<IList<FailedImportModel>>().StartNew(_erroneousImportsRepository.GetAll);
            task.ContinueWith(t =>
                {
                    if (t.Exception != null)
                    {
                        CreateError(t.Exception);
                    }
                    else
                    {
                        FailedImports.BeginUpdate();
                        FailedImports.Clear();
                        FailedImports.AddRange(t.Result);
                        FailedImports.EndUpdate();
                    }
                    IsLoading = false;
                }, _guiThreadTaskScheduler);
        }

        private bool CanUpdateFailedImports()
        {
            return IsLoading == false;
        }

        /// <summary>
        /// Calls the method RaiseCanExecuteChanged on all DelegateCommands in this class.
        /// </summary>
        private void RaiseCanExecuteChangedOnCommands()
        {
            var propertyInfos = new List<PropertyInfo>(GetType().GetProperties());

            foreach (PropertyInfo propertyInfo in propertyInfos)
            {
                if (propertyInfo.PropertyType.IsSubclassOf(typeof(DelegateCommandBase)))
                {
                    var delegateCommand = (DelegateCommandBase)propertyInfo.GetValue(this, null);
                    delegateCommand.RaiseCanExecuteChanged();
                }
            }
        }

        #region INotifyPropertyChanged

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion INotifyPropertyChanged
    }
}
