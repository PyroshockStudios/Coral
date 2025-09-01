using Coral.Managed.Interop;

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Coral.Managed;

using static ManagedHost;

internal enum ManagedType
{
    Unknown,

    SByte,
    Byte,
    Short,
    UShort,
    Int,
    UInt,
    Long,
    ULong,

    Float,
    Double,

    Bool,

    String,

    Pointer
};

internal static class ManagedObject
{

    [UnmanagedCallersOnly]
    internal static unsafe IntPtr CreateObject(int InTypeID, Bool32 InWeakRef, IntPtr InParameters,
        ManagedType* InParameterTypes, int InParameterCount, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            if (!TypeInterface.s_CachedTypes.TryGetValue(InTypeID, out var type))
            {
                LogMessage($"Failed to find type with id '{InTypeID}'.", MessageLevel.Error);
                return IntPtr.Zero;
            }

            ConstructorInfo? constructor = null;

            var currentType = type;
            while (currentType != null)
            {
                ReadOnlySpan<ConstructorInfo> constructors = currentType.GetConstructors(BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance);

                constructor = TypeInterface.FindSuitableMethod(".ctor", InParameterTypes, InParameterCount, constructors);

                if (constructor != null)
                    break;

                currentType = currentType.BaseType;
            }

            if (constructor == null)
            {
                LogMessage($"Failed to find constructor for type {Util.TypeNameOrNull(type)} with {InParameterCount} parameters.", MessageLevel.Error);
                return IntPtr.Zero;
            }

            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, constructor);

            object? result = null;

            if (type == null)
            {
                LogMessage($"Failed to instantiate type {Util.TypeNameOrNull(type)}.", MessageLevel.Error);
                return IntPtr.Zero;
            }

            if (currentType != type || parameters == null)
            {
                result = TypeInterface.CreateInstance(type);

                if (currentType != type)
                    constructor.Invoke(result, parameters);
            }
            else
            {
                result = TypeInterface.CreateInstance(type, parameters);
            }

            if (result == null)
            {
                LogMessage($"Failed to instantiate type {Util.TypeNameOrNull(type)}.", MessageLevel.Error);
            }

            var handle = GCHandle.Alloc(result, InWeakRef ? GCHandleType.Weak : GCHandleType.Normal);
#if DEBUG
            AssemblyLoader.RegisterHandle(type.Assembly, handle);
#endif
            return GCHandle.ToIntPtr(handle);
        }
        catch (Exception ex)
        {
            if (exception != null)
            {
                var handle = GCHandle.Alloc(ex, GCHandleType.Normal);
#if DEBUG
                AssemblyLoader.RegisterHandle(ex.GetType().Assembly, handle);
#endif
                *exception = GCHandle.ToIntPtr(handle);
            }
            else
            {

                HandleException(ex);
            }
            return IntPtr.Zero;
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe IntPtr CopyObject(IntPtr InObjectHandle)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InObjectHandle).Target;

            if (target == null)
            {
                LogMessage($"Cannot copy object with handle {InObjectHandle}. Target was null.", MessageLevel.Error);
                return IntPtr.Zero;
            }

            var handle = GCHandle.Alloc(target, GCHandleType.Normal);
#if DEBUG
            var type = target.GetType();
            AssemblyLoader.RegisterHandle(type.Assembly, handle);
