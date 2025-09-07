using Coral.Managed.Interop;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Remoting;

namespace Coral.Managed;

using static ManagedHost;

internal static class ManagedArray
{
    [UnmanagedCallersOnly]
    internal static IntPtr CreateNewManagedArray(int InLength, int InTypeID)
    {
        try
        {
            // Lookup the type by ID
            if (!TypeInterface.s_CachedTypes.TryGetValue(InTypeID, out var type) || type == null)
            {
                LogMessage($"Failed to find type with id '{InTypeID}'.", MessageLevel.Error);
                return IntPtr.Zero;
            }

            var array = Array.CreateInstance(type, InLength);
            var handle = GCHandle.Alloc(array, GCHandleType.Normal);
            return GCHandle.ToIntPtr(handle);
        }
        catch (Exception ex)
        {
            HandleException(ex);
            return IntPtr.Zero;
        }
    }
    [UnmanagedCallersOnly]
    internal static void SetArrayElement(IntPtr InArrayHandle, IntPtr InValue, Bool32 InIsObject, int InIndex)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InArrayHandle).Target;

            if (target == null)
            {
                LogMessage($"Cannot modify array object with handle {InArrayHandle}. Target was null.", MessageLevel.Error);
                return;
            }


            Array array = (Array)target;
            object? value = null;
            if (InIsObject)
            {
                value = GCHandle.FromIntPtr(Marshal.ReadIntPtr(InValue)).Target;
            }
            else
            {
                Type? elementType = array.GetType().GetElementType();
                value = Marshalling.MarshalPointer(InValue, elementType!);
            }
            array.SetValue(value, InIndex);
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }

    [UnmanagedCallersOnly]
    internal static void GetArrayElement(IntPtr InArrayHandle, IntPtr InResultStorage, Bool32 InIsObject, int InIndex)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InArrayHandle).Target;

            if (target == null)
            {
                LogMessage($"Cannot read array object with handle {InArrayHandle}. Target was null.", MessageLevel.Error);
                return;
            }


            Array array = (Array)target;
            object? value = array.GetValue(InIndex);
            if (InIsObject)
            {
                Marshal.WriteIntPtr(InResultStorage, GCHandle.ToIntPtr(GCHandle.Alloc(value, GCHandleType.Normal)));
            }
            else
            {
                Type? elementType = array.GetType().GetElementType();
                Marshalling.MarshalReturnValue(value, elementType!, InResultStorage);
            }
        }
        catch (Exception ex)
        {
            HandleException(ex);
        }
    }
}