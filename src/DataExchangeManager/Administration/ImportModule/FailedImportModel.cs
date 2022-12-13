using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using DataExchange.Administration.ImportModule.Annotations;

namespace DataExchange.Administration.ImportModule
{
    public class FailedImportModel : INotifyPropertyChanged
    {
        private bool _isSelected;

        public string Timestamp { get; set; }

        public bool IsAnyPropertyModified
        {
            get
            {
                foreach(FailedImportProperty fip in ImportProperties)
                {
                    if(fip.IsValueModified)
                    {
                        return true;
                    }
                }
                return false;
            }
        }

        public void RevertPropertyModifications()
        {
            foreach (FailedImportProperty fip in ImportProperties)
            {
                fip.RevertChanges();
            }
        }

        public List<FailedImportProperty> ImportProperties { get; set; }

        public string ExternalReference
        {
            get { return GetImportPropertyValue(FailedImportPropertyName.ExternalReference); }
        }

        public string Protocol
        {
            get { return GetImportPropertyValue(FailedImportPropertyName.Protocol); }
        }

        public string Country
        {
            get { return GetImportPropertyValue(FailedImportPropertyName.Country); }
        }

        public string SubAddress
        {
            get { return GetImportPropertyValue(FailedImportPropertyName.SubAddress); }
        }

        public string ProductCode
        {
            get { return GetImportPropertyValue(FailedImportPropertyName.ProductCode); }
        }

        public string SenderName
        {
            get { return GetImportPropertyValue(FailedImportPropertyName.SenderName); }
        }

        public string ExternalText
        {
            get { return GetImportPropertyValue(FailedImportPropertyName.ExternalText); }
        }

        private string GetImportPropertyValue(FailedImportPropertyName propertyName)
        {
            if (ImportProperties == null)
            {
                return string.Empty;
            }

            var importProperty = ImportProperties.FirstOrDefault(failedImportProperty => failedImportProperty.PropertyName == propertyName);

            if (importProperty == null)
            {
                return string.Empty;
            }

            return importProperty.PropertyValue;
        }

        public string ImportMessage { get; set; }

        public string InternalId { get; set; }

        public bool IsSelected
        {
            get
            {
                return _isSelected;
            }
            set
            {
                _isSelected = value;
                OnPropertyChanged("IsSelected");
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
