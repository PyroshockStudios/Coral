#include "Coral/Assembly.hpp"
#include "Coral/HostInstance.hpp"
#include "Coral/StringHelper.hpp"

#include "TypeCache.hpp"
#include "CoralManagedFunctions.hpp"
#include "Verify.hpp"

namespace Coral {

    void Assembly::AddInternalCall(StdStringView InClassName, StdStringView InVariableName, void* InFunctionPtr)
    {
        CORAL_VERIFY(InFunctionPtr != nullptr);

        StdString assemblyQualifiedName(InClassName);
        assemblyQualifiedName += "+";
        assemblyQualifiedName += InVariableName.data();
        assemblyQualifiedName += ", ";
        assemblyQualifiedName += m_Name;

        const auto& name = m_InternalCallNameStorage.emplace_back(StringHelper::ConvertUtf8ToUC(assemblyQualifiedName));

        InternalCall internalCall;
        // TODO(Emily): This would require proper conversion from UTF8 to native UC encoding.
        internalCall.Name = name.c_str();
        internalCall.NativeFunctionPtr = InFunctionPtr;
        m_InternalCalls.emplace_back(internalCall);
    }

    void Assembly::UploadInternalCalls()
    {
        s_ManagedFunctions.SetInternalCallsFptr(m_OwnerContextId, m_InternalCalls.data(), static_cast<int32_t>(m_InternalCalls.size()));
    }

    static Type s_NullType;

    Type Assembly::GetType(StdStringView InClassName) const
    {
        auto it = m_LocalTypeNameCache.find(StdString(InClassName));
        return it == m_LocalTypeNameCache.end() ? s_NullType : it->second;
    }

    const StdVector<Type>& Assembly::GetTypes() const
    {
        return m_LocalTypes;
    }

    MethodInfo Assembly::GetMethodFromToken(MetadataToken InToken) const
    {
        MethodInfo info{};
        info.m_Handle = s_ManagedFunctions.GetMethodInfoFromTokenFptr(m_OwnerContextId, m_AssemblyId, InToken);
        return info;
    }

    FieldInfo Assembly::GetFieldFromToken(MetadataToken InToken) const
    {
        FieldInfo info{};
        info.m_Handle = s_ManagedFunctions.GetFieldInfoFromTokenFptr(m_OwnerContextId, m_AssemblyId, InToken);
        return info;
    }

    // TODO(Emily): Massive de-dup needed between `LoadAssembly` and `LoadAssemblyFromMemory`.
    Assembly& AssemblyLoadContext::LoadAssembly(StdStringView InFilePath)
    {
        auto filepath = NativeString::New(InFilePath);

        auto [idx, result] = m_LoadedAssemblies.EmplaceBack();
        result.m_Host = m_Host;
        result.m_AssemblyId = s_ManagedFunctions.LoadAssemblyFptr(m_ContextId, filepath);
        result.m_OwnerContextId = m_ContextId;
        result.m_LoadStatus = s_ManagedFunctions.GetLastLoadStatusFptr();

        LoadAssemblyData(result);

        NativeString::Free(filepath);
        return result;
    }

    Assembly& AssemblyLoadContext::LoadAssemblyFromMemory(const std::byte* data, int64_t dataLength)
    {
        auto [idx, result] = m_LoadedAssemblies.EmplaceBack();
        result.m_Host = m_Host;
        result.m_AssemblyId = s_ManagedFunctions.LoadAssemblyFromMemoryFptr(m_ContextId, data, dataLength);
        result.m_OwnerContextId = m_ContextId;
        result.m_LoadStatus = s_ManagedFunctions.GetLastLoadStatusFptr();

        LoadAssemblyData(result);

        return result;
    }

    void AssemblyLoadContext::LoadSystemAssembly()
    {
        auto [idx, result] = m_LoadedAssemblies.EmplaceBack();
        result.m_Host = m_Host;
        result.m_AssemblyId = s_ManagedFunctions.GetSystemAssemblyFptr(m_ContextId);
        result.m_OwnerContextId = m_ContextId;
        result.m_LoadStatus = s_ManagedFunctions.GetLastLoadStatusFptr();

        LoadAssemblyData(result);

        TypeCache::Get().m_VoidType = result.GetType("System.Void");
        TypeCache::Get().m_ByteType = result.GetType("System.Byte");
        TypeCache::Get().m_SByteType = result.GetType("System.SByte");
        TypeCache::Get().m_ShortType = result.GetType("System.Int16");
        TypeCache::Get().m_UShortType = result.GetType("System.UInt16");
        TypeCache::Get().m_IntType = result.GetType("System.Int32");
        TypeCache::Get().m_UIntType = result.GetType("System.UInt32");
        TypeCache::Get().m_LongType = result.GetType("System.Int64");
        TypeCache::Get().m_ULongType = result.GetType("System.UInt64");
        TypeCache::Get().m_FloatType = result.GetType("System.Single");
        TypeCache::Get().m_DoubleType = result.GetType("System.Double");
        TypeCache::Get().m_BoolType = result.GetType("System.Boolean");
        TypeCache::Get().m_CharType = result.GetType("System.Char");
        TypeCache::Get().m_StringType = result.GetType("System.String");
        TypeCache::Get().m_ObjectType = result.GetType("System.Object");
        TypeCache::Get().m_IntPtrType = result.GetType("System.IntPtr");
        TypeCache::Get().m_UIntPtrType = result.GetType("System.UIntPtr");
        TypeCache::Get().m_DecimalType = result.GetType("System.Decimal");
        TypeCache::Get().m_DateTimeType = result.GetType("System.DateTime");
        TypeCache::Get().m_ExceptionType = result.GetType("System.Exception");
        TypeCache::Get().m_ArrayType = result.GetType("System.Array");
        m_SystemAssembly = &result;
    }
    void AssemblyLoadContext::LoadAssemblyData(Assembly& assembly)
    {
        if (assembly.m_LoadStatus == AssemblyLoadStatus::Success)
        {
            auto assemblyName = s_ManagedFunctions.GetAssemblyNameFptr(m_ContextId, assembly.m_AssemblyId);
            assembly.m_Name = assemblyName;
            NativeString::Free(assemblyName);

            // TODO(Emily): Is it always desirable to preload every type from an assembly?
            int32_t typeCount = 0;
            s_ManagedFunctions.GetAssemblyTypesFptr(m_ContextId, assembly.m_AssemblyId, nullptr, &typeCount);

            StdVector<TypeId> typeIds(static_cast<size_t>(typeCount));
            s_ManagedFunctions.GetAssemblyTypesFptr(m_ContextId, assembly.m_AssemblyId, typeIds.data(), &typeCount);

            // reserve to avoid bad references after resizing
            assembly.m_LocalTypes.reserve(typeIds.size());
            for (TypeId typeId : typeIds)
            {
                Type type;
                type.m_Id = typeId;
                assembly.m_LocalTypes.emplace_back(type);
                assembly.m_LocalTypeNameCache[type.GetFullName()] = type;
            }
        }
    }
}
