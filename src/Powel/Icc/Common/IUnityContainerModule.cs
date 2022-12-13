using Microsoft.Practices.Unity;

namespace Powel.Icc.Common
{
    public interface IUnityContainerModule
    {
        void Register(IUnityContainer container);
    }
}
