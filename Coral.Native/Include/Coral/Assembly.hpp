#pragma once

#include "Type.hpp"

#include "StableVector.hpp"

namespace Coral {

    enum class AssemblyLoadStatus
    {
        Success,
        FileNotFound,
        FileLoadFailure,
        InvalidFilePath,
        InvalidAssembly,
        UnknownError
    };

    class HostInstance;

    class Assembly
    {
    public:
        int32_t GetAssemblyID() const { return m_AssemblyId; }
        AssemblyLoadStatus GetLoadStatus() const { return m_LoadStatus; }
        StdStringView GetName() const { return m_Name; }

        void AddInternalCall(StdStringView InClassName, StdStringView InVariableName, void* InFunctionPtr);
        void UploadInternalCalls();

        Type GetType(StdStringView InClassName) const;

        const StdVector<Type>& GetTypes() const;

        MethodInfo GetMethodFromToken(MetadataToken InToken) const;
        FieldInfo GetFieldFromToken(MetadataToken InToken) const;
    private:
        HostInstance* m_Host = nullptr;
        int32_t m_AssemblyId = -1;
        int32_t m_OwnerContextId = 0;
        AssemblyLoadStatus m_LoadStatus = AssemblyLoadStatus::UnknownError;
        StdString m_Name;

        StdVector<UCString> m_InternalCallNameStorage;

        StdVector<InternalCall> m_InternalCalls;

        StdVector<Type> m_LocalTypes;
        StdUnorderedMap<StdString, Type> m_LocalTypeNameCache;

        friend class HostInstance;
        friend class AssemblyLoadContext;
    };

    class AssemblyLoadContext
    {
    public:
        Assembly& LoadAssembly(StdStringView InFilePath);
        Assembly& LoadAssemblyFromMemory(const std::byte* data, int64_t dataLength);
        const StableVector<Assembly>& GetLoadedAssemblies() const { return m_LoadedAssemblies; }

        const Assembly& GetSystemAssembly() const
        {
            return *m_SystemAssembly;
        }

    private:
        void LoadSystemAssembly();

        void LoadAssemblyData(Assembly& assembly);

        int32_t m_ContextId;
        StableVector<Assembly> m_LoadedAssemblies;
        Assembly* m_SystemAssembly = nullptr;

        HostInstance* m_Host = nullptr;

        friend class HostInstance;
    };

}
