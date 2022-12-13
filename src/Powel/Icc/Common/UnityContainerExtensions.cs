using System;
using System.Collections.Generic;
using Microsoft.Practices.Unity;
using log4net;

namespace Powel.Icc.Common
{
    public static class UnityContainerExtensions
    {
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public static IUnityContainer RegisterModule<T>(this IUnityContainer container)  where T : class, IUnityContainerModule, new()
        {
            var module = new T();
            module.Register(container);
            return container;
        }

        public static IUnityContainer RegisterModule(this IUnityContainer container, IUnityContainerModule module)
        {
            if(module != null)
            {
                module.Register(container);
            }
            return container;
        }

        public static IUnityContainer RegisterFactory<T>(this IUnityContainer container)
        {
            container.RegisterType<Func<T>>(new InjectionFactory(c => c.Factory<T>()));
            container.RegisterType<Func<string, T>>(new InjectionFactory(c => c.FactoryForSingle<T>()));
            container.RegisterType<Func<IEnumerable<T>>>(new InjectionFactory(c => c.FactoryForAll<T>()));
            return container;
        }

        private static Func<T> Factory<T>(this IUnityContainer container)
        {
            return () =>
                {
                    T instance;
                    try
                    {
                        instance = container.Resolve<T>();
                    }
                    catch (Exception ex)
                    {
                        Log.Warn("Failed to resolve object of type " + typeof (T) + ".", ex);
                        instance = default(T);
                    }
                    return instance;
                };
        }

        private static Func<string,T> FactoryForSingle<T>(this IUnityContainer container)
        {
            return name =>
                {
                    T instance;
                    try
                    {
                        instance = container.Resolve<T>(name);
                    }
                    catch (ResolutionFailedException ex)
                    {
                        Log.Warn("Failed to resolve the object of type " + typeof(T) + " with name " + name + ".", ex);
                        instance = default(T);
                    }
                    return instance;
                };
        }

        private static Func<IEnumerable<T>> FactoryForAll<T>(this IUnityContainer container)
        {
            return () =>
                {
                    IEnumerable<T> instances;
                    try
                    {
                        instances = container.ResolveAll<T>();
                    }
                    catch (ResolutionFailedException ex)
                    {
                        Log.Warn("Failed to resolve all objects of type " + typeof (T) + ".", ex);
                        instances = null;
                    }
                    return instances;
                };
        }

        public static bool RegisterTypeIfNotRegistered<T,TU>(this IUnityContainer container) where TU : T
        {
            bool registered = false;

            if(!container.IsRegistered<T>())
            {
                container.RegisterType<T, TU>();
                registered = true;
            }

            return registered;
        }
        
        public static bool RegisterTypeIfNotRegistered<T,TU>(this IUnityContainer container, string name) where TU : T
        {
            bool registered = false;

            if(!container.IsRegistered<T>(name))
            {
                container.RegisterType<T, TU>(name);
                registered = true;
            }

            return registered;
        }
    }
}
