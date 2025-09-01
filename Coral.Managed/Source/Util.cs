using Coral.Managed.Interop;

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Coral.Managed;

using static Coral.Managed.ManagedObject;
using static ManagedHost;
internal class Util
{
    public readonly struct MethodKey : IEquatable<MethodKey>
    {
        public readonly string TypeName;
        public readonly string Name;
        public readonly ManagedType[] Types;
        public readonly int ParameterCount;

        public MethodKey(string InTypeName, string InName, ManagedType[] InTypes, int InParameterCount)
        {
            TypeName = InTypeName;
            Name = InName;
            Types = InTypes;
            ParameterCount = InParameterCount;
        }

        public override bool Equals([NotNullWhen(true)] object? obj) => obj is MethodKey other && Equals(other);

        bool IEquatable<MethodKey>.Equals(MethodKey other)
        {
            if (TypeName != other.TypeName || Name != other.Name)
                return false;

            for (int i = 0; i < Types.Length; i++)
            {
                if (Types[i] != other.Types[i])
                    return false;
            }

            return ParameterCount == other.ParameterCount;
        }

        public override int GetHashCode()
        {
            // NOTE(Peter): Josh Bloch's Hash (from https://stackoverflow.com/questions/263400/what-is-the-best-algorithm-for-overriding-gethashcode)
            unchecked
            {
                int hash = 17;

                hash = hash * 23 + TypeName.GetHashCode();
                hash = hash * 23 + Name.GetHashCode();
                foreach (var type in Types)
                    hash = hash * 23 + type.GetHashCode();
                hash = hash * 23 + ParameterCount.GetHashCode();

                return hash;
            }
        }
    }

    internal static Dictionary<MethodKey, MethodInfo> s_CachedMethods = new Dictionary<MethodKey, MethodInfo>();

    internal static string TypeNameOrNull(Type? InType)
    {
        if (InType != null)
        {
            return InType.FullName != null ? InType.FullName : "<null>";
        }
        else return "<null>";
    }

    internal static string NativeStringOrNull(NativeString? InString)
    {
        if (InString == null) return "<null>";

        string? ret = InString.ToString();

        return ret != null ? ret : "<null>";
    }
    internal static unsafe MethodInfo? TryGetMethodInfo(Type InType, string? InMethodName, ManagedType* InParameterTypes, int InParameterCount, BindingFlags InBindingFlags)
    {
        MethodInfo? methodInfo = null;

        var parameterTypes = new ManagedType[InParameterCount];

        if (InMethodName == null) return null;

        unsafe
        {
            fixed (ManagedType* parameterTypesPtr = parameterTypes)
            {
                ulong size = sizeof(ManagedType) * (ulong)InParameterCount;
                Buffer.MemoryCopy(InParameterTypes, parameterTypesPtr, size, size);
            }
        }

        var methodKey = new MethodKey(TypeNameOrNull(InType), InMethodName, parameterTypes, InParameterCount);

        if (!s_CachedMethods.TryGetValue(methodKey, out methodInfo))
        {
            List<MethodInfo> methods = new(InType.GetMethods(InBindingFlags));

            Type? baseType = InType.BaseType;
            while (baseType != null)
            {
                methods.AddRange(baseType.GetMethods(InBindingFlags));
                baseType = baseType.BaseType;
            }

            methodInfo = TypeInterface.FindSuitableMethod<MethodInfo>(InMethodName, InParameterTypes, InParameterCount, CollectionsMarshal.AsSpan(methods));

            if (methodInfo == null)
            {
                LogMessage($"Failed to find method '{InMethodName}' for type {InType.FullName} with {InParameterCount} parameters.", MessageLevel.Error);
                return null;
            }

            s_CachedMethods.Add(methodKey, methodInfo);
        }

        return methodInfo;
    }

}