#endif
            return GCHandle.ToIntPtr(handle);
        }
        catch (Exception ex)
        {
            HandleException(ex);
            return IntPtr.Zero;
        }
    }

    [UnmanagedCallersOnly]
    internal static void DestroyObject(IntPtr InObjectHandle)
    {
        try
        {
            GCHandle handle = GCHandle.FromIntPtr(InObjectHandle);
#if DEBUG
            var type = handle.Target?.GetType();
            if (type is not null)
            {
                AssemblyLoader.DeregisterHandle(type.Assembly, handle);
            }
#endif
            handle.Free();
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void InvokeStaticMethod(int InType, int InMethod, IntPtr InParameters,
        ManagedType* InParameterTypes, int InParameterCount, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            if (!TypeInterface.s_CachedMethods.TryGetValue(InMethod, out var methodInfo) || methodInfo == null)
            {
                LogMessage($"Cannot invoke method id={InMethod} as it does not exist.", MessageLevel.Error);
                return;
            }
            if (!TypeInterface.s_CachedTypes.TryGetValue(InType, out var type) || type == null)
            {
                LogMessage($"Cannot invoke method {methodInfo.Name} on a null type.", MessageLevel.Error);
                return;
            }

            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, methodInfo);

            methodInfo.Invoke(null, parameters);
        }
        catch (Exception ex)
        {
            if (exception != null)
            {
                var handle = GCHandle.Alloc(ex, GCHandleType.Normal);
#if DEBUG
                AssemblyLoader.RegisterHandle(ex.GetType().Assembly, handle);
#endif
                *exception = GCHandle.ToIntPtr(handle);
            }
            else
            {

                HandleException(ex);
            }
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void InvokeStaticMethodRet(int InType, int InMethod, IntPtr InParameters,
        ManagedType* InParameterTypes, int InParameterCount, IntPtr InResultStorage, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            if (!TypeInterface.s_CachedMethods.TryGetValue(InMethod, out var methodInfo) || methodInfo == null)
            {
                LogMessage($"Cannot invoke method id={InMethod} as it does not exist.", MessageLevel.Error);
                return;
            }
            if (!TypeInterface.s_CachedTypes.TryGetValue(InType, out var type) || type == null)
            {
                LogMessage($"Cannot invoke method {methodInfo.Name} on a null type.", MessageLevel.Error);
                return;
            }


            var methodParameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, methodInfo);

            object? value = methodInfo.Invoke(null, methodParameters);

            if (value == null)
                return;

            Marshalling.MarshalReturnValue(null, value, methodInfo, InResultStorage);
        }
        catch (Exception ex)
        {
            if (exception != null)
            {
                var handle = GCHandle.Alloc(ex, GCHandleType.Normal);
#if DEBUG
                AssemblyLoader.RegisterHandle(ex.GetType().Assembly, handle);
#endif
                *exception = GCHandle.ToIntPtr(handle);
            }
            else
            {

                HandleException(ex);
            }
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void InvokeMethod(IntPtr InObjectHandle, int InMethod, IntPtr InParameters,
        ManagedType* InParameterTypes, int InParameterCount, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            if (!TypeInterface.s_CachedMethods.TryGetValue(InMethod, out var methodInfo) || methodInfo == null)
            {
                LogMessage($"Cannot invoke method id={InMethod} as it does not exist.", MessageLevel.Error);
                return;
            }
            var target = GCHandle.FromIntPtr(InObjectHandle).Target;

            if (target == null)
            {
                LogMessage($"Cannot invoke method {methodInfo.Name} on a null type.", MessageLevel.Error);
                return;
            }
            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, methodInfo);

            methodInfo.Invoke(target, parameters);
        }
        catch (Exception ex)
        {
            if (exception != null)
            {
                var handle = GCHandle.Alloc(ex, GCHandleType.Normal);
#if DEBUG
                AssemblyLoader.RegisterHandle(ex.GetType().Assembly, handle);
#endif
                *exception = GCHandle.ToIntPtr(handle);
            }
            else
            {

                HandleException(ex);
            }
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void InvokeMethodRet(IntPtr InObjectHandle, int InMethod, IntPtr InParameters,
        ManagedType* InParameterTypes, int InParameterCount, IntPtr InResultStorage, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            if (!TypeInterface.s_CachedMethods.TryGetValue(InMethod, out var methodInfo) || methodInfo == null)
            {
                LogMessage($"Cannot invoke method id={InMethod} as it does not exist.", MessageLevel.Error);
                return;
            }
            var target = GCHandle.FromIntPtr(InObjectHandle).Target;

            if (target == null)
            {
                LogMessage($"Cannot invoke method {methodInfo.Name} on a null type.", MessageLevel.Error);
                return;
            }

            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, methodInfo);
            object? value = methodInfo.Invoke(target, parameters);

            if (value == null)
                return;

            Marshalling.MarshalReturnValue(target, value, methodInfo, InResultStorage);
        }
        catch (Exception ex)
        {
            if (exception != null)
            {
                var handle = GCHandle.Alloc(ex, GCHandleType.Normal);
#if DEBUG
                AssemblyLoader.RegisterHandle(ex.GetType().Assembly, handle);
#endif
                *exception = GCHandle.ToIntPtr(handle);
            }
            else
            {

                HandleException(ex);
            }
        }
    }

    [UnmanagedCallersOnly]
    internal static void SetFieldValue(IntPtr InTarget, NativeString InFieldName, IntPtr InValue)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot set value of field {InFieldName} on object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            var targetType = target.GetType();
            var fieldInfo = targetType.GetField(InFieldName!, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

            if (fieldInfo == null)
            {
                LogMessage($"Failed to find field '{InFieldName}' in type '{targetType.FullName}'.", MessageLevel.Error);
                return;
            }

            if (fieldInfo.FieldType == typeof(string))
            {
                object? fieldValue = Marshalling.MarshalPointer(InValue, typeof(NativeString));

                if (fieldValue == null)
                {
                    LogMessage($"Failed to get field '{InFieldName}' value in type '{targetType.FullName}'.", MessageLevel.Error);
                    return;
                }

                NativeString value = (NativeString)fieldValue;
                fieldInfo.SetValue(target, value.ToString());
            }
            else if (fieldInfo.FieldType == typeof(bool))
            {
                object? fieldValue = Marshalling.MarshalPointer(InValue, typeof(Bool32));

                if (fieldValue == null)
                {
                    LogMessage($"Failed to get field '{InFieldName}' value in type '{targetType.FullName}'.", MessageLevel.Error);
                    return;
                }

                Bool32 value = (Bool32)fieldValue;
                fieldInfo.SetValue(target, (bool)value);
            }
            else
            {
                object? value = Marshalling.MarshalPointer(InValue, fieldInfo.FieldType);

                fieldInfo.SetValue(target, value);
            }
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }

    [UnmanagedCallersOnly]
    internal static void GetFieldValue(IntPtr InTarget, NativeString InFieldName, IntPtr OutValue)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot get value of field {InFieldName} from object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            var targetType = target.GetType();
            var fieldInfo = targetType.GetField(InFieldName!, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

            if (fieldInfo == null)
            {
                LogMessage($"Failed to find field '{InFieldName}' in type '{targetType.FullName}'.", MessageLevel.Error);
                return;
            }

            // Handles strings gracefully internally.
            Marshalling.MarshalReturnValue(target, fieldInfo.GetValue(target), fieldInfo, OutValue);
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void SetPropertyValue(IntPtr InTarget, NativeString InPropertyName, IntPtr InValue, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot set value of property {InPropertyName} on object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            var targetType = target.GetType();
            var propertyInfo = targetType.GetProperty(InPropertyName!, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

            if (propertyInfo == null)
            {
                LogMessage($"Failed to find property '{InPropertyName}' in type '{targetType.FullName}'", MessageLevel.Error);
                return;
            }

            if (propertyInfo.SetMethod == null)
            {
                LogMessage($"Cannot set value of property '{InPropertyName}'. No setter was found.", MessageLevel.Error);
                return;
            }

            object? value = Marshalling.MarshalPointer(InValue, propertyInfo.PropertyType);
            propertyInfo.SetValue(target, value);
        }
        catch (Exception ex)
        {
            if (exception != null)
            {
                var handle = GCHandle.Alloc(ex, GCHandleType.Normal);
#if DEBUG
                AssemblyLoader.RegisterHandle(ex.GetType().Assembly, handle);
#endif
                *exception = GCHandle.ToIntPtr(handle);
            }
            else
            {

                HandleException(ex);
            }
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void GetPropertyValue(IntPtr InTarget, NativeString InPropertyName, IntPtr OutValue, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot get value of property '{InPropertyName}' from object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            var targetType = target.GetType();
            var propertyInfo = targetType.GetProperty(InPropertyName!, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

            if (propertyInfo == null)
            {
                LogMessage($"Failed to find property '{InPropertyName}' in type '{targetType.FullName}'.", MessageLevel.Error);
                return;
            }

            if (propertyInfo.GetMethod == null)
            {
                LogMessage($"Cannot get value of property '{InPropertyName}'. No getter was found.", MessageLevel.Error);
                return;
            }

            Marshalling.MarshalReturnValue(target, propertyInfo.GetValue(target), propertyInfo, OutValue);
        }
        catch (Exception ex)
        {
            if (exception != null)
            {
                var handle = GCHandle.Alloc(ex, GCHandleType.Normal);
#if DEBUG
                AssemblyLoader.RegisterHandle(ex.GetType().Assembly, handle);
#endif
                *exception = GCHandle.ToIntPtr(handle);
            }
            else
            {

                HandleException(ex);
            }
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void GetObjectTypeId(IntPtr InTarget, int* typeId)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot get type of object. Target was null.", MessageLevel.Error);
                return;
            }

            *typeId = TypeInterface.s_CachedTypes.Add(target.GetType());
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }
}
