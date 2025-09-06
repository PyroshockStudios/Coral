#include "Coral/Assembly.hpp"
#include "Coral/HostInstance.hpp"
#include "Coral/StringHelper.hpp"
#include "Coral/TypeCache.hpp"

#include "CoralManagedFunctions.hpp"
#include "Verify.hpp"

namespace Coral {

    void Assembly::AddInternalCall(std::string_view InClassName, std::string_view InVariableName, void* InFunctionPtr)
    {
        CORAL_VERIFY(InFunctionPtr != nullptr);

        std::string assemblyQualifiedName(InClassName);
        assemblyQualifiedName += "+";
        assemblyQualifiedName += InVariableName;
        assemblyQualifiedName += ", ";
        assemblyQualifiedName += m_Name;

        const auto& name = m_InternalCallNameStorage.emplace_back(StringHelper::ConvertUtf8ToWide(assemblyQualifiedName));

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

    Type& Assembly::GetType(std::string_view InClassName) const
    {
        auto it = m_LocalTypeNameCache.find(std::string(InClassName));
        return it == m_LocalTypeNameCache.end() ? s_NullType : *it->second;
    }

    Type& Assembly::GetType(TypeId InClassId) const
    {
        auto it = m_LocalTypeIdCache.find(InClassId);
        return it == m_LocalTypeIdCache.end() ? s_NullType : *it->second;
    }

    const std::vector<Type*>& Assembly::GetTypes() const
    {
        return m_LocalTypeRefs;
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
    Assembly& AssemblyLoadContext::LoadAssembly(std::string_view InFilePath)
    {
        auto filepath = String::New(InFilePath);

        auto [idx, result] = m_LoadedAssemblies.EmplaceBack();
        result.m_Host = m_Host;
        result.m_AssemblyId = s_ManagedFunctions.LoadAssemblyFptr(m_ContextId, filepath);
        result.m_OwnerContextId = m_ContextId;
        result.m_LoadStatus = s_ManagedFunctions.GetLastLoadStatusFptr();

        LoadAssemblyData(result);

        String::Free(filepath);
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

        TypeCache::Get().m_VoidType = TypeCache::Get().GetTypeByName("System.Void");
        TypeCache::Get().m_ByteType = TypeCache::Get().GetTypeByName("System.Byte");
        TypeCache::Get().m_SByteType = TypeCache::Get().GetTypeByName("System.SByte");
        TypeCache::Get().m_ShortType = TypeCache::Get().GetTypeByName("System.Int16");
        TypeCache::Get().m_UShortType = TypeCache::Get().GetTypeByName("System.UInt16");
        TypeCache::Get().m_IntType = TypeCache::Get().GetTypeByName("System.Int32");
        TypeCache::Get().m_UIntType = TypeCache::Get().GetTypeByName("System.UInt32");
        TypeCache::Get().m_LongType = TypeCache::Get().GetTypeByName("System.Int64");
        TypeCache::Get().m_ULongType = TypeCache::Get().GetTypeByName("System.UInt64");
        TypeCache::Get().m_FloatType = TypeCache::Get().GetTypeByName("System.Single");
        TypeCache::Get().m_DoubleType = TypeCache::Get().GetTypeByName("System.Double");
        TypeCache::Get().m_BoolType = TypeCache::Get().GetTypeByName("System.Boolean");
        TypeCache::Get().m_CharType = TypeCache::Get().GetTypeByName("System.Char");
        TypeCache::Get().m_StringType = TypeCache::Get().GetTypeByName("System.String");
        TypeCache::Get().m_ObjectType = TypeCache::Get().GetTypeByName("System.Object");
        TypeCache::Get().m_IntPtrType = TypeCache::Get().GetTypeByName("System.IntPtr");
        TypeCache::Get().m_UIntPtrType = TypeCache::Get().GetTypeByName("System.UIntPtr");
        TypeCache::Get().m_DecimalType = TypeCache::Get().GetTypeByName("System.Decimal");
        TypeCache::Get().m_DateTimeType = TypeCache::Get().GetTypeByName("System.DateTime");
        TypeCache::Get().m_ExceptionType = TypeCache::Get().GetTypeByName("System.Exception");
        TypeCache::Get().m_ArrayType = TypeCache::Get().GetTypeByName("System.Array");
        m_SystemAssembly = &result;
    }
    void AssemblyLoadContext::LoadAssemblyData(Assembly& assembly)
    {
        if (assembly.m_LoadStatus == AssemblyLoadStatus::Success)
        {
            auto assemblyName = s_ManagedFunctions.GetAssemblyNameFptr(m_ContextId, assembly.m_AssemblyId);
            assembly.m_Name = assemblyName;
            String::Free(assemblyName);

            // TODO(Emily): Is it always desirable to preload every type from an assembly?
            int32_t typeCount = 0;
            s_ManagedFunctions.GetAssemblyTypesFptr(m_ContextId, assembly.m_AssemblyId, nullptr, &typeCount);

            std::vector<TypeId> typeIds(static_cast<size_t>(typeCount));
            s_ManagedFunctions.GetAssemblyTypesFptr(m_ContextId, assembly.m_AssemblyId, typeIds.data(), &typeCount);

            // reserve to avoid bad references after resizing
            assembly.m_LocalTypes.reserve(typeIds.size());
            assembly.m_LocalTypeRefs.reserve(typeIds.size());
            for (auto typeId : typeIds)
            {
                // global cache
                Type type;
                type.m_Id = typeId;
                TypeCache::Get().CacheType(std::move(type));
                // local cache
                Type type2;
                type2.m_Id = typeId;
                Type& inserted = assembly.m_LocalTypes.emplace_back(std::move(type2));
                assembly.m_LocalTypeRefs.push_back(&inserted);
                assembly.m_LocalTypeIdCache[inserted.GetTypeId()] = &inserted;
                assembly.m_LocalTypeNameCache[inserted.GetFullName()] = &inserted;
            }
        }
    }
}
