using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace Powel.Wpf.Common
{
    public class ExtendedObservableCollection<T> : ObservableCollection<T>
    {
        private bool _suppressNotification;

        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            if (_suppressNotification)
            {
                return;
            }

            base.OnCollectionChanged(e);
        }

        public void BeginUpdate()
        {
            _suppressNotification = true;
        }

        public void EndUpdate()
        {
            _suppressNotification = false;

            OnPropertyChanged(new PropertyChangedEventArgs("Count"));
            OnPropertyChanged(new PropertyChangedEventArgs("Item[]"));
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }
        
        //public void UpdateItem(T newItem, T oldItem, int index)
        //{
        //    OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, newItem, oldItem, index));
        //}

        public void AddRange(IEnumerable<T> items)
        {
            if (items == null)
            {
                return;
            }

            BeginUpdate();
            
            foreach (var item in items)
            {
                Add(item);
            }

            EndUpdate();
        }

        //public void RemoveRange(IEnumerable<T> items)
        //{
        //    if (items == null)
        //    {
        //        return;
        //    }

        //    BeginUpdate();

        //    foreach (var item in items)
        //    {
        //        Remove(item);
        //    }

        //    EndUpdate();
        //}

        public void Reset(IEnumerable<T> items)
        {
            BeginUpdate();
            Clear();

            if (items != null)
            {
                foreach (var item in items)
                {
                    Add(item);
                }
            }

            EndUpdate();
        }
    }
}