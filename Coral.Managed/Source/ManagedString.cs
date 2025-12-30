using Coral.Managed.Interop;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics.CodeAnalysis;
using System.Drawing;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Remoting;

namespace Coral.Managed;

using static ManagedHost;

internal static class ManagedString
{
    [UnmanagedCallersOnly]
    internal static unsafe IntPtr CreateNewManagedString(char* InInitialValueUtf16, int InLength)
    {
        try
        {
            string str = InInitialValueUtf16 == null ? new string('\0', InLength) : new string(InInitialValueUtf16, 0, InLength);
            var handle = GCHandle.Alloc(str, GCHandleType.Normal);
            return GCHandle.ToIntPtr(handle);
        }
        catch (Exception ex)
        {
            HandleException(ex);
            return IntPtr.Zero;
        }
    }
    [UnmanagedCallersOnly]
    internal static unsafe int GetStringContents(IntPtr InStringHandle, IntPtr InStringStorage)
    {
        try
        {
            var target = GCHandle.FromIntPtr(InStringHandle).Target;

            if (target == null)
            {
                LogMessage($"Cannot read string with handle {InStringHandle}. Target was null.", MessageLevel.Error);
                return -1;
            }

            string str = (string)target;
            fixed (char* ptr = str)
            {
                long byteCount = str.Length * sizeof(char);
                Buffer.MemoryCopy(ptr, (void*)InStringStorage, byteCount, byteCount);
            }
            return str.Length;
        }
        catch (Exception ex)
        {
            HandleException(ex);
            return -1;
        }
    }
}