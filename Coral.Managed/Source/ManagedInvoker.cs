using System;
using System.Collections.Concurrent;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;

using ManagedMethodCall = System.Func<object?, object?[]?, object?>;
namespace Coral.Managed;
internal static class ManagedInvoker
{
    // Cache generated delegates to avoid regenerating for the same MethodInfo
    private static readonly ConcurrentDictionary<MethodInfo, ManagedMethodCall> s_DelegateCache
        = new ConcurrentDictionary<MethodInfo, ManagedMethodCall>();

    internal static object? Invoke(MethodInfo InMethod, object? InTarget, object?[]? InParameters)
    {    
        if (InParameters == null)
            InParameters = Array.Empty<object?>();
        var invoker = s_DelegateCache.GetOrAdd(InMethod, CreateInvoker);
        return invoker(InTarget, InParameters);
    }

    private static ManagedMethodCall CreateInvoker(MethodInfo method)
    {
        var paramInfos = method.GetParameters();
        var dynamicMethod = new DynamicMethod(
            $"Invoke_{method.Name}",
            typeof(object),
            new Type[] { typeof(object), typeof(object?[]) },
            method.DeclaringType.Module,
            true // skip visibility checks
        );

        var il = dynamicMethod.GetILGenerator();

        // Load target instance for instance/abstract methods
        if (!method.IsStatic)
        {
            il.Emit(OpCodes.Ldarg_0); // target object
            if (method.DeclaringType.IsValueType)
                il.Emit(OpCodes.Unbox, method.DeclaringType);
            else
                il.Emit(OpCodes.Castclass, method.DeclaringType);
        }

        // Load parameters
        for (int i = 0; i < paramInfos.Length; i++)
        {
            il.Emit(OpCodes.Ldarg_1); // load object[] array
            il.Emit(OpCodes.Ldc_I4, i); // index
            il.Emit(OpCodes.Ldelem_Ref); // load element

            var paramType = paramInfos[i].ParameterType;

            if (paramType.IsByRef)
            {
                throw new NotSupportedException("ByRef parameters not supported.");
            }
            else if (paramType.IsPointer)
            {
                throw new NotSupportedException("Pointer parameters not supported.");
            }
            else if (paramType.IsValueType)
            {
                il.Emit(OpCodes.Unbox_Any, paramType); // unbox value types
            }
            else
            {
                il.Emit(OpCodes.Castclass, paramType); // cast reference types
            }
        }

        // Call method
        if (method.IsVirtual && !method.IsFinal && !method.IsStatic)
            il.EmitCall(OpCodes.Callvirt, method, null);
        else
            il.EmitCall(OpCodes.Call, method, null);

        // Handle return value
        if (method.ReturnType == typeof(void))
        {
            il.Emit(OpCodes.Ldnull);
        }
        else if (method.ReturnType.IsPointer)
        {
            throw new NotSupportedException("Pointer return types are not supported.");
        }
        else if (method.ReturnType.IsValueType)
        {
            il.Emit(OpCodes.Box, method.ReturnType); // box value type
        }

        il.Emit(OpCodes.Ret);

        return (ManagedMethodCall)dynamicMethod.CreateDelegate(typeof(ManagedMethodCall));
    }
}
