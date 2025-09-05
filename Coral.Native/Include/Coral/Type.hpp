#pragma once

#include "Core.hpp"
#include "String.hpp"
#include "ManagedObject.hpp"
#include "MethodInfo.hpp"
#include "FieldInfo.hpp"
#include "PropertyInfo.hpp"

#include <optional>

namespace Coral {

    class Type
    {
    public:
        String GetFullName() const;
        String GetName() const;
        String GetNamespace() const;
        String GetAssemblyQualifiedName() const;

        Type& GetBaseType();
        std::vector<Type*>& GetInterfaceTypes();

        int32_t GetSize() const;

        bool IsSubclassOf(const Type& InOther) const;
        bool IsAssignableTo(const Type& InOther) const;
        bool IsAssignableFrom(const Type& InOther) const;

        std::vector<MethodInfo> GetMethods() const;
        std::vector<FieldInfo> GetFields() const;
        std::vector<PropertyInfo> GetProperties() const;

        MethodInfo GetMethod(std::string_view MethodName, bool InStatic = false) const;
        MethodInfo GetMethod(std::string_view MethodName, int32_t InParamCount, bool InStatic = false) const;
        MethodInfo GetMethod(std::string_view MethodName, const std::vector<const Type*>& InParamTypes, bool InStatic = false) const;
        FieldInfo GetField(std::string_view FieldName, bool InStatic = false) const;
        PropertyInfo GetProperty(std::string_view PropertyName, bool InStatic = false) const;

        bool HasAttribute(const Type& InAttributeType) const;
        std::vector<Attribute> GetAttributes() const;

        ManagedType GetManagedType() const;

        bool IsSZArray() const;
        Type& GetElementType();

        bool operator==(const Type& InOther) const;

        operator bool() const { return m_Id != -1; }

        TypeId GetTypeId() const { return m_Id; }

    public:
        template <typename... TArgs>
        ManagedObject CreateInstance(MethodParams<TArgs...>&& InParameters = {}, ManagedObject* OutException = nullptr) const
        {
            if (InParameters.paramCount > 0)
            {
                return CreateInstanceInternal(OutException, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount);
            }
            else
            {
                return CreateInstanceInternal(OutException, nullptr, nullptr, 0);
            }
        }
        
        template <typename TReturn = void, typename... TArgs>
        auto InvokeStaticMethod(const MethodInfo& InMethod, MethodParams<TArgs...>&& InParameters = {}, ManagedObject* OutException = nullptr) const
        {
            if constexpr (std::is_void_v<TReturn>)
            {
                if (InParameters.paramCount > 0)
                {
                    InvokeStaticMethodInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount);
                }
                else
                {
                    InvokeStaticMethodInternal(OutException, InMethod, nullptr, nullptr, 0);
                }
            }
            else
            {
                TReturn result{};
                if (InParameters.paramCount > 0)
                {
                    InvokeStaticMethodRetInternal(OutException, InMethod, InParameters.parameterValues, InParameters.parameterTypes, InParameters.paramCount, &result);
                }
                else
                {
                    InvokeStaticMethodRetInternal(OutException, InMethod, nullptr, nullptr, 0, &result);
                }
                return result;
            }
        }

    private:
        ManagedObject CreateInstanceInternal(ManagedObject* OutException, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
        void InvokeStaticMethodInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength) const;
        void InvokeStaticMethodRetInternal(ManagedObject* OutException, const MethodInfo& InMethod, const void** InParameters, const ManagedType* InParameterTypes, size_t InLength, void* InResultStorage) const;

    private:
        TypeId m_Id = -1;
        Type* m_BaseType = nullptr;
        std::optional<std::vector<Type*>> m_InterfaceTypes = std::nullopt;
        Type* m_ElementType = nullptr;

        friend class HostInstance;
        friend class ManagedAssembly;
        friend class AssemblyLoadContext;
        friend class MethodInfo;
        friend class FieldInfo;
        friend class PropertyInfo;
        friend class Attribute;
        friend class ReflectionType;
        friend class ManagedObject;
    };

    class ReflectionType
    {
    public:
        operator Type& () const;

    public:
        TypeId m_TypeID;
    };

    static_assert(offsetof(ReflectionType, m_TypeID) == 0);
    static_assert(sizeof(ReflectionType) == 4);
}
