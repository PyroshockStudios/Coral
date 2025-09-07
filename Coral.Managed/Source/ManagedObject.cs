using Coral.Managed.Interop;

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Remoting;

namespace Coral.Managed;

using static ManagedHost;

public enum ManagedType : uint
{
    Unknown,

    Void,

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

    Char,

    NativeString,
    NativeArray,
    Object,

    Pointer
};


[StructLayout(LayoutKind.Explicit, Pack = 1)]
internal struct ManagedObjectData
{
    [FieldOffset(0)]
    internal IntPtr m_Handle;
    [FieldOffset(8)]
    internal unsafe void* m_Type;
}
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

            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, InParameterTypes, constructor);

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
            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, InParameterTypes, methodInfo);
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
        ManagedType* InParameterTypes, int InParameterCount, IntPtr InResultStorage, Bool32 InRetIsObject, IntPtr* exception)
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


            var methodParameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, InParameterTypes, methodInfo);

            object? value = methodInfo.Invoke(null, methodParameters);

            if (value == null)
                return;

            if (InRetIsObject)
            {
                Marshal.WriteIntPtr(InResultStorage, GCHandle.ToIntPtr(GCHandle.Alloc(value)));
            }
            else
            {
                Marshalling.MarshalReturnValue(value, methodInfo.ReturnType, InResultStorage);
            }
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

            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, InParameterTypes, methodInfo);

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
        ManagedType* InParameterTypes, int InParameterCount, IntPtr InResultStorage, Bool32 InRetIsObject, IntPtr* exception)
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
            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, InParameterTypes, methodInfo);
            object? value = methodInfo.Invoke(target, parameters);

            if (value == null)
                return;

            if (InRetIsObject)
            {
                Marshal.WriteIntPtr(InResultStorage, GCHandle.ToIntPtr(GCHandle.Alloc(value)));
            }
            else
            {
                Marshalling.MarshalReturnValue(value, methodInfo.ReturnType, InResultStorage);
            }
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
    internal static unsafe void InvokeDelegate(IntPtr InObjectHandle, IntPtr InParameters,
       ManagedType* InParameterTypes, int InParameterCount, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            var target = GCHandle.FromIntPtr(InObjectHandle).Target;
            if (target == null)
            {
                LogMessage($"Cannot invoke null delegate! Ignoring...", MessageLevel.Error);
                return;
            }
            Delegate @delegate = (Delegate)target;
            MethodInfo methodInfo = @delegate.Method;
            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, InParameterTypes, methodInfo);
            @delegate.DynamicInvoke(parameters);
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
    internal static unsafe void InvokeDelegateRet(IntPtr InObjectHandle, IntPtr InParameters,
        ManagedType* InParameterTypes, int InParameterCount, IntPtr InResultStorage, Bool32 InRetIsObject, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            var target = GCHandle.FromIntPtr(InObjectHandle).Target;
            if (target == null)
            {
                LogMessage($"Cannot invoke null delegate! Ignoring...", MessageLevel.Error);
                return;
            }
            Delegate @delegate = (Delegate)target;
            MethodInfo methodInfo = @delegate.Method;
            var parameters = Marshalling.MarshalParameterArray(InParameters, InParameterCount, InParameterTypes, methodInfo);
            object? value = @delegate.DynamicInvoke(parameters);

            if (value == null)
                return;
            if (InRetIsObject)
            {
                Marshal.WriteIntPtr(InResultStorage, GCHandle.ToIntPtr(GCHandle.Alloc(value)));
            }
            else
            {
                Marshalling.MarshalReturnValue(value, methodInfo.ReturnType, InResultStorage);
            }
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
    internal static void SetFieldValue(IntPtr InTarget, int InField, IntPtr InValue, Bool32 InIsObject)
    {
        try
        {
            if (!TypeInterface.s_CachedFields.TryGetValue(InField, out var fieldInfo) || fieldInfo == null)
            {
                LogMessage($"Cannot get field id={InField} as it does not exist.", MessageLevel.Error);
                return;
            }
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot set value of field {fieldInfo.Name} on object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            if (InIsObject)
            {
                fieldInfo.SetValue(target, GCHandle.FromIntPtr(Marshal.ReadIntPtr(InValue)).Target);
            }
            else if (fieldInfo.FieldType == typeof(string))
            {
                object? fieldValue = Marshalling.MarshalPointer(InValue, typeof(NativeString));

                if (fieldValue == null)
                {
                    LogMessage($"Failed to set field '{fieldInfo.Name}' value in type '{target.GetType().FullName}'.", MessageLevel.Error);
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
                    LogMessage($"Failed to set field '{fieldInfo.Name}' value in type '{target.GetType().FullName}'.", MessageLevel.Error);
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
    internal static void GetFieldValue(IntPtr InTarget, int InField, IntPtr OutValue, Bool32 InIsObject)
    {
        try
        {
            if (!TypeInterface.s_CachedFields.TryGetValue(InField, out var fieldInfo) || fieldInfo == null)
            {
                LogMessage($"Cannot get field id={InField} as it does not exist.", MessageLevel.Error);
                return;
            }
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot get value of field {fieldInfo.Name} on object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            object? value = fieldInfo.GetValue(target);
            if (value == null) return;
            if (InIsObject)
            {
                Marshal.WriteIntPtr(OutValue, GCHandle.ToIntPtr(GCHandle.Alloc(target)));
            }
            else
            {
                Marshalling.MarshalReturnValue(value, fieldInfo.FieldType, OutValue);
            }
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }

    [UnmanagedCallersOnly]
    internal static unsafe void SetPropertyValue(IntPtr InTarget, int InProperty, IntPtr InValue, Bool32 InIsObject, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            if (!TypeInterface.s_CachedProperties.TryGetValue(InProperty, out var propertyInfo) || propertyInfo == null)
            {
                LogMessage($"Cannot set property id={InProperty} as it does not exist.", MessageLevel.Error);
                return;
            }
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot set value of property {propertyInfo.Name} on object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            if (propertyInfo.SetMethod == null)
            {
                LogMessage($"Cannot set value of property '{propertyInfo.Name}'. No setter was found.", MessageLevel.Error);
                return;
            }
            if (InIsObject)
            {
                propertyInfo.SetValue(target, GCHandle.FromIntPtr(Marshal.ReadIntPtr(InValue)).Target);
            }
            else if (propertyInfo.PropertyType == typeof(string))
            {
                object? fieldValue = Marshalling.MarshalPointer(InValue, typeof(NativeString));

                if (fieldValue == null)
                {
                    LogMessage($"Failed to set property '{propertyInfo.Name}' value in type '{target.GetType().FullName}'.", MessageLevel.Error);
                    return;
                }

                NativeString value = (NativeString)fieldValue;
                propertyInfo.SetValue(target, value.ToString());
            }
            else if (propertyInfo.PropertyType == typeof(bool))
            {
                object? fieldValue = Marshalling.MarshalPointer(InValue, typeof(Bool32));

                if (fieldValue == null)
                {
                    LogMessage($"Failed to set property '{propertyInfo.Name}' value in type '{target.GetType().FullName}'.", MessageLevel.Error);
                    return;
                }

                Bool32 value = (Bool32)fieldValue;
                propertyInfo.SetValue(target, (bool)value);
            }
            else
            {
                object? value = Marshalling.MarshalPointer(InValue, propertyInfo.PropertyType);

                propertyInfo.SetValue(target, value);
            }
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
    internal static unsafe void GetPropertyValue(IntPtr InTarget, int InProperty, IntPtr OutValue, Bool32 InIsObject, IntPtr* exception)
    {
        if (exception != null) *exception = IntPtr.Zero;
        try
        {
            if (!TypeInterface.s_CachedProperties.TryGetValue(InProperty, out var propertyInfo) || propertyInfo == null)
            {
                LogMessage($"Cannot get property id={InProperty} as it does not exist.", MessageLevel.Error);
                return;
            }
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot get value of property {propertyInfo.Name} on object with handle {InTarget}. Target was null.", MessageLevel.Error);
                return;
            }

            if (propertyInfo.GetMethod == null)
            {
                LogMessage($"Cannot get value of property '{propertyInfo.Name}'. No getter was found.", MessageLevel.Error);
                return;
            }

            object? value = propertyInfo.GetValue(target);
            if (value == null) return;
            if (InIsObject)
            {
                Marshal.WriteIntPtr(OutValue, GCHandle.ToIntPtr(GCHandle.Alloc(value)));
            }
            else
            {
                Marshalling.MarshalReturnValue(value, propertyInfo.PropertyType, OutValue);
            }
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

    [UnmanagedCallersOnly]
    internal static IntPtr GetObjectBoxedValue(IntPtr InValue, int InValueSize, int InTypeID)
    {
        try
        {
            if (!TypeInterface.s_CachedTypes.TryGetValue(InTypeID, out var type) || type == null)
            {
                LogMessage($"Failed to find type with id '{InTypeID}'.", MessageLevel.Error);
                return IntPtr.Zero;
            }
            if (!type.IsValueType)
            {
                LogMessage($"Cannot box non-value type: {type.FullName}", MessageLevel.Error);
                return IntPtr.Zero;
            }
            int expectedSize = Marshal.SizeOf(type);
            if (expectedSize != InValueSize)
            {
                LogMessage($"Size mismatch: expected={expectedSize}, got={InValueSize}", MessageLevel.Error);
                return IntPtr.Zero;
            }
            return Marshalling.MarhsalBoxValue(InValue, type);
        }
        catch (Exception ex)
        {
            HandleException(ex);
            return IntPtr.Zero;
        }
    }

    [UnmanagedCallersOnly]
    internal static void GetObjectUnboxedValue(IntPtr InTarget, IntPtr OutValue)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InTarget).Target;

            if (target == null)
            {
                LogMessage($"Cannot get unboxed value of object. Target was null.", MessageLevel.Error);
                return;
            }

            Type targetType = target.GetType();

            // Only handle blittable value types
            if (!targetType.IsValueType)
            {
                LogMessage($"Target is not a value type. Type: {targetType.FullName}", MessageLevel.Error);
                return;
            }

            Marshalling.MarshalUnboxValue(target, targetType, OutValue);
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }

}
