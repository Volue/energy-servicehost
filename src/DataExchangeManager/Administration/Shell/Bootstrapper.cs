using System.Windows;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.UnityExtensions;

namespace DataExchange.Administration.Shell
{
    public class Bootstrapper : UnityBootstrapper
    {
        protected override DependencyObject CreateShell()
        {
            var view = new ShellView
                {
                    DataContext = new ShellViewModel()
                };

            view.Show();

            return view;
        }

        protected override IModuleCatalog CreateModuleCatalog()
        {
            var catalog = new ConfigurationModuleCatalog();

            catalog.AddModule(typeof (ImportModule.ImportModule));

            return catalog;
        }
    }
}
