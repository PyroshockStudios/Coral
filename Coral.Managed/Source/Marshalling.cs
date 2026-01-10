using Coral.Managed.Interop;

using System;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Runtime.InteropServices;

namespace Coral.Managed;

public static class Marshalling
{
#pragma warning disable 0649
    // This needs to map to Coral::NativeArray, hence the unused ArrayHandle
    struct ValueArrayContainer
    {
        public IntPtr Data;
        public IntPtr ArrayHandle;
        public int Length;
    };

    // This needs to map to Coral::NativeArray, hence the unused ArrayHandle
    struct ObjectArrayContainer
    {
        public IntPtr Data;
        public IntPtr ArrayHandle;
        public int Length;
    };

    private struct ArrayObject
    {
        public IntPtr Handle;
        public IntPtr Padding;
    }
#pragma warning restore 0649

    public static void MarshalReturnValue(object? InValue, Type? InElementType, IntPtr OutValue)
    {
        if (InElementType == null)
            return;

        if (InElementType.IsSZArray)
        {
            var pinnedArray = GCHandle.Alloc(InValue, GCHandleType.Pinned);
            Marshal.WriteIntPtr(OutValue, pinnedArray.AddrOfPinnedObject());
            pinnedArray.Free();
        }
        else if (InElementType == typeof(string) && InValue != null)
        {
            NativeString nativeString = (NativeString)(string)InValue;
            Marshal.StructureToPtr(nativeString, OutValue, false);
        }
        else if (InElementType == typeof(bool) && InValue != null)
        {
            Bool32 value = (Bool32)(bool)InValue;
            Marshal.StructureToPtr(value, OutValue, false);
        }
        else if (InElementType == typeof(NativeString) && InValue != null)
        {
            NativeString nativeString = (NativeString)InValue;
            Marshal.StructureToPtr((NativeString)InValue, OutValue, false);
        }
        else if (InElementType.IsPointer)
        {
            unsafe
            {
                if (InValue == null)
                {
                    Marshal.WriteIntPtr(OutValue, IntPtr.Zero);
                }
                else
                {
                    void* valuePointer = Pointer.Unbox(InValue);
                    Buffer.MemoryCopy(&valuePointer, OutValue.ToPointer(), IntPtr.Size, IntPtr.Size);
                }
            }
        }
        else if (InElementType.IsClass)
        {
            unsafe
            {
                ((ManagedObjectData*)OutValue.ToPointer())->m_Handle =
                    GCHandle.ToIntPtr(GCHandle.Alloc(InValue, GCHandleType.Normal));
            }
        }
        else
        {
            int valueSize = InElementType.IsEnum ? Marshal.SizeOf(Enum.GetUnderlyingType(InElementType)) : Marshal.SizeOf(InElementType);
            var handle = GCHandle.Alloc(InValue, GCHandleType.Pinned);

            unsafe
            {
                Buffer.MemoryCopy(handle.AddrOfPinnedObject().ToPointer(), OutValue.ToPointer(), valueSize, valueSize);
            }

            handle.Free();
        }
    }

    public static object? MarshalArray(IntPtr InArray, Type? InElementType)
    {
        if (InElementType == null)
            return null;

        Array? result;

        if (InElementType.IsValueType)
        {
            var arrayContainer = MarshalPointer<ValueArrayContainer>(InArray);

            result = Array.CreateInstance(InElementType, arrayContainer.Length);

            int elementSize = Marshal.SizeOf(InElementType);

            unsafe
            {
                for (int i = 0; i < arrayContainer.Length; i++)
                {
                    IntPtr source = (IntPtr)(((byte*)arrayContainer.Data.ToPointer()) + (i * elementSize));
                    result.SetValue(Marshal.PtrToStructure(source, InElementType), i);
                }
            }
        }
        else
        {
            var arrayContainer = MarshalPointer<ObjectArrayContainer>(InArray);

            result = Array.CreateInstance(InElementType, arrayContainer.Length);

            unsafe
            {
                for (int i = 0; i < arrayContainer.Length; i++)
                {
                    IntPtr source = (IntPtr)(((byte*)arrayContainer.Data.ToPointer()) + (i * Marshal.SizeOf<ArrayObject>()));
                    var managedObject = MarshalPointer<ArrayObject>(source);
                    var target = GCHandle.FromIntPtr(managedObject.Handle).Target;
                    result.SetValue(target, i);
                }
            }
        }

        return result;
    }

    /*public static void CopyArrayToBuffer(GCHandle InArrayHandle, NativeArray? InArray, Type? InElementType)
	{
		if (InArray == null || InElementType == null)
			return;

		var elementSize = Marshal.SizeOf(InElementType);
		int byteLength = InArray.Length * elementSize;

		int offset = 0;

		foreach (var elem in InArray)
		{
			var elementHandle = GCHandle.Alloc(elem, GCHandleType.Pinned);

			unsafe
			{
				Buffer.MemoryCopy(elementHandle.AddrOfPinnedObject().ToPointer(), ((byte*)mem.ToPointer()) + offset, elementSize, elementSize);
			}

			offset += elementSize;

			elementHandle.Free();
		}

		ArrayContainer container = new()
		{
			Data = mem,
			Length = InArray.Length
		};

		var handle = GCHandle.Alloc(container, GCHandleType.Pinned);
		
		unsafe
		{
			Buffer.MemoryCopy(handle.AddrOfPinnedObject().ToPointer(), InBuffer.ToPointer(), Marshal.SizeOf<ArrayContainer>(), Marshal.SizeOf<ArrayContainer>());
		}

		handle.Free();
	}*/

