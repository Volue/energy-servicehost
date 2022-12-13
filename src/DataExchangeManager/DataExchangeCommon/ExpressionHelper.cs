using System;
using System.Linq.Expressions;
using System.Reflection;

namespace Powel.Icc.Messaging.DataExchangeCommon
{
    public static class ExpressionHelper
    {
        public static string GetPropertyName<TSource, TProperty>(TSource source, Expression<Func<TSource, TProperty>> propertyLambda)
        {
            Type type = typeof(TSource);
            var propInfo = GetPropertyInfoFromExpression(propertyLambda);

            if (type != propInfo.ReflectedType && propInfo.ReflectedType != null && !type.IsSubclassOf(propInfo.ReflectedType))
                throw new ArgumentException($"Expresion '{propertyLambda}' refers to a property that is not from type {type}.");

            return $"{type.Name}.{propInfo.Name}";
        }

        public static string GetPropertyName(Expression<Func<object>> propertyLambda)
        {
            var propInfo = GetPropertyInfoFromExpression(propertyLambda);
            return propInfo.Name;
        }

        private static PropertyInfo GetPropertyInfoFromExpression<T1>(Expression<Func<T1>> propertyLambda)
        {
            return propertyLambda.Body is UnaryExpression
                ? PropertyInfoFromUnaryExpression(propertyLambda.Body as UnaryExpression, propertyLambda)
                : PropertyInfoFromMemberExpression(propertyLambda.Body as MemberExpression, propertyLambda);
        }

        public static PropertyInfo GetPropertyInfoFromExpression<T1, T2>(this Expression<Func<T1,T2>> propertyLambda)
        {
            return propertyLambda.Body is UnaryExpression
                ? PropertyInfoFromUnaryExpression(propertyLambda.Body as UnaryExpression, propertyLambda)
                : PropertyInfoFromMemberExpression(propertyLambda.Body as MemberExpression, propertyLambda);
        }

        private static PropertyInfo PropertyInfoFromMemberExpression(MemberExpression member, Expression  propertyLambda)
        {
            if (member == null)
                throw new ArgumentException($"Expression '{propertyLambda}' refers to a method, not a property.");

            PropertyInfo propInfo = member.Member as PropertyInfo;
            if (propInfo == null)
                throw new ArgumentException($"Expression '{propertyLambda}' refers to a field, not a property.");
            return propInfo;
        }
        private static PropertyInfo PropertyInfoFromUnaryExpression(UnaryExpression unary, Expression propertyLambda)
        {
            if (unary == null)
                throw new ArgumentException($"Expression '{propertyLambda}' does not refer to a unary expression.");
            MemberExpression me = (MemberExpression)unary.Operand;

            if (me == null)
            {
                throw new ArgumentException($"Expression '{unary.Operand}' is not a member expression.");
            }
            return PropertyInfoFromMemberExpression(me, unary);
        }
        //public static PropertyInfo GetPropertyInfoFromExpression<T1, T2>(this Expression<Func<T1, T2>> propertyLambda)
        //{
        //    return propertyLambda.Body is UnaryExpression
        //        ? PropertyInfoFromUnaryExpression(propertyLambda.Body as UnaryExpression, propertyLambda)
        //        : PropertyInfoFromMemberExpression(propertyLambda.Body as MemberExpression, propertyLambda);
        //}
    }
}
