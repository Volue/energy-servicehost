using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Powel.Extensions
{
    public static class LinqExtensions
    {
        public delegate TReturnType Action<TInputType, TReturnType>(TInputType obj);

        public static IEnumerable<TReturnType> ForEach<TInputType, TReturnType>(this IEnumerable<TInputType> collection, Action<TInputType, TReturnType> action)
        {
            if (collection == null)
            {
                throw new ArgumentNullException("collection");
            }

            if (action == null)
            {
                throw new ArgumentNullException("action");
            }

            return GetResultsFromActionOnItemsInCollection(collection, action);
        }

        /// <summary>
        ///   should be called Map, or Translate
        /// </summary>
        private static IEnumerable<TReturnType> GetResultsFromActionOnItemsInCollection<TInputType, TReturnType>(IEnumerable<TInputType> collection, Action<TInputType, TReturnType> action)
        {
            var result = new Collection<TReturnType>();

            foreach (var item in collection)
            {
                result.Add(action(item));
            }

            return result;
        }

        public static void ForEach<T>(this IEnumerable<T> collection, Action<T> action)
        {
            if (collection == null)
            {
                throw new ArgumentNullException("collection");
            }

            if (action == null)
            {
                throw new ArgumentNullException("action");
            }

            ExecuteActionOnItemsInCollection(collection, action);
        }

        private static void ExecuteActionOnItemsInCollection<T>(IEnumerable<T> collection, Action<T> action)
        {
            foreach (var item in collection)
            {
                action(item);
            }
        }

        public static void AddRange<T>(this ICollection<T> collection, IEnumerable<T> values)
        {
            if (collection == null)
            {
                throw new ArgumentNullException("collection");
            }

            if (values == null)
            {
                throw new ArgumentNullException("values");
            }

            foreach (var item in values)
            {
                collection.Add(item);
            }
        }
    }
}