    public static object? MarshalPointer(IntPtr InValue, Type InType)
    {
        if (InType.IsPointer || InType == typeof(IntPtr))
            return InValue;

        // NOTE(Peter): Marshal.PtrToStructure<bool> doesn't seem to work
        //				instead we have to read it as a single byte and check the raw value
        if (InType == typeof(bool))
            return Marshal.PtrToStructure<byte>(InValue) > 0;

        if (InType == typeof(string))
        {
            var nativeString = Marshal.PtrToStructure<NativeString>(InValue);
            return nativeString.ToString();
        }
        else if (InType == typeof(NativeString))
        {
            return Marshal.PtrToStructure<NativeString>(InValue);
        }

        if (InType.IsSZArray)
            return MarshalArray(InValue, InType.GetElementType());

        if (InType.IsGenericType)
        {
            if (InType == typeof(NativeArray<>).MakeGenericType(InType.GetGenericArguments().First()))
            {
                var elements = Marshal.ReadIntPtr(InValue, 0);
                var elementCount = Marshal.ReadInt32(InValue, Marshal.SizeOf<IntPtr>());
                var genericType = typeof(NativeArray<>).MakeGenericType(InType.GetGenericArguments().First());
                return TypeInterface.CreateInstance(genericType, elements, elementCount);
            }
        }

        if (InType.IsValueType)
        {
            // Handle primitive/struct/etc.
            return Marshal.PtrToStructure(InValue, InType);
        }

        // Everything else is a reference type: class, interface, delegate, string, array
        // Use GCHandle to get the real object
        return GCHandle.FromIntPtr(Marshal.ReadIntPtr(InValue)).Target;
    }
    public static T? MarshalPointer<T>(IntPtr InValue) => Marshal.PtrToStructure<T>(InValue);

    public static IntPtr[] NativeArrayToIntPtrArray(IntPtr InNativeArray, int InLength)
    {
        try
        {
            if (InNativeArray == IntPtr.Zero || InLength == 0)
                return [];

            IntPtr[] result = new IntPtr[InLength];

            for (int i = 0; i < InLength; i++)
                result[i] = Marshal.ReadIntPtr(InNativeArray, i * Marshal.SizeOf<nint>());

            return result;
        }
        catch (Exception ex)
        {
            ManagedHost.HandleException(ex);
            return [];
        }
    }

    public static unsafe object?[]? MarshalParameterArray(IntPtr InNativeArray, int InLength, ManagedType* InParameterTypes, MethodBase? InMethodInfo)
    {
        if (InMethodInfo == null)
            return null;

        if (InNativeArray == IntPtr.Zero || InLength == 0)
            return null;

        var parameterInfos = InMethodInfo.GetParameters();
        var parameterPointers = NativeArrayToIntPtrArray(InNativeArray, InLength);
        var result = new object?[parameterPointers.Length];

        for (int i = 0; i < parameterPointers.Length; i++)
        {
            result[i] = InParameterTypes[i] == ManagedType.Object ?
                GCHandle.FromIntPtr(Marshal.ReadIntPtr(parameterPointers[i])).Target :
                MarshalPointer(parameterPointers[i], parameterInfos[i].ParameterType);
        }

        return result;
    }

    public static IntPtr MarhsalBoxValue(IntPtr InValue, Type InType)
    {
        // Copy unmanaged bytes into managed struct
        object boxed = Marshal.PtrToStructure(InValue, InType)!;

        // Create a GCHandle so C++ can hold onto the object
        GCHandle handle = GCHandle.Alloc(boxed, GCHandleType.Normal);
        return GCHandle.ToIntPtr(handle);
    }

    public static void MarshalUnboxValue(object InValue, Type InValueType, IntPtr OutValue)
    {
        int size = Marshal.SizeOf(InValueType);

        // Pin the boxed object to get a stable pointer
        GCHandle pinned = GCHandle.Alloc(InValue, GCHandleType.Pinned);
        unsafe
        {
            try
            {
                void* srcPtr = pinned.AddrOfPinnedObject().ToPointer();
                void* dstPtr = OutValue.ToPointer();
                Buffer.MemoryCopy(srcPtr, dstPtr, size, size);
            }
            finally
            {
                pinned.Free();
            }
        }
    }
    public static T? MarshalDelegate<T>(IntPtr functionPtr) where T : Delegate
    {
        if (functionPtr == IntPtr.Zero)
        {
            return null;
        }
        return (T)(object)Marshal.GetDelegateForFunctionPointer(functionPtr, typeof(T));
    }
}
